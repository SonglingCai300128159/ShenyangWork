#ifndef _DATACOLLECTION_H
#define _DATACOLLECTION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct{
    int openCanID;
    int i2cID;
    int gpsID;
    int batteryID;
    int RS485ID;
}sensorIDs;

#include "CANDriver/canDriver.h"
#include "i2cDriver/i2cDriver.h"
#include "GPSDriver/GPSDriver.h"
#include "BatteryDriver/BatteryDriver.h"
#include "RS232Driver/RS232Driver.h"
#include "motionControl.h"




int dataCollectionInitial(int canID,sensorIDs * s,communicationIDs * c);
int getCommandFromCan(sensorIDs * s);
int sendSensorData(int * command,sensorIDs * s);
int sendTRHData(sensorIDs * s);
int sendGPSData(sensorIDs * s);
int sendBatteryData(sensorIDs * s);
#endif
