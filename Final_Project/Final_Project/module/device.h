#ifndef _DEVICE_H
#define _DEVICE_H


#define FND 0
#define MOT 1
#define DOT 2
#define LCD 3
#define RESET 4

#define DEVICE_NUM 5

//device file
#define FND_DEVICE "/dev/fpga_fnd"
#define LED_DEVICE "/dev/fpga_led"
#define FPGA_DOT_DEVICE "/dev/fpga_dot"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define RESET_DEVICE "/dev/fpga_dip_switch"

static unsigned char *iom_fpga_addr[DEVICE_NUM];
#define IOM_FND_ADDRESS 0x08000004
#define IOM_LED_ADDRESS 0x08000016
#define IOM_FPGA_DOT_ADDRESS 0x08000210
#define IOM_FPGA_LCD_ADDRESS 0x08000090
#define IOM_FPGA_DIP_SWITCH_ADDRESS 0x08000000
#define IOM_FPGA_STEP_MOTOR_ADDRESS 0x0800000C
//fnd
#define MAX_DIGIT 4

//dot
#define FULL 10
#define BLANK 11

//lcd
#define MAX_BUFF 32
#define LINE_BUFF 16

void iom_fpga_init(void);
void iom_fpga_exit(void);
int fnd_device(int pos,int num);
void mot_device(int num);
void dot_device(int num);
void lcd_device(int p1,int p2);



#endif