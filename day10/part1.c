#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "include/fileio.h"

#define GROUND '.'
#define PIPE_NORTH_SOUTH '|'
#define PIPE_EAST_WEST '-'
#define PIPE_NORTH_EAST 'L'
#define PIPE_NORTH_WEST 'J'
#define PIPE_SOUTH_WEST '7'
#define PIPE_SOUTH_EAST 'F'
#define STARTING_POSITION 'S'

typedef struct {
    int row;
    int col;
} pos_t;


/**
 * Find the next position on the map given
 * the current and the previous position.
 */
pos_t find_next_position(char current_pipe, pos_t *prev, pos_t *current) {
    pos_t next;

    switch (current_pipe) {
        /* Either up or down */
        case PIPE_NORTH_SOUTH:
            next.col = current->col;
            if (prev->row == current->row - 1) {
                next.row = current->row + 1;
            } else {
                next.row = current->row - 1;
            }
            break;
        
        /* Either left or right */
        case PIPE_EAST_WEST:
            next.row = current->row;
            if (prev->col == current->col - 1) {
                next.col = current->col + 1;
            } else {
                next.col = current->col - 1;
            }
            break;

        /* Either up or right */
        case PIPE_NORTH_EAST:
            if (prev->row == current->row - 1) {
                next.row = current->row;
                next.col = current->col + 1;
            } else {
                next.row = current->row - 1;
                next.col = current->col;
            }
            break;

        /* Either up or left */
        case PIPE_NORTH_WEST:
            if (prev->row == current->row - 1) {
                next.row = current->row;
                next.col = current->col - 1;
            } else {
                next.row = current->row - 1;
                next.col = current->col;
            }
            break;

        /* Either down or left */
        case PIPE_SOUTH_WEST:
            if (prev->row == current->row + 1) {
                next.row = current->row;
                next.col = current->col - 1;
            } else {
                next.row = current->row + 1;
                next.col = current->col;
            }
            break;

        /* Either down or right */
        case PIPE_SOUTH_EAST:
            if (prev->row == current->row + 1) {
                next.row = current->row;
                next.col = current->col + 1;
            } else {
                next.row = current->row + 1;
                next.col = current->col;
            }
            break;
    };

    return next;
}

pos_t find_start(fio_DataRead *map) {
    char *line;
    pos_t start;

    start.row = -1;
    start.col = -1;

    for (size_t i = 0; i < map->length; i++) {
        line = map->lines[i];
        for (size_t j = 0; j < map->str_lens[i]; j++) {
            if (line[j] == STARTING_POSITION) {
                start.row = i;
                start.col = j;
                return start;
            }
        }
    }

    // Something has gone wrong.
    return start;
}

char map_at(fio_DataRead *map, int row, int col) {
    if (row < 0 || col < 0) return GROUND;
    if (row >= map->length) return GROUND;

    if (col >= map->str_lens[row]) return GROUND;
    return map->lines[row][col];
}

pos_t replace_start(fio_DataRead *map) {
    pos_t start = find_start(map);
    char map_item;
    char start_item;

    bool up_valid = false;
    bool down_valid = false;
    bool left_valid = false;
    bool right_valid = false;
    
    /* if the pipe above has a south end then it's valid */
    map_item = map_at(map, start.row-1, start.col);
    if (map_item == PIPE_NORTH_SOUTH || map_item == PIPE_SOUTH_EAST ||
            map_item == PIPE_SOUTH_WEST) {
        up_valid = true;
    }

    /* if the pipe below has a north end then it's valid */
    map_item = map_at(map, start.row+1, start.col);
    if (map_item == PIPE_NORTH_SOUTH || map_item == PIPE_NORTH_EAST ||
            map_item == PIPE_NORTH_WEST) {
        down_valid = true;
    }

    /* if the pipe to the right has a west end then it's valid */
    map_item = map_at(map, start.row, start.col + 1);
    if (map_item == PIPE_NORTH_WEST || map_item == PIPE_SOUTH_WEST ||
            map_item == PIPE_EAST_WEST) {
        right_valid = true;
    }

    /* if the pipe to the left has an east end then it's valid */
    map_item = map_at(map, start.row, start.col - 1);
    if (map_item == PIPE_NORTH_EAST || map_item == PIPE_SOUTH_EAST ||
            map_item == PIPE_EAST_WEST) {
        left_valid = true;
    }

    if (up_valid && down_valid) start_item = PIPE_NORTH_SOUTH;
    else if (left_valid && right_valid) start_item = PIPE_EAST_WEST;
    else if (up_valid && left_valid) start_item = PIPE_NORTH_WEST;
    else if (up_valid && right_valid) start_item = PIPE_NORTH_EAST;
    else if (down_valid && left_valid) start_item = PIPE_SOUTH_WEST;
    else if (down_valid && right_valid) start_item = PIPE_SOUTH_EAST;
    else {
        // something went wrong
        start_item = GROUND;
    }

    map->lines[start.row][start.col] = start_item;
    return start;
}

int traverse_loop(fio_DataRead *map, pos_t start) {
    pos_t current;
    pos_t previous;
    int steps;
    char map_item;

    current.row = -1;

    previous.row = start.row;
    previous.col = start.col;

    /* if the pipe below has a north end then it's valid */
    map_item = map_at(map, start.row+1, start.col);
    if (current.row == -1 && (map_item == PIPE_NORTH_SOUTH ||
            map_item == PIPE_NORTH_EAST || map_item == PIPE_NORTH_WEST)) {
        current.row = start.row+1;
        current.col = start.col;
    }

    /* if the pipe to the right has a west end then it's valid */
    map_item = map_at(map, start.row, start.col + 1);
    if (current.row == -1 && (map_item == PIPE_NORTH_WEST ||
                map_item == PIPE_SOUTH_WEST || map_item == PIPE_EAST_WEST)) {
        current.row = start.row;
        current.col = start.col + 1;
    }

    /* if the pipe above has a south end then it's valid */
    map_item = map_at(map, start.row-1, start.col);
    if (current.row == -1 && (map_item == PIPE_NORTH_SOUTH ||
            map_item == PIPE_SOUTH_EAST || map_item == PIPE_SOUTH_WEST)) {
        current.row = start.row-1;
        current.col = start.col;
    }

    /* At this point we're guarenteed that current has been set since we've
     * search 3/4 of the options and each pipe must connect at least 2 parts.
     */

    steps = 1;
    pos_t tmp;
    while (!(current.row == start.row && current.col == start.col)) {
        map_item = map_at(map, current.row, current.col);
        tmp = find_next_position(map_item, &previous, &current);
        previous = current;
        current = tmp;
        steps++;
    }
    return steps;
}

int find_max_distance(fio_DataRead *map, pos_t start) {
    int steps = traverse_loop(map, start);
    if (steps % 2 == 0) {
        return steps / 2;
    } else {
        return (steps / 2) + 1;
    }
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

    fio_DataRead *map = fio_read_lines(f);

    pos_t start = replace_start(map);

    printf("%d\n", find_max_distance(map, start));

    fio_free_DataRead(map);
    fclose(f);

    return EXIT_SUCCESS;
}
