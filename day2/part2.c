#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum { Red, Blue, Green } Color;

typedef struct {
    int red;
    int blue;
    int green;
} Group;

/*
 * Identify the game ID from a given line.
 * This function modifies the provided line
 * and also the current pointer.
 */
int identify_game_id(char **cur) {
    /* "Game x: ...."
     * turns into
     * "Game x\0 ..."
     * and *cur now points to the character after \0
     */
    char *start_of_game_id = *cur;
    // Identify the game
    while (*start_of_game_id != ' ') start_of_game_id++;
    start_of_game_id++;

    char *end_of_game_id = start_of_game_id+1;
    while (*end_of_game_id != ':') end_of_game_id++;
    *end_of_game_id = '\0';

    int game_id = atoi(start_of_game_id);

    *cur = end_of_game_id + 1;
    return game_id;
}

int parse_num_till_space(char **cur) {

    char *start_of_num = *cur;
    char *end_of_num = start_of_num+1;
    while (*end_of_num != ' ') end_of_num++;
    *end_of_num = '\0';

    int game_id = atoi(start_of_num);

    *cur = end_of_num + 1;
    return game_id;
}

Color parse_color_till_comma_or_semicolon(char **cur) {
    Color color;
    if (**cur == 'r') {
        color = Red;
    } else if (**cur == 'b') {
        color = Blue;
    } else if (**cur == 'g') {
        color = Green;
    }

    while (**cur != ',' && **cur != ';' && **cur != '\0') (*cur)++;
    return color;
}

Group parse_game_till_semicolon(char **cur) {
    Group hand;
    hand.red = 0;
    hand.blue = 0;
    hand.green = 0;

    while (true) {
        int num = parse_num_till_space(cur);
        Color color = parse_color_till_comma_or_semicolon(cur);
        if (color == Red) {
            hand.red = num;
        } else if (color == Blue) {
            hand.blue = num;
        } else if (color == Green) {
            hand.green = num;
        }

        if (**cur == ';' || **cur == '\0') {
            break;
        } else {
            // move from , to next number
            *cur += 2;
        }
    } 

    return hand;
}

/**
 * If it's a valid game, returns the ID of the game,
 * otherwise returns 0.
 */
int game_power(char *line) {
    Group hand;

    char *cur = line;
    int game_id = identify_game_id(&cur);

    Group max_hand;
    max_hand.red = 0;
    max_hand.blue = 0;
    max_hand.green = 0;

    // now *cur points to the space after the :
    cur++;
    while (*cur != '\0') {
        hand = parse_game_till_semicolon(&cur);

        if (max_hand.red < hand.red) {
            max_hand.red = hand.red;
        }

        if (max_hand.blue < hand.blue) {
            max_hand.blue = hand.blue;
        }

        if (max_hand.green < hand.green) {
            max_hand.green = hand.green;
        }

        if (*cur == ';') {
            cur += 2;
        }
    } 

    return max_hand.red * max_hand.blue * max_hand.green;
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
    ssize_t read;

    while ((read = getline(&line, &len, f)) != -1) {
        if (line[read-1] == '\n') line[read-1] = '\0';
        sum += game_power(line);
    }

    printf("%d\n", sum);

    if (line)
        free(line);

    return EXIT_SUCCESS;
}
