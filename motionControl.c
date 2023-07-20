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
    c->CanID = initialCan(selfCanID);
    if(c->CanID<0){
        printf("Initial Can Error\n");
        return -1;
    }
    c->RS232ID=RS232SInitial(addr,1);
    if(c->RS232ID<0){
        printf("Initial RS232 Error\n");
        return -1;
    }
    return 1;
}

int transferCommand(communicationIDs * c){
    int data[8]={0};
    while(1){
        for(int i=0;i<8;i++){
            read(c->RS232ID,&data[i],1);
            //printf("%x\n",data[i]);
        }
        translateAndSendCommand(data,c);
        //sendCan(c->CanID,0x260,8,data);
        
    }
}

int translateAndSendCommand(int * data,communicationIDs * c){
    switch(data[0]){
        case 0x11:
            goStraight(data,c);
            break;
        case 0x33:
            swerve(data,c);
            break;
        case 0x44:
            extend(data,c);
            break;
        case 0x55:
            cameraMoving(data,c);
            break;

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
		int setDirection[8]={0x11,0x22,data[2],0xFF,0x00,0x00,0x00,((0x11+0x22+data[2]+0xFF+0x00+0x00+0x00)&0xff)};
	}
	sendCan(c->CanID,0x260,8,setDirection);
	return 1;
}

int swerve(int * data,communicationIDs * c){}
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
    communicationIDs c;
    int data[]={0x55,0x33,0x00,0x02,0x28,0x38,0xc4,0x00};
    int data2[]={0x11,0xCC,0xFF,0x00,0x00,0x01,0x4d,0x00};
    motionControlInitial(0x11,"/dev/ttyUSB0",&c);
    cameraMoving(data,&c);
    transferCommand(&c);
    return 0;
}
