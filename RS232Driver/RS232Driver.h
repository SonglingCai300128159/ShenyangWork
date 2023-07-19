#ifndef _RS232DRIVER_H
#define _RS232DRIVER_H
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>

int RS232SInitial(char * addr,int vmin);
#endif
