#ifndef MODE_CTRL_H
#define MODE_CTRL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

// Mode definitions
typedef enum {
    MODE_MOMENTARY = 0,
    MODE_LATCH = 1
} Mode_t;

// Channel pair definitions
typedef enum {
    PAIR_1 = 0, // C and D
    PAIR_2 = 1, // E and F
    PAIR_3 = 2, // G and H
    PAIR_4 = 3, // M and N
    PAIR_5 = 4, // A and P
    PAIR_6 = 5, // J and L
    PAIR_COUNT
} Channel_Pair_t;

// Function prototypes
void Mode_Init(void);
void Mode_CheckStartupKey(void);
void Mode_SetLatch(Channel_Pair_t pair);
void Mode_SetMomentary(Channel_Pair_t pair);
void Mode_StorePrev(void);
Mode_t Mode_GetPairMode(Channel_Pair_t pair);

#endif /* MODE_CTRL_H */