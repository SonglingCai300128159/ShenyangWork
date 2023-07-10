#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <pthread.h>


pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
int data[8];
int flag=0;

int initialCan(int canID){
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
        struct can_frame frame;
	struct can_filter rfilter[1];
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW); //创建套接字
	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr); //指定 can0 设备
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr *)&addr, sizeof(addr)); //将套接字与 can0 绑定
	rfilter[0].can_id=canID;
	rfilter[0].can_mask=CAN_SFF_MASK;
	setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,&rfilter,sizeof(rfilter));
	return s;
}

void * receiveCan(int s){
	while(1)
	{
		nbytes = read(s, &frame, sizeof(frame));
		if(nbytes>0){
			//pthread_mutex_lock(&mutex);
			flag=1;
			//printf("%x",frame.data[0]);
			for(int i=0;i<frame.can_dlc;i++){
				data[i]=frame.data[i];
				//printf("%x",data[i]);
			}
			sendCan(s,0x0A,8,data);//转发数据
			//pthread_mutex_unlock(&mutex);
		}
		/*if(nbytes > 0)
		{
			printf("ID=0x%X DLC=%d data[0]=0x%X\n", frame.can_id,frame.can_dlc, frame.data[0]);
		}*/
	}
	close(s);
	return 0;
}

/*int handleData(int s){
	while(1){
		if(flag==1){
			pthread_mutex_lock(&mutex);
			printf("%x\n",data[0]);
			flag=0;
			pthread_mutex_unlock(&mutex);
		}
	}
	return 1;
}*/

int sendCan(int s,int id, int length,int * data){
	frame[0].can_id = id;
	frame[0].can_dlc = length;
	for(int i=0;i<length;i++)
		frame[0].data[i]=data[i];
	nbytes = write(s, &frame[0], sizeof(frame[0]));
	return 1;
}

int dataTransferAndSend(int s,int * data){
	switch(data[0]){
		case 0x11:
			straightMotion(s, data);
			break;
		case 0x22:
			
		

	}
}

int straightMotion(int s, int * data){
	int setSpeed[8]={0x11,0x11,data[2],data[3],data[4],data[5],data[6],(0x11+0x11+data[2]+data[3]+data[4]+data[5]+data[6])&0xff};
	sendCan(s,0x0A,8,setSpeed);
	if(data[1]==0xAA)
		int setDirection[8]={0x11,0x22,data[2],0x55,0x00,0x00,0x00,(0x11+0x22+data[2]+data[1]+0x00+0x00+0x00)&0xff};
	else if(data[1]==0xBB)
		int setDirection[8]={0x11,0x22,data[2],0xAA,0x00,0x00,0x00,(0x11+0x22+data[2]+data[1]+0x00+0x00+0x00)&0xff};
	else
		int setDirection[8]={0x11,0x22,data[2],0xFF,0x00,0x00,0x00,(0x11+0x22+data[2]+data[1]+0x00+0x00+0x00)&0xff};
	sendCan(s,0x0A,8,setDirection);
	return 1;
}

int main(){
	pthread_t threads[3];
	int ret;
	int s=initialCan(0x011);
	ret=pthread_create(&threads[0],NULL,receiveCan,&s);
	while(1){

	}
	return 0;
}

