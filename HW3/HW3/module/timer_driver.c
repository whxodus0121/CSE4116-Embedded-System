#include "all.h"

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

static struct timer_list my_timer;
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static struct class *cls;

//종료를 위해 vol_down버튼이 눌렸는지를 체크하는 변수들
int pause;
int check_voldown_press;

//시간 표현하기 위한 변수들
unsigned long current_time; //현재 시간
unsigned long print_time; //출력할 시간
unsigned long voldown_time; //voldown이 눌린 시간
unsigned long pause_time; //pause가 눌린 시간
unsigned long acc_pause_time; //pause가 쌓인 시간

//stopwatch의 현재상태
MODE mode;

//응용프로그램을 sleep상태로 만들기 위한 waitq
wait_queue_head_t waitq;
DECLARE_WAIT_QUEUE_HEAD(waitq);
//workqueue 생성을 위한 my_wq
static struct workqueue_struct *my_wq;

static int device_open(struct inode *inode, struct file *file){ 

    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

    printk(KERN_INFO "Device open\n");
   
    init_timer(&my_timer);
    pause=1;
    inter_open();
    acc_pause_time=0;
    current_time=0;
    mode=READY;

    try_module_get(THIS_MODULE);
    return SUCCESS;
}


static int device_release(struct inode *inode, struct file *file){

    printk(KERN_INFO "Device release\n");
    
    fnd_device(0);
    dot_device(-1);

    inter_release();

    atomic_set(&already_open, CDEV_NOT_USED);
    module_put(THIS_MODULE);
    return SUCCESS;
}
//응용 프로그램을 waitq에서 sleep 시키기 위해 쓰임
static int device_write(struct file *filp, const char __user *buff, size_t len, loff_t *off){

    interruptible_sleep_on(&waitq);
    return SUCCESS;
}
//GPIO 핀에 대한 interrupt 설정
void inter_open(void){
	int ret;
	int irq;

	printk(KERN_ALERT "Open Module\n");


    gpio_direction_input(IMX_GPIO_NR(1, 11));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 11));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, (irq_handler_t)inter_handler_home, IRQF_TRIGGER_FALLING, "home", NULL);
    if (ret) {
        printk(KERN_ALERT "Unable to request IRQ for home: %d\n", ret);
        return;
    }

    gpio_direction_input(IMX_GPIO_NR(1, 12));
    irq = gpio_to_irq(IMX_GPIO_NR(1, 12));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, (irq_handler_t)inter_handler_back, IRQF_TRIGGER_FALLING, "back", NULL);
    if (ret) {
        printk(KERN_ALERT "Unable to request IRQ for back: %d\n", ret);
        return;
    }


    gpio_direction_input(IMX_GPIO_NR(2, 15));
    irq = gpio_to_irq(IMX_GPIO_NR(2, 15));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, (irq_handler_t)inter_handler_volup, IRQF_TRIGGER_FALLING, "volup", NULL);
    if (ret) {
        printk(KERN_ALERT "Unable to request IRQ for volup: %d\n", ret);
        return;
    }

    gpio_direction_input(IMX_GPIO_NR(5, 14));
    irq = gpio_to_irq(IMX_GPIO_NR(5, 14));
    printk(KERN_ALERT "IRQ Number : %d\n", irq);
    ret = request_irq(irq, (irq_handler_t)inter_handler_voldown, IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING, "voldown", NULL);
    if (ret) {
        printk(KERN_ALERT "Unable to request IRQ for voldown: %d\n", ret);
        return;
    }

}
//interrupt 설정 해제
void inter_release(void){

	free_irq(gpio_to_irq(IMX_GPIO_NR(1, 11)), NULL);
    
    free_irq(gpio_to_irq(IMX_GPIO_NR(1, 12)), NULL);
   
    free_irq(gpio_to_irq(IMX_GPIO_NR(2, 15)), NULL);

    free_irq(gpio_to_irq(IMX_GPIO_NR(5, 14)), NULL);

	
	printk(KERN_ALERT "Release Module\n");
}
//home버튼(start)눌렸을 때 동작 READY상태일때만 동작(최초상태거나 volup버튼(RESET) 누르면 READY상태 )
irqreturn_t inter_handler_home(int irq, void* dev_id, struct pt_regs* reg){

    if(mode!=READY) return 0;
    mode=RUN;
    //del_timer(&my_timer);
    my_timer.expires = get_jiffies_64(); 
    my_timer.function = timer_handler;
    add_timer(&my_timer);
    return IRQ_HANDLED;
}
//back버튼(pause)눌렀을 때 동작 RUN과 PAUSE상태일때 동작하며 상태를 바꿔가며 동작을 수행
irqreturn_t inter_handler_back(int irq, void* dev_id, struct pt_regs* reg){

    if(mode==READY) return 0;

    else if(mode==PAUSE){ //pause 되어 있던 시간 저장 하고 다시 동작
        acc_pause_time+=current_time-pause_time;
        mode=RUN;
        my_timer.expires=get_jiffies_64()+HZ/10;
        my_timer.function=timer_handler;
        add_timer(&my_timer);
    }
    else if(mode==RUN){ //pause 된 시간 저장하고 일시정지
        mode=PAUSE;
        pause_time=current_time;
        del_timer(&my_timer);
    }
    return IRQ_HANDLED;
}
//volup버튼(reset) 눌렀을 때 동작 모든 시간값 초기화하고 my_wq에 디바이스들(fnd,dot) 초기화하는 작업 추가
irqreturn_t inter_handler_volup(int irq, void* dev_id, struct pt_regs* reg){
    if(mode==READY) return 0;
    current_time=0;
    acc_pause_time=0;
    voldown_time=0;
    print_time=0;
    mode=READY;
    struct work_struct *device_work = (struct work_struct *)kmalloc(sizeof(struct work_struct), GFP_KERNEL);
    if (device_work) {
        INIT_WORK((struct work_struct *)device_work, my_wq_device_function);
        queue_work(my_wq, (struct work_struct *)device_work);
    }

    del_timer(&my_timer);
    return IRQ_HANDLED;
}
//voldown버튼(stop) 버튼이 눌렸는지 안눌렸는지를 체크해서 pause와 check_voldown_press값을 설정
irqreturn_t inter_handler_voldown(int irq, void* dev_id, struct pt_regs* reg){

    pause=gpio_get_value(IMX_GPIO_NR(5, 14));
   
    if(pause!=1){
        //버튼이 눌리면 voldown_time을 현재시간으로 설정
        check_voldown_press=0;
        voldown_time=current_time;
    }
    else check_voldown_press=1;
    
   
    return IRQ_HANDLED;
}


//workqueue에 등록될 함수 fnd와 dot 출력하는 역할
static void my_wq_device_function(struct work_struct *work){

    fnd_device(print_time);
    dot_device(print_time);
 
}
//workqueue에 등록될 함수 프로세스를 깨우고 stopwatch를 종료시키는 역할
static void my_wq_end_function(struct work_struct *work){
    del_timer(&my_timer);
    wake_up_interruptible(&waitq);
}

void timer_handler(unsigned long data){ //timer_handler

    current_time++;

    print_time=current_time-acc_pause_time;
    if(mode==PAUSE) acc_pause_time++;

    //fnd,dot출력 작업 workqueue 이용
    struct work_struct *device_work = (struct work_struct *)kmalloc(sizeof(struct work_struct), GFP_KERNEL);
    if (device_work) {
        INIT_WORK((struct work_struct *)device_work, my_wq_device_function);
        queue_work(my_wq, (struct work_struct *)device_work);
    }
    //버튼눌린지 3초가 되면
    if(!pause&&!check_voldown_press&& current_time-voldown_time>=30){

        //종료 작업 workqueue이용
        struct work_struct *end_work=(struct work_struct*)kmalloc(sizeof(struct work_struct),GFP_KERNEL);
        if(end_work){
            INIT_WORK((struct work_struct *)end_work, my_wq_end_function);
            queue_work(my_wq, (struct work_struct *)end_work);
        }
    }

    //0.1초마다 갱신
    my_timer.expires=get_jiffies_64()+HZ/10;
    my_timer.function=timer_handler;
    add_timer(&my_timer);
    
}


static struct file_operations dev_driver_fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .write=device_write,
};

static int __init dev_driver_init(void){
   
   int register_result = register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &dev_driver_fops);
    
    if (register_result < 0) {
        pr_alert("Registering dev_device failed with %d\n", register_result);
        return register_result;
    }
   
    my_wq=create_workqueue("my_queue");
    iom_fpga_init();
    
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
    inter_release();

    device_destroy(cls, MKDEV(DEVICE_MAJOR_NUMBER, 0));
    class_destroy(cls);
    
    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    pr_info("Exit device on /dev/%s\n", DEVICE_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("GPL");
