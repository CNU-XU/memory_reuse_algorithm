#include "common.h"

int createGraphByFile(Graph *graph,char *filename) {
	int i, j;
	FILE *fp;
	Node *node, *tmp_node;
	int node_num = 0;
	int edge_num = 0;
	int num1, num2, adj_num;
	char line[128];
	fp = fopen(filename, "r");
	/* get the number of nodes and edges */
	while(fgets(line,sizeof(line),fp)) {
		sscanf(line,"%d %d %d",&num1,&num2,&adj_num);
		edge_num += adj_num;
		node_num++;
	}
	printf("node_num = %d, edge_num = %d\n", node_num, edge_num);
	graph->nodenum = node_num;
	graph->edgenum = edge_num;

	rewind(fp);

	graph->node = (Node *)calloc(graph->nodenum, sizeof(Node));

	i = 0;
	while(1){
		node = graph->node + i;
		fscanf(fp,"%d", &node->ID);
		fscanf(fp,"%d", &node->size);
		fscanf(fp,"%d", &node->adj_num);
		node->end = node->ID; //assume schedule according to breadth first

		if (node->adj_num) {
			node->adj_idx = (int *)calloc(node->adj_num, sizeof(int));
			for (j = 0; j < node->adj_num; j++){
				fscanf(fp,"%d", &node->adj_idx[j]);
				tmp_node = graph->node + node->adj_idx[j] - 1;
				tmp_node->indegree++;
			}
			node->end = node->adj_idx[node->adj_num - 1];
		}
		i++;
		if (i >= graph->nodenum) {
			break;
		}
	}

	graph->origin = (Node *)calloc(graph->nodenum, sizeof(Node));
	for (i = 0; i < graph->nodenum; i++) {
		*(graph->origin + i) = *(graph->node + i);
	}
	fclose(fp);
	return 0;
}

void output_graph2file(Graph graph, FILE *fp) {
	int i, j;
	Node *node;
	fprintf(fp, "----------------output the graph in adjacency list----------------\n");
	for(i = 0; i < graph.nodenum; i++) {
		node = graph.node + i;
		fprintf(fp, " [%d]", node->ID);
		for(j = 0; j < node->adj_num; j++) {
			fprintf(fp, "----->");
			fprintf(fp, "[%d]", node->adj_idx[j]);
		}
		fprintf(fp, "\n");
	}
}


/* judge if current node overlaps compared node in their output tensors' lifetime */
int is_lifetime_overlap(Node *cmp_node, Node *cur_node)
{
	if (cmp_node->end < cur_node->ID 
			|| cur_node->end <cmp_node->ID){
		return 0;
	}

	return 1;

}

void swap(int* a,int* b){
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void dump_result(FILE *fp, Graph *graph, struct timeval tv[2],
		int memory_size, int min, enum algorithmType type)
{
	Node *node;
	int i = 0;
	char algorithm[40];

	switch(type) {
		case large_tensor_first_v1_type:
			sprintf(algorithm, "Large tensor first v1\t");
			break;
		case large_tensor_first_v2_type:
			sprintf(algorithm, "Large tensor first\t");
			break;
		case short_lifetime_first_type:
			sprintf(algorithm, "Short lifetime first\t");
			break;
		case eager_reuse_type:
			sprintf(algorithm, "Eager Reuse\t\t");
			break;
		case global_optimization_type:
			sprintf(algorithm, "Global Optimization\t");
			break;

	}

	fprintf(fp, "\n%s:\n", algorithm);

	for (i = 0; i < graph->nodenum; i++){
		node = graph->node + i;
		if (min < 0) {
			(*(graph->node + i)).addr -= min;
		}
		fprintf(fp, "Tensor_ID[%2d]\tBase_address[%2d]\tBase+Size[%2d]\tlifetime_begin[%2d]\tlifetime_end[%2d]\n",
				i + 1,
				node->addr,
				node->addr + node->size,
				node->ID, node->end);
	}

	fprintf(fp, "%s Result:\tmemory_size=%d\toverhead=%ld us\n\n", 
			algorithm,
			memory_size - min,
			(tv[1].tv_sec - tv[0].tv_sec) * 1000000 + tv[1].tv_usec - tv[0].tv_usec);
}
