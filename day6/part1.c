#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "include/fileio.h"

int find_arith_sum_even(int a) {
    float x = sqrt(4 * a + 1);
    int ans = floor(0.5 + 0.5 * x);

    printf("Doing sum for even nums: %d\n", ans);
    return ans;
}

int find_arith_sum_odd(int a) {
    float x = sqrt(a);
    int ans = floor(x);

    // if perfect square then return one less
    if (ans * ans == a)
        ans = ans - 1;

    printf("Doing sum for odd nums: %d\n", ans);
    return ans;
}

int count_possible_win_options(int time, int dist_to_beat) {
    printf("Function called (time=%d, dist=%d)\n", time, dist_to_beat);
    int optimal;
    int count;

    if (time % 2 == 0) {
        optimal = time / 2;
        optimal = optimal * optimal;
        printf("Optimal calculated = %d\n", optimal);
        count = find_arith_sum_odd(optimal - dist_to_beat);
        count = count * 2 + 1;
        printf("Did math for even time with count: %d\n", count);
    } else {
        optimal = time / 2;
        optimal = optimal * (optimal + 1);
        printf("Optimal calculated = %d\n", optimal);
        count = find_arith_sum_even(optimal - dist_to_beat);
        count = count * 2;
        printf("Did math for odd time with count: %d\n", count);
    }

    return count;
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

    char *saveptr1, *saveptr2;
    const char *delimiter = " \t";

    char *token1 = strtok_r(fio_data->lines[0], delimiter, &saveptr1);
    char *token2 = strtok_r(fio_data->lines[1], delimiter, &saveptr2);

    // skip the first time to skip row names
    token1 = strtok_r(NULL, delimiter, &saveptr1);
    token2 = strtok_r(NULL, delimiter, &saveptr2);

    int time;
    int distance;
    int total = 1;

    // Tokenize and print tokens from both lines simultaneously
    while (token1 != NULL && token2 != NULL) {

        printf("==== \n");
        time = atoi(token1);
        distance = atoi(token2);
        total *= count_possible_win_options(time, distance);
        printf("\n");

        token1 = strtok_r(NULL, delimiter, &saveptr1);
        token2 = strtok_r(NULL, delimiter, &saveptr2);
    }

    fclose(f);
    fio_free_DataRead(fio_data);

    printf("%d\n", total);

    return EXIT_SUCCESS;
}
