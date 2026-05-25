#include "all.h"

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

struct timer_list my_timer;
value val;
int count; 
int mode;
int pos_lcd;

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static unsigned char *iom_fpga_dip_switch_addr;
static struct class *cls;

static int device_open(struct inode *inode, struct file *file){ 

    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    printk(KERN_INFO "Device open\n");

    try_module_get(THIS_MODULE);
    return SUCCESS;
}


static int device_release(struct inode *inode, struct file *file){

    printk(KERN_INFO "Device release\n");
    
    atomic_set(&already_open, CDEV_NOT_USED);
    
    module_put(THIS_MODULE);
    return SUCCESS;
}

void timer_handler(unsigned long data){ //timer_handler

    int *cnt=(int*)data;
    int current_pos,num;
    int temp;
    temp=val.timer_init;
    //timer_cnt가 0이 됐을때부터의 종료 시퀸스 expires를 1초로 설정하고 출력해야 할 문구에서 숫자부분만 3 2 1로 바꾼 후 화면 모두 빈칸
    if(*cnt==val.timer_cnt){
        
        fnd_device(0,0);
        led_device(0);
        dot_device(-1);
        lcd_device(-4);
        (*cnt)++;
        my_timer.expires=get_jiffies_64()+10*HZ/10;
        my_timer.data=(unsigned long)cnt;
        my_timer.function=timer_handler;
        add_timer(&my_timer);
    }
    else if(*cnt==val.timer_cnt+1){

        lcd_device(-3);
        (*cnt)++;
        my_timer.expires=get_jiffies_64()+10*HZ/10;
        my_timer.data=(unsigned long)cnt;
        my_timer.function=timer_handler;
        add_timer(&my_timer);
    }
    else if(*cnt==val.timer_cnt+2){

        lcd_device(-2);
        (*cnt)++;
        my_timer.expires=get_jiffies_64()+10*HZ/10;
        my_timer.data=(unsigned long)cnt;
        my_timer.function=timer_handler;
        add_timer(&my_timer);
    }
    else if(*cnt==val.timer_cnt+3){

        lcd_device(-5);
        return 0;
    }
    else{ //동작중일때
        //처음에 init에서 0이 아닌 숫자가 무엇이 입력되었는지 체크
        while(temp>10){
            temp/=10;
        }
        //몇번 timer가 지나갔는지를 이용하여 현재 위치와 표시해야할 숫자 계산
        current_pos=(val.pos+(*cnt/8))%4;
        num = (temp+*cnt)%8;
        if(num==0) num=8;
        //device에 출력
        fnd_device(current_pos,num);
        led_device(num);
        dot_device(num);
        lcd_device(*cnt);
        //timer 세팅
        (*cnt)++;
        my_timer.expires=get_jiffies_64()+val.timer_interval*HZ/10;
        my_timer.data=(unsigned long)cnt;
        my_timer.function=timer_handler;
        add_timer(&my_timer);
    }
}

ssize_t iom_fpga_dip_switch_read(struct file *inode, char *gdata, size_t length, loff_t *off_what) { //별도로 reset버튼 입력만을 확인하기 위한 함수

    unsigned char dip_sw_value;    
    unsigned short _s_dip_sw_value;  
    
    if (!iom_fpga_dip_switch_addr ) {
        pr_err("iom_fpga_addr[RESET] is not mapped\n");
        return 0;
    }
    
    _s_dip_sw_value = inw((unsigned int)iom_fpga_dip_switch_addr);
   
    dip_sw_value = _s_dip_sw_value & 0xFF;
    
    if (copy_to_user(gdata, &dip_sw_value, 1))
        return -EFAULT;

    return length;    
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) { //app에서 ioctl이용시 사용되는 함수
   
    int temp,current_pos,num;

    if(ioctl_num==COMMAND){ //timer시작

        my_timer.expires=get_jiffies_64();
        my_timer.data = (unsigned long)&count;
        my_timer.function=timer_handler;
        add_timer(&my_timer);
    }
    else if (ioctl_num==SET_OPTION){ //입력값 받아와서 화면에 입력 값 출력하고 timer 초기화  

        copy_from_user(&val, (value*)ioctl_param, sizeof(value));
        init_timer(&my_timer);
        mode=1;
        pos_lcd=15;
        count=0;

        temp=val.timer_init;
        while(temp>10){
            temp/=10;
        }

        num = (temp)%8;
        if(num==0) num=8;

        fnd_device(val.pos,num);
        led_device(num);
        dot_device(num);
        lcd_device(-1);
    }
    else{

        pr_alert("Enter the correct ioctl_num\n");
    }
    
    return SUCCESS;
}

static struct file_operations dev_driver_fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .read = iom_fpga_dip_switch_read,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

static int __init dev_driver_init(void){
    int register_result = register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &dev_driver_fops);
    
    if (register_result < 0) {
        pr_alert("Registering dev_device failed with %d\n", register_result);
        return register_result;
    }
    //디바이스의 물리주소를 매핑//
    iom_fpga_init();
    iom_fpga_dip_switch_addr = ioremap(IOM_FPGA_DIP_SWITCH_ADDRESS, 0x1);
    
    pr_info("Assigned major number : %d.\n", DEVICE_MAJOR_NUMBER);
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(DEVICE_MAJOR_NUMBER, 0), NULL, DEVICE_NAME);
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return SUCCESS;
}

static void __exit dev_driver_exit(void){
    
    //해제 후 timer삭제
    iom_fpga_exit();
    del_timer(&my_timer);

    device_destroy(cls, MKDEV(DEVICE_MAJOR_NUMBER, 0));
    class_destroy(cls);
    
    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    pr_info("Exit device on /dev/%s\n", DEVICE_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("GPL");