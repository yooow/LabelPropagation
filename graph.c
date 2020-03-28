#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"


# define HIGH_PROBABILITY ((float)0.3)
# define LOW_PROBABILITY ((float)0.001)
# define GRAPH_SIZE (400)
# define CLUSTERS_COUNT (4)

graph_t* new_graph(int n) {
	graph_t* g = malloc(sizeof(graph_t));

	g->nb_nodes = n;
	g->nodes = calloc(n, sizeof(node_t));

	for (int i=0; i<g->nb_nodes; ++i) {
		g->nodes[i].label = i;
		g->nodes[i].value = i;
	}
	return g;
}

void save_to(graph_t* g, char *filename) {
	FILE *file = fopen(filename, "w");

	fprintf(file, "graph my_graph\n{\n");
	for (int i = 0; i < g->nb_nodes; ++i) {
		int label = g->nodes[i].label;
		int r = (label * 12) & 0xff;
		int v = (label * 490) & 0xff;
		int b = (label * 125) & 0xff;
		int color = r << 16 | v << 7 | b;

		fprintf(file, "\t%d [fontcolor=\"#%.6x\" color=\"#%.6x\"];\n", i, color, color);

		nodelist_t* voisin = g->nodes[i].voisins;
		while (voisin != NULL) {
			int voisin_label = voisin->node->label;
			int r = (voisin_label * 12) & 0xff;
			int v = (voisin_label * 490) & 0xff;
			int b = (voisin_label * 125) & 0xff;
			int voisin_color = r << 16 | v << 7 | b;
			fprintf(file, "\t%d -- %d [color=\"#%.6x;0.5:#%.6x\"];\n", i, voisin->node->value, color, voisin_color);
			voisin = voisin->next;
		}
	}
	fprintf(file, "}");

	fclose(file);
}

void print_graph(graph_t* g, int affiche_voisins) { //affichage console du graphe
	for (int i=0; i<g->nb_nodes; ++i) {
		node_t* n = &g->nodes[i];
		printf("node %d; label: %d\n", i, n->label);
		if (affiche_voisins)  {
			nodelist_t* voisin = n->voisins;

			while (voisin != NULL) {
				printf("=> %d - %d\n", voisin->node->value, voisin->node->label);
				voisin = voisin->next;
			}
			printf("=> NULL\n");
		}
	}
}

void add_voisin(nodelist_t** list, node_t* voisin) {
	if (*list == NULL) {
		*list = malloc(sizeof(nodelist_t));
		(*list)->node = voisin;
		(*list)->next = NULL;
		return;
	}

	nodelist_t* last = (*list);
	while (last->next != NULL) {
		last = last->next;
	}

	nodelist_t* new = malloc(sizeof(nodelist_t));
	new->node = voisin;
	new->next = NULL;

	last->next = new;

	return;
}

void add_edge(graph_t* g, int src, int dst) {
	node_t* src_node = &g->nodes[src];
	node_t* dst_node = &g->nodes[dst];

	add_voisin(&src_node->voisins, dst_node);
	add_voisin(&dst_node->voisins, src_node);
}

void random_edges(graph_t* g) {
	srand(42); //seed
	int cluster_size = g->nb_nodes / CLUSTERS_COUNT;

	for(int i=0; i<g->nb_nodes-1; i++){
		int cluster_i = i / cluster_size;

		for (int j = i+1; j < g->nb_nodes; ++j) {
			if (j == i) continue;

			int cluster_j = j / cluster_size;
			float proba = (cluster_i == cluster_j) ? HIGH_PROBABILITY : LOW_PROBABILITY;
			
			float result = (float)rand() / RAND_MAX;
			if (result < proba) {
				add_edge(g, i, j);
			}
		}
	}
}

void shuffle(int *array, size_t n)
{
	size_t i;
	for (i = 0; i < n - 1; i++)
	{
		size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
		int t = array[j];
		array[j] = array[i];
		array[i] = t;
	}
}

int* copy_nodes(graph_t* g) {
	int* cpy = malloc(g->nb_nodes*sizeof(int));
	for (int i = 0; i < g->nb_nodes; i++) {
		cpy[i] = i;
	}
	return cpy;
}

void label_propagate(graph_t *g) {
	int* nodes = copy_nodes(g);
	int* label_occurences = malloc(g->nb_nodes*sizeof(int));
	int changed = 0;

	int iterations = 0;
	do {
		changed = 0;
		++iterations;

		shuffle(nodes, g->nb_nodes);
		for (int i = 0; i < g->nb_nodes; ++i) {
			node_t* current_node = &g->nodes[i];

			memset(label_occurences, 0, g->nb_nodes*sizeof(int));

			nodelist_t* voisin = current_node->voisins;
			while (voisin != NULL) {
				int label_voisin = voisin->node->label;
				label_occurences[label_voisin]++;
				voisin = voisin->next;
			}
			int max = 0;
			int max_label = 0;
			for (int label = 0; label < g->nb_nodes; ++label) {
				if (label_occurences[label] > max) {
					max = label_occurences[label];
					max_label = label;
				}
			}

			int current_occurence = label_occurences[current_node->label];
			if (max > 0 && current_occurence < max) {
				current_node->label = max_label;
				changed = 1;
			}

		}
	} while (changed != 0);
	printf("iterations %d\n", iterations);
}


int main() {
	
	printf("init graph\n");
	graph_t* g = new_graph(GRAPH_SIZE);
	printf("random edges\n");
	random_edges(g);

	//print_graph(g, 1);

	save_to(g, "./before.dot");
	label_propagate(g);
	printf("======\n");
	//print_graph(g, 0);
	save_to(g, "./after.dot");

	return 0;
}
