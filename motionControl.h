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