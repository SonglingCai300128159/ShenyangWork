#ifndef _BATTERYDRIVER_H
#define _BATTERYDRIVER_H 

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>

typedef struct{
	float totalVotage;
	float totalCurrent;
	float remainingCapacity;
	float totalCapacity;
	float temp[2];
}batteryStatue;

int batteryInit();
int batteryData(int serialPort,batteryStatue * b);
#endif
