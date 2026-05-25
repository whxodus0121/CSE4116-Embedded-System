#ifndef _TIMER_DRIVER_H
#define _TIMER_DRIVER_H

typedef enum MODE_ {   //stopwatch의 현재상태 표현
    READY,
    RUN,
    PAUSE,
}MODE;

#define SUCCESS 0


#define DEVICE_NAME "stopwatch"
#define DEVICE_MAJOR_NUMBER 242

static void my_wq_device_function(struct work_struct *work);
static void my_wq_end_function(struct work_struct *work);
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg);
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg);
void inter_open(void);
void inter_release(void);
void timer_handler(unsigned long data);
static int device_open(struct inode *inode, struct file *file);
static int device_release(struct inode *inode, struct file *file);



#endif