#ifndef _TIMER_DRIVER_H
#define _TIMER_DRIVER_H

#define SUCCESS 0

struct shot { //총을 쏠 때의 정보
    int to;//0은 me 1은 enemy
    int damage; //double damage쓰면 2 아니면 1
};

#define SET_HP _IOW(DEVICE_MAJOR_NUMBER,0,int*) //앱에서 입력받은 HP를 device에 표시
#define SET_BULLET _IO(DEVICE_MAJOR_NUMBER,1) //게임 시작시 랜덤으로 실탄과 공포탄을 장전
#define SHOT _IOW(DEVICE_MAJOR_NUMBER,2,struct shot*) //총을 쐈을 때 변경되는 부분
#define MAGNIFIER _IO(DEVICE_MAJOR_NUMBER,3) //아이템 Magnifier 사용
#define RECOVERY _IO(DEVICE_MAJOR_NUMBER,4) //아이템 Recovery 사용
#define HANDCUFFS _IO(DEVICE_MAJOR_NUMBER,5) //아이템 Handcuffs 사용
#define SET _IO(DEVICE_MAJOR_NUMBER,6) //디바이스들 대기 상태 출력
#define GET_END_FLAG _IOR(DEVICE_MAJOR_NUMBER,7,int*) //앱에서 endflag를 받아서 게임이 종료되면 다시 대기화면으로 전환 

#define DEVICE_NAME "roullete_driver"
#define DEVICE_MAJOR_NUMBER 242

void init_device();

#endif