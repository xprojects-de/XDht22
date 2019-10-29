#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <bcm2835.h>

#define TRUE 1
#define FALSE 0
#define XDEBUG FALSE
#define OK 0
#define ERROR 1
#define MAXTIMINGS 100
#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */
#define DHT11 11
#define DHT22 22
#define AM2302 22

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0') 

#define CALIBITEMSLENGTH 20
#define CALIBITEMSSTARTINDEX -10
#define CALIBITEMSENDTINDEX 30
#define CALIBITEMSSTEP 2

typedef struct {
    float start;
    float stop;
    float factor;
} calibItem;

calibItem calibItems[CALIBITEMSLENGTH];

int readDHT(int type, int pin);
float getTemp(float temp);
void initCalibItems(int argc, char** argv);
int startsWith(const char *a, const char *b);
void printCalibs();

#endif /* GLOBAL_H */

