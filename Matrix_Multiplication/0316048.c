#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <error.h>

int Matrix_mul(int shmid,int shmid2,int shmid3,int input,int p,int index){
        int i,j,k;
        unsigned int tmp_sum;
        unsigned int *M1,*M2,*M3;
        if( ( M1 =(unsigned int*) shmat(shmid, NULL, 0)) == (unsigned int*) -1 ){
		            perror("attach share memory error\n");
	      }
        if( ( M2 =(unsigned int*) shmat(shmid2, NULL, 0)) == (unsigned int*) -1 ){
		            perror("attach share memory error\n");
	      }
        if( ( M3 =(unsigned int*) shmat(shmid3, NULL, 0)) == (unsigned int*) -1 ){
		            perror("attach share memory error\n");
	      }

        for(i=index;i<input;i += p){
                for(j=0;j<input;j++){
                        tmp_sum = 0;
                        for(k=0;k<input;k++){
                                tmp_sum += *(M1+(i*input+k)) * *(M2+(k*input+j));

                        }
                        *(M3+(i*input+j)) = tmp_sum;
                }
        }
        shmdt(M1);
        shmdt(M2);
        shmdt(M3);
}

int main(){
	      unsigned int i,j,k;
	      int input;
        int shmid,shmid2,shmid3;
	      unsigned int *M1,*M2,*M3;
        unsigned int tmp_sum;
        unsigned int check_sum=0;
        int pid;
        struct timeval start,end;
        int sec,usec;

	      scanf("%d",&input);

	      key_t key=0;
	      if( (shmid = shmget(key, input*input*sizeof(unsigned int) , IPC_CREAT | 0666)) < 0){
		            perror("get share memory error\n");
	      }
        if( (shmid2 = shmget(key, input*input*sizeof(unsigned int) ,IPC_CREAT | 0666)) <0 ){
		            perror("get share memory error\n");
        }
        if( (shmid3 = shmget(key, input*input*sizeof(unsigned int) ,IPC_CREAT | 0666)) <0 ){
		            perror("get share memory error\n");
        }
        if( ( M1 =(unsigned int*) shmat(shmid, NULL, 0)) == (unsigned int*) -1 ){
		            perror("attach share memory error\n");
	      }
        if( ( M2 =(unsigned int*) shmat(shmid2, NULL, 0)) == (unsigned int*) -1 ){
		            perror("attach share memory error\n");
	      }

        for(i=0;i<input;i++){
                for(j=0;j<input;j++){
                        *(M1+(i*input+j)) = input*i+j;
                        *(M2+(i*input+j)) = input*i+j;
                }
        }
        shmdt(M1);
        shmdt(M2);
       
        for(i=1;i<=16;i++){
                check_sum = 0;
                gettimeofday(&start,0);
                //printf("I = %d \n",i);
                for(j=0;j<i;j++){
                        //printf("J= %d \n",j);
                        switch(pid = fork()){
                                case 0:
                                        Matrix_mul(shmid,shmid2,shmid3,input,i,j);
                                        exit(0);
                                case -1:
                                        perror("fork error\n");
                        }
                }
                for(k=0;k<i;k++){
                        wait(NULL);
                }
                if( ( M3 =(unsigned int*) shmat(shmid3, NULL, 0)) == (unsigned int*) -1 ){
		                    perror("attach share memory error\n");
	              }
                for(k=0;k<input;k++){
                        for(j=0;j<input;j++){
                                check_sum += *(M3+(k*input+j));
                                *(M3+(k*input+j)) = 0;
                        }
                }
                shmdt(M3);
                gettimeofday(&end,0);
                sec = end.tv_sec - start.tv_sec;
                usec = end.tv_usec - start.tv_usec;
                printf("Multiplying matrices using %d process\n",i);
                printf("Elapsed time:%f sec, Checksum:%u\n",sec+(usec/1000000.0),check_sum);
        }
         
                
      


        shmctl( shmid, IPC_RMID, NULL);
        shmctl( shmid2, IPC_RMID, NULL);
        shmctl( shmid3, IPC_RMID, NULL);

        return 0;
}
