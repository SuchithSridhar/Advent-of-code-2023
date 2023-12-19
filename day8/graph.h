#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>

#define NAME_LENGTH 3
#define SYMBOL_COUNT 26
#define SYMBOL_OFFSET 'A'

typedef char name_t[NAME_LENGTH];

typedef struct graph_node {
    struct graph_node *left;
    struct graph_node *right;
    int uuid;
    name_t name;
} graph_node_t;

typedef struct hash_node {
    graph_node_t *node;
    struct hash_node *next;
} hash_node_t;

typedef struct {
    hash_node_t *map;
    int length;
} hashmap_t;


int hash_position_calc(name_t);

#endif
