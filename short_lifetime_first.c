#include "common.h"

void sort_short_lifetime(Graph *graph)
{
	int i, j;
	int *num;

	num = (int *)malloc(sizeof(int) * graph->nodenum);
	graph->idx = (int *)malloc(sizeof(int) * graph->nodenum);

	for (i = 0; i < graph->nodenum; i++){
		*(num + i) = (*(graph->node + i)).end - (*(graph->node + i)).ID;
		*(graph->idx + i) = i;
	}

	for (j = 0; j < graph->nodenum; j++){
		for(i = 0; i < graph->nodenum - 1 - j; i++){
			if(*(num + i) > *(num + i + 1)){
				swap((num + i),(num + i + 1));
				swap((graph->idx + i),(graph->idx + i+1));
			}
		}
	} 

	free(num);

	return;
}


int short_lifetime_first(Graph *graph, int step) 
{
	Node *cmp_node, *cur_node;
	int i;
	int max = 0;

	cur_node = graph->node + *(graph->idx + step);

	for (i = 0; i < step; i++) {
		cmp_node = graph->node + *(graph->idx + i);
		if (is_lifetime_overlap(cmp_node, cur_node)) {
			if (max < cmp_node->addr + cmp_node->size) {
				max = cmp_node->addr + cmp_node->size;}
		}
	}

	cur_node->addr = max;

	return 0;
}


