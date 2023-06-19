#include "common.h"

int global_optimization(Graph *graph, int step) 
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

void full_permutation(Global_opt *Opt, Graph graph, int level)
{
	int i, j;
	if (Opt->count++ > 36288000) return;
	if (level >= Opt->nodenum)
	{
		int memory_size = 0;
		Node cur_node;
		int this_top;
		graph.idx = Opt->result;
		for (j = 0; j < Opt->nodenum; j++) {
			if (global_optimization(&graph, j)) {
				return;
			}
			cur_node = *(graph.node + *(graph.idx + j));
			this_top = cur_node.addr + cur_node.size;
			if (memory_size < this_top) {
				memory_size = this_top;
			}
		}
		if (Opt->memory_size > memory_size) {
			Opt->memory_size = memory_size;
			Opt->graph = graph;
			for (j = 0; j < graph.nodenum; j++) {
				*(Opt->node + j) = *(graph.node + j);
			}
		}
		Opt->count++;
		return;
	}
	for (i = 0; i < Opt->nodenum; i++)
	{
		if (Opt->flag[i] == 0)
		{
			Opt->flag[i] = 1;
			Opt->result[level++] = Opt->index[i];
			full_permutation(Opt, graph, level);
			level--;
			Opt->flag[i] = 0;
		}
	}
}

int sum_permutation(int N)
{
	if (N == 1)
		return N;
	return sum_permutation(N - 1) * N;

}
