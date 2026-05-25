#include "devicecontrol.h"

unsigned char val[9]={0x00,0xF0,0x80,0x40,0x10,0x0F,0x08,0x04,0x01};
char *mode[3]={"PUT MODE","GET MODE","MERGE MODE"};

void fnd_device(int num){

    int dev,tmp=1,i;
	unsigned char data[MAX_DIGIT] = {0};
	unsigned char retval;

    dev = open(FND_DEVICE, O_RDWR);
    if (dev<0) {
        printf("Device open error : %s\n",FND_DEVICE);
        close(dev);
		return;
    }

    //각 자리 숫자로 분리하여 data에 넣기
    for (i = MAX_DIGIT - 1; i >= 0; i--) {
        
        data[i] = (num / tmp) % 10;
        tmp *= 10;
    }

    
    if ((retval = write(dev, data, sizeof(data))) < 0){
        
        printf("Write Error!\n");
    }
    close(dev);
    return;
}

void motor_device(bool check){

    int dev;
    unsigned char retval;
    unsigned char motor_state[3]={0};

    dev = open(FPGA_STEP_MOTOR_DEVICE, O_WRONLY);
	if (dev<0) {
		printf("Device open error : %s\n",FPGA_STEP_MOTOR_DEVICE);
		return;
	}
    //motor_state[0]은 on/off motor_state[1]은 direction motor_state[2]는 speed
    if(check) motor_state[0]=1;
    motor_state[2]=5;

    if((retval = write(dev, motor_state, sizeof(motor_state))) < 0) {
        printf("motor write error\n");
    }

    close(dev);
	return;
}

void switch_device(){

    int dev;
    unsigned char retval;
    
    dev = open(FPGA_PUSH_SWITCH, O_RDWR);

	if (dev<0){
		printf("Device open error : %s\n",FPGA_PUSH_SWITCH);
		return;
	}

    //키 입력 받아오기
    if((retval = read(dev, &push_sw_buff, sizeof(push_sw_buff))) < 0){
        printf("key read error\n");
    }
    close(dev);
    return;
}

void lcd_device(int mode_num, char *kv){

    int dev,data_size;
    unsigned char retval;
    char data[MAX_BUFF];
    int len= strlen(kv);

    dev = open(FPGA_TEXT_LCD_DEVICE, O_WRONLY);
	if (dev<0) {
		printf("Device open error : %s\n",FPGA_TEXT_LCD_DEVICE);
		return;
	}

    if(len>MAX_BUFF){

        printf("32 alphanumeric characters on a data!\n");
		len=MAX_BUFF;
    }
    //처음엔 mode 입력
    if(mode_num<0){
        data_size=0;
        strncat(data," ",data_size);
        memset(data+data_size,' ',LINE_BUFF-data_size);
    }
    else{
        data_size=strlen(mode[mode_num]);
        if(data_size>0) {
            strncat(data,mode[mode_num],data_size);
            memset(data+data_size,' ',LINE_BUFF-data_size);
        }
    }
    

    //앞에서 16글자 채우고 나머지를 key,value로 채우기
	data_size=strlen(kv);
	if(data_size>0) {
		strncat(data,kv,data_size);
		memset(data+LINE_BUFF+data_size,' ',LINE_BUFF-data_size);
	}
    else{
        data_size=0;
        strncat(data,"\0",data_size);
        memset(data+LINE_BUFF+data_size,' ',LINE_BUFF-data_size);
    }

    write(dev,data,MAX_BUFF);

    close(dev);
    return;
}

void led_device_open(){

    int dev;

    dev = open(LED_DEVICE, O_RDWR|O_SYNC);
    if (dev<0){
		printf("Device open error : %s\n",LED_DEVICE);
		return;
	}

    ledbaseaddr = (unsigned long *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, dev, (off_t)LED_BASE_ADDR);

    led_con=(unsigned long*)(ledbaseaddr+CON_OFFSET);
    led_dat=(unsigned long*)(ledbaseaddr+DAT_OFFSET);

    *led_con |= 0x11111111;

    return;
}

void use_led_device(int num){

    *led_dat=val[num];
}

void readkey_device(){

    int dev;
    unsigned char retval;
    
    dev = open(EVENT_DEVICE, O_RDONLY|O_NONBLOCK);

	if (dev<0){
		printf("%s is not a valid device\n",EVENT_DEVICE);
		return;
	}
    //키 입력 받아오기
    if((retval = read(dev, &ev, sizeof(ev))) < 0){
        printf("key read error\n");
    }

    close(dev);
    return;
}

int reset_device(){


    int dev;
    unsigned char dip_sw_buff=0;

    dev = open(RESET_DEVICE, O_RDWR);

	if (dev<0){
		printf("Device open error : %s\n",RESET_DEVICE);
		return -1;
	}

    read(dev,&dip_sw_buff,1);
    close(dev);
    return dip_sw_buff;
}
