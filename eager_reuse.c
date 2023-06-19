#include "common.h"

int find_match_for_deep_search(uint64_t **match, Three_layers *base, Three_layers *structure) {
	int count = 0;
	int i = 0;
	Three_layers *temp = structure->next;
	while (temp) {
		if (base->middle == temp->first && base->last == temp->middle && temp->done == 0) {
			count++;
		}
		temp = temp->next;
	}
	if (count == 0) {
		*match = NULL;
		return 0;
	}
	*match = (uint64_t *)malloc(sizeof(uint64_t) * count);
	temp = structure->next;
	while (temp) {
		if (base->middle == temp->first && base->last == temp->middle) {
			(*match)[i++] = (uint64_t)temp;
		}
		temp = temp->next;
	}
	return count;

}

int deep_search(Three_layers *base, Three_layers *structure, Three_layers **end) {
	int i, count;
	int flag = 1;
	uint64_t **match = (uint64_t **)malloc(sizeof(uint64_t *));
	Three_layers *data;
	count = find_match_for_deep_search(match, base, structure);
	if ( count == 0) {
		free(match);
		return 0;
	}
	for (i = 0; i < count; i++) {
		Three_layers *temp = (Three_layers *)(*match)[i];
		data = (Three_layers *)malloc(sizeof(Three_layers));
		data->first = temp->first;
		data->middle = temp->middle;
		data->last = temp->last;
		data->done = 0;
		(*end)->next = data;
		*end = data;
		(*end)->next = NULL;

		temp->done = 1;
		deep_search(temp, structure, end);
	}
	if (*match != NULL) {
		free(*match);
		*match = NULL;
	}
	free(match);
	return 0;
}

void find_structure(Graph graph, Three_layers *s) {
	int i, j, k, first, middle, last;
	Three_layers *end = s;
	Three_layers *data;
	Node *first_node, *middle_node;

	for(i = 0; i < graph.nodenum; i++) {
		first = (*(graph.node + i)).ID;
		first_node = graph.node + first - 1;
		for(j = 0; j < first_node->adj_num; j++) {
			middle = *(first_node->adj_idx + j);
			middle_node = graph.node + middle - 1;
			for(k = 0; k < middle_node->adj_num; k++) {
				last = *(middle_node->adj_idx + k);
				data = (Three_layers *)malloc(sizeof(Three_layers));
				data->first = first;
				data->middle = middle;
				data->last = last;
				data->done = 0;
				end->next = data;
				end = data;
			}
		}
	}
	end->next = NULL;
}

static void reduce_adj_node_indegree(Graph *graph, Node *three_layer_node){
	int node_idx;

	for (int i = 0; i < three_layer_node->adj_num; i++){
		node_idx = *(three_layer_node->adj_idx + i) - 1;
		((*(graph->node + node_idx)).indegree)--;
	}
}

static void increase_adj_node_indegree(Graph *graph, Node *three_layer_node){
	int node_idx;

	for (int i = 0; i < three_layer_node->adj_num; i++){
		node_idx = *(three_layer_node->adj_idx + i) - 1;
		((*(graph->node + node_idx)).indegree)++;
	}
}
/*
static int reduce_ithStructure_adj_node_indegree(Graph *graph, Three_layers *structure, int idx, Node **first_node, Node **middle_node, Node **last_node) {
	int node_idx;
	int i;

	for (i = 0; i < idx; i++) {
		structure = structure->next;
	}
	if (structure == NULL) {
		return 1;
	}
	*first_node = graph->node + structure->first - 1;
	if ((*first_node)->fake_done == 0) {
		reduce_adj_node_indegree(graph, *first_node);
		(*first_node)->fake_done = 1;
	}
	*middle_node = graph->node + structure->middle - 1;
	if ((*middle_node)->fake_done == 0) {
		reduce_adj_node_indegree(graph, *middle_node);
		(*middle_node)->fake_done = 1;
	}
	*last_node = graph->node + structure->last - 1;
	if ((*last_node)->fake_done == 0) {
		reduce_adj_node_indegree(graph, *last_node);
		(*last_node)->fake_done = 1;
	}

	return 0;
}
*/

/* move the i_th three-layer structure to the head of link-list */
static int swap_ith2head(Three_layers **temp, Three_layers **front, Three_layers **s, int idx){
	int i;

	if (idx == 0) {
		return 1;
	}
	if (idx == 1) {
		*front = (*temp)->next;
		(*s)->next = (*front)->next;
		(*front)->next = *s;
		*s = *front;
		*temp = *s;
	} else {
		for(i = 0; i < idx; i++){
			*temp = (*temp)->next;
			if (i == idx - 2) {
				*front = *temp;
			}
		}
		if (*temp == NULL) {
			return 0;
		} else {
			(*front)->next = (*temp)->next;
			(*temp)->next = *s;
			*s = *temp;
		}
	}
	return 1;
}

int determine_execution_order_v1(Graph *graph, Three_layers **s) {
	int i = 1, j = 0;
	int node_idx;
	Three_layers *t = *s;
	Three_layers *front;
	Node *first_node, *middle_node, *last_node;

	while(1) {
		first_node = graph->node + t->first - 1;
		middle_node = graph->node + t->middle - 1;
		last_node = graph->node + t->last - 1;

		reduce_adj_node_indegree(graph,first_node);
		reduce_adj_node_indegree(graph,middle_node);
		reduce_adj_node_indegree(graph,last_node);

		if (first_node->indegree <= 0 && middle_node->indegree <= 0
				&& last_node->indegree <= 0){
			return 1;
		} else {
			if(!swap_ith2head(&t, &front, s, i)){
				break;
			}
			i++;
		}
		if (i > 20){
			printf("something wrong happen\n");
			break;
		}
	}
	return 0;
}

int determine_execution_order_v2(Graph *graph, Three_layers **s) {
	int i = 0, j = 0;
	int node_idx;
	Three_layers *t = *s;
	Three_layers *ith = *s;
	Three_layers *front;
	Node *first_node, *middle_node, *last_node;
	Graph temp_graph = *graph;
	Node *temp_node = (Node *)malloc(sizeof(Node) * graph->nodenum);
	for (j = 0; j < graph->nodenum; j++) {
		*(temp_node + j) = *(graph->node + j);
	}
	temp_graph.node = temp_node;

	while(1) {
		first_node = temp_graph.node + ith->first - 1;
		middle_node = temp_graph.node + ith->middle - 1;
		last_node = temp_graph.node + ith->last - 1;

		if (first_node->fake_done == 0) {
			reduce_adj_node_indegree(&temp_graph, first_node);
			first_node->fake_done = 1;
		}
		if (middle_node->fake_done == 0) {
			reduce_adj_node_indegree(&temp_graph, middle_node);
			middle_node->fake_done = 1;
		}
		if (last_node->fake_done == 0) {
			reduce_adj_node_indegree(&temp_graph, last_node);
			last_node->fake_done = 1;
		}
		
		if (first_node->indegree <= 0 && middle_node->indegree <= 0
				&& last_node->indegree <= 0 &&
				swap_ith2head(&t, &front, s, i)) {
			first_node = graph->node + ith->first - 1;
			middle_node = graph->node + ith->middle - 1;
			last_node = graph->node + ith->last - 1;

			if (first_node->fake_done == 0) {
				reduce_adj_node_indegree(graph, first_node);
				first_node->fake_done = 1;
			}
			if (middle_node->fake_done == 0) {
				reduce_adj_node_indegree(graph, middle_node);
				middle_node->fake_done = 1;
			}
			if (last_node->fake_done == 0) {
				reduce_adj_node_indegree(graph, last_node);
				last_node->fake_done = 1;
			}

			if (temp_node) {
				free(temp_node);
			}
			return 1;
		}
		i++;
		ith = ith->next;
		if (ith == NULL) {
			break;
		}
		if (i > 20) {
			printf("something wrong happen\n");
			break;
		}
	}
	if (temp_node) {
		free(temp_node);
	}
	return 0;
}
/*
int determine_execution_order_v3(Graph *graph, Three_layers **s) {
	int i = 0, j = 0;
	int node_idx;
	Three_layers *t = *s;
	Three_layers *ith = *s;
	Three_layers *front;
	Node *first_node, *middle_node, *last_node;
	Graph temp_graph = *graph;
	Node *temp_node = (Node *)malloc(sizeof(Node) * graph->nodenum);
	for (j = 0; j < graph->nodenum; j++) {
		*(temp_node + j) = *(graph->node + j);
	}
	temp_graph.node = temp_node;

	while(1) {
		first_node = temp_graph.node + ith->first - 1;
		middle_node = temp_graph.node + ith->middle - 1;
		last_node = temp_graph.node + ith->last - 1;

		if (first_node->fake_done == 0 && enhance_judge_dependency(temp_graph, first_node)) {
			reduce_adj_node_indegree(&temp_graph, first_node);
			first_node->fake_done = 1;
		}
		if (middle_node->fake_done == 0) {
			reduce_adj_node_indegree(&temp_graph, middle_node);
			middle_node->fake_done = 1;
		}
		if (last_node->fake_done == 0) {
			reduce_adj_node_indegree(&temp_graph, last_node);
			last_node->fake_done = 1;
		}
		
		if (first_node->indegree <= 0 && middle_node->indegree <= 0
				&& last_node->indegree <= 0 &&
				swap_ith2head(&t, &front, s, i)) {
			first_node = graph->node + ith->first - 1;
			middle_node = graph->node + ith->middle - 1;
			last_node = graph->node + ith->last - 1;

			if (first_node->fake_done == 0) {
				reduce_adj_node_indegree(graph, first_node);
				first_node->fake_done = 1;
			}
			if (middle_node->fake_done == 0) {
				reduce_adj_node_indegree(graph, middle_node);
				middle_node->fake_done = 1;
			}
			if (last_node->fake_done == 0) {
				reduce_adj_node_indegree(graph, last_node);
				last_node->fake_done = 1;
			}

			if (temp_node) {
				free(temp_node);
			}
			return 1;
		}
		i++;
		ith = ith->next;
		if (ith == NULL) {
			break;
		}
		if (i > 20) {
			printf("something wrong happen\n");
			break;
		}
	}
	if (temp_node) {
		free(temp_node);
	}
	return 0;
}
*/
int find_bottom(Graph *graph, int address, int idx) {
	int i;
	int temp = address;
	int addr_plus_size; 
	int diff = MAX;
	Node *cmp_node;
	Node *cur_node = graph->node + idx;
	address = address + (*(graph->node + idx)).size;

	for (i = 0; i < graph->nodenum; i++) {
		cmp_node = graph->node + i;

		if (cmp_node->done) {
			if (is_lifetime_overlap(cmp_node, cur_node)) {
				addr_plus_size = cmp_node->addr + cmp_node->size;

				if (address > addr_plus_size
						&& diff > address - addr_plus_size) {
					diff = address - addr_plus_size;
					temp = addr_plus_size;
				}
			}
		}
	}

	return temp;
}

void adjust_addr(Graph *graph, int address, int shift, int idx) {
	int i;
	Node *cmp_node;
	Node *cur_node = graph->node + idx;

	for (i = 0; i < graph->nodenum; i++) {
		cmp_node = graph->node + i;
		if (cmp_node->done) {
			if (is_lifetime_overlap(cmp_node, cur_node)) {
				if (address < cmp_node->addr){
					cmp_node->addr += shift;
				}
			}
		}
	}
	return;
}

int solve_conflict(int address, Graph *graph, int idx, int direction) {
	int i;
	int found_flag = 0;
	int min = MAX;
	int bottom;
	int addr_plus_size;
	Node *cmp_node;
	Node *cur_node = graph->node + idx;

	if (direction == 1) {
		for (i = 0; i < graph->nodenum; i++) {
			cmp_node = graph->node + i;
			if (cmp_node->done) {
				if (is_lifetime_overlap(cmp_node, cur_node)) {
					addr_plus_size = cmp_node->addr + cmp_node->size;
					if (address < addr_plus_size){
						address = addr_plus_size;
					}
				}
			}
		}
		return address;
	}

	if (direction == -1) {
		for (i = 0; i < graph->nodenum; i++){
			cmp_node = graph->node + i;
			if (cmp_node->done) {
				if (is_lifetime_overlap(cmp_node, cur_node)) {
					addr_plus_size = cmp_node->addr + cmp_node->size;
					int tmp = address + cur_node->size - 1;
					if (!(tmp >= addr_plus_size || tmp < cmp_node->addr)) {
						found_flag = 1;
					}
					if (min > cmp_node->addr){
						min = cmp_node->addr;	
					}
				}
			}
		}

		if (found_flag) {
			return min - cur_node->size;
		}

		bottom = find_bottom(graph, address, idx);
		if (address >= bottom) {
			return address;
		} 

		adjust_addr(graph, bottom, bottom - address, idx);

		return bottom;
	}
	return 0;
}


void eager_reuse_for_one_threelayer_structure(Graph *graph, Three_layers *s) {
	Node *node1, *node2;
	Node *first_node, *middle_node, *last_node;
	int t[4];
	int temp_addr;
	int i;

	first_node = graph->node + s->first - 1;
	middle_node = graph->node + s->middle - 1;
	last_node = graph->node + s->last - 1;

	if (first_node->tag != 0) {
		middle_node->tag = -1 * first_node->tag;
		last_node->tag = first_node->tag;
	} else if (middle_node->tag != 0) {
		first_node->tag = -1 * middle_node->tag;
		last_node->tag = -1 * middle_node->tag;
	} else if (last_node->tag != 0) {
		first_node->tag = last_node->tag;
		middle_node->tag = -1 * last_node->tag;
	} else {
		first_node->tag = 1;
		middle_node->tag = -1;
		last_node->tag = 1;
		first_node->addr = 0;
		first_node->done = 1;
	}

	if (first_node->done) {
		t[0]= s->middle - 1;
		t[1] = s->first - 1;
		t[2] = s->last - 1;
		t[3] = s->middle - 1;
	} else if (middle_node->done) {
		t[0] = s->first - 1;
		t[1] = s->middle - 1;
		t[2] = s->last - 1;	
		t[3] = s->middle - 1;
	} else if (last_node->done) {
		t[0] = s->middle - 1;
		t[1] = s->last - 1;
		t[2] = s->first - 1;	
		t[3] = s->middle - 1;
	} else {
		printf("something wrong happen\n");
		return;
	}

	for (i = 0; i < 4; i += 2) {
		node1 = graph->node + t[i];
		node2 = graph->node + t[i + 1];

		if (node1->done == 0) {
			if (node2->tag == 1) {
				temp_addr = node2->addr + node2->size;
				node1->addr = solve_conflict(temp_addr, graph, t[i], 1);
			} else {
				temp_addr = node2->addr - node1->size;
				node1->addr = solve_conflict(temp_addr, graph, t[i], -1);
			}
			node1->done = 1;
		}
	}

	return;
}
