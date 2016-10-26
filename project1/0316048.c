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

int main(int argc, char ** argv){
        int i;
        char input[2048];
        char *token;
        char **arg_list;
        char file[257];
        int pid;
        unsigned int capacity,filled;
        printf("> ");
        while( fgets(input,sizeof(input),stdin) != NULL ){
                capacity = 10;
                filled = 0;
                arg_list = calloc(capacity, sizeof(char*));
                token = strtok(input," \n");
                push_arg_list(&arg_list, &capacity, &filled, token);
                while( (token = strtok(NULL," \n")) != NULL){
                        push_arg_list(&arg_list, &capacity, &filled, token);
                }
                for(i=0;i<filled;i++){                  //print arg_list
                        printf("arg %d : %s\n",i,arg_list[i]);
                }
                if(strstr(arg_list[0],"/") != NULL){
                        strcpy(file, arg_list[0]);

                }else{
                        sprintf(file,"/bin/%s",arg_list[0]);
                }
                printf("FILE %s\n",file);
                switch(pid = fork()){
                        case 0:
                                exit(execvp(file,arg_list));
                                
                        case -1:
                                printf("ERROR occurred when fork child process\n");
                                break;
                        default:
                                wait(NULL);
                }
                free(arg_list);
                printf("> ");
        }
        return 0;
}

