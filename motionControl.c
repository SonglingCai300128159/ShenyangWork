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
        translateAndSendCommand(data);
        //sendCan(c->CanID,0x260,8,data);
        
    }
}

int translateAndSendCommand(int * data){
    switch(data[0]){
        case 0x11:
            goStraight(data);
            break;
        case 0x33:
            swerve(data);
            break;
        case 0x44:
            extend(data);
            break;
        case 0x55:
            cameraMoving(data);
            break;

    }
}

int goStraight(int * data){
    int setSpeed[8]={0x11,0x11,data[2],data[3],data[4],data[5],data[6],(0x11+0x11+data[2]+data[3]+data[4]+data[5]+data[6])&0xff};
	sendCan(s,0x260,8,setSpeed);
	if(data[1]==0xAA)
		int setDirection[8]={0x11,0x22,data[2],0x55,0x00,0x00,0x00,(0x11+0x22+data[2]+data[1]+0x00+0x00+0x00)&0xff};
	else if(data[1]==0xBB)
		int setDirection[8]={0x11,0x22,data[2],0xAA,0x00,0x00,0x00,(0x11+0x22+data[2]+data[1]+0x00+0x00+0x00)&0xff};
	else
		int setDirection[8]={0x11,0x22,data[2],0xFF,0x00,0x00,0x00,(0x11+0x22+data[2]+data[1]+0x00+0x00+0x00)&0xff};
	sendCan(s,0x260,8,setDirection);
	return 1;
}

int swerve(int * data){}
int extend(int * data){}
int cameraMoving(int * data){}

int main(){
    communicationIDs c;
    motionControlInitial(0x11,"/dev/ttyUSB0",&c);
    transferCommand(&c);
    return 0;
}
