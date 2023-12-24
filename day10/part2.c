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
#define LOOP_TURN_MARK '#'
#define LOOP_VERT_MARK 'V'
#define LOOP_HORZ_MARK 'H'
#define OUTSIDE_MARK 'O'
#define INSIDE_MARK 'I'

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

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

int traverse_and_replace_loop(fio_DataRead *map, pos_t start) {
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

        /* replace previous */
        switch(map->lines[previous.row][previous.col]){
            case PIPE_NORTH_SOUTH:
                map->lines[previous.row][previous.col] = LOOP_VERT_MARK;
                break;
            case PIPE_EAST_WEST:
                map->lines[previous.row][previous.col] = LOOP_HORZ_MARK;
                break;
            default:
                map->lines[previous.row][previous.col] = LOOP_TURN_MARK;
                break;
        }
        previous = current;
        current = tmp;
        steps++;
    }

    switch(map->lines[previous.row][previous.col]){
        case PIPE_NORTH_SOUTH:
            map->lines[previous.row][previous.col] = LOOP_VERT_MARK;
            break;
        case PIPE_EAST_WEST:
            map->lines[previous.row][previous.col] = LOOP_HORZ_MARK;
            break;
        default:
            map->lines[previous.row][previous.col] = LOOP_TURN_MARK;
            break;
    }

    return steps;
}

void update_neighbor_squares(fio_DataRead *map, int row, int col, char mark) {

    if (row < 0 || col < 0) return;
    if (row >= map->length) return;

    if (col >= map->str_lens[row]) return;

    if (map->lines[row][col] == LOOP_VERT_MARK) return;
    if (map->lines[row][col] == LOOP_TURN_MARK) return;
    if (map->lines[row][col] == LOOP_HORZ_MARK) return;
    if (map->lines[row][col] == INSIDE_MARK) return;
    if (map->lines[row][col] == OUTSIDE_MARK) return;
    map->lines[row][col] = mark;

    update_neighbor_squares(map, row - 1, col, mark);
    update_neighbor_squares(map, row + 1, col, mark);
    update_neighbor_squares(map, row, col - 1, mark);
    update_neighbor_squares(map, row, col + 1, mark);
}

bool is_inside_pipes(fio_DataRead *map, int row, int col) {

    if (map->lines[row][col] == INSIDE_MARK) return true;
    if (map->lines[row][col] == OUTSIDE_MARK) return false;
    if (map->lines[row][col] == LOOP_VERT_MARK) return false;
    if (map->lines[row][col] == LOOP_TURN_MARK) return false;
    if (map->lines[row][col] == LOOP_HORZ_MARK) return false;

    // It's on the edge and it's not a loop
    if (row == 0 || col == 0 ||
        row == map->length - 1 || col == map->str_lens[row] - 1) {

        update_neighbor_squares(map, row, col, OUTSIDE_MARK);
        return false;
    }

    int north_count = 0;
    int south_count = 0;
    int west_count = 0;
    int east_count = 0;

    char map_item;

    for (int i = row - 1; i >= 0; i--) {
        map_item = map_at(map, i, col);
        if (map_item == LOOP_HORZ_MARK || map_item == LOOP_TURN_MARK) north_count++;
    }

    for (int i = row + 1; i < map->length; i++) {
        map_item = map_at(map, i, col);
        if (map_item == LOOP_HORZ_MARK || map_item == LOOP_TURN_MARK) south_count++;
    }

    for (int i = col - 1; i >= 0; i--) {
        map_item = map_at(map, row, i);
        if (map_item == LOOP_VERT_MARK || map_item == LOOP_TURN_MARK) west_count++;
    }

    for (int i = col + 1; i < map->str_lens[row]; i++) {
        map_item = map_at(map, row, i);
        if (map_item == LOOP_VERT_MARK || map_item == LOOP_TURN_MARK) east_count++;
    }

    if (row == 2 && col == 3) {
        printf("Original = %c\n", map->lines[row][col]);
        printf("north_count = %d\n", north_count);
        printf("south_count = %d\n", south_count);
        printf("east_count = %d\n", east_count);
        printf("west_count = %d\n", west_count);
    }

    if (north_count % 2 == 1 || south_count % 2 == 1 || east_count % 2 == 1 || west_count % 2 == 1) {
        // This is an inside square.
        update_neighbor_squares(map, row, col, INSIDE_MARK);
        return true;
    } else {
        update_neighbor_squares(map, row, col, OUTSIDE_MARK);
        return false;
    }
}

int count_inside_squares(fio_DataRead *map) {
    int count = 0;
    for (int i = 0; i < map->length; i++) {
        for (int j = 0; j < map->str_lens[i]; j++) {
            if (is_inside_pipes(map, i, j)) {
                count ++;
            }
        }
    }
    return count;
}

void print_map(fio_DataRead *map) {
    printf("============================================================== MAP =========================================================================\n");
    char item;
    for (size_t i = 0; i < map->length; i++) {
        for (size_t j = 0; j < map->str_lens[i]; j++) {
            item = map->lines[i][j];
            if (item == LOOP_TURN_MARK || item == LOOP_HORZ_MARK || item == LOOP_VERT_MARK) {
                printf(ANSI_COLOR_BLUE "%c" ANSI_COLOR_RESET, item);
            } else if (item == INSIDE_MARK) {
                printf(ANSI_COLOR_RED "%c" ANSI_COLOR_RESET, item);
            } else if (item == OUTSIDE_MARK) {
                printf(ANSI_COLOR_GREEN "%c" ANSI_COLOR_RESET, item);
            } else {
                printf("%c", item);
            }
        }
        printf("\n");
    }
    printf("============================================================================================================================================\n");
    printf("\n");
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

    traverse_and_replace_loop(map, start);

    print_map(map);

    int count = count_inside_squares(map);

    print_map(map);

    printf("%d\n", count);

    fio_free_DataRead(map);

    return EXIT_SUCCESS;
}
