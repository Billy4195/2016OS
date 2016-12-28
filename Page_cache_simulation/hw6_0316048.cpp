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
    struct Tree_node *tree_node;
};

struct Tree_node{
    Tree_node(char *a){
        addr = a;
        list_node = NULL;
        parent = NULL;
        Rchild = NULL;
        Lchild = NULL;
    }
    char *addr;
    struct List_node *list_node;
    struct Tree_node *parent;
    struct Tree_node *Rchild;
    struct Tree_node *Lchild;
};

class Memory{
public:
    Memory(){
        root = NULL;
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
    }
    int use(char *addr);
    struct List_node *find(char *addr,struct Tree_node **parent);
    void update_list(struct List_node *recent_use);
    void new_frame(char *addr,struct Tree_node *parent);
    void delete_frame();
private:
    Policy_t policy;
    int capacity;
    int filled;
    struct Tree_node *root;
    struct List_node *head;
    struct List_node *tail;
};


int Memory::use(char *addr){
    struct Tree_node *parent;
    struct List_node *recent_use = find(addr,&parent);
    if(recent_use){ //hit
        update_list(recent_use);
        free(addr);
        return 1;
    }else{          //miss
        new_frame(addr,parent);        
        return 0;
    }
}

struct List_node *Memory::find(char *addr,struct Tree_node **parent){
    struct Tree_node *cur=root;
    int ret;
    *parent = root;
    while(cur != NULL){
        ret = strcmp(addr,cur->addr);
        if(ret == 0){
            return cur->list_node;
        }else if(ret < 0){
            *parent = cur;
            cur = cur->Lchild;
        }else{
            *parent = cur;
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

void Memory::new_frame(char *addr,struct Tree_node *parent){
    if(filled == capacity){
        delete_frame();
    }
    struct Tree_node *new_T;
    struct List_node *new_L;
    new_T = new struct Tree_node(addr);
    new_L = new struct List_node();
    if(parent == NULL){
        root = new_T;
    }else{
        if(strcmp(addr,parent->addr) < 0){
            parent->Lchild = new_T;
        }else{
            parent->Rchild = new_T;
        }
    }
    new_T->parent = parent;
    new_T->list_node = new_L;
    new_L->tree_node = new_T;
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
    struct Tree_node *t_node = tail->tree_node;
    struct Tree_node *successor=NULL;
    struct List_node *new_tail = tail->prev;
    delete tail;
        
    if(t_node->Lchild){
        successor = t_node->Lchild;
        while(successor->Rchild != NULL){
            successor = successor->Rchild;
        }
        successor->parent->Rchild = successor->Lchild;
    }else if(t_node->Rchild){
        successor = t_node->Rchild;
        while(successor->Lchild != NULL){
            successor = successor->Lchild;
        }
        successor->parent->Lchild = successor->Rchild;
    }
    if(successor && successor->Rchild){
        successor->Rchild->parent = successor->parent;
    }
    if(!t_node->parent){
        root = successor;
    }else if(t_node->parent->Rchild == t_node){
        t_node->parent->Rchild = successor;
    }else if(t_node->parent->Lchild == t_node){
        t_node->parent->Lchild = successor;
    }
    if(successor){
        successor->Rchild = t_node->Rchild;
        successor->Lchild = t_node->Lchild;
    }
    free(t_node->addr);
    delete t_node;
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
    mem.set_policy(FIFO);
    mem.set_capacity(64);
    
    while(!fp.eof()){
        fp >> type >> addr; 
        cout << hit_count << " "<<miss_count <<endl;
//        cout << type <<" "<<  addr.substr(0,5) << endl;
        if(mem.use(strndup(addr,5))){
            hit_count++;
//            cout << "Hit "<<addr.substr(0,5)<<endl;
        }else{
            miss_count++;
//            cout << "Miss "<<addr.substr(0,5)<<endl;
        }
//        cout <<endl;
    }
    cout << "total hit"<<hit_count<<endl;
    cout << "total miss"<<miss_count <<endl;
    fp.close();
    return 0;
}

