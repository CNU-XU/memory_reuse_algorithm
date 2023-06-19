#include "common.h"

typedef Graph* Graph_t;

typedef Node* Node_t;

typedef struct addr_frag{
    int start;
    int end;
    struct addr_frag* next;
}addr_frag;

typedef struct{
    addr_frag *list;
}mem_;

void memAlloc(Node_t *array,int len);

int checkMemAllocRight(Node_t *array,int len);

Node_t *sortTensorLifetimeFromSmallToLarge(Graph_t graph);

