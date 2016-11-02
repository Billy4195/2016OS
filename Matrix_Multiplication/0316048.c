#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <error.h>

int main(){
	      unsigned int i,j,k;
	      int input;
        int shmid,shmid2,shmid3;
	      unsigned int *M1,*M2,*M3;
        unsigned int tmp_sum;
        unsigned int check_sum=0;

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
        if( ( M3 =(unsigned int*) shmat(shmid3, NULL, 0)) == (unsigned int*) -1 ){
		            perror("attach share memory error\n");
	      }

        for(i=0;i<input;i++){
                for(j=0;j<input;j++){
                        *(M1+(i*input+j)) = input*i+j;
                        *(M2+(i*input+j)) = input*i+j;
                }
        }

        for(i=0;i<input;i++){
                for(j=0;j<input;j++){
                        tmp_sum = 0;
                        for(k=0;k<input;k++){
                                tmp_sum += *(M1+(i*input+k)) * *(M2+(k*input+j));

                        }
                        *(M3+(i*input+j)) = tmp_sum;
                }
        }
 
                
      
        for(i=0;i<input;i++){
                for(j=0;j<input;j++){
                        check_sum += *(M3+(i*input+j));
                }
        }
        printf("%u\n",check_sum);
        shmdt(M1);
        shmdt(M2);
        shmdt(M3);

        shmctl( shmid, IPC_RMID, NULL);
        shmctl( shmid2, IPC_RMID, NULL);
        shmctl( shmid3, IPC_RMID, NULL);

        return 0;
}
