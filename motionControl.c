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
        sendCan(c->CanID,0x260,8,data);
        
    }
}

int main(){
    communicationIDs c;
    motionControlInitial(0x11,"/dev/ttyUSB0",&c);
    transferCommand(&c);
    return 0;
}
