#ifndef CAN_LOOKUP_H
#define CAN_LOOKUP_H

#include "common.h"

typedef enum {
    FUNCTION_C = 0,
    FUNCTION_D,
    FUNCTION_E,
    FUNCTION_F,
    FUNCTION_G,
    FUNCTION_H,
    FUNCTION_M,
    FUNCTION_N,
    FUNCTION_A,
    FUNCTION_P,
    FUNCTION_J,
    FUNCTION_L,
    FUNCTION_COUNT
} Function_t;

typedef enum {
    PAIR_1 = 0,
    PAIR_2,
    PAIR_3,
    PAIR_4,
    PAIR_5,
    PAIR_6,
    PAIR_COUNT
} Pair_t;

typedef struct {
    uint8_t byte_index;
    uint8_t value;
    Function_t function;
    Pair_t pair;
} CAN_Lookup_Entry_t;

extern const CAN_Lookup_Entry_t can_lookup_table[FUNCTION_COUNT];

Function_t CAN_get_function_from_data(uint8_t byte_index, uint8_t value);
Pair_t CAN_get_pair_for_function(Function_t function);

#endif // CAN_LOOKUP_H