#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>

#define RECORD_SIZE 4096

typedef struct record{
    char key[5];
    int start;
    int end;
}record_t;

int c_to_i(char c){
    if( c >= '0' && c <= '9'){
        return c - '0';
    }
    switch(c){
    case 'a':
        return 10;
    case 'b':
        return 11;
    case 'c':
        return 12;
    case 'd':
        return 13;
    case 'e':
        return 14;
    case 'f':
        return 15;
    }
}

int get_key_index(char *key){
    int i;
    int index=0;
    for(i=0;i<4;i++){
        index *= 16;
        index += c_to_i(key[i]);
    }
    return index;
}

int main(){
    int fd;
    char tmp[5];
    char cur_key[5];
    struct stat sb;
    int N;
    char *addr;
    int len;
    int start,end;
    int cur_idx;
    record_t Table[65536];
    int record_idx;
    FILE *ofile;
    int i;
    
    for(i=0;i<65536;i++){
        Table[i].key[0] = '\0';
        Table[i].start = -1;
        Table[i].end = -1;
    }

    tmp[4] = cur_key[4] = '\0';
    fd = open("data.txt", O_RDWR);
    fstat(fd,&sb);
    N = sb.st_size;
    addr = mmap(NULL,N,PROT_READ |PROT_WRITE, MAP_PRIVATE, fd, 0);
    
    record_idx = 0;
    len = N / RECORD_SIZE;
    cur_idx = 0;

    strncpy(tmp,(addr+cur_idx*RECORD_SIZE),4);
    strncpy(cur_key,tmp,4);
    record_idx = get_key_index(cur_key);
    Table[record_idx].start = cur_idx;
    strncpy(Table[record_idx].key,cur_key,5);

    while(cur_idx < len){
        strncpy(tmp,(addr+cur_idx*RECORD_SIZE),4);
        if(cur_idx < 15){
            printf("%s \n",tmp);
        }
        if( strcmp(tmp,cur_key) != 0){
            Table[record_idx].end = cur_idx; //old key's end

            strcpy(cur_key,tmp);
            record_idx = get_key_index(cur_key);

            Table[record_idx].start = cur_idx;
            strncpy(Table[record_idx].key,cur_key,5);
        }
        cur_idx++;
    }

    ofile = fopen("new_data.txt","w");
    for(i=0;i<65536;i++){
        fprintf(ofile,"%4s %10d %10d\n",Table[i].key,Table[i].start,Table[i].end);
    }
    for(i=0;i<N;i++){
        fprintf(ofile,"%c",addr[i]);
    }
    for(i=0;i<25;i++){
        printf("Key %3d %4s %10d %10d\n",i,Table[i].key,Table[i].start,Table[i].end);
    }
    munmap(addr,N);
    fclose(ofile);
    close(fd); 

    return 0;

}
