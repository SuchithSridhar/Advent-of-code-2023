#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "include/fileio.h"

typedef struct {
    int row;
    int start_col;
    int end_col;
} PartNumRange;

// https://stackoverflow.com/a/9631424
int str2int(const char* str, int len) {
    int i;
    int ret = 0;
    for(i = 0; i < len; ++i)
    {
        ret = ret * 10 + (str[i] - '0');
    }
    return ret;
}

bool is_char_at_symbol(fio_DataRead *data, int row, int col) {
    if (row < 0 || row >= data->length) return false;
    if (col < 0 || col >= data->str_lens[row]) return false;

    char c = data->lines[row][col];
    return (c != '.' && !(c >= '0' && c <= '9'));
}

bool is_char_at_digit(fio_DataRead *data, int row, int col) {
    if (row < 0 || row >= data->length) return false;
    if (col < 0 || col >= data->str_lens[row]) return false;

    char c = data->lines[row][col];
    return (c >= '0' && c <= '9');
}

bool in_part_range_array(int row, int col, PartNumRange *pnr, int count) {
    for (int i = 0; i < count; i++) {
        if (row == pnr[i].row && col >= pnr[i].start_col && col <= pnr[i].end_col) {
            return true;
        }
    }
    return false;
}

int save_part_num(fio_DataRead *data, int row, int col, PartNumRange *pnr, int count) {
    int start_c = col;
    int end_c = col;

    if (in_part_range_array(row, col, pnr, count)) return count;

    while (is_char_at_digit(data, row, end_c+1)) {
        if (in_part_range_array(row, end_c+1, pnr, count)) {
            // Don't do anything just exit.
            return count;
        }
        end_c++;
    }

    while (is_char_at_digit(data, row, start_c-1)) {
        if (in_part_range_array(row, start_c-1, pnr, count)) {
            // Don't do anything just exit.
            return count;
        }

        start_c--;
    }

    // We have a new range
    pnr[count].row = row;
    pnr[count].start_col = start_c;
    pnr[count].end_col = end_c;

    return count+1;
}

int calc_gear_ratio_from_pnr(fio_DataRead *data, PartNumRange *pnr) {
    // Assumes there are 2 parts.
    int row = pnr[0].row;
    int col = pnr[0].start_col;
    int len = pnr[0].end_col - pnr[0].start_col + 1;

    int first = str2int(&data->lines[row][col], len);

    row = pnr[1].row;
    col = pnr[1].start_col;
    len = pnr[1].end_col - pnr[1].start_col + 1;

    int second = str2int(&data->lines[row][col], len);

    return first * second;
}

/**
 * Get the gear ratio for the given gear.
 * If it's not a gear then 0 is returned.
 */
int get_gear_ratio(fio_DataRead *data, int i, int c) {
    // You can have at most 6 unique part numbers around a symbol
    PartNumRange pnr[6];
    int count = 0;

    // top left
    if (is_char_at_digit(data, i-1, c-1)) {
        count = save_part_num(data, i-1, c-1, pnr, count);
    }
    // top middle
    if (is_char_at_digit(data, i-1, c)) {
        count = save_part_num(data, i-1, c, pnr, count);
    }
    // top right
    if (is_char_at_digit(data, i-1, c+1)) {
        count = save_part_num(data, i-1, c+1, pnr, count);
    }

    if (count > 2) return 0;

    // left
    if (is_char_at_digit(data, i, c-1)) {
        count = save_part_num(data, i, c-1, pnr, count);
    }
    // right
    if (is_char_at_digit(data, i, c+1)) {
        count = save_part_num(data, i, c+1, pnr, count);
    }
    
    if (count > 2) return 0;

    // bottom left
    if (is_char_at_digit(data, i+1, c-1)) {
        count = save_part_num(data, i+1, c-1, pnr, count);
    }
    // bottom middle
    if (is_char_at_digit(data, i+1, c)) {
        count = save_part_num(data, i+1, c, pnr, count);
    }
    // bottom right
    if (is_char_at_digit(data, i+1, c+1)) {
        count = save_part_num(data, i+1, c+1, pnr, count);
    }

    if (count != 2) return 0;
    return calc_gear_ratio_from_pnr(data, pnr);
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

    fio_DataRead *data = fio_read_lines(f);

    printf("Len: %d\n", data->length);


    int sum = 0;

    for (int r = 0; r < data->length; r++) {
        for (int c = 0; c < data->str_lens[r]; c++) {
            if (data->lines[r][c] == '*') {
                sum += get_gear_ratio(data, r, c);
            }
        }
    }

    printf("%d\n", sum);

    return EXIT_SUCCESS;
}
