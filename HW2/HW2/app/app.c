#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <linux/ioctl.h>

#define TIMER_DRIVER "/dev/dev_driver"
#define DEVICE_MAJOR_NUMBER 242


typedef struct value{//처음에 입력받는 변수 3개와 init에서 숫자의 위치를 표현하기 위한 pos

    int timer_interval;
    int timer_cnt;
    int timer_init;
    int pos;
}value;

#define SET_OPTION _IOW(DEVICE_MAJOR_NUMBER,0,value*)
#define COMMAND _IO(DEVICE_MAJOR_NUMBER,1)


int main(int argc, char**argv){

    int i,init_size,fd;
    value val;
    unsigned char dip_sw_buff=0;
    int flag=0; //init에서 0이 아닌 숫자의 개수를 체크하기 위한 flag
    

    if(argc!=4){

        printf("Enter 4 argument\n");
        return 0;
    }
    val.timer_interval=atoi(argv[1]);
    val.timer_cnt=atoi(argv[2]);
    val.timer_init=atoi(argv[3]);
    val.pos=4;
    if(val.timer_interval<1||val.timer_interval>100){

        printf("Invalid TIMER_INTERVAL value");
        return 0;
    }

    if(val.timer_cnt<1||val.timer_cnt>200){

        printf("Invalid TIMER_CNT value");
        return 0;
    }

    init_size=strlen(argv[3]);
    if(init_size>4){

        printf("Enter 4 digit number\n");
        return 0;
    }
    for(i=0;i<init_size;i++){

        if((argv[3][i]<'0')||(argv[3][i])>'8') {
            printf("Invalid TIMER_INIT value\n");
            return 0;
        } 
        else if (argv[3][i] != '0') { //0이 아닌 숫자 발견시 위치를 pos에 저장하고 flag++
            val.pos = i;
            flag++;
        }
        if(flag>1){ //두번 이상 0이 아닌 숫자 발견시

            printf("Invalid TIMER_INIT value\n");
            return 0;
        }
    }
    if(val.pos==4){
        printf("Invalid TIMER_INIT value\n");
        return 0;
    }

    fd=open(TIMER_DRIVER,O_RDWR);
    
    ioctl(fd,SET_OPTION,&val);   //변수 초기화 및 기본 화면 세팅

    while(1) { //reset버튼이 눌릴때 까지 대기
        usleep(400000);
        read(fd,&dip_sw_buff,1);
        if(dip_sw_buff==0) break;
    }
   

    ioctl(fd,COMMAND); //reset버튼 눌리면 timer시작
   
    return 0;
}