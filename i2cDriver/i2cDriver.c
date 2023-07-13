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
#include <math.h>
#include"i2cDriver.h"

int initialI2C(){
    int fd = open("/dev/i2c-1", O_RDWR);
    if(fd<0){
    	printf("open error\n");
    	return -1;
    }
    return fd;
}

int sendCommand(int fd, enum sht40Command c){
    char buff=0;
    switch(c){
        case 1:
            buff=0xfd;
            break;
        case 2:
            buff=0xf6;
            break;
        case 3:
            buff=0xe0;
            break;
        case 4:
            buff=0x89;
            break;
        case 5:
            buff=0x94;
            break;
        case 6:
            buff=0x39;
            break;
        case 7:
            buff=0x32;
            break;
        case 8:
            buff=0x2f;
            break;
        case 9:
            buff=0x24;
            break;
        case 10:
            buff=0x1e;
            break;
        case 11:
            buff=0x15;
            break;
    }
    
    struct i2c_msg msg = {
        .addr = 0x44,
        .flags = 0,
        .len = 1,
        .buf = &buff,
    };
    struct i2c_rdwr_ioctl_data rdwr_msg = {
        .msgs = &msg,
        .nmsgs = 1,
    };
     
    int ret = ioctl(fd, I2C_RDWR, &rdwr_msg);
    usleep(10000);
    return 1;
}

int receiveData(int fd,sht40Data * d){
    char r[6]="";
    struct i2c_msg msg2 = {
        .addr = 0x44,
        .flags = I2C_M_RD,
        .len = 6,
        .buf = r,
    };
    struct i2c_rdwr_ioctl_data rdwr_msg2 = {
        .msgs = &msg2,
        .nmsgs = 1,
    };
     
    int ret2 = ioctl(fd, I2C_RDWR, &rdwr_msg2);
    
    int st=r[0]*16*16+r[1];
    d->temperature=175*(st/(pow(2,16)-1))-45;
    
    int srh=r[3]*16*16+r[4];
    d->humidity=125*(srh/(pow(2,16)-1))-6;
    
    d->serialNumber[0]=r[0];
    d->serialNumber[1]=r[1];
    d->serialNumber[2]=r[3];
    d->serialNumber[3]=r[4];
    return 1;
}

int main(){
    sht40Data sd;
    int f;
    enum sht40Command co=measureHighPrecision;
    
    f=initialI2C();
    sendCommand(f, co);
    receiveData(f,&sd);
    
    printf("%f %f\n",sd.temperature,sd.humidity);
    
    return 0;
    
}
