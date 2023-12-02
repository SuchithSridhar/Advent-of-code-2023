#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#define DIGITS_COUNT 10

char *SPELLED_DIGITS[DIGITS_COUNT] = {
    "zero",
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
};

/*
 * Check if a spelled out digit exists at the given location.
 * If direction is 1 it looks in the forward direction,
 * if direction is -1 it looks in the backward direction.
 * Only looks for up to max_len characters in specified direction.
 */
int is_spelled_digit(char *start, int max_len) {
    for (int i = 0; i < DIGITS_COUNT; i++) {
        int index = 0;
        int match = 1;
        while (index < max_len) {
            if (SPELLED_DIGITS[i][index] == '\0') break;
            if (SPELLED_DIGITS[i][index] != start[index]) {
                match = 0;
                break;
            }
            index++;
        }

        if (match) {
            return i;
        }
    }

    return -1;
}


int extract_two_digit_value(char *line, ssize_t size) {

    int first_digit = -1;
    int last_digit = -1;

    for (ssize_t i = 0; i < size; i++) {
        int digit = 0;
        if (line[i] >= '0' && line[i] <= '9') {
            digit = line[i] - '0';
            first_digit = digit;
            break;
        }
        digit = is_spelled_digit(&line[i], size-1-i);
        if (digit != -1) {
            first_digit = digit;
            break;
        }
    }

    for (ssize_t i = size-1; i >= 0; i--) {
        int digit = 0;
        if (line[i] >= '0' && line[i] <= '9') {
            digit = line[i] - '0';
            last_digit = digit;
            break;
        }
        digit = is_spelled_digit(&line[i], size-i);
        if (digit != -1) {
            last_digit = digit;
            break;
        }
    }

    return ((first_digit * 10) + last_digit);
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
    int value = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    while ((characters_read = getline(&line, &len, f)) != -1) {
        value = extract_two_digit_value(line, characters_read);
        sum += value;
        printf("%d\n", value);
    }

    printf("%d\n", sum);

    if (line)
        free(line);

    return EXIT_SUCCESS;
}
