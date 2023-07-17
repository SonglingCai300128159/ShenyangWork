#ifndef _I2CDRIVER_H
#define _I2CDRIVER_H
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

typedef struct{
	float temperature;
	float humidity;
	int serialNumber[4];
} sht40Data;

enum sht40Command{
	measureHighPrecision=1, measureMediumPrecision, measureLowPrecision, readSerial, softReset, heat200For1, heat200For01, heat110For1, heat110For01, heat20For1, heat20for01
};

int initialI2C();
int sendi2cCommand(int fd, enum sht40Command c);
int receivei2cData(int fd,sht40Data * d);
#endif
