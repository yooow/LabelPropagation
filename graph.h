#ifndef GRAPH_H_
# define GRAPH_H_

struct nodelist;

typedef struct node {
    int value;
    int label;
    struct nodelist* voisins;
} node_t;

typedef struct nodelist {
    node_t* node;
    struct nodelist* next;
} nodelist_t;

typedef struct graph {
    int nb_nodes;
    node_t* nodes;
} graph_t;

#endif