#include "main.h"


void init_device(){

    using_led(0);
    stop_led();
    fnd_device(0);
    sleep(1);
}

void *operate_led(void *arg){

    int input_key = *((int*)arg);
    int i;
    switch(input_key) {
    case KEY:
        while (1) {
            use_led_device(0);
            use_led_device(3);
            sleep(1);
            use_led_device(0);
            use_led_device(4);
            sleep(1);
        }
        break;
    case VALUE:
        while (1) {
            use_led_device(0);
            use_led_device(7);
            sleep(1);
            use_led_device(0);
            use_led_device(8);
            sleep(1);
        }
        break;
    case PUT_MODE_LED:
        use_led_device(0);
        use_led_device(1);
        
        break;
    case GET_MODE_LED:
        use_led_device(0);
        use_led_device(5);
        
        break;
    default :
        use_led_device(0);
        for(i=0;i<8;i++){
            use_led_device(i);
        }
}

}

void using_led(int input_key){

    pthread_create((&led_thread),NULL,operate_led,&input_key);
    return;
}
//led 끄기 led mode 바뀔 때 사용
void stop_led(){

    pthread_cancel(led_thread);
    return;
}
//led활용을 위한 눌렸는지 체크
bool check_press_switch(){

    int i;
    for(i=0;i<MAX_BUTTON;i++){
        if(push_sw_buff[i]==1){
            return true;
        }
    }
    return false;
}
//동시에 키입력 몇개 됐나 확인 2개는 저장 입력이 있음
int check_switch_num(){

    int i,count=0;
    for(i=0;i<MAX_BUTTON;i++){
        if(push_sw_buff[i]==1){
            count++;
        }
    }
    return count;
}

//key값 숫자를 받는 함수
int using_switch_num(){

    int i;
    for(i=0;i<MAX_BUTTON;i++){
        if(push_sw_buff[i]==1){
            return (i+1);
        }
    }
    return 0;
}


void using_switch_value(){

    int i,temp=10,count_btn=0,num,value_size=0,check_first=0;
    int count_change=0;

    while(1){
        
        if(check_switch_num()==1){

            num=using_switch_num();
            //value 입력할때는 영어 숫자 다 사용가능하기 때문에 1번 특수키 + 초기화를 위한 시간 체크
            if(num==1) {
                shmim->switch_mode=(shmim->switch_mode+1)%2;
                count_change++;
            }
            //초기화
            if(count_change>=4) {

                for(i=0;i<MAX_BUFF_KVS;i++){

                    kvs->keys[i]=0;
                    memset(kvs->values[i],'\0',sizeof(int)*MAX_BUFF_KVS);
                }
                fnd_device(0);
                lcd_device(PUT,"");
            }
            
            if(shmim->switch_mode==ALPHA){ //영어 입력
                
                if(num==temp) count_btn++;
                else {
                    if(!check_first) {
                        temp=num;
                        check_first++; 
                        continue;
                    }
                    else{
                        kvs->values[kvs->size][value_size]=switch_keypad[temp][count_btn-1];
                        value_size++;
                        lcd_device(PUT,kvs->values[kvs->size]);   
                        count_btn=0;
                    }
                }
            }
            else{ //숫자 입력

                kvs->values[kvs->size][value_size]=num;
                value_size++;
                lcd_device(PUT,kvs->values[kvs->size]);
                count_btn=0;
            }

        }
        else{
            //button 두개가 눌렸을때 4,6이면 main에게 put 요청함
            for(i=0;i<MAX_BUTTON;i++){
                if(push_sw_buff[i]!=save[i]) continue;
            }
            shmim->put_req=1;
            break;
        }
        if(value_size==5) break;
        
        usleep(700000);
    }   
}



void put(){


    kvs->input_mode = KEY;
    int num,fnd_key;
    int i,count_change,check_tmp;
    char key[5];
    init_device();
    using_led(PUT_MODE_LED);
    if(check_press_switch) using_led(KEY);

    if(kvs->size==3){

        shmim->put_req=1;
        while(1){
            if(shmim->put_req==0) break;
            usleep(400000);
        }
    }
    while(!shmim->put_req){
        switch(kvs->input_mode){
            
            case KEY:

                kvs->num=0;
                fnd_key=0;
                while(kvs->num<=4){
                    
                    //key는 숫자로 입력 받기 때문에 체크
                    if(shmim->switch_mode==DIGIT) num=using_switch_num();
                    else printf("Error : Only digit can be entered in key"); continue;
                    //0.4초마다 입력을 확인 하는데 1이 4번 이상 연속으로 입력된 경우-> 꾹 눌렀다고 판단
                    check_tmp=num;
                    
                    if(check_tmp==1) count_change++;
                    else count_change=0;
                    //초기화
                    if(count_change>=4){
                        
                        for(i=0;i<MAX_BUFF_KVS;i++){

                            kvs->keys[i]=0;
                            memset(kvs->values[i],0,sizeof(int)*MAX_BUFF_KVS);
                        }
                        fnd_device(0);
                        lcd_device(PUT,"");
                    }
                    
                    //0이 아니라면 fnd에 출력
                    if(!num){
                        
                        //왼쪽부터 채우기 위해 10을 3-채워진 숫자 개수 만큼 곱해줌 
                        for(i=0;i<=MAX_KEY-kvs->num;i++){
                            num*=10;
                        }
                        kvs->keys[kvs->num]=num;
                        fnd_key+=num;
                        sprintf(key,"%d",fnd_key);
                        lcd_device(PUT,key);
                        kvs->num++;
                    }
                    
                    //입력이 끝난 후 reset스위치 눌리면 바로 종료 1230처럼 0이 마지막에 있는 값들도 고려
                    if(reset_device()) break;
                    usleep(400000);
                }
                
                kvs->input_mode=VALUE;
                break;

            case VALUE:

                using_led(VALUE);
                using_switch_value();
                sprintf(result_lcd,"(%d, %d,%s)",kvs->num,fnd_key,kvs->values[kvs->size]);
                kvs->size++;
                lcd_device(PUT,result_lcd);
                kvs->input_mode=KEY;
                //////kvs에 key랑 value가 저장되어 있음 이걸 main에 넘겨서 저장시켜야 함////////
                break;
            default:
                break;

        }
    }

    shmim->put_req=0;
    
    using_led(ALL_LED);
    usleep(400000);
    using_led(PUT_MODE_LED);

}

void get(){
    

    using_led(0);
    using_led(GET_MODE_LED);
    lcd_device(GET,NULL);

    int count=0,i,num;
    shmim->get_key=0;

    while(count<4){
        
        if(check_press_switch()){
            using_led(0);
            using_led(KEY);
        }

        //key는 숫자로 입력 받기 때문에 체크
        if(shmim->switch_mode==DIGIT) num=using_switch_num();
        else printf("Error : Only digit can be entered in key"); continue;
        
        //0이 아니라면 fnd에 출력
        if(!num){
            
            //왼쪽부터 채우기 위해 10을 3-채워진 숫자 개수 만큼 곱해줌 
            for(i=0;i<=MAX_KEY;i++){
                shmim->get_key=shmim->get_key*10+num;
            }
            //kvs->keys[kvs->num]=num;          
            
            count++;
        }
        if(reset_device()) shmim->get_req=1;
        
        while(shmim->get_req){

            ulseep(400000);
        }
        
        //입력이 끝난 후 reset스위치 눌리면 바로 종료 1230처럼 0이 마지막에 있는 값들도 고려
        if(reset_device()) break;
        usleep(400000);
    }
    using_led(0);
    using_led(GET_MODE_LED);
 
    
}

void io_merge(){

    int check_merge;
    mesim->need_to_merge=1;
    if(msgsnd(mesim_id,&mesim->need_to_merge,sizeof(mesim->need_to_merge),0)==-1){
        perror("Error");
        shmim->exit=true;
    }
    while(1){

        check_merge=msgrcv(mesim_id,&mesim->need_to_merge,sizeof(mesim->need_to_merge),1,0);
        if(!check_merge) break;
        usleep(400000);
    }

}



void io_process(){

    semlock();
    init_device(); 
    lcd_device(PUT,"ABCD");
    while(true){

        readkey_device();
        if(ev[0].value==KEY_PRESS){

            ////////led 컨트롤 필요하면 추가////////
            switch(ev[0].code){
                //종료
                case BACK:
                    shmim->exit=true;
                    init_device();
                    break;
                //PUT->GET->MERGE->PUT
                case VOL_UP:  
                    if(shmim->mode==2) shmim->mode=0;
                    else shmim->mode++;
                    break;  
                //역순
                case VOL_DOWN:
                    if(shmim->mode==0) shmim->mode=2;
                    else shmim->mode--;
                    break;
                default:
                    break;
            }
            if(shmim->exit){
                shmim->mode=3;
                break;
            } 
            
            
        }

        switch(shmim->mode){
            case PUT:
                if(kvs->size==3){

                    
                    mesim->need_to_merge=1;
                    if(msgsnd(mesim_id,&mesim->need_to_merge,sizeof(mesim->need_to_merge),0)==-1){
                        perror("Error");
                        shmim->exit=true;
                    }

                    while(mesim->need_to_merge){

                        usleep(400000);
                    }
                    
                }
                put();
                break;
            case GET:
                get();
                break;
            case MERGE:
                io_merge();
                break;
            default:
                break;
        }
    }
    semunlock();
}

int count_kvs_in_storage_table(const char* filename){

    int count=0;
    int tmp;
    FILE *fp=fopen(filename,"r");
    if(fp==NULL){

        printf("Error : Can't open file");
        return 0;
    }

    while((tmp=fgetc(fp))!=EOF){
        if(tmp='\n') count++;
    }
    fclose(fp);
    return count;
}

void create_new_storage_table(){

    FILE *fp;
    char filename[6];
    file_num++;
    sprintf(filename,"%d.sst",file_num);

    fp=fopen(filename,"w");
    if(fp==NULL){

        printf("Error : Can't open file");
        return;
    }
    fclose(fp);
}

int compare_asc(const void *a, const void *b){

    int num_a = atoi(*(char**)a);
    int num_b = atoi(*(char**)b);
    if (num_a < num_b) {
        return -1;
    } else if (num_a > num_b) {
        return 1;
    } else {
        return 0;
    }
}

void merge_process(){
    

  
    FILE *fp1;
    FILE *fp2;
    FILE *fp3;
    int check_merge,i,j;
    int check_dup; // 중복됐는지 확인하기 위한 변수
    int count;//3.sst에 저장할 kvs의 개수

    TMPKVS tmp[6];
    TMPKVS merge_kvs[6];

    for(i = 0; i < 6; i++){
       
        merge_kvs[i].num = 0;
        merge_kvs[i].key = 0;
        memset(merge_kvs[i].values,'\0',sizeof(merge_kvs[i].values));
        tmp[i].num = 0;
        tmp[i].key = 0;
        memset(tmp[i].values,'\0',sizeof(tmp[i].values));
    }

    while(1){
       
        check_merge=msgrcv(mesim_id,&mesim->need_to_merge,sizeof(mesim->need_to_merge),1,0);
       
        if(check_merge){
            
            fp1=fopen("1.sst","r");
            fp2=fopen("2.sst","r");
            fp3=fopen("3.sst","w");
            

            for(i=0;i<3;i++){
                fscanf(fp1,"%d %d %s\n",&merge_kvs[i].num,&merge_kvs[i].key,merge_kvs[i].values);
            }
            for(i=3;i<6;i++){
                fscanf(fp2,"%d %d %s\n",&merge_kvs[i].num,&merge_kvs[i].key,merge_kvs[i].values);
            }
            //중복 제거하고 key작은 거 입력
            count=0;
            for(i=0;i<6;i++){
                check_dup=0;
                for(j=0;j<count;j++){
                    if(merge_kvs[i].key==tmp[j].key){
                        if(merge_kvs[i].num>tmp[j].num){
                            tmp[j]=merge_kvs[i];
                        }
                        check_dup=1;
                        break;
                    }
                }
                if(!check_dup){
                    tmp[count++]=merge_kvs[i];
                }
            }

            qsort(tmp,count,sizeof(TMPKVS),compare_asc);

            for(i=1;i<=count;i++){
                
                fprintf(fp3,"%d %d %s\n",i,tmp[i-1].key,tmp[i-1].values);
            }

            mesim->need_to_merge=0;
            if(msgsnd(mesim_id,&mesim->need_to_merge,sizeof(mesim->need_to_merge),0)==-1){
                perror("Error");
            }
            break;
        }

        usleep(400000);
    }

    motor_device(true);
    sleep(2);
    motor_device(false);
    fclose(fp1);
    fclose(fp2);
    fclose(fp3);

 
}
//io에서 kvs입력 받은 값중 제일 먼저 들어온 값들만 main에서 storage table에 저장
void kvs_put(){

    FILE *fp;
    char filename[6];
    int i;
    sprintf(filename,"%d.sst",file_num);

    //만약 이미 파일에 값들이 있다면 추가모드로 열어서 다음줄에 쓰고 아니면 그냥 쓰기 모드
    if(count_kvs_in_storage_table(filename)>=1){
        fp=fopen(filename,"a");
    }
    else{
        fp=fopen(filename,"w");
    }
    if(fp==NULL){

        printf("Error : Can't open file");
        return;
    }

    for(i=0;i<kvs->size;i++){
        if(data_num<=6){

            data_num++;
            if(data_num<=3){

                fprintf(fp,"%d %d %s",data_num,kvs->keys[i],kvs->values[i]);
            }
            else{
                fclose(fp);
                create_new_storage_table();
                sprintf(filename,"%d.sst",file_num);
                if(count_kvs_in_storage_table(filename)>=1){
                    fp=fopen(filename,"a");
                }
                else{
                    fp=fopen(filename,"w");
                }
                if(fp==NULL){

                    printf("Error : Can't open file");
                    return;
                }
                fprintf(fp,"%d %d %s",data_num,kvs->keys[i],kvs->values[i]);
            }
        }
    }
    kvs->size=0;
    fclose(fp);
}



void kvs_get(){


    int i,j;
    char print_lcd[LINE_BUFF]={'\0'};
    FILE *fp1,*fp2;
    for(i=kvs->size;i>=0;i--){

        if(shmim->get_key==kvs->keys[i]){
            sprintf(print_lcd,"(%d, %d,%s)",i,kvs->keys[i],kvs->values[i]);
            lcd_device(GET,print_lcd);
            shmim->get_req=0;
            return;
        }
    }

    
    TMPKVS tmp[6];
    for(i = 0; i < 6; i++){
       
        tmp[i].num = 0;
        tmp[i].key = 0;
        memset(tmp[i].values,'\0',sizeof(tmp[i].values));
    }    

    fp1=fopen("1.sst","r");
    fp2=fopen("2.sst","r");
    if(fp1!=NULL){
        for(i=0;i<3;i++){
            fscanf(fp1,"%d %d %s\n",&tmp[i].num,&tmp[i].key,tmp[i].values);
        }
    }
    if(fp2!=NULL){
        for(i=3;i<6;i++){
            fscanf(fp2,"%d %d %s\n",&tmp[i].num,&tmp[i].key,tmp[i].values);
        }
    }

    for(j=i;j>=0;j--){
        if(shmim->get_key==tmp[i].key){
            sprintf(print_lcd,"(%d, %d,%s)",tmp[i].num,tmp[i].key,tmp[i].values);
            lcd_device(GET,print_lcd);
            shmim->get_req=0;
            return;
        }
    }
    lcd_device(GET,"Error");
    shmim->get_req=0;
    return;
}

int main(){

    init_shared_memory();
    init_message_queue();

    pid_t pid_io,pid_merge;
    
    
    if((pid_io = fork()) == -1){
        printf("fork error\n");
        exit(0);
    }
   
    /* I/O process */
    if (pid_io == 0) {
        io_process();
        printf("io process done\n");
    }
   
    if((pid_merge = fork()) == -1){
        printf("fork error\n");
        exit(0);
    }
   
    /* Merge process */
    if (pid_merge == 0) {
        merge_process();
        printf("merge process done\n");
    }
   
    
    while(1){
        /* I/O process에서 quit신호 주기 전까지 계속 실행 */
        if(!shmim->exit){

            switch(shmim->mode){

                case PUT:
                    if(shmim->put_req){
                        kvs_put();
                    }
                    break;
                case GET:
                    if(shmim->get_req){
                        kvs_get();
                    }
                    break;
            }

        }
        else break;

        usleep(400000);
    }

    if (kill(pid_io, SIGKILL) == -1) {
        printf("error kill child process\n");
    } else {
        waitpid(pid_io, NULL, 0);
    }

    if (kill(pid_merge, SIGKILL) == -1) {
        printf("error kill child process\n");
    } else {
        waitpid(pid_merge, NULL, 0);
    }

    return 0;
}