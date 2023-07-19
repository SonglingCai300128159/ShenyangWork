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
    if(s->openCanID<0){
        printf("Initial Can Error\n");
        return -1;
    }
    s->RS232ID=RS232SInitial(addr,1);
    if(s->RS232ID<0){
        printf("Initial RS232 Error\n");
        return -1;
    }
    return 1;
}

int transferCommand(communicationIDs * c){
    int data[8];
    while(1){
        for(int i=0;i<8;i++){
            read(s->RS232ID,&data[i],1);
        }
        
    }
}
