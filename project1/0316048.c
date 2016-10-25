#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void push_arg_list(char ***arg_list, unsigned int *capacity, unsigned int *filled, char *token){
        char ** tmp_list;
        if(*filled == *capacity){
                tmp_list = malloc( sizeof(char*) * (*capacity+10) );
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
        char **arg_list=malloc( sizeof(char*) * 10);
        unsigned int capacity,filled;
        capacity = 10;
        filled = 0;
        printf("> ");
        while( fgets(input,sizeof(input),stdin) != NULL ){
                token = strtok(input," ");
                push_arg_list(&arg_list, &capacity, &filled, token);
                while( (token = strtok(NULL," \n")) != NULL){
                        push_arg_list(&arg_list, &capacity, &filled, token);
                }
                for(i=0;i<filled;i++){
                        printf("arg %d : %s\n",i,arg_list[i]);
                }
                filled = 0;
                printf("> ");
        }
        printf("\n");
        free(arg_list);
}

