#include<stdlib.h>
#include<stdio.h>
#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<wait.h>
#include<sys/time.h>
#include<sys/resource.h>

#define RECORD_SIZE 4096
#define TABLE_SIZE 65536*27 


typedef struct record{
    char key[5];
    int start;
    int end;
}record_t;

record_t Table[65536];

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

int search(char* addr , int leng, char*keyword, int start, int end, int index){

    int low=Table[index].start,  high= Table[index].end-1, ret;
    static char buffer[4100];
    static char tmp[5];
    tmp[4] = '\0';
    while (low <= high){
        int mid = (low + high) / 2;
        strncpy(tmp,(addr+mid*RECORD_SIZE),4);
        ret = strcmp(tmp, keyword);
        if (ret == 0){
            memset(buffer, '\0', sizeof(buffer));
            strncpy(buffer, addr + RECORD_SIZE * mid + 4 + start,  end - start + 1);
            printf("key %s found : %s\n", keyword, buffer);
            return mid;
        } else if (ret > 0){
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    printf("key %s not found\n", keyword);
    return -1;
}
void print_max_rss(){
    struct rusage r_stat;
    getrusage(RUSAGE_SELF, &r_stat);
    printf("max rss = %ld\n", r_stat.ru_maxrss);    
}

int main(int argc, char* argv[]){
    char *addr;
    int dataset_fd;
    struct stat sb;
    int N, start, end, ret;
    FILE *testcase ;
    char key[5];
    {
        int i;
        FILE *fp;
        fp = fopen("new_data.txt","r");
        for(i=0;i<65536;i++){
            fscanf(fp,"%4s %10d %10d\n",Table[i].key,&Table[i].start,&Table[i].end);
        }
        fclose(fp);
    }
    
    key[4] = '\0';
    dataset_fd = open("new_data.txt", O_RDWR);
    testcase = fopen(argv[1], "r");
    fstat(dataset_fd,&sb);
    N = sb.st_size; 
    addr =  mmap(NULL, N-TABLE_SIZE , PROT_READ| PROT_WRITE , MAP_PRIVATE ,dataset_fd ,TABLE_SIZE);
    while( fscanf(testcase, "%4s %d %d\n", key, &start, &end)!= EOF){
     
        int index = get_key_index(key);
        search(addr,N-TABLE_SIZE, key, start , end, index);
    }
    munmap(addr, N-TABLE_SIZE);
    print_max_rss();
    close(dataset_fd);
    return 0;

}
