#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include "RS232Driver.h"

int RS232SInitial(){
	int serialPort = open("/dev/ttyS5", O_RDWR);
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


        cfsetospeed(&tty, B115200);
        cfsetispeed(&tty, B115200);
        tty.c_cflag |= CS8;
        tty.c_cflag &= ~PARENB;
        tty.c_cflag &= ~CSTOPB;
        tcsetattr(serialPort, TCSANOW, &tty);   //设置终端控制属性,TCSANOW：不等数据传输完毕就立即改变属性
		return serialPort;
}

