#include <iostream>
#include <fstream>

using namespace std;
    
typedef enum {FIFO , LRU} Policy_t;

struct List_node{
    long addr;
    struct List_node *prev;
    struct List_node *next;
    struct Tree_node *tree_node;
};

struct Tree_node{
    long addr;
    struct List_node *ptr;
    struct Tree_node *Rchild;
    struct Tree_node *Lchild;
};

class Memory{
    Memory(){
        root = NULL;
        head = NULL;
        tail = NULL;
        capacity = 0;
    }
    void set_policy(Policy_t p){
        policy = p;
    }
    void set_capacity(int c){
        capacity = c;
    }
    int use(long addr);
    struct List_node *find(long addr);
    void update_list(struct List_node *recent_use);
    void new_frame(long addr);
private:
    Policy_t policy;
    int capacity;
    struct Tree_node *root;
    struct List_node *head;
    struct List_node *tail;
};


int Memory::use(long addr){
    struct List_node *recent_use = find(addr);
    if(recent_use){ //hit
        update_list(recent_use);
        return 1;
    }else{          //miss
        new_frame(addr);        
        return 0;
    }
}

struct List_node *Memory::find(long addr){
    struct Tree_node *cur=root;
    while(cur != NULL){
        if(addr == cur->addr){
            return cur->ptr;
        }else if(addr < cur->addr){
            cur = cur->Lchild;
        }else{
            cur = cur->Rchild;
        }
    }
    return NULL;
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

void Memory::new_frame(long addr){

}

int main(){
    fstream fp;
    
}

