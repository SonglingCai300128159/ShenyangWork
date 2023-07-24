#ifndef _MOTIONCONTROL_H
#define _MOTIONCONTROL_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>

typedef struct{
    int CanID;
    int RS232ID;
    int RS485ID;
    int turingDiff;
}communicationIDs;

#include "CANDriver/canDriver.h"
#include "RS232Driver/RS232Driver.h"
#include "dataCollection.h"



int motionControlInitial(int selfCanID, char * addr,communicationIDs * c);
int transferCommand(communicationIDs * c,sensorIDs * s);
int translateAndSendCommand(int * data,communicationIDs * c,sensorIDs * s);
int goStraight(int * data,communicationIDs * c);
int swerve(int * data,communicationIDs * c);
int extend(int * data,communicationIDs * c);
int cameraMoving(int * data,communicationIDs * c);
#endif
