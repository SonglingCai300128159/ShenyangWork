#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "dataCollection.h"


int dataCollectionInitial(int canID,sensorIDs * s,communicationIDs * c){
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
    s->RS485ID=c->RS485ID;
    
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
    int data[2]={0};
    int temp=(int)sd.temperature;
    int temp2=(int)((sd.temperature-temp)*100);
    data[0]=0x155F+(temp+temp2*256)*256*256;//temperature temp.temp2, data[0]=temp2temp1155f
    temp=(int)sd.humidity;
    temp2=(int)((sd.humidity-temp)*100);
    data[1]=temp+temp2*256;//humidity temp.temp2, data[1]=0000temp2temp1
    printf("%f %f\n",sd.temperature,sd.humidity);
    
    return write(s->RS485ID,data,8);
}

int sendGPSData(sensorIDs * s){
    gpsData gd;
    //printf("2***\n");
    int f=getGpsData(s->gpsID,&gd);
    //printf("***\n");
    if(f<0){
        printf("Read GPSData Invaild\n");
        int data[8]={0};
        data[0]=0x0000155F;
        data[1]=0x00000000;
    
        return write(s->RS485ID,data,8);
    }
    
    int data[2]={0};
    int temp,temp2;
    
    temp=(int)gd.latitude.value;
    temp2=(int)((gd.latitude.value-temp)*100);
    data[0]=0x155f+(temp+temp2*256)*256*256;
    
    temp=(int)gd.longitude.value;
    temp2=(int)((gd.longitude.value-temp)*100);
    data[1]=(temp+temp2*256);
    
    temp=(int)gd.height;
    temp2=(int)((gd.height-temp)*100);
    data[1]=data[1]+(temp+temp2*256)*256*256;
    
    return write(s->RS485ID,data,8);
}

int sendBatteryData(sensorIDs * s){
    batteryStatue b;

    batteryData(s->batteryID,&b);

    int data[2]={0};
    int temp,temp2;
    
    temp=(int)b.totalVotage;
    temp2=(int)((b.totalVotage-temp)*100);
    data[0]=0x155f+(temp+temp2*256)*256*256;
    
    temp=(int)b.totalCurrent;
    temp2=(int)((b.totalCurrent-temp)*100);
    data[1]=(temp+temp2*256);
    //printf("%d %d %x\n",temp,temp2,data[1]);
    
    temp=(int)(b.remainingCapacity/100);
    temp2=(int)(b.remainingCapacity-temp*100);
    data[1]=data[1]+(temp+temp2*256)*256*256;
    //printf("%d %d %x\n",temp,temp2,data[1]);
    
    //printf("%f %f %f\n",b.totalVotage,b.totalCurrent,b.remainingCapacity);
    
    return write(s->RS485ID,data,8);
 }

/*int main(){
	sensorIDs s;
	initialSensor(0x11,0x19,&s);
	sendTRHData(&s);
	printf("1\n");
	sendGPSData(&s);
	printf("2\n");
	sendBatteryData(&s);
	printf("3\n");
	return 0;
}*/
