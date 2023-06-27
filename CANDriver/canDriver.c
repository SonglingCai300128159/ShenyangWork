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

void * receiveCan(){
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
	rfilter[0].can_id=0x011;
	rfilter[0].can_mask=CAN_SFF_MASK;
	setsockopt(s,SOL_CAN_RAW,CAN_RAW_FILTER,&rfilter,sizeof(rfilter));
	while(1)
	{
		nbytes = read(s, &frame, sizeof(frame));
		if(nbytes>0){
			pthread_mutex_lock(&mutex);
			flag=1;
			//printf("%x",frame.data[0]);
			for(int i=0;i<frame.can_dlc;i++){
				data[i]=frame.data[i];
				//printf("%x",data[i]);
			}
			pthread_mutex_unlock(&mutex);
		}
		/*if(nbytes > 0)
		{
			printf("ID=0x%X DLC=%d data[0]=0x%X\n", frame.can_id,frame.can_dlc, frame.data[0]);
		}*/
	}
	close(s);
	return 0;
}

void * handleData(){
	while(1){
		if(flag==1){
			pthread_mutex_lock(&mutex);
			printf("%x\n",data[0]);
			flag=0;
			pthread_mutex_unlock(&mutex);
		}
	}
	return 0;
}

int sendCan(int id, int length,int * data){
	int s, nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame[2] = {{0}};
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);//创建套接字
	strcpy(ifr.ifr_name, "can0" );
	ioctl(s, SIOCGIFINDEX, &ifr); //指定 can0 设备
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	bind(s, (struct sockaddr *)&addr, sizeof(addr));
	frame[0].can_id = id;
	frame[0]. can_dlc = length;
	for(int i=0;i<length;i++)
		frame[0].data[i]=data[i];
	nbytes = write(s, &frame[0], sizeof(frame[0]));
	close(s);
	return 1;
}

int main(){
	pthread_t threads[3];
	int ret;
	int d[8];
	d[0]=1;
	d[1]=2;
	d[2]=3;
	d[3]=4;
	ret=pthread_create(&threads[0],NULL,receiveCan,NULL);
	ret=pthread_create(&threads[1],NULL,handleData,NULL);
	sendCan(1,4,d);
	while(1){

	}
	return 0;
}

