#ifndef __SHT40DRIVER_H
#define __SHT40DRIVER_H

enum sht40Command
{
        c0xFD=1,c0xF6,c0xE0,c0x89,c0x94,c0x39,c0x32,c0x2F,c0x24,c0x1E,c0x15
};


int getSht40Data(enum sht40Command c,double * result);

int measureT_RH_HighPrecision(double * r);
int measureT_RH_MediumPrecision(double * r);
int measureT_RH_LowestPrecision(double * r);

int readSerial(double * r);

int softReset();

int heatAndMeasure1s(double * r);
int heatAndMeasure0_1(double * r);

int getDataFromShell(char * command, char * data);

int calT_RH(char * rawData,double * result);

int calSerial(char * rawData, double * result);

double hexToDec(char c);

#endif