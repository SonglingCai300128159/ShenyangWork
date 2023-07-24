#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "motionControl.h"

int motionControlInitial(int selfCanID, char * addr,communicationIDs * c){
    c->CanID = initialCan(selfCanID);// CAN port
    if(c->CanID<0){
        printf("Initial Can Error\n");
        return -1;
    }
    printf("wireless");
    c->RS232ID=RS232SInitial(addr,1); //wireless port
    if(c->RS232ID<0){
        printf("Initial RS232 Error\n");
        return -1;
    }
    printf("4G");
    c->RS485ID=RS232SInitial("/dev/ttyS1",1); //4G port
    if(c->RS232ID<0){
        printf("Initial RS485 Error\n");
        return -1;
    }
    c->turingDiff=0x3C;
    return 1;
}

void * transferCommandFromWireless(IDs * id){ //使用无线收发指令
    int data[8]={0};
    while(1){
        for(int i=0;i<8;i++){
            read(id->c.RS232ID,&data[i],1);
            //printf("%x\n",data[i]);
        }
        translateAndSendCommand(data,&(id->c),&(id->s));
        //sendCan(c->CanID,0x260,8,data);
        
    }
}

void * transferCommandFrom4G(IDs * id){ //使用4g收发指令
    int data[8]={0};
    printf("tc4G:%d\n",id->c.RS485ID);
    while(1){
        for(int i=0;i<8;i++){
            read(id->c.RS485ID,&data[i],1);
            //printf("%x\n",data[i]);
        }
        translateAndSendCommand(data,&(id->c),&(id->s));
        //sendCan(c->CanID,0x260,8,data);
        
    }
}

int translateAndSendCommand(int * data,communicationIDs * c,sensorIDs * s){
    switch(data[0]){
        case 0x11:
            goStraight(data,c); //直行
            break;
        case 0x33:
            swerve(data,c); //转弯
            break;
        case 0x44:
            extend(data,c); //举升
            break;
        case 0x55:
            cameraMoving(data,c); //摄像头运动
            break;
        case 0x5F:
            sendSensorData(data,s); //向4G发送收集到的数据
        case 0xFE:
            sendCan(c->CanID,0x0000,8,data); //串口IO控制器
              

    }
}

int goStraight(int * data,communicationIDs * c){ 
    int setSpeed[8]={0x11,0x11,data[2],data[3],data[4],data[5],data[6],((0x11+0x11+data[2]+data[3]+data[4]+data[5]+data[6])&0xff)};
	sendCan(c->CanID,0x260,8,setSpeed);
	usleep(20000);
	int setDirection[8];
	if(data[1]==0xAA){
		int setDirection[8]={0x11,0x22,data[2],0x55,0x00,0x00,0x00,((0x11+0x22+data[2]+0x55+0x00+0x00+0x00)&0xff)};
	}
	else if(data[1]==0xBB){
		int setDirection[8]={0x11,0x22,data[2],0xAA,0x00,0x00,0x00,((0x11+0x22+data[2]+0xAA+0x00+0x00+0x00)&0xff)};
	}
	else{
		int setDirection[8]={0x11,0x22,0xFF,0xFF,0x00,0x00,0x00,((0x11+0x22+0xFF+0xFF+0x00+0x00+0x00)&0xff)};
	}
	sendCan(c->CanID,0x260,8,setDirection);
	return 1;
}

int swerve(int * data,communicationIDs * c){
    if(data[1]==0xAA){
        int leftSpeed=((data[3]*256+data[4])*256+data[5])*256+data[6]-(c->turingDiff); //计算左右轮数据
        int rightSpeed=((data[3]*256+data[4])*256+data[5])*256+data[6]+(c->turingDiff);
        int setSpeed0[8]={0x11,0x11,0x00,(leftSpeed/256/256/256),(leftSpeed/256/256%256),(leftSpeed/256%256),(leftSpeed%256),((0x11+0x11+0x00+(leftSpeed/256/256/256)+(leftSpeed/256/256%256)+(leftSpeed/256%256)+(leftSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed0);//将leftspeed速度按每两位分离，依次放入data[3][4][5][6]，以下同理
        usleep(20000);
        int setSpeed1[8]={0x11,0x11,0x11,(rightSpeed/256/256/256),(rightSpeed/256/256%256),(rightSpeed/256%256),(rightSpeed%256),((0x11+0x11+0x11+(rightSpeed/256/256/256)+(rightSpeed/256/256%256)+(rightSpeed/256%256)+(rightSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed1);
        usleep(20000);
        int setSpeed2[8]={0x11,0x11,0x22,(leftSpeed/256/256/256),(leftSpeed/256/256%256),(leftSpeed/256%256),(leftSpeed%256),((0x11+0x11+0x22+(leftSpeed/256/256/256)+(leftSpeed/256/256%256)+(leftSpeed/256%256)+(leftSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed2);
        usleep(20000);
        int setSpeed3[8]={0x11,0x11,0x33,(rightSpeed/256/256/256),(rightSpeed/256/256%256),(rightSpeed/256%256),(rightSpeed%256),((0x11+0x11+0x33+(rightSpeed/256/256/256)+(rightSpeed/256/256%256)+(rightSpeed/256%256)+(rightSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed3);
        usleep(20000);
        int setDirection[8]={0x11,0x22,0xEE,0x55,0x11,0x00,0x00,((0x11+0x22+(0xEE)+0x55+0x11+0x00+0x00)&0xff)};
        sendCan(c->CanID,0x260,8,setDirection);
    }else if(data[1]==0xBB){
        int leftSpeed=((data[3]*256+data[4])*256+data[5])*256+data[6]+(c->turingDiff);
        int rightSpeed=((data[3]*256+data[4])*256+data[5])*256+data[6]-(c->turingDiff);
        int setSpeed0[8]={0x11,0x11,0x00,(leftSpeed/256/256/256),(leftSpeed/256/256%256),(leftSpeed/256%256),(leftSpeed%256),((0x11+0x11+0x00+(leftSpeed/256/256/256)+(leftSpeed/256/256%256)+(leftSpeed/256%256)+(leftSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed0);
        usleep(20000);
        int setSpeed1[8]={0x11,0x11,0x11,(rightSpeed/256/256/256),(rightSpeed/256/256%256),(rightSpeed/256%256),(rightSpeed%256),((0x11+0x11+0x11+(rightSpeed/256/256/256)+(rightSpeed/256/256%256)+(rightSpeed/256%256)+(rightSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed1);
        usleep(20000);
        int setSpeed2[8]={0x11,0x11,0x22,(leftSpeed/256/256/256),(leftSpeed/256/256%256),(leftSpeed/256%256),(leftSpeed%256),((0x11+0x11+0x22+(leftSpeed/256/256/256)+(leftSpeed/256/256%256)+(leftSpeed/256%256)+(leftSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed2);
        usleep(20000);
        int setSpeed3[8]={0x11,0x11,0x33,(rightSpeed/256/256/256),(rightSpeed/256/256%256),(rightSpeed/256%256),(rightSpeed%256),((0x11+0x11+0x33+(rightSpeed/256/256/256)+(rightSpeed/256/256%256)+(rightSpeed/256%256)+(rightSpeed%256))&0xff)};
        sendCan(c->CanID,0x260,8,setSpeed3);
        usleep(20000);
        int setDirection[8]={0x11,0x22,0xEE,0x55,0x11,0x00,0x00,((0x11+0x22+(0xEE)+0x55+0x11+0x00+0x00)&0xff)};
        sendCan(c->CanID,0x260,8,setDirection);
    }else{
        c->turingDiff=((data[3]*256+data[4])*256+data[5])*256+data[6];
    }
}

int extend(int * data,communicationIDs * c){
	int setHeight[8]={0x44,0x33,0x00,data[3],data[4],data[5],data[6],((0x44+0x33+data[3]+data[4]+data[5]+data[6])&0xff)};
	sendCan(c->CanID,0x260,8,setHeight);
	usleep(20000);
	int doExtend[8]={0x44,0x22,0x00,0x55,0x00,0x00,0x00,0xbb};
	sendCan(c->CanID,0x260,8,doExtend);
	return 1;
}

int cameraMoving(int * data,communicationIDs * c){
	if(data[2]==0x00){
		int setDirection[8]={0x55,0x33,0x00,data[3],data[4],data[5],data[6],((0x55+0x33+data[3]+data[4]+data[5]+data[6])&0xff)};
		sendCan(c->CanID,0x260,8,setDirection);
		usleep(20000);
		int doCameraMoving[8]={0x55,0x22,0x00,0x55,0x00,0x00,0x00,0xCC};
		sendCan(c->CanID,0x260,8,doCameraMoving);
		return 1;
	}else{
		int setDirection[8]={0x55,0x33,0x11,data[3],data[4],data[5],data[6],((0x55+0x33+0x11+data[3]+data[4]+data[5]+data[6])&0xff)};
		sendCan(c->CanID,0x260,8,setDirection);
		usleep(20000);
		int doCameraMoving[8]={0x55,0x22,0x11,0x55,0x00,0x00,0x00,0xDD};
		sendCan(c->CanID,0x260,8,doCameraMoving);
		return 1;
	}
}

int main(){
    communicationIDs co;
    sensorIDs se;
    IDs i;
    
    int selfCanID=0x11;
    int data[]={0x5F,0x15,0xC0,0x02,0x9a,0x10,0x68,0x00};
    int data2[]={0x11,0xCC,0xFF,0x00,0x00,0x01,0x4d,0x00};
    motionControlInitial(selfCanID,"/dev/ttyUSB0",&co);
    dataCollectionInitial(selfCanID,&se,&co);
    i.c=co;
    i.s=se;
    //translateAndSendCommand(data,&c,&s);
    //extend(data,&c);
    /*sleep(5);*/
    //goStraight(data2,&c);
    pthread_t Pthread[2];
    int ret = pthread_create(&Pthread[0], NULL, transferCommandFrom4G, &i); //创建线程
    transferCommandFromWireless(&i);
    return 0;
}
