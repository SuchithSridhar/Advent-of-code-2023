#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "include/ssvector.h"
#include "include/fileio.h"

#define EMPTY_LOCATION '.'
#define GALAXY_LOCATION '#'

typedef struct {
    int row;
    int col;
} point_t;

void find_universe_features(fio_DataRead *map, ssv_vector_t *galaxies,
                   ssv_vector_t *empty_rows, ssv_vector_t *empty_cols) {

    bool *galaxy_in_row = malloc(map->length * sizeof(bool));
    bool *galaxy_in_col = malloc(map->str_lens[0] * sizeof(bool));

    /* Initialize the array to all true */
    for (size_t i = 0; i < map->length; i++) {
        galaxy_in_row[i] = false;
    }
    for (size_t i = 0; i < map->str_lens[0]; i++) {
        galaxy_in_col[i] = false;
    }

    point_t point;
    for (int row = 0; row < map->length; row++) {
        for (int col = 0; col < map->str_lens[row]; col++) {
            if (map->lines[row][col] == GALAXY_LOCATION) {
                galaxy_in_row[row] = true;
                galaxy_in_col[col] = true;
                point.row = row;
                point.col = col;
                ssv_push(galaxies, &point);
            }
        }
    }
    for (int i = 0; i < map->length; i++) {
        if (!galaxy_in_row[i]) {
            ssv_push(empty_rows, &i);
        }
    }
    for (int i = 0; i < map->str_lens[0]; i++) {
        if (!galaxy_in_col[i]) {
            ssv_push(empty_cols, &i);
        }
    }
}

void print_int(void *item) {
    printf("%d", *(int *)item);
}

void print_point(void *item) {
    point_t *pt = item;
    printf("(%d, %d)", pt->row, pt->col);
}

int sort_pt_col(const void* a, const void* b) {
    const point_t *pta = a;
    const point_t *ptb = b;
    return pta->col - ptb->col;
}

int sort_pt_row(const void* a, const void* b) {
    const point_t *pta = a;
    const point_t *ptb = b;
    return pta->row - ptb->row;
}

void expand_universe(ssv_vector_t *galaxies, ssv_vector_t *empty_rows,
                     ssv_vector_t *empty_cols) {

    point_t *galaxy_array = galaxies->array;
    size_t galaxy_index = 0;
    int *row_array = empty_rows->array;
    size_t row_index = 0;

    while (galaxy_index < galaxies->size && row_index < empty_rows->size) {

        /* Will never be equal since array of empty rows */
        if (galaxy_array[galaxy_index].row < row_array[row_index]) {
          galaxy_array[galaxy_index].row += row_index;
          galaxy_index++;
        } else {
            row_index++;
        }

    }

    while (galaxy_index < galaxies->size) {
        galaxy_array[galaxy_index].row += (int)row_index;
        galaxy_index++;
    }

    ssv_sort(galaxies, sort_pt_col);

    galaxy_index = 0;
    int *col_array = empty_cols->array;
    size_t col_index = 0;

    while (galaxy_index < galaxies->size && col_index < empty_cols->size) {

        /* Will never be equal since array of empty cols */
        if (galaxy_array[galaxy_index].col < col_array[col_index]) {
          galaxy_array[galaxy_index].col += col_index;
          galaxy_index++;
        } else {
            col_index++;
        }

    }

    while (galaxy_index < galaxies->size) {
        galaxy_array[galaxy_index].col += (int)col_index;
        galaxy_index++;
    }


}

void print_universe(ssv_vector_t *galaxies, int rows, int cols) {
    point_t t;
    for (size_t r = 0; r < rows; r++) {
        for (size_t c = 0; c < cols; c++) {
            t.row = r;
            t.col = c;
            if (ssv_index_of(galaxies, &t) != SIZE_MAX) {
                printf("#");
            } else {
                printf(".");
            }
        }
        printf("\n");
    }
}

int pt_dist(point_t *a, point_t *b) {
    int row_dist = abs(a->row - b->row);
    int col_dist = abs(a->col - b->col);

    return row_dist + col_dist;
}

int find_shortest_path_sum(ssv_vector_t *galaxies) {
    int sum = 0;
    point_t *arr = galaxies->array;
    for (size_t i = 0; i < galaxies->size; i++) {
        for (size_t j = i+1; j < galaxies->size; j++) {
            sum += pt_dist(&arr[i], &arr[j]);
        }
    }
    return sum;
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

    fio_DataRead *universe = fio_read_lines(f);

    int width = universe->str_lens[0];
    int height = universe->length;

    ssv_vector_t *galaxies = ssv_init(sizeof(point_t), 8);
    ssv_vector_t *empty_rows = ssv_init(sizeof(int), 8);
    ssv_vector_t *empty_cols = ssv_init(sizeof(int), 8);

    find_universe_features(universe, galaxies, empty_rows, empty_cols);

    printf("Galaxy locations (before expansion): ");
    ssv_print(galaxies, print_point);

    expand_universe(galaxies, empty_rows, empty_cols);

    printf("Galaxy locations (after expansion): ");
    ssv_print(galaxies, print_point);

    printf("shortest distance sum: %d\n", find_shortest_path_sum(galaxies));


    fio_free_DataRead(universe);
    fclose(f);
    return EXIT_SUCCESS;
}
