#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

long str2int(const char *str, long len) {
    long ret = 0;
    for (long i = 0; i < len; i++) {
        if (str[i] > '9' || str[i] < '0')
            continue;
        ret = ret * 10 + (str[i] - '0');
    }
    return ret;
}

long find_arith_sum_even(long a) {
    double x = sqrt(4 * a + 1);
    long ans = floor(0.5 + 0.5 * x);

    printf("Doing sum for even nums: %ld\n", ans);
    return ans;
}

long find_arith_sum_odd(long a) {
    float x = sqrt(a);
    long ans = floor(x);

    // if perfect square then return one less
    if (ans * ans == a)
        ans = ans - 1;

    printf("Doing sum for odd nums: %ld\n", ans);
    return ans;
}

long count_possible_win_options(long time, long dist_to_beat) {
    printf("Function called (time=%ld, dist=%ld)\n", time, dist_to_beat);
    long optimal;
    long count;

    if (time % 2 == 0) {
        optimal = time / 2;
        optimal = optimal * optimal;
        printf("Optimal calculated = %ld\n", optimal);
        count = find_arith_sum_odd(optimal - dist_to_beat);
        count = count * 2 + 1;
        printf("Did math for even time with count: %ld\n", count);
    } else {
        optimal = time / 2;
        optimal = optimal * (optimal + 1);
        printf("Optimal calculated = %ld\n", optimal);
        count = find_arith_sum_even(optimal - dist_to_beat);
        count = count * 2;
        printf("Did math for odd time with count: %ld\n", count);
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

    long time;
    long distance;
    long total = 1;

    char *line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    characters_read = getline(&line, &len, f);

    // first line is time
    time = str2int(line, characters_read);

    characters_read = getline(&line, &len, f);
    distance = str2int(line, characters_read);

    if (line)
        free(line);

    fclose(f);

    total *= count_possible_win_options(time, distance);

    printf("%ld\n", total);

    return EXIT_SUCCESS;
}
