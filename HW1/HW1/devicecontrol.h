#ifndef DEVICECONTROL_H
#define DEVICECONTROL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/input.h> 
#include <sys/ioctl.h>
#include <signal.h>
#include <string.h>
#include <stdbool.h>

//device file
#define FND_DEVICE "/dev/fpga_fnd"
#define FPGA_STEP_MOTOR_DEVICE "/dev/fpga_step_motor"
#define FPGA_PUSH_SWITCH "/dev/fpga_push_switch"
#define FPGA_TEXT_LCD_DEVICE "/dev/fpga_text_lcd"
#define LED_DEVICE "/dev/mem"
#define EVENT_DEVICE "/dev/input/event0"
#define RESET_DEVICE "/dev/fpga_dip_switch"

//fnd
#define MAX_DIGIT 4


//motor
#define MOTOR_ATTRIBUTE_ERROR_RANGE 4

//switch
#define MAX_BUTTON 9
unsigned char push_sw_buff[MAX_BUTTON];

//lcd
#define PUT 0
#define GET 1
#define MERGE 2
#define MAX_BUFF 32
#define LINE_BUFF 16

//led
#define LED_BASE_ADDR 0x114000000
#define CON_OFFSET 0x40
#define DAT_OFFSET 0x44
#define MAX_LED 8;

unsigned long *ledbaseaddr;
unsigned long *led_con;
unsigned long *led_dat;


//readkey
#define BUF_SIZE 64
#define KEY_RELEASE 0
#define KEY_PRESS 1
#define HOME 102
#define BACK 158
#define PROG 116
#define VOL_UP 115
#define VOL_DOWN 114
struct input_event ev[BUF_SIZE];


void fnd_device(int num);
void motor_device(bool check);
void switch_device();
void lcd_device(int mode_num, char *kv);
void led_device_open();
void use_led_device(int num);
void readkey_device();
int reset_device();
#endif