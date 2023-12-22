#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "include/ssvector.h"


void parse_data_line(char *line, ss_Vector *vec) {
    char *p = strtok(line, " ");
    int value;
    while(p != NULL) {
        value = atoi(p);
        ssv_push(vec, &value);
        p = strtok(NULL, " ");
    }
}

bool is_line_zero(ss_Vector *vec) {
    int *array = vec->array;
    for (size_t i = 0; i < vec->size; i++) {
        if (array[i] != 0) return false;
    }
    return true;
}

ss_Vector* build_next_line(ss_Vector *vec) {
    ss_Vector *nextline = ssv_init(vec->memb_size, vec->size - 1);
    int *top = vec->array;
    int *bottom = nextline->array;

    int value;
    for (size_t i = 0; i < vec->size - 1; i++) {
        value = top[i+1] - top[i];
        ssv_push(nextline, &value);
    }

    return nextline;
}

void print_vector(ss_Vector *vec) {
    int *array = vec->array;
    printf("[");
    for (size_t i = 0; i < vec->size-1; i++) {
        printf("%d ", array[i]);
    }
    printf("%d]\n", array[vec->size-1]);
}

void print_meta_vector(ss_Vector *meta_vec) {
    ss_Vector **vec_array = meta_vec->array;

    printf("=== Meta Vec ===\n");
    for (size_t i = 0; i < meta_vec->size; i++) {
        printf("At %ld: ", i);
        print_vector(vec_array[i]);
    }
    printf("================\n");
}

void meta_vector_destory(ss_Vector *meta_vec) {

    ss_Vector **vec_array = meta_vec->array;

    // Start at 1 because we do not want to free the first
    // vector as that will be reused.
    for (size_t i = 1; i < meta_vec->size; i++) {
        ssv_destroy(vec_array[i]);
    }
    ssv_destroy(meta_vec);
}

int predict_prev_value(ss_Vector *vec) {

    ss_Vector *meta_vec = ssv_init(sizeof(ss_Vector*), 4);

    ssv_push(meta_vec, &vec);

    ss_Vector *cur_vec = vec;

    /* building pyramid */
    while (!is_line_zero(cur_vec)) {
        cur_vec = build_next_line(cur_vec);
        ssv_push(meta_vec, &cur_vec);
    }

    /* extrapolate prediction */
    int bottom = 0;
    int left = 0;
    int right = 0;

    ss_Vector **meta_array = meta_vec->array;
    
    for (int i = meta_vec->size - 2; i >= 0; i--) {

        right = *(int*) ssv_get(meta_array[i], 0);
        left = right - bottom;
        bottom = left;
    }

    meta_vector_destory(meta_vec);

    return left;
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

    int sum = 0;

    char * line = NULL;
    size_t len = 0;
    ssize_t characters_read;
    ss_Vector *vec = ssv_init(sizeof(int), 16);

    while ((characters_read = getline(&line, &len, f)) != -1) {
        ssv_clear(vec);
        parse_data_line(line, vec);
        sum += predict_prev_value(vec);
    }

    printf("%d\n", sum);

    if (line)
        free(line);

    ssv_destroy(vec);

    return EXIT_SUCCESS;
}
