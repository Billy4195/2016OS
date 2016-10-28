#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

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
}
int main(int argc, char ** argv){
        int i;
        char input[2048];
        char *tmp;
        char *re_out;
        char *re_in;
        char *token;
        char *arg;
        char *arg2;
        char **arg_list;
        char **arg_list2;
        int pid;
        int *bg_pid_list=NULL;
        int list_len;
        int list_filled;
        unsigned int capacity,filled;
        unsigned int capacity2,filled2;
        char background;
        int in,out;
        int fd[2];
        list_len = allocate_bg_proc_list(&bg_pid_list,0);
        list_filled = 0;
        printf("> ");
        while( fgets(input,sizeof(input),stdin) != NULL ){
                background = 0;
                arg = NULL;
                arg2 = NULL;
                capacity = capacity2 = 10;
                filled = filled2 =  0;
                re_out = re_in = NULL;
                arg_list = calloc(capacity, sizeof(char*));
                arg_list2 = calloc(capacity2, sizeof(char*));
                if( strcmp(input,"\n") == 0){
                        free(arg_list);
                        free(arg_list2);
                        printf("> ");
                        continue;
                }
                if( (arg = strstr(input,">")) != NULL){
                        arg = strtok(input,">");
                        re_out = strtok(NULL," \n");
                }
                if( (arg = strstr(input,"<")) != NULL){
                        arg = strtok(input,"<");
                        re_in = strtok(NULL," \n");
                }
                if( (arg = strstr(input,"|")) != NULL){
                        arg = strtok(input,"|");
                        arg2 = strtok(NULL,"\n");
                }
                if(!arg){
                        arg = input;
                }
                token = strtok(arg," \n");
                push_arg_list(&arg_list, &capacity, &filled, token);
                while( (token = strtok(NULL," \n")) != NULL){
                        push_arg_list(&arg_list, &capacity, &filled, token);
                }
                if( strcmp(arg_list[filled-1],"&") == 0){
                        background =1;
                        arg_list[filled-1] = NULL;
                        filled--;
                }
                if(arg2){
                        token = strtok(arg2," \n");
                        push_arg_list(&arg_list2, &capacity2, &filled2, token);
                        while( (token = strtok(NULL," \n")) != NULL){
                                push_arg_list(&arg_list2, &capacity2, &filled2, token);
                        }
                        if( strcmp(arg_list2[filled2-1],"&") == 0){
                                background =1;
                                arg_list[filled-1] = NULL;
                                filled--;
                        }
                }
                for(i=0;i<filled;i++){                  //print arg_list
                        printf("arg %d : %s\n",i,arg_list[i]);
                }
                for(i=0;i<filled2;i++){
                        printf("arg2 %d : %s\n",i,arg_list2[i]);
                }
                if(arg2){
                        pipe(fd);
                }
                switch(pid = fork()){
                        case 0:
                                if(arg2){
                                        close(fd[0]);
                                        dup2(fd[1],STDOUT_FILENO);
                                        close(fd[1]);
                                }
                                if(re_out){
                                        out = open(re_out,O_WRONLY|O_CREAT,0644);
                                        dup2(out,STDOUT_FILENO);
                                        close(out);
                                }
                                if(re_in){
                                        in = open(re_in,O_RDONLY);
                                        dup2(in,STDIN_FILENO);
                                        close(in);
                                }
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
                if(arg2){
                        switch(pid = fork()){

                                case 0:
                                        close(fd[1]);
                                        dup2(fd[0], STDIN_FILENO);
                                        close(fd[0]);
                                        exit(execvp(arg_list2[0],arg_list2));

                                case -1:
                                        printf("ERROR occurred when fork child process\n");
                                default:
                                        close(fd[0]);
                                        close(fd[1]);
                                        wait(pid,NULL,0);
                        }
                }
                free(arg_list);
                free(arg_list2);
                printf("> ");
        }
        check_bg_proc_list(bg_pid_list, &list_filled, 1);
        printf("\n");
        free(bg_pid_list);
        return 0;
}

