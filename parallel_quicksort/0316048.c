#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

void quicksort(int *,int,int,int);
void sort(int *,int,int);
void print_vector(int*,int);

int main(){
        FILE *ifp,*ofp;
        char ifile[256];
        int input_num;
        int *numbers;
        int i,tmp;

        printf("Input file name: ");
        scanf("%s",ifile);
        ifp = fopen(ifile,"r"); 
        fscanf(ifp,"%d",&input_num);
        printf("input num :%d\n",input_num);
        if( (numbers = malloc(sizeof(int) * input_num ) ) == NULL){
                printf("malloc failed\n");
        }
        for(i=0;i<input_num;i++){
                fscanf(ifp,"%d",&tmp);         
                numbers[i] = tmp;
        }
        fclose(ifp);

        quicksort(numbers,0,input_num,1);
        ofp = fopen("output1.txt","w");
        for(i=0;i<input_num-1;i++){
                fprintf(ofp,"%d ",numbers[i]);
        }
        fprintf(ofp,"%d\n",numbers[input_num-1]);
        fclose(ofp);

        free(numbers);

        return 0;
}

void quicksort(int *numbers,int begin,int end,int depth){
        int pivot_ptr,cur_ptr;
        int tmp;
        int i;
        if(depth < 4){  //divide 
                if(begin != end){
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
                        quicksort(numbers,begin,pivot_ptr,depth+1);
                        quicksort(numbers,pivot_ptr+1,end,depth+1);
                }
        }else{          //sort
                sort(numbers,begin,end);
        }
}
void sort(int *numbers,int begin,int end){
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
}

void print_vector(int *numbers,int input_num){
        int i;
        for(i=0;i<input_num;i++){
                printf("%d ",numbers[i]);
        }printf("\n");
}
