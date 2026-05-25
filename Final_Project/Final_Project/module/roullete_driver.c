#include "all.h"

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

int turn; //차례 변수
int p1_hp,p2_hp,set_hp,total_bullet,live_bullet,blank_bullet;
int bullet_array[10]; //탄창 배열
int bullet_count; //탄 순서
int flag; //handcuffs를 위한 변수
int end_flag=0; //종료 변수

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);
static struct class *cls;

void init_device(){ //대기시 디바이스들 출력

    turn=1;
    dot_device(turn);
    lcd_device(-1,-1);
    fnd_device(0,0);
}

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

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param) { 
   
    int i,j,temp;
    struct shot shot_info;
    
    switch (ioctl_num){

    case SET_HP: //앱에서 입력받은 HP를 device에 표시
        flag=0; 
        turn=1;
 
        if (copy_from_user(&set_hp, (int*)ioctl_param, sizeof(int))) {
            return -EFAULT;
        }

        p1_hp=set_hp;
        p2_hp=set_hp;
        
        lcd_device(p1_hp,p2_hp);
        break;
    case SET_BULLET: //게임 시작시 랜덤으로 실탄과 공포탄을 장전

        bullet_count=0;
        do { //난수 생성 , 음수도 생성되기 때문에 양수가 나올 때까지 생성
            get_random_bytes(&total_bullet,sizeof(int));
        } while (total_bullet < 0);
        total_bullet=(total_bullet%5)+5; //전체 탄 개수 5~9발
        do {
            get_random_bytes(&live_bullet,sizeof(int));
        } while (live_bullet < 0);
        if(live_bullet<0) live_bullet=(-1)*live_bullet;
        live_bullet=(live_bullet%(total_bullet-1))+1; //실탄 개수 : 1~total_bullet-1 
        blank_bullet=total_bullet-live_bullet; //공포탄 

        fnd_device(live_bullet,blank_bullet);
        //탄창 배열에 순차적으로 대입
        for (i = 0; i < live_bullet; i++) {
                bullet_array[i] = 1;
            }
        for (i = live_bullet; i < total_bullet; i++) {
            bullet_array[i] = 0;
        }
        
        //배열을 랜덤하게 섞기
        for (i = total_bullet - 1; i > 0; i--) {
            do {
                get_random_bytes(&j, sizeof(int));
            } while (j < 0);
            j = j % (i + 1);
            temp = bullet_array[i];
            bullet_array[i] = bullet_array[j];
            bullet_array[j] = temp;
        }

        break;
    case SHOT: //총을 쐈을 때 변경되는 부분
      
        if (copy_from_user(&shot_info, (struct shot __user *)ioctl_param, sizeof(struct shot))) {
                return -EFAULT;
        }
        
        if(shot_info.to==0){ //me 버튼을 눌렀을 때
        
            
            if(bullet_array[bullet_count]){ //실탄이라면 

                if(turn==1) p1_hp=p1_hp-shot_info.damage;
                else p2_hp=p2_hp-shot_info.damage;
                turn=turn%2+1; //자신에게 쐈는데 실탄이면 턴이 넘어감
                flag=0;
                live_bullet--;
            }
            else blank_bullet--; //자신에게 쐈는데 공포탄이면 턴이 유지됨
             
            bullet_count++;
        } 
        else{ //enemy 버튼 눌렀을 때
         
    
            if(bullet_array[bullet_count]){//실탄이라면
                if(turn==2) p1_hp=p1_hp-shot_info.damage;
                else p2_hp=p2_hp-shot_info.damage;
                live_bullet--;
            }
            else blank_bullet--;
            
            if(!flag) { //handcuffs를 사용안했다면 턴이 넘어감
       
                turn=turn%2+1;
          
            }
            flag=0;//handcuffs를 사용했을 때를 가정해서 다시 0으로 초기화
            bullet_count++;
            
        }
        if(p1_hp<=0 || p2_hp<=0) end_flag=1; //둘 중 한명의 체력이 0이되면 end_flag=1
        lcd_device(p1_hp,p2_hp);
        fnd_device(live_bullet,blank_bullet);
        dot_device(turn);
        if(live_bullet==0&&blank_bullet<=0){ //공포탄과 실탄 모두 사용시 다시 장전

            bullet_count=0;
            do {
                get_random_bytes(&total_bullet,sizeof(int));
            } while (total_bullet < 0);
            total_bullet=(total_bullet%5)+5;
            do {
                get_random_bytes(&live_bullet,sizeof(int));
            } while (live_bullet < 0);
            live_bullet=(live_bullet%(total_bullet-1))+1;
            blank_bullet=total_bullet-live_bullet;
            fnd_device(live_bullet,blank_bullet);
            for (i = 0; i < live_bullet; i++) {
                    bullet_array[i] = 1;
                }
            for (i = live_bullet; i < total_bullet; i++) {
                bullet_array[i] = 0;
            }

            //배열을 랜덤하게 섞기
            for (i = total_bullet - 1; i > 0; i--) {
                do {
                    get_random_bytes(&j, sizeof(int));
                } while (j < 0);
                j = j % (i + 1);
                temp = bullet_array[i];
                bullet_array[i] = bullet_array[j];
                bullet_array[j] = temp;
            }
        }
        
        break;
    case MAGNIFIER: //Magnifier 사용시 실탄차례면 MOTOR 디바이스를 2초간 작동
    
        if (bullet_array[bullet_count]) {
            mot_device(1);
            msleep(2000);
            mot_device(0);
        }
        
        break;
    case RECOVERY: //Recovery 사용시 체력을 채워줌(상한 10)


        if(turn==1&&p1_hp<=10) p1_hp++;
        else if(turn==2 && p2_hp<=10)p2_hp++;
        lcd_device(p1_hp,p2_hp);

        break;
    case HANDCUFFS: //Handcuffs 사용시 flag=1
        flag=1;
        break;
    case SET: //대기화면 
        init_device();
        end_flag=0;
        break;
    case GET_END_FLAG: //앱에서 end_flag를 읽어서 end_flag=1이면 종료시키고 대기화면으로 전환
        if (copy_to_user((int*)ioctl_param, &end_flag, sizeof(int))) {
            return -EFAULT;
        }

        break;
    default:
        return -EINVAL;
    }
    
    return SUCCESS;
}

static struct file_operations dev_driver_fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl,
};

static int __init dev_driver_init(void){
    int register_result = register_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME, &dev_driver_fops);
    
    if (register_result < 0) {
        pr_alert("Registering dev_device failed with %d\n", register_result);
        return register_result;
    }
  
    iom_fpga_init();
    init_device();

    pr_info("Assigned major number : %d.\n", DEVICE_MAJOR_NUMBER);
    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(DEVICE_MAJOR_NUMBER, 0), NULL, DEVICE_NAME);
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return SUCCESS;
}

static void __exit dev_driver_exit(void){
    

    device_destroy(cls, MKDEV(DEVICE_MAJOR_NUMBER, 0));
    class_destroy(cls);
    
    iom_fpga_exit();

    unregister_chrdev(DEVICE_MAJOR_NUMBER, DEVICE_NAME);
    pr_info("Exit device on /dev/%s\n", DEVICE_NAME);
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);
MODULE_LICENSE("GPL");