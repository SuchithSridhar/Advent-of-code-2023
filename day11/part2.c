#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "include/ssvector.h"
#include "include/fileio.h"

#define EMPTY_LOCATION '.'
#define GALAXY_LOCATION '#'
#define EXPANSION_FACTOR 1000000

typedef struct {
    long row;
    long col;
} point_t;

void find_universe_features(fio_DataRead *map, ssv_vector_t *galaxies,
                   ssv_vector_t *empty_rows, ssv_vector_t *empty_cols) {

    bool *galaxy_in_row = malloc(map->length * sizeof(bool));
    bool *galaxy_in_col = malloc(map->str_lens[0] * sizeof(bool));

    /* Initialize the array to all true */
    for (long i = 0; i < map->length; i++) {
        galaxy_in_row[i] = false;
    }
    for (long i = 0; i < map->str_lens[0]; i++) {
        galaxy_in_col[i] = false;
    }

    point_t point;
    for (long row = 0; row < map->length; row++) {
        for (long col = 0; col < map->str_lens[row]; col++) {
            if (map->lines[row][col] == GALAXY_LOCATION) {
                galaxy_in_row[row] = true;
                galaxy_in_col[col] = true;
                point.row = row;
                point.col = col;
                ssv_push(galaxies, &point);
            }
        }
    }
    for (long i = 0; i < map->length; i++) {
        if (!galaxy_in_row[i]) {
            ssv_push(empty_rows, &i);
        }
    }
    for (long i = 0; i < map->str_lens[0]; i++) {
        if (!galaxy_in_col[i]) {
            ssv_push(empty_cols, &i);
        }
    }
}

void print_long(void *item) {
    printf("%ld", *(long *)item);
}

void print_point(void *item) {
    point_t *pt = item;
    printf("(%ld, %ld)", pt->row, pt->col);
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
    long galaxy_index = 0;
    long *row_array = empty_rows->array;
    long row_index = 0;

    // Because 1 already exists, we need to grow by the difference
    long growth_factor = EXPANSION_FACTOR - 1;

    while (galaxy_index < galaxies->size && row_index < empty_rows->size) {

        /* Will never be equal since array of empty rows */
        if (galaxy_array[galaxy_index].row < row_array[row_index]) {
          galaxy_array[galaxy_index].row += row_index * growth_factor;
          galaxy_index++;
        } else {
            row_index++;
        }

    }

    while (galaxy_index < galaxies->size) {
        galaxy_array[galaxy_index].row += row_index * growth_factor;
        galaxy_index++;
    }

    ssv_sort(galaxies, sort_pt_col);
    galaxy_index = 0;
    long *col_array = empty_cols->array;
    long col_index = 0;

    while (galaxy_index < galaxies->size && col_index < empty_cols->size) {

        /* Will never be equal since array of empty cols */
        if (galaxy_array[galaxy_index].col < col_array[col_index]) {
          galaxy_array[galaxy_index].col += col_index * growth_factor;
          galaxy_index++;
        } else {
            col_index++;
        }

    }

    while (galaxy_index < galaxies->size) {
        galaxy_array[galaxy_index].col += col_index * growth_factor;
        galaxy_index++;
    }


}

void print_universe(ssv_vector_t *galaxies, int rows, int cols) {
    point_t t;
    for (long r = 0; r < rows; r++) {
        for (long c = 0; c < cols; c++) {
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

long abs_long(long a) {
    if (a < 0) return -a;
    return a;
}

long pt_dist(point_t *a, point_t *b) {
    long row_dist = abs_long(a->row - b->row);
    long col_dist = abs_long(a->col - b->col);

    return row_dist + col_dist;
}

long find_shortest_path_sum(ssv_vector_t *galaxies) {
    long sum = 0;
    point_t *arr = galaxies->array;
    for (long i = 0; i < galaxies->size; i++) {
        for (long j = i+1; j < galaxies->size; j++) {
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

    long width = universe->str_lens[0];
    long height = universe->length;

    ssv_vector_t *galaxies = ssv_init(sizeof(point_t), 8);
    ssv_vector_t *empty_rows = ssv_init(sizeof(long), 8);
    ssv_vector_t *empty_cols = ssv_init(sizeof(long), 8);

    find_universe_features(universe, galaxies, empty_rows, empty_cols);

    print_universe(galaxies, width, height);

    printf("Galaxy locations (before expansion): ");
    ssv_print(galaxies, print_point);

    printf("Empty Rows: ");
    ssv_print(empty_rows, print_long);

    printf("Empty Cols: ");
    ssv_print(empty_cols, print_long);

    expand_universe(galaxies, empty_rows, empty_cols);

    printf("Galaxy locations (after expansion): ");
    ssv_print(galaxies, print_point);

    printf("shortest distance sum: %ld\n", find_shortest_path_sum(galaxies));



    fio_free_DataRead(universe);
    fclose(f);
    return EXIT_SUCCESS;
}
