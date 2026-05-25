#include "ipc.h"


void init_shared_memory(){

    /* I/O <-> Main  device 관련 데이터*/
    int i=0;
    
    if ((shmim_id = shmget(SHMIM_KEY, sizeof(shmim), IPC_CREAT | 0644)) == -1){
        perror("Error : Fail to get shmim_id");
        exit(-1);
    }
    shmim=shmat(shmim_id,0,0);

    shmim->mode=PUT;
    shmim->put_req=0;
    //memset(shmim->put_values,0,sizeof(shmim->put_values));

    shmim->get_req=0;
    shmim->get_key=0;
    //memset(shmim->get_values,0,sizeof(shmim->get_values));

    shmim->exit=false;
    shmim->switch_mode=DIGIT;
    
    /* I/O <-> Main  KVS 관련 데이터*/
    
    if ((kvs_id = shmget(KVS_KEY, sizeof(kvs), IPC_CREAT | 0644)) == -1){
        perror("Error : Fail to get kvs_id");
        exit(-1);
    }
    kvs=shmat(kvs_id,0,0);

    kvs->size=0;
    kvs->num=0;
    kvs->input_mode=KEY;
    for(i=0;i<MAX_BUFF_KVS;i++){

        kvs->keys[i]=0;
        memset(kvs->values[i],0,sizeof(int)*MAX_BUFF_KVS);
    }
}

void init_message_queue(){

    int i,j;
    /* I/O <-> MERGE KVS 관련 데이터*/
    
    if ((mesim_id = msgget(MESIM_KEY, IPC_CREAT | 0644)) == -1){
        perror("Error : Fail to get mesim_id");
        exit(-1);
    }

    mesim = (MessageQueueIoMerge*) malloc(sizeof(MessageQueueIoMerge));
    if (mesim == NULL) {
        perror("Error : Failed to allocate memory for mesim");
        exit(-1);
    }

    mesim->need_to_merge=0;
    //mesim->num=0;

    
    /*for(i=0;i<MAX_BUFF_KVS;i++){

        mesim->keys[i]=0;
        memset(mesim->values[i],0,sizeof(int)*MAX_BUFF_KVS);
    }*/
    
}

void init_sem(){

    if ((sem_id = semget (SEM_KEY, 1, IPC_CREAT | IPC_EXCL|0660)) == -1) {
        perror("Error");
        exit(-1);
    }
    semctl(sem_id,0,SETVAL,1);
}

void semlock() {
    
    struct sembuf sem_buf;
    sem_buf.sem_num = 0;
    sem_buf.sem_op = -1;
    sem_buf.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_buf, 1) == -1){

        perror("Error ");
        exit(-1);
    }
}


void semunlock() {

    struct sembuf sem_buf;
    sem_buf.sem_num = 0;
    sem_buf.sem_op = 1;
    sem_buf.sem_flg = SEM_UNDO;

    if (semop(sem_id, &sem_buf, 1) == -1){

        perror("Error ");
        exit(-1);
    }
}

void free_shared_memory(){

    shmctl(shmim_id,IPC_RMID,(struct shmid_ds*)NULL);
    shmctl(kvs_id,IPC_RMID,(struct shmid_ds*)NULL);
}

void free_message_queue(){

    msgctl(mesim_id,IPC_RMID,(struct msqid_ds*)NULL);
}