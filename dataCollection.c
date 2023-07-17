#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "CANDriver/canDriver.h"
#include "i2cDriver/i2cDriver.h"
#include "GPSDriver/GPSDriver.h"
#include "BatteryDriver/BatteryDriver.h"
#include "RS232Driver/RS232Driver.h"
#include "dataCollection.h"


int initialSensor(int canID,int masterID,sensorIDs * s){
    s->openCanID = initialCan(canID);
    if(s->openCanID<0){
        printf("Initial Can Error\n");
        return -1;
    }
    s->i2cID=initialI2C();
    if(s->i2cID<0){
        printf("Initial i2c Error\n");
        return -1;
    }
    s->gpsID=gpsInitial();
    if(s->gpsID<0){
        printf("Initial gps Error\n");
        return -1;
    }
    s->batteryID=batteryInit();
    if(s->batteryID<0){
        printf("Initial battery Error\n");
        return -1;
    }
    s->masterID=masterID;
    s->RS232ID=RS232SInitial();
    if(s->RS232ID<0){
        printf("Initial RS232 Error\n");
        return -1;
    }
    return 1;
}

int getCommandFromCan(sensorIDs * s){
    while(1){
        int command[8]={0};
        if(receiveCan(s->openCanID,command)>0){
            sendSensorData(command,s);
        }
    }
}

int sendSensorData(int * command,sensorIDs * s){
    switch(command[2]){
		case 0xC1:
			return sendTRHData(s);
		case 0xC2:
			return sendGPSData(s);
		case 0xC3:
			return sendBatteryData(s);
        case 0xC0:
            sendTRHData(s);
			sendGPSData(s);
            sendBatteryData(s);
            break;
        default:
            printf("Unknow Command\n");
            return -1;
	}
    return 1;
}

int sendTRHData(sensorIDs * s){
    sht40Data sd;
    enum sht40Command co=measureHighPrecision;
    
    sendi2cCommand(s->i2cID, co);
    receivei2cData(s->i2cID,&sd);

    int data[8]={0};
    data[0]=0x5F;
    data[1]=0x15;
    data[2]=(int)sd.temperature;
    data[3]=(sd.temperature-data[2])*100;
    data[4]=(int)sd.humidity;
    data[5]=(sd.humidity-data[4])*100;

    return write(s->RS232ID,data,8);
}

int sendGPSData(sensorIDs * s){
    gpsData gd;

	int f=getGpsData(s->gpsID,&gd);
    if(f<0){
        printf("Read GPSData Invaild\n");
        return -1;
    }
    
    int data[8]={0};
    data[0]=0x5F;
    data[1]=0x15;
    data[2]=(int)gd.latitude.value;
    data[3]=(gd.latitude.value-data[2])*100;
    data[4]=(int)gd.longitude.value;
    data[5]=(gd.longitude.value-data[4])*100;
    data[6]=(int)gd.height;
    data[7]=(gd.height-data[6])*100;
    
    return write(s->RS232ID,data,8);
}

int sendBatteryData(sensorIDs * s){
    batteryStatue b;

	batteryData(s->batteryID,&b);

    int data[8]={0};
    data[0]=0x5F;
    data[1]=0x15;
    data[2]=(int)b.totalVotage;
    data[3]=(b.totalVotage-data[2])*100;
    data[4]=(int)b.totalCurrent;
    data[5]=(b.totalCurrent-data[4])*100;
    data[6]=(int)b.remainingCapacity;
    data[7]=(b.remainingCapacity-data[6])*100;
    
    return write(s->RS232ID,data,8);
}
