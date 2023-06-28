#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "sht40Driver.h"

int getSht40Data(enum sht40Command c,double * result)
{
        switch(c){
                case c0xFD:
                        return measureT_RH_HighPrecision(result);
                case c0xF6:
                        return measureT_RH_MediumPrecision(result);
                case c0xE0:
                        return measureT_RH_LowestPrecision(result);
                case c0x89:
                        return readSerial(result);
                case c0x94:
                        return softReset();
                case c0x39:
                        return heatAndMeasure1s(result);
                case c0x32:
                        return heatAndMeasure0_1(result);
                case c0x2F:
                        return heatAndMeasure1s(result);
                case c0x24:
                        return heatAndMeasure0_1(result);
                case c0x1E:
                        return heatAndMeasure1s(result);
                case c0x15:
                        return heatAndMeasure0_1(result);
                default:
                        printf("Command not support \n");
                        return 0;
        }

}

int measureT_RH_HighPrecision(double * r){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0xfd;sleep 0.01;sudo i22
ctransfer -f -y 1 r6@0x44";
        char data[32]="" ;
        getDataFromShell(command,data);
        if(strlen(data)!=0){
                return calT_RH(data, r);
        }else{
                return 0;
        }
}

int measureT_RH_MediumPrecision(double * r){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0xF6;sleep 0.01;sudo i22
ctransfer -f -y 1 r6@0x44";
        char data[32]="";
        getDataFromShell(command,data);
        if(strlen(data)!=0){
                return calT_RH(data, r);
        }else{
                return 0;
        }

}

int measureT_RH_LowestPrecision(double * r){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0xfd;sleep 0.01;sudo i22
ctransfer -f -y 1 r6@0x44";
        char data[32]="";
        getDataFromShell(command,data);
        if(strlen(data)!=0){
                return calT_RH(data, r);
        }else{
                return 0;
        }

}

int readSerial(double * r){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0x89;sleep 0.001;sudo ii
2ctransfer -f -y 1 r6@0x44";
        char data[32]= "";
        getDataFromShell(command,data);
        if(strlen(data)!=0){
                return calSerial(data, r);
        }else{
                return 0;
        }

}

int softReset(){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0x94;";
        char data[32]="";
        getDataFromShell(command,data);
        return 1;
}

int heatAndMeasure1s(double * r){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0x39;sleep 1;sudo i2ctrr
ansfer -f -y 1 r6@0x44";
        char data[32]="";
        getDataFromShell(command,data);
        if(strlen(data)!=0){
                return calT_RH(data, r);
        }else{
                return 0;
        }

}

int heatAndMeasure0_1(double * r){
        char command[]="sudo i2ctransfer -f -y 1 w1@0x44 0x32;sleep 0.1;sudo i2cc
transfer -f -y 1 r6@0x44";
        char data[32]= "";
        getDataFromShell(command,data);
        if(strlen(data)!=0){
                return calT_RH(data, r);
        }else{
                return 0;
        }

}

int getDataFromShell(char * command, char * d){
        FILE *ptr;
        //char result[32]="";
        char buf_ps[32]="";
        ptr=popen(command,"r");
        if(ptr!=NULL){
                while(fgets(buf_ps,32,ptr)!=NULL){
                        strcat(d,buf_ps);
                        if(strlen(d)>32) break;
                }
                //        printf("TMP and RH is %s \n",result);
                pclose(ptr);
                ptr=NULL;
                //printf("%s",d);
                return 1;
        }else{
                printf("popen %s error/n",command);
                return 0;
        }

}

int calT_RH(char * rawData,double * result){
        double t,st;
        double rh,srh;

        st=hexToDec(rawData[2])*16*16*16+hexToDec(rawData[3])*16*16+hexToDec(raww
Data[7])*16+hexToDec(rawData[8]);
        t=175*(st/(pow(2,16)-1))-45;

        srh=hexToDec(rawData[17])*16*16*16+hexToDec(rawData[18])*16*16+hexToDec((
rawData[22])*16+hexToDec(rawData[23]);
        rh=125*(srh/(pow(2,16)-1))-6;

        result[0]=t;
        result[1]=rh;
        return 1;
}

int calSerial(char * rawData, double * result){
        return 0;
}

double hexToDec(char c){
        switch(c){
                case '0':
                        return 0;
                case '1':
                        return 1.0;
                case '2':
                        return 2.0;
                case '3':
                        return 3.0;
                case '4':
                        return 4.0;
                case '5':
                        return 5.0;
                case '6':
                        return 6.0;
                case '7':
                        return 7.0;
                case '8':
                        return 8.0;
                case '9':
                        return 9.0;
                case 'a':
                        return 10.0;
                case 'b':
                        return 11.0;
                case 'c':
                        return 12.0;
                case 'd':
                        return 13.0;
                case 'e':
                        return 14.0;
                case 'f':
                        return 15.0;
                default:
                        return 0;
        }
}

/*int main(int argc, char**argv){
                enum sht40Command c;
                c=c0xFD;
                double r[2];
                getData(c,r);
                printf("%f %f",r[0],r[1]);
                //sleep(1000);
        return 0;
}*/
