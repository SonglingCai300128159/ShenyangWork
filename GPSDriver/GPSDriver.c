#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include "GPSDriver.h"

int gpsInitial(){
	int serialPort = open("/dev/ttyUSB0", O_RDWR);
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

char buffer[1024]="";
int getGpsData(int serialPort,gpsData * igd){
                char target[]="$GNGGA";
		char *rawlgv;
		char *rawlgd;
		char *rawltv;
		char *rawltd;
		char *rawh;
		char *flag;
		int directionValue;
                int bytes_read = read(serialPort, buffer, sizeof(buffer));
                //printf("%d Received: %s\n", strlen(buffer),buffer);
                if(strlen(buffer) >1 && NULL != strstr(buffer,target)){
                        //printf("%s\n", buffer);
			strtok(buffer,",");
			strtok(NULL,",");
			rawltv=strtok(NULL,",");
			//printf("%s\n",temp);
			rawltd=strtok(NULL,",");
			//printf("%s\n",temp);
			rawlgv=strtok(NULL,",");
                        //printf("%s\n",temp);
                        rawlgd=strtok(NULL,",");
                        //printf("%s\n",temp);
			flag=strtok(NULL,",");
			strtok(NULL,",");
			strtok(NULL,",");
			rawh=strtok(NULL,",");
			
			//printf("%s\n",flag);
			if(flag[0]=='1'){
				igd->latitude.value=atof(rawltv)/100;
				igd->latitude.direction=rawltd[0];
				igd->longitude.value=atof(rawlgv)/100;
				igd->longitude.direction=rawlgd[0];
				igd->height=atof(rawh);
				return 1;
			}

                }
	return -1;
}

/*int main() {
	gpsData gd;
	int f,g;
	g=gpsInitial();
	while(1){
		f=getGpsData(g,&gd);
		if(f>0)	printf("%f%c %f%c %f\n",gd.latitude.value,gd.latitude.direction,gd.longitude.value,gd.longitude.direction, gd.height);
	}
	//close(serialPort);
	return 0;

}*/

