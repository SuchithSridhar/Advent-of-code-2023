#ifndef HAND_H
#define HAND_H

#include <stdint.h>

#define HAND_SIZE 5

typedef enum {
    JOKER = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5,
    SIX = 6,
    SEVEN = 7,
    EIGHT = 8,
    NINE = 9,
    TEN = 10,
    QUEEN = 12,
    KING = 13,
    ACE = 14
} card_t;

typedef enum {
    HIGHCARD = 0,
    ONE_PAIR = 1,
    TWO_PAIR = 2,
    THREE_OAK = 3,
    FULL_HOUSE = 4,
    FOUR_OAK = 5,
    FIVE_OAK = 6,
    UNCLASSIFED = 7
} class_t;

typedef struct {
    int64_t bid;
    card_t cards[HAND_SIZE];
    class_t hand_class;
} hand_t;





#endif
