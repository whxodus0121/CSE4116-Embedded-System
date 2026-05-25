#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <linux/input.h>
#include <pthread.h>
#include "devicecontrol.h"
#include "ipc.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_t led_thread;

unsigned char save[9]={0,0,0,1,0,1,0,0,0};
const char switch_keypad[9][4] = {
    "", "ABC", "DEF",
    "GHI", "JKL", "MNO",
    "PQRS", "TUV", "WXYZ" 
};

typedef struct{

    int num;
    int key;
    char values[MAX_BUFF_KVS];
}TMPKVS;
//1.sst와 2.sst에 있는 값들을 읽어올 값들

unsigned char result_lcd[16]={'\0'};

unsigned int file_num;
unsigned int data_num;

void init_device();

void *operate_led(void *arg);
void using_led(int input_key);
void stop_led();

bool check_press_switch();
int check_switch_num();
int using_switch_num();
void using_switch_value();

void put();
void get();
void io_process();

int count_kvs_in_storage_table(const char* filename);
void create_new_storage_table();
int compare_asc(const void *a, const void *b);
void merge_process();

void kvs_put();
void kvs_get();



#endif