#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "CANDriver/canDriver.h"
#include "i2cDriver/i2cDriver.h"
#include "GPSDriver/GPSDriver.h"
#include "BatteryDriver/BatteryDriver.h"


typedef struct{
    int openCanID;
    int i2cID;
    int gpsID;
    int batteryID;
    int masterID;
}sensorIDs;

int initialSensor(int canID,int masterID,sensorIDs * s);
int getCommandFromCan(sensorIDs * s);
int sendSensorData(int * command,sensorIDs * s);
int sendTRHData(sensorIDs * s);
int sendGPSData(sensorIDs * s);
int sendBatteryData(sensorIDs * s);