#include "common.h"

void sort_large_first(Graph *graph)
{
	int i, j;

	int *num = (int *)malloc(sizeof(int) * graph->nodenum);
	graph->idx = (int *)malloc(sizeof(int) * graph->nodenum);
	for (i = 0; i < graph->nodenum; i++){
		*(num + i) = (*(graph->node + i)).size;
		*(graph->idx + i) = i;
	}
	for (j = 0; j < graph->nodenum; j++){
		for(i = 0; i < graph->nodenum - 1 - j; i++){
			if(*(num + i) < *(num + i + 1)){
				swap((num + i),(num + i + 1));
				swap((graph->idx + i),(graph->idx + i+1));
			}
		}
	} 
	free(num);
}


int large_tensor_first_v1(Graph *graph, int step)
{
	Node *cmp_node, *cur_node;
	int i = 0;
	int max = 0;

	cur_node = graph->node + *(graph->idx + step);
	for (i = 0; i < step; i++) {
		cmp_node = graph->node + *(graph->idx + i);
		if (is_lifetime_overlap(cmp_node, cur_node)) {
			if (max < cmp_node->addr + cmp_node->size) {
				max = cmp_node->addr + cmp_node->size;			
			}
		}
	}
	cur_node->addr = max;
	return 0;
}

int large_tensor_first_v2(Graph *graph, int step)
{
	Node *cmp_node, *tmp_node,*cur_node;
	int i, j, flag;
	int top = MIN;
	int bottom = MAX;
	int *p = NULL;
	int overlap_count = 0;
	int temp0, temp1;

	cur_node = graph->node + *(graph->idx + step);

	if (!step) 
		goto final;

	// get the number of overlap tensors, bottom and top of allocated tensors address
	for (i = 0; i < step; i++){
		cmp_node = graph->node + *(graph->idx + i);
		if (is_lifetime_overlap(cmp_node, cur_node)) {
			if (top < cmp_node->addr + cmp_node->size) {
				top = cmp_node->addr + cmp_node->size;
			}
			if (bottom > cmp_node->addr) {
				bottom = cmp_node->addr;
			}
			overlap_count++;
		}
	}
	if (bottom >= cur_node->size){
		top = 0;
		goto final;
	}

	if (overlap_count > 1 && (top - bottom) >= cur_node->size){
		p = (int *)calloc(overlap_count, sizeof(int));
		j = 0;
		for (i = 0; i < step; i++){
			cmp_node = graph->node + *(graph->idx + i);
			if (is_lifetime_overlap(cmp_node, cur_node)) {
				*(p + j++) = i;
			}
		}
	} else {
		goto final;
	}

	for (i = overlap_count - 1; i >= 0; i--) {
		tmp_node = graph->node + *(graph->idx + *(p + i));
		flag = 1;
		temp0 = tmp_node->addr + tmp_node->size;
		temp1 = temp0 + cur_node->size;

		for (j = overlap_count - 1; j >= 0; j--){
			cmp_node = graph->node + *(graph->idx + *(p + j));
			if (temp0 < cmp_node->addr + cmp_node->size &&
				       	temp1 >= cmp_node->addr) {
				flag = 0;
			}
			if (temp0 == top) {
				flag = 0;
			}
		}
		if (flag){
			top = temp0;
			break;
		}
	}

final:
	if (p) {
		free(p);
	}
	cur_node->addr = top;
	return 0;
}
