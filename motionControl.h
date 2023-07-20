#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include "CANDriver/canDriver.h"
#include "RS232Driver/RS232Driver.h"

typedef struct{
    int CanID;
    int RS232ID;
}communicationIDs;

int motionControlInitial(int selfCanID, char * addr,communicationIDs * c);
int transferCommand(communicationIDs * c);
int translateAndSendCommand(int * data,communicationIDs * c);
int goStraight(int * data,communicationIDs * c);
int swerve(int * data,communicationIDs * c);
int extend(int * data,communicationIDs * c);
int cameraMoving(int * data,communicationIDs * c);
