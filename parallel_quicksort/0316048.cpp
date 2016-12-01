#include <cstdio>
#include <cstdlib>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <queue>
#include <assert.h>
#include <sys/time.h>

#define IFILE "input.txt"

using namespace std;

void quicksort(int *,int,int,int,int);
void print_vector(int*,int);
int read_input_file(int**);
void* thread_fn(void *);
int select_worker();
void write_to_file(int*,int,int);

typedef struct param{
        int *numbers;
        int begin;
        int end;
        int depth;
        int thd_idx;
        param(int *n,int b,int e,int d):numbers(n),begin(b),end(e),depth(d){
        
        }
}param;


sem_t sema[8];
sem_t idle_worker;
sem_t wait_lock,task,ref_lock;
sem_t ret;
char bit_map;
int ref=0;
queue<param> waiting_queue,task_queue;

int main(){
        FILE *fp;
        int input_num;
        int *numbers;
        pthread_t threads[8];
        int worker_idx,worker_limit;
        struct timeval start_clk,end_clk;
        sem_init(&idle_worker,0,1); //worker limit = 1;
        for(long i=0;i<8;i++){
                pthread_create(&threads[i],NULL,thread_fn,(void*)i);
                sem_init(&sema[i],0,0);
        }
        worker_limit = 0;
        sem_init(&task,0,0);
        sem_init(&wait_lock,0,1);
        sem_init(&ref_lock,0,1);
        bit_map=0x0;
        
        while(worker_limit < 8){
                int count=0;
                worker_limit++;
                input_num = read_input_file(&numbers);
                gettimeofday(&start_clk,0);
                waiting_queue.push(param(numbers,0,input_num,1));
                sem_post(&task);
                while(1){
                        if(count <15){
                                sem_wait(&task);
                        }else{
                                for(int i=0;i<8;i++){
                                        sem_wait(&ret);
                                }
                                break;
                        }
                        sem_wait(&idle_worker);

                        sem_wait(&ref_lock); 
                        worker_idx = select_worker();
                        if(worker_idx != -1){
                                sem_post(&sema[worker_idx]);
                                count++;
                        }else{
                                printf("error no available worker");
                        }
                        bit_map |= 1 << worker_idx;
                        sem_post(&ref_lock);
                }
                assert(waiting_queue.empty());
        
                gettimeofday(&end_clk,0);
                {
                int sec,usec;
                sec = end_clk.tv_sec - start_clk.tv_sec;
                usec = end_clk.tv_usec - start_clk.tv_usec;
                printf("%d threads sorting elapsed time : %lf s\n",worker_limit,sec+(usec/1000000.0));
                }
                write_to_file(numbers,input_num,worker_limit);
                delete [] numbers;
                sem_post(&idle_worker);
        }
        return 0;
}

void quicksort(int *numbers,int begin,int end,int depth){
        if(depth < 4){  //divide 
                int sum=0;
                int count=0;
                int pivot;
                int store_index;
                for(int i=0;i<50 && begin+i<end;i++){
                        count++;
                        sum += numbers[begin+i];
                }
                if(begin != end)
                        pivot = sum / count;

                store_index = begin;
                for(int i=begin;i<end;i++){
                        if(numbers[i] < pivot){
                                int tmp;
                                tmp = numbers[i];
                                numbers[i] = numbers[store_index];
                                numbers[store_index] = tmp;
                                store_index++;
                        }
                }

                sem_wait(&wait_lock);
                waiting_queue.push(param(numbers,begin,store_index,depth+1));           //race
                sem_post(&task);
                sem_post(&wait_lock);

                sem_wait(&wait_lock);
                waiting_queue.push(param(numbers,store_index,end,depth+1));         //race
                sem_post(&task);
                sem_post(&wait_lock);

        }else{          //sort
                int i,j;
                int len = end - begin;
                int tmp;
                for(i=0;i<len;i++){
                        for(j=begin;j<begin+len-1-i;j++){
                                if(numbers[j] > numbers[j+1]){
                                        tmp = numbers[j];
                                        numbers[j] = numbers[j+1];
                                        numbers[j+1] = tmp;
                                }
                        }
                }
                sem_post(&ret);
        }
}

int read_input_file(int **numbers_ptr){
        int input_num;
        int tmp;
        FILE* fp;
        fp = fopen(IFILE,"r");
        fscanf(fp,"%d",&input_num);
        *numbers_ptr = new int[input_num];
        for(int i=0;i<input_num;i++){
                fscanf(fp,"%d",&tmp);
                (*numbers_ptr)[i] = tmp;
        }
        fclose(fp);
        return input_num;
}

void print_vector(int *numbers,int input_num){
        int i;
        for(i=0;i<input_num;i++){
                printf("%d ",numbers[i]);
        }printf("\n");
}
void* thread_fn(void *para){
        long thd_idx = (long)para;
        while(1){
                sem_wait(&sema[thd_idx]);
                sem_wait(&wait_lock);
                param arg = waiting_queue.front(); //race
                waiting_queue.pop();               //race
                sem_post(&wait_lock);
                quicksort(arg.numbers, arg.begin, arg.end, arg.depth);

                sem_wait(&ref_lock);
                ref--;                          //race
                bit_map &= ~(1 << thd_idx);
                sem_post(&ref_lock);
                sem_post(&idle_worker);
        }
}
int select_worker(){
        char shift = 0;
        while(shift < 8){
                if( !(bit_map & (1 << shift)) ){ //worker not using
                        return shift;
                }
                shift++;
        }
        return -1;
}
void write_to_file(int *numbers,int input_num,int index){
        FILE *fp;
        char ofile[257];
        sprintf(ofile,"output_%d.txt",index);
        fp = fopen(ofile,"w");
        for(long i=0;i<input_num-1;i++){
                fprintf(fp,"%d ",numbers[i]);
        }
        fprintf(fp,"%d",numbers[input_num-1]);
        fclose(fp);
}
