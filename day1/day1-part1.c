#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>


int extract_two_digit_value(char *line, ssize_t size) {

    int first_digit = -1;
    int last_digit = -1;
    int digit = 0;

    for (ssize_t i = 0; i < size; i++) {
        if (line[i] >= '0' && line[i] <= '9') {
            digit = line[i] - '0';
            if (first_digit == -1) {
                first_digit = digit;
                break;
            } 
        }
    }

    for (ssize_t i = size-1; i >= 0; i--) {
        if (line[i] >= '0' && line[i] <= '9') {
            digit = line[i] - '0';
            if (last_digit == -1) {
                last_digit = digit;
                break;
            } 
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
    char * line = NULL;
    size_t len = 0;
    ssize_t characters_read;

    while ((characters_read = getline(&line, &len, f)) != -1) {
        sum += extract_two_digit_value(line, characters_read);
    }

    printf("%d\n", sum);

    if (line)
        free(line);

    return EXIT_SUCCESS;
}
