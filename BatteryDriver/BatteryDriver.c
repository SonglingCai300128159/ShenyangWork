#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include "BatteryDriver.h"

int batteryInit(){
	int serialPort = open("/dev/ttyS0", O_RDWR);
        if (serialPort < 0) {
                perror("Error opening serial port");
                return -1;
        }

        struct termios tty;
        memset(&tty, 0, sizeof(tty));
        if (tcgetattr(serialPort, &tty) != 0) {
                perror("Error configuring serial port");
                return -1;
        }

        cfsetospeed(&tty, B9600);
        cfsetispeed(&tty, B9600);
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tty.c_iflag &= ~(INLCR | ICRNL);
        tty.c_iflag &= ~(IXON | IXOFF |IXANY);
        tty.c_oflag &= ~OPOST;
        tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        tty.c_cc[VMIN] = 1;
        tcflush(serialPort,TCIFLUSH);
        tcsetattr(serialPort, TCSANOW, &tty);
	return serialPort;
}

int batteryData(int serialPort,batteryStatue * b){
	long i=0x77FDFF0003A5DD;
        write(serialPort,&i,7);
	
	int tempData[34];
	//int temp=0;
	for(int i=0;i<34;i=i+1){
		tempData[i]=0;
		int j=read(serialPort,&tempData[i],1);
		if(i<0) return -1;
		//tempData[i]=temp;
		//printf("%d %x\n",i,tempData[i]);
	}
	b->totalVotage =(tempData[4]*16*16+tempData[5])/100.0;
	b->totalCurrent=(65536-(tempData[6]*16*16+tempData[7]))/100.0;
	b->remainingCapacity=(tempData[8]*16*16+tempData[9])*10.0;
	b->totalCapacity=(tempData[10]*16*16+tempData[11])*10;
	b->temp[0]=((tempData[27]*16*16+tempData[28])-2731)*0.1;
	b->temp[1]=((tempData[29]*16*16+tempData[30])-2731)*0.1;
	return 1;
}

int main(){	
	batteryStatue b;
	int fd;
	fd=batteryInit();
	batteryData(fd,&b);
	printf("%fV %fA %fmAH %fmAH %fC %Fc\n",b.totalVotage,b.totalCurrent,b.remainingCapacity,b.totalCapacity,b.temp[0],b.temp[1]);
	return 0;
}


