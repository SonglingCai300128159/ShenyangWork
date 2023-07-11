#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int main(){
    int fd = open("/dev/i2c-1", O_RDWR);
    int buff=0xfd;

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
     
    ret = ioctl(fd, I2C_RDWR, &rdwr_msg);

    char r[32]="";
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
     
    ret = ioctl(fd, I2C_RDWR, &rdwr_msg2);

    printf("%s\n",r);
    return 0;
}