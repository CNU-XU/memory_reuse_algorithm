#include <stdbool.h>
#include "shortlifetimefirst.h"

static inline void swap_short(Node_t* a,Node_t* b){
    Node_t temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

static inline int calLifetime(int ID,int end){
    return end-ID+1;
}

static void bubbleSortSmallToLarge(Node_t *array,int len) {
    Node_t temp;
    for (int i = 0; i < len - 1; i++) {
        for (int j = 0; j < len - 1 - i; j++) {
            int lifetime= calLifetime(array[j]->ID,array[j]->end);
            int lifetimeNext= calLifetime(array[j+1]->ID,array[j+1]->end);
            if (lifetime > lifetimeNext) {
                swap_short(&array[j],&array[j+1]);
            }
            //When the lifetime lengths of two nodes are equal, the one whose lifetime starts earlier will allocate memory first
            else if(lifetime == lifetimeNext){
                if(array[j]->ID > array[j + 1]->ID)
                    swap_short(&array[j],&array[j+1]);
            }
        }
    }
}

static bool checkPlaceRight(mem_* mem,int highMark,Node_t node){
    addr_frag* frag;
    for(int i=node->ID;i<=node->end;i++){
        frag=mem[i-1].list;
        while (!(frag->start<=highMark&& highMark <= frag->end)) {
            if(!frag->next){return false;}
            frag = frag->next;
        }
        if(!frag){
            return false;}
        if(frag->end-frag->start<node->size){
            return false;}
    }
    return true;
}

static void findHighestAddr(mem_* mem,int *highMark,Node_t node){
    addr_frag* frag;
    for(int i=node->ID;i<=node->end;i++) {
        frag = mem[i-1].list;
        while(frag->next){
            frag=frag->next;//find last frag
        }
        if(*highMark<frag->start)
            *highMark=frag->start;
    }
}

static void updateMemInfo(mem_* mem,int highMark,Node_t node) {
    addr_frag* frag,next;
    for(int i=node->ID;i<=node->end;i++) {
        frag = mem[i-1].list;
        while (!(frag->start <= highMark && highMark <= frag->end)) {
            frag = frag->next;
        }
        if(highMark==frag->start){
            frag->start+=node->size;
        }
        else{
            addr_frag* new_frag=(addr_frag*) malloc(sizeof (addr_frag));
            new_frag->next=frag->next;
            frag->next=new_frag;
            new_frag->end=frag->end;
            frag->end=highMark;
            new_frag->start=highMark+node->size;
        }
    }
}

void memAlloc(Node_t *array,int len){

    mem_ mem[len];
    for(int i=0;i<len;i++){
        mem[i].list= (addr_frag *)malloc(sizeof(addr_frag));
        mem[i].list->start=0;
        mem[i].list->end=MAX;
    }
    int highMark;
    addr_frag *frag;

    for(int i=0;i<len-1;i++){
        highMark=0;
        for(int j=array[i]->ID;j<=array[i]->end;j++) {
            frag = mem[j-1].list;
            while (frag->end - frag->start < array[i]->size) {
                frag = frag->next;
            }
            if (frag->start > highMark)
                highMark = frag->start;
        }
        if(!checkPlaceRight(mem,highMark,array[i])){
            findHighestAddr(mem,&highMark,array[i]);
        }
        array[i]->addr=highMark;
        updateMemInfo(mem,highMark,array[i]);
    }

}

int checkMemAllocRight(Node_t *array,int len){
    int totalMem=0;
    for(int i=0;i<len-1;i++){
        if(array[i]->addr+array[i]->size>totalMem){
            totalMem=array[i]->addr+array[i]->size;
        }
    }
    return totalMem;
}

Node_t *sortTensorLifetimeFromSmallToLarge(Graph_t graph){

    if(!graph)
        return NULL;
    if(graph->nodenum<3)
        return &graph->node;

    Node_t *array=(Node_t *)malloc((graph->nodenum-1)*sizeof(Node_t));

    for(int i=0;i< graph->nodenum-1;i++){
        array[i]=(graph->node+i);
    }

    bubbleSortSmallToLarge(array,graph->nodenum-1);

    return  array;
}