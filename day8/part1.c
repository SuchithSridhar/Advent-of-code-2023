#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "include/fileio.h"
#include "graph.h"

void print_graph_node(graph_node_t *node) {
    if (node == NULL) {
        return;
    }
    printf("%c%c%c :: (%c%c%c, %c%c%c)",
            node->name[0], node->name[1], node->name[2],
            node->left->name[0], node->left->name[1], node->left->name[2],
            node->right->name[0], node->right->name[1], node->right->name[2]
    );
}

void print_hashmap(hashmap_t *hashmap) {
    for (int i = 0; i < hashmap->length; ++i) {
        hash_node_t *current = &hashmap->map[i];

        if (current == NULL || current->node == NULL)
            continue;

        while (current != NULL && current->node != NULL) {
            print_graph_node(current->node);
            current = current->next;
            printf(" --> ");
        }
        printf("\n");
    }
}

int hash_position_calc(name_t name) {
    return SYMBOL_COUNT * (name[0] - SYMBOL_OFFSET) + (name[1] - SYMBOL_OFFSET);
}

void hash_insert(hash_node_t *hashmap, graph_node_t *node) {
    int index = hash_position_calc(node->name);

    if (hashmap[index].node == NULL) {
        hashmap[index].node = node;
        // should be set as whole hash map is initialized to 0.
        assert(hashmap[index].next == 0);

    } else {
        hash_node_t *last = &hashmap[index];
        while (last->next != NULL) {
            last = last->next;
        }

        last->next = malloc(sizeof(hash_node_t));
        last->next->node = node;
        last->next->next = NULL;
    }
}

graph_node_t* hash_get(hash_node_t *map, name_t name) {
    int pos = hash_position_calc(name);
    hash_node_t *item = &map[pos];
    while (item != NULL && item->node->name[2] != name[2]) {
        item = item->next;
    }

    if (item == NULL) {
        return NULL;
    } else {
        return item->node;
    }
}

void hash_set_children(hash_node_t *map, name_t root_name, name_t left_name,
                       name_t right_name) {

    graph_node_t *root, *left, *right;
    root = hash_get(map, root_name);
    left = hash_get(map, left_name);
    right = hash_get(map, right_name);

    root->left = left;
    root->right = right;
}

hashmap_t* graph_build(fio_DataRead *data) {
    // line w/ index 0 contains contains Ls and Rs 
    // line w/ index 1 is empty
    
    // line w/ index 2 starts the graph.
    int node_count = data->length - 2;
    int hashmap_count = SYMBOL_COUNT * SYMBOL_COUNT;
    
    // initialize the hash map values to 0
    hash_node_t *hashmap = calloc(hashmap_count, sizeof(hash_node_t));

    hashmap_t *container = malloc(sizeof(hashmap_t));
    container->map = hashmap;
    container->length = hashmap_count;

    graph_node_t *node;

    // Set up all the graphs
    for (int i = 0; i < node_count; i++) {
        node = malloc(sizeof(graph_node_t));

        node->name[0] = data->lines[i+2][0];
        node->name[1] = data->lines[i+2][1];
        node->name[2] = data->lines[i+2][2];

        hash_insert(hashmap, node);
    }

    name_t node_name;
    name_t left_name;
    name_t right_name;
    // AAA = (BBB, CCC)
    // 0123456789012345
    for (int i = 0; i < node_count; i++) {

        node_name[0] = data->lines[i+2][0];
        node_name[1] = data->lines[i+2][1];
        node_name[2] = data->lines[i+2][2];

        left_name[0] = data->lines[i+2][7];
        left_name[1] = data->lines[i+2][8];
        left_name[2] = data->lines[i+2][9];

        right_name[0] = data->lines[i+2][12];
        right_name[1] = data->lines[i+2][13];
        right_name[2] = data->lines[i+2][14];

        hash_set_children(hashmap, node_name, left_name, right_name);
    }

    return container;
}


int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Invalid call to program.\n");
        fprintf(stderr, "./%s <input-file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Unable to open file: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    fio_DataRead *fio_data = fio_read_lines(f);

    hashmap_t *hashmap = graph_build(fio_data);
    print_hashmap(hashmap);

    fclose(f);
    fio_free_DataRead(fio_data);

    return EXIT_SUCCESS;
}
