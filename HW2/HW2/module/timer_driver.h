#ifndef _TIMER_DRIVER_H
#define _TIMER_DRIVER_H

typedef struct value{//처음에 입력받는 변수 3개와 init에서 숫자의 위치를 표현하기 위한 pos

    int timer_interval;
    int timer_cnt;
    int timer_init;
    int pos;
}value;

extern struct timer_list my_timer;
extern value val;
extern int mode;
extern int pos_lcd;

#define SUCCESS 0

#define SET_OPTION _IOW(DEVICE_MAJOR_NUMBER,0,value*)
#define COMMAND _IO(DEVICE_MAJOR_NUMBER,1)

#define DEVICE_NAME "dev_driver"
#define DEVICE_MAJOR_NUMBER 242
#define IOM_FPGA_DIP_SWITCH_ADDRESS 0x08000000

void timer_handler(unsigned long data);
ssize_t iom_fpga_dip_switch_read(struct file *inode, char *gdata, size_t length, loff_t *off_what);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);
static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param);

#endif