#include "all.h"


unsigned char fpga_number[10][10]={
    {0x3e,0x7f,0x63,0x73,0x73,0x6f,0x67,0x63,0x7f,0x3e},
    {0x0c,0x1c,0x1c,0x0c,0x0c,0x0c,0x0c,0x0c,0x0c,0x1e},
    {0x7e,0x7f,0x03,0x03,0x3f,0x7e,0x60,0x60,0x7f,0x7f},
    {0xfe,0x7f,0x03,0x03,0x7f,0x7f,0x03,0x03,0x7f,0x7e},
    {0x66,0x66,0x66,0x66,0x66,0x66,0x7f,0x7f,0x06,0x06},
    {0x7f,0x7f,0x60,0x60,0x7e,0x7f,0x03,0x03,0x7f,0x7e},
    {0x60,0x60,0x60,0x60,0x7e,0x7f,0x63,0x63,0x7f,0x3e},
    {0x7f,0x7f,0x63,0x63,0x03,0x03,0x03,0x03,0x03,0x03},
    {0x3e,0x7f,0x63,0x63,0x7f,0x7f,0x63,0x63,0x7f,0x3e},
    {0x3e,0x7f,0x63,0x63,0x7f,0x3f,0x03,0x03,0x03,0x03}
};

unsigned char fpga_set_blank[10]={
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

void iom_fpga_init(void){ //디바이스의 물리 주소를 mapping
    
    iom_fpga_addr[FND]=ioremap(IOM_FND_ADDRESS, 0x4);
    iom_fpga_addr[LED]=ioremap(IOM_LED_ADDRESS, 0x1);
    iom_fpga_addr[DOT]=ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
    iom_fpga_addr[LCD]=ioremap(IOM_FPGA_LCD_ADDRESS, 0x32);
}

void iom_fpga_exit(void){//해제

    int i;
    for(i=0;i<DEVICE_NUM;i++) iounmap(iom_fpga_addr[i]);
}

int fnd_device(int pos,int num){ //pos와 num를 이용해서 fnd에 값 출력 

    unsigned char value[4]={0,};
    unsigned short int value_short=0;

    value[pos]=num;
    value_short=value[0] << 12 | value[1] << 8 |value[2] << 4 |value[3];
    outw(value_short,(unsigned int)iom_fpga_addr[FND]);

    return 0;
}

void led_device(int num){ //num을 led에 출력

	unsigned short _s_value=0;
    
    _s_value=1<<(8-num);
    if(num==0) _s_value=0;
    outw(_s_value,(unsigned int)iom_fpga_addr[LED]);
}

void dot_device(int num){ //num을 dot에 출력 만약 -1입력시 빈칸 출력

    int i=0;
    unsigned short _s_value;
    if(num==-1){

        for(i=0;i<10;i++){
            _s_value=fpga_set_blank[i] & 0x7F;
            outw(_s_value,(unsigned int)iom_fpga_addr[DOT]+i*2);
        }

        return;
    }
    for(i=0;i<10;i++){
        _s_value=fpga_number[num][i] & 0x7F;
        outw(_s_value,(unsigned int)iom_fpga_addr[DOT]+i*2);
    }
    return;
}

void lcd_device(int cnt){ //timer가 진행된 횟수 cnt를 입력받아 lcd에 출력
    
    unsigned short int _s_value = 0;
    unsigned char output[MAX_BUFF+1] = {'\0'};
    int i, timer_cnt_len = 0;
    int temp = val.timer_cnt;
    

    
    while (temp > 0) { //출력해야할 timer_cnt가 몇자리수인지 계산
        temp /= 10;
        timer_cnt_len++;
    } 
    if (cnt == -5) { //-5입력시 빈칸으로 초기화
        memset(output, ' ', MAX_BUFF);
    } else if (cnt <= -2 && cnt >= -4) { //-4 ~ -2 입력시 종료 후 문구 출력
        
        memset(output, ' ', MAX_BUFF);
        strncpy(output, "Time's up!     0", LINE_BUFF);
        cnt=(-1)*cnt-1;
        snprintf(output + 16, MAX_BUFF - 15, "Shutdown in %d...",cnt);
    } else { //timer 동작시 문구 출력
        //첫번째줄 출력 학번 입력 후 오른쪽 정렬하여 남은 timer_cnt출력
        memset(output, ' ', MAX_BUFF);
        strncpy(output, "20181688", MAX_BUFF);
        if(cnt==-1) snprintf(output + 8, MAX_BUFF - 8, "%*d", 16 - 8, val.timer_cnt );
        else snprintf(output + 8, MAX_BUFF - 8, "%*d", 16 - 8, val.timer_cnt - cnt);
        
        //두번째줄 출력 pos_lcd와 mode를 이용하여 이니셜의 처음위치와 현재 상황이 오른쪽으로 진행해야 하는지 왼쪽으로 진행해야 하는지를 체크
        if (mode == 1) { // 오른쪽으로 이동 
            if (pos_lcd < 29) {
                pos_lcd++;
            } else {
                mode = 0; // 방향 전환
                pos_lcd--;
            }
        } else { // 왼쪽으로 이동 
            if (pos_lcd > 16) {
                pos_lcd--;
            } else {
                mode = 1; // 방향 전환
                pos_lcd++;
            }
        }

        strncpy(output+16,"                ",16);
        strncpy(output + pos_lcd, "JTY", 3);
        
    }

    for (i = 0; i < MAX_BUFF; i += 2) {
        _s_value = (output[i] & 0xFF) << 8 | (output[i + 1] & 0xFF);
        outw(_s_value, (unsigned int)iom_fpga_addr[LCD] + i);
    }
}
