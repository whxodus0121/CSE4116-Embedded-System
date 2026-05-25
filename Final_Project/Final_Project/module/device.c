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
    iom_fpga_addr[MOT]=ioremap(IOM_FPGA_STEP_MOTOR_ADDRESS, 0x4);
    iom_fpga_addr[DOT]=ioremap(IOM_FPGA_DOT_ADDRESS, 0x10);
    iom_fpga_addr[LCD]=ioremap(IOM_FPGA_LCD_ADDRESS, 0x32);
}

void iom_fpga_exit(void){//해제

    int i;
    for(i=0;i<DEVICE_NUM;i++) iounmap(iom_fpga_addr[i]);
}

int fnd_device(int live_bullet,int blank_bullet){ //실탄과 공포탄을 표현

    unsigned short int value_short=0;

    value_short= 0 << 12 | live_bullet << 8 |0 << 4 |blank_bullet;
    outw(value_short,(unsigned int)iom_fpga_addr[FND]);

    return 0;
}

void mot_device(int num){ //Magnifier 사용시 작동하는 MOTOR

	unsigned char value[3];
	unsigned short int _s_value = 0;
    value[0]=num;
    value[1]=num;
    value[2]=num;
    _s_value = value[0] & 0xF;
    outw(_s_value,(unsigned int)iom_fpga_addr[MOT]);
    _s_value = value[1] & 0xF;
    outw(_s_value,(unsigned int)iom_fpga_addr[MOT] + 2);
    _s_value = value[2] & 0xFF;
    outw(_s_value,(unsigned int)iom_fpga_addr[MOT] + 4);
	
}

void dot_device(int num){ //turn을 dot에 출력 만약 -1입력시 빈칸 출력

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

void lcd_device(int p1,int p2){ //두 플레이어의 HP표시 / 대기 할때, 게임 끝났을 때 문구출력
    
    unsigned short int _s_value = 0;
    unsigned char output[MAX_BUFF+1] = {'\0'};
    int i;
    memset(output, ' ', MAX_BUFF);
    if(p1 <=0 && p2>0){
        strncpy(output,"P1's Hp become 0",LINE_BUFF);
        strncpy(output+LINE_BUFF,"P2 WIN!         ",LINE_BUFF);
    }
    else if(p2<=0 && p1>0){
        strncpy(output,"P2's Hp become 0",LINE_BUFF);
        strncpy(output+LINE_BUFF,"P1 WIN!         ",LINE_BUFF);
    }
    else if(p1>0 && p2>0){

        strcpy(output,"P1:");
        for(i=0;i<p1;i++)strcpy(output+3+i,"8");
        for (i = strlen(output); i < LINE_BUFF; i++) {
            output[i] = ' ';
        }
        
        strcpy(output + LINE_BUFF, "P2:");
        for(i = 0; i < p2; i++) strcpy(output + LINE_BUFF+3 + i, "8");
        for (i = strlen(output); i < MAX_BUFF; i++) {
            output[i] = ' ';
        }
        
    }
    else{

        strncpy(output, "Play Game!      ", LINE_BUFF);
    }

    for (i = 0; i < MAX_BUFF; i += 2) {
        _s_value = (output[i] & 0xFF) << 8 | (output[i + 1] & 0xFF);
        outw(_s_value, (unsigned int)iom_fpga_addr[LCD] + i);
    }
}
