#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

typedef struct{
        float value;
        char direction;
}gpsDirection;

typedef struct{
        gpsDirection longitude;
        gpsDirection latitude;
        float height;
}gpsData;

int gpsInitial();
int getGpsData(int serialPort, gpsData *);


