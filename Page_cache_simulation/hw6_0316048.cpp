#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace std;
    
typedef enum {FIFO , LRU} Policy_t;

struct List_node{
    List_node(){
        prev = NULL;
        next = NULL;
        tree_node = NULL;
    }
    struct List_node *prev;
    struct List_node *next;
    struct node *tree_node;
};

struct node{
    node(){
        list_node = NULL;
    }
    char *addr;
    struct List_node *list_node;
};

class Memory{
public:
    Memory(){
        table = NULL;
        head = NULL;
        tail = NULL;
        capacity = 0;
        filled = 0;
    }
    void set_policy(Policy_t p){
        policy = p;
    }
    void set_capacity(int c){
        capacity = c;
        table = new node[capacity];
    }
    int use(char *addr);
    struct List_node *find(char *addr);
    int hash(char *addr);
    void update_list(struct List_node *recent_use);
    void new_frame(char *addr);
    void delete_frame();
private:
    Policy_t policy;
    int capacity;
    int filled;
    struct node *table;
    struct List_node *head;
    struct List_node *tail;
};


int Memory::use(char *addr){
    struct List_node *recent_use = find(addr);
    if(recent_use){ //hit
        update_list(recent_use);
        free(addr);
        return 1;
    }else{          //miss
        new_frame(addr);        
        return 0;
    }
}

struct List_node *Memory::find(char *addr){
    int key = hash(addr);
    int cur;
    if(table[key].list_node && strcmp(table[key].addr,addr) == 0){
        return table[key].list_node;
    }
    cur = (key + 1) % capacity;
    while(cur != key){
        if(table[cur].list_node == NULL){
            break;
        }else if(strcmp(table[cur].addr,addr) == 0){
            return table[cur].list_node;
        }
        cur = (cur+1) % capacity;
    }
    return NULL;
}

int Memory::hash(char *addr){
    int sum=0;
    for(int i=0;i<5;i++){
        sum += addr[i];
    }
    return sum % capacity;
}

void Memory::update_list(struct List_node *recent_use){
    switch(policy){
    case LRU:
        if(recent_use->prev){
            recent_use->prev->next = recent_use->next;
            if(recent_use->next){
                recent_use->next->prev = recent_use->prev;
            }else{      //recent use is tail
                tail = recent_use->prev;
            }
            recent_use->prev = NULL;
            recent_use->next = head;
            head = recent_use;
        }
        break;
    case FIFO:      //no need to update list
        break;
    }
}

void Memory::new_frame(char *addr){
    if(filled == capacity){
        delete_frame();
    }
    int key = hash(addr);
    int cur=key;
    if(table[cur].list_node != NULL){
        do{
            cur = (cur+1) % capacity;
            if(table[cur].list_node == NULL){
                break;
            }
        }while(cur != key);
    }
    struct List_node *new_L = new List_node;
    table[cur].list_node = new_L;
    table[cur].addr = addr;
    new_L->tree_node = &table[cur];
    if(head == NULL && tail == NULL){
        head = new_L;
        tail = new_L;
    }else{
        head->prev = new_L;
        new_L->next = head;
        head = new_L;
    }
    filled++;
}

void Memory::delete_frame(){
    struct node *t_node = tail->tree_node;
    struct List_node *new_tail = tail->prev;
    t_node->list_node = NULL;
    delete [] t_node->addr;
    delete tail;
    if(!new_tail){ //empty
        head = NULL;
    }
    tail = new_tail;
    filled--;
}

int main(){
    Memory mem;
    char type,comma;
    char addr[20];
    int size;
    fstream fp("trace.txt",ios::in);    
    int hit_count,miss_count;
    hit_count = miss_count = 0;
    mem.set_policy(LRU);
    mem.set_capacity(64);
    
    while(fp >> type >> addr){
        if(mem.use(strndup(addr,5))){
            hit_count++;
        }else{
            miss_count++;
        }
    }
    cout << "total hit"<<hit_count<<endl;
    cout << "total miss"<<miss_count <<endl;
    fp.close();
    return 0;
}

