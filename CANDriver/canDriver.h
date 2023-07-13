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

int initialCan(int canID)
int receiveCan(int s,int * recData);
//void * handleData();
int sendCan(int s, int id, int length, int * data);
