#ifndef IPC_HEADER_INCLUDE
#define IPC_HEADER_INCLUDE

#include <sys/types.h> 
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include "devicecontrol.h"

#define MAX_BUFF_LCD 32
#define LINE_BUFF_LCD 16
#define DIGIT 0
#define ALPHA 1

#define SHMIM_KEY (key_t) 1000
#define KVS_KEY (key_t) 2000
#define MESIM_KEY (key_t) 3000
#define SEM_KEY (key_t) 4000
#define MAX_KEY 3
#define MAX_BUFF_KVS 5
#define KEY 0
#define VALUE 1
#define PUT_MODE_LED 2
#define GET_MODE_LED 3
#define ALL_LED 4

//io와 main 사이에 주고 받을 데이터들
typedef struct ShmIoMain{

    int mode;//put,get,merge
    int put_req; //io에서 main으로 put요청 체크
    //char put_values[MAX_KEY][MAX_BUFF_LCD];//설정할 LCD 

    int get_req; //io에서 main으로 get요청 체크
    int get_key; //io에서 main으로 찾아달라고 요청할 key
    //char get_values[MAX_BUFF_LCD];//LCD에 출력할 데이터들
    
    int switch_mode;

    int exit;
}ShmIoMain;
ShmIoMain *shmim;
int shmim_id;

typedef struct ShmKVS{

    int size;//kv의 개수 최대3;
    int input_mode; //key입력 or value입력
    int num; //key값 4자리 중 몇자리 입력 중인지를 받기 위한 값
    int keys[MAX_KEY]; //key값
    char values[MAX_KEY][MAX_BUFF_KVS]; //value는 최대 5자리까지 가능
    int need_to_merge;
}ShmKVS;
ShmKVS *kvs;
int kvs_id;

//main과 merge 사이에 ipc이용해서 주고 받을 데이터들(st파일들)
typedef struct MessageQueueIoMerge{

    int need_to_merge;
    //int num;
    //int keys[MAX_KEY]; //key값
    //int values[MAX_KEY][MAX_BUFF_KVS]; //value는 최대 5자리까지 가능
}MessageQueueIoMerge;
MessageQueueIoMerge *mesim;
int mesim_id;

int sem_id;

void init_shared_memory();
void init_message_queue();
void init_sem();
void semlock();
void semunlock();
void free_shared_memory();
void free_message_queue();


#endif