#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void push_arg_list(char ***arg_list, unsigned int *capacity, unsigned int *filled, char *token){
        char ** tmp_list;
        if(*filled == *capacity){
                tmp_list = calloc(*capacity+10, sizeof(char*));
                memcpy((void*)tmp_list, (void*)*arg_list, 
                        sizeof(char*) * (*capacity));
                free(*arg_list);
                *arg_list = tmp_list;
                *capacity += 10;
        }
        (*arg_list)[(*filled)++] = token;
}

int allocate_bg_proc_list(int **bg_pid_list_ptr,int len){
        int *tmp = malloc( sizeof(int) * (len+10));
        if(*bg_pid_list_ptr !=NULL){
                free(*bg_pid_list_ptr);
                memcpy((void*)tmp,(void*)*bg_pid_list_ptr,sizeof(int)*len);
        }
        *bg_pid_list_ptr = tmp;
        return len+10;
}

void push_bg_proc_list(int **bg_pid_list_ptr,int *len,int *filled,int pid){
        if(*filled == *len){
                *len = allocate_bg_proc_list(bg_pid_list_ptr, 
                                (*len) );
        }
        (*bg_pid_list_ptr)[*filled] = pid;
        (*filled)++;
}

int check_bg_proc_list(int *bg_pid_list, int *filled, char block){
        int i;
        int j;
        int status;
        if(block){
                while(*filled){
                        wait(&status);
                        (*filled)--;
                }
        }else{
                for(i=0; i< *filled;i++){
                        waitpid(bg_pid_list[i],&status,WNOHANG);
                        if(WIFEXITED(status)){
                               printf("[%d] exited\n",bg_pid_list[i]);
                               for(j=0;j<*filled-1;j++){
                                        bg_pid_list[j] = bg_pid_list[j+1];
                               }
                               bg_pid_list[*filled-1] = 0;
                               (*filled)--;
                        }
                }
        }
        //need free list
}
int main(int argc, char ** argv){
        int i;
        char input[2048];
        char *token;
        char **arg_list;
        int pid;
        int *bg_pid_list=NULL;
        int list_len;
        int list_filled;
        unsigned int capacity,filled;
        char background;
        list_len = allocate_bg_proc_list(&bg_pid_list,0);
        list_filled = 0;
        printf("> ");
        while( fgets(input,sizeof(input),stdin) != NULL ){
                background = 0;
                capacity = 10;
                filled = 0;
                arg_list = calloc(capacity, sizeof(char*));
                token = strtok(input," \n");
                push_arg_list(&arg_list, &capacity, &filled, token);
                while( (token = strtok(NULL," \n")) != NULL){
                        push_arg_list(&arg_list, &capacity, &filled, token);
                }
                if( strcmp(arg_list[filled-1],"&") == 0){
                        background =1;
                        arg_list[filled-1] = NULL;
                        filled--;
                }
                for(i=0;i<filled;i++){                  //print arg_list
                        printf("arg %d : %s\n",i,arg_list[i]);
                }
                switch(pid = fork()){
                        case 0:
                                exit(execvp(arg_list[0],arg_list));
                                
                        case -1:
                                printf("ERROR occurred when fork child process\n");
                                break;
                        default:
                                if(background){
                                        printf("[%d]\n",pid);
                                        push_bg_proc_list(&bg_pid_list, &list_len, &list_filled, pid);
                                        check_bg_proc_list(bg_pid_list, &list_filled, 0);
                                }else{
                                        check_bg_proc_list(bg_pid_list, &list_filled, 0);
                                        waitpid(pid,NULL,0);
                                }
                }
                free(arg_list);
                printf("> ");
        }
        check_bg_proc_list(bg_pid_list, &list_filled, 1);
        printf("\n");
        free(bg_pid_list);
        return 0;
}

