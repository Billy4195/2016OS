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
    void use(long addr);
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


void Memory::use(long addr){
    struct List_node *recent_use = find(addr);
    if(recent_use){
        update_list(recent_use);
    }else{
        new_frame(addr);        
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

}

void Memory::new_frame(long addr){

}
int main(){
    fstream fp;
    
}

