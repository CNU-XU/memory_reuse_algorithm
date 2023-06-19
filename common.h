#ifndef _COMMON_H_
#define _COMMON_H_
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX 2147483647
#define MIN 0

typedef struct Node_st{
	int ID;
	int *adj_idx;
	int adj_num;
	int indegree;
	int end; 
	int done;
	int size;
	int addr;
	int tag;
	int fake_done;
} Node;

typedef struct graph_st{
	Node *node;
	Node *origin;
	int nodenum;
	int edgenum;
	int *idx;
} Graph;

typedef struct three_layers_st{
	int first;
	int middle;
	int last;
	struct three_layers_st *next;
	int done;
} Three_layers;

enum algorithmType{
    eager_reuse_type,
    large_tensor_first_v1_type,
    large_tensor_first_v2_type,
    short_lifetime_first_type,
    global_optimization_type,
};

typedef struct global_opt_st{
	int *flag;
	int *result;
	int memory_size;
	uint64_t count;
	int nodenum;
	int *index;
	Graph graph;
	Node *node;
} Global_opt;

int is_lifetime_overlap(Node *cmp_node, Node *cur_node);

int deep_search(Three_layers *base, Three_layers *structure, Three_layers **end);

void find_structure(Graph graph, Three_layers *s);

int determine_execution_order_v1(Graph *graph, Three_layers **s);

int determine_execution_order_v2(Graph *graph, Three_layers **s);

void eager_reuse_for_one_threelayer_structure(Graph *graph, Three_layers *s);

void output_graph2file(Graph graph, FILE *fp);

void sort_large_first(Graph *graph);

int large_tensor_first_v1(Graph *graph, int step);

int large_tensor_first_v2(Graph *graph, int step);

void sort_short_lifetime(Graph *graph);

int short_lifetime_first(Graph *graph, int step);

int sum_permutation(int N);

void full_permutation(Global_opt *Opt, Graph graph, int level);

int global_optimization(Graph *graph, int step);

void swap(int* a,int* b);

int createGraphByFile(Graph *a,char *filename);

void dump_result(FILE *fp, Graph *graph, struct timeval tv[2], int memory_size, int min, enum algorithmType type);

#endif
