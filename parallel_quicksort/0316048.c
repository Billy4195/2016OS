#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>

void quicksort(int *,int,int,int,int);
void print_vector(int*,int);
void* thread_fn(void *);

typedef struct param{
        int *numbers;
        int begin;
        int end;
        int depth;
        int thd_idx;
}param;


pthread_t *threads;
param arg[15];
sem_t sema[15];
sem_t s_;

int main(){
        FILE *ifp,*ofp;
        char ifile[256];
        int input_num;
        int *numbers,*numbers2;
        int i,tmp;
        clock_t start_clk,end_clk;
        clock_t s,e;
        if( (threads = (pthread_t *) malloc( sizeof(pthread_t) * 15) ) == 
                NULL){
                perror("malloc thread failed\n");
        }
        for(i=0;i<15;i++){
                sem_init(&sema[i],0,0);
        }
        for(i=0;i<15;i++){
                pthread_create(&threads[i],NULL,thread_fn,(void*)(long)i);
        }
        printf("Input file name: ");
        scanf("%s",ifile);
        if( (ifp = fopen(ifile,"r")) == NULL ){
                perror("input file open failed\n");
        }
        fscanf(ifp,"%d",&input_num);
        printf("input num :%d\n",input_num);
        if( (numbers = malloc(sizeof(int) * input_num ) ) == NULL){
                perror("malloc failed\n");
        }
        if( (numbers2 = malloc(sizeof(int) * input_num ) ) == NULL){
                perror("malloc failed\n");
        }
        for(i=0;i<input_num;i++){
                fscanf(ifp,"%d",&tmp);         
                numbers[i] = tmp;
        }
        fclose(ifp);
        memcpy(numbers2,numbers,sizeof(int) * input_num );
/***             init finished             ***/
        start_clk = clock();
        quicksort(numbers,0,input_num,4,-1);
        end_clk = clock();
        printf("Single-thread elapsed : %lf s\n",(double)(end_clk-start_clk) / CLOCKS_PER_SEC);

        start_clk = clock();
        arg[0].numbers = numbers2;
        arg[0].begin = 0;
        arg[0].end = input_num;
        arg[0].depth = 1;
        arg[0].thd_idx = 1;
        sem_post(&sema[0]);
        
        for(i=0;i<8;i++){
                sem_wait(&s_);
        }
        end_clk = clock();
        printf("Multi-thread elapsed : %lf s\n",(double)(end_clk-start_clk) / CLOCKS_PER_SEC);
        ofp = fopen("output1.txt","w");
        for(i=0;i<input_num-1;i++){
                fprintf(ofp,"%d ",numbers[i]);
        }
        fprintf(ofp,"%d",numbers[input_num-1]);
        fclose(ofp);
        ofp = fopen("output2.txt","w");
        for(i=0;i<input_num-1;i++){
                fprintf(ofp,"%d ",numbers2[i]);
        }
        fprintf(ofp,"%d",numbers2[input_num-1]);
        fclose(ofp);
        free(threads);
        free(numbers);
        return 0;
}

void quicksort(int *numbers,int begin,int end,int depth,int thd_idx){
        int pivot_ptr,cur_ptr;
        int tmp;
        int i;
        if(depth < 4){  //divide 
                pivot_ptr = begin;
                for(cur_ptr= begin+1;cur_ptr < end;cur_ptr++){
                        if(numbers[cur_ptr] < numbers[pivot_ptr]){
                                tmp = numbers[cur_ptr];
                                for(i=cur_ptr;i > pivot_ptr;i--){
                                        numbers[i] = numbers[i-1];
                                }
                                numbers[pivot_ptr] = tmp;
                                pivot_ptr++;
                        }
                }
                if(thd_idx < 0){ //serial version
                        quicksort(numbers,begin,pivot_ptr,depth+1
                            ,thd_idx);
                        quicksort(numbers,pivot_ptr+1,end,depth+1
                            ,thd_idx);
                }else{
                        arg[thd_idx*2-1].numbers = numbers;
                        arg[thd_idx*2-1].begin = begin;
                        arg[thd_idx*2-1].end = pivot_ptr;
                        arg[thd_idx*2-1].depth = depth + 1;
                        arg[thd_idx*2-1].thd_idx = thd_idx*2;
                        sem_post(&sema[thd_idx*2-1]);
                        arg[thd_idx*2].numbers = numbers;
                        arg[thd_idx*2].begin = pivot_ptr+1;
                        arg[thd_idx*2].end = end;
                        arg[thd_idx*2].depth = depth+1;
                        arg[thd_idx*2].thd_idx = thd_idx*2+1;
                        sem_post(&sema[thd_idx*2]);
                }
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
                if(thd_idx != -1)
                        sem_post(&s_);
        }
}

void print_vector(int *numbers,int input_num){
        int i;
        for(i=0;i<input_num;i++){
                printf("%d ",numbers[i]);
        }printf("\n");
}
void* thread_fn(void *para){
        long thd_idx = (long)para;
        sem_wait(&sema[thd_idx]);
        int *numbers= arg[thd_idx].numbers;
        int begin = arg[thd_idx].begin;
        int end = arg[thd_idx].end;
        int depth = arg[thd_idx].depth;
        quicksort(numbers,begin,end,depth,thd_idx+1);
}
