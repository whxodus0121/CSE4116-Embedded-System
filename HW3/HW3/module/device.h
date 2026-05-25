#ifndef _DEVICE_H
#define _DEVICE_H


#define FND 0
#define DOT 1

#define DEVICE_NUM 2

static unsigned char *iom_fpga_addr[DEVICE_NUM];
#define IOM_FND_ADDRESS 0x08000004
#define IOM_FPGA_DOT_ADDRESS 0x08000210
//fnd
#define MAX_DIGIT 4

//dot
#define FULL 10
#define BLANK 11

void iom_fpga_init(void);
void iom_fpga_exit(void);
int fnd_device(unsigned long);
void dot_device(unsigned long);



#endif