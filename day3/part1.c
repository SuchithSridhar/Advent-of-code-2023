#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "include/fileio.h"


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

/**
 * Get the part number for the given character.
 * If it's not part number then returns 0.
 * Updated c to point to next non-digit character.
 */
int get_part_number(fio_DataRead *data, int i, int *c) {
    int start_row = i;
    int start_col = *c;
    int end_row = i;
    int end_col = *c;

    bool is_part = false;

    while (is_char_at_digit(data, end_row, end_col)) {
        // Check around:
        is_part = (
            is_part ||
            is_char_at_symbol(data, end_row-1, end_col-1) ||
            is_char_at_symbol(data, end_row-1, end_col) ||
            is_char_at_symbol(data, end_row-1, end_col+1) ||

            is_char_at_symbol(data, end_row, end_col-1) ||
            is_char_at_symbol(data, end_row, end_col+1) ||

            is_char_at_symbol(data, end_row+1, end_col-1) ||
            is_char_at_symbol(data, end_row+1, end_col) ||
            is_char_at_symbol(data, end_row+1, end_col+1)
        );
        end_col += 1;
    }

    if (!is_part) return 0;
    *c = end_col;
    return str2int(&data->lines[start_row][start_col], end_col - start_col);
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
            if (is_char_at_digit(data, r, c)) {
                // Current character is a digit.
                // c will be updated by get_part_number to point after number
                sum += get_part_number(data, r, &c);
            }
        }
    }

    printf("%d\n", sum);

    return EXIT_SUCCESS;
}
