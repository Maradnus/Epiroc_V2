#include "can_lookup.h"

const CAN_Lookup_Entry_t can_lookup_table[FUNCTION_COUNT] = {
    {2, 0x04, FUNCTION_C, PAIR_1},
    {2, 0x01, FUNCTION_D, PAIR_1},
    {3, 0x01, FUNCTION_E, PAIR_2},
    {3, 0x02, FUNCTION_F, PAIR_2},
    {3, 0x04, FUNCTION_G, PAIR_3},
    {3, 0x08, FUNCTION_H, PAIR_3},
    {6, 0x40, FUNCTION_M, PAIR_4},
    {6, 0x10, FUNCTION_N, PAIR_4},
    {6, 0x04, FUNCTION_A, PAIR_5},
    {6, 0x01, FUNCTION_P, PAIR_5},
    {2, 0x40, FUNCTION_J, PAIR_6},
    {2, 0x10, FUNCTION_L, PAIR_6}
};

Function_t CAN_get_function_from_data(uint8_t byte_index, uint8_t value) {
    for (uint8_t i = 0; i < FUNCTION_COUNT; i++) {
        if (can_lookup_table[i].byte_index == byte_index && 
            can_lookup_table[i].value == value) {
            return can_lookup_table[i].function;
        }
    }
    return FUNCTION_COUNT; // Invalid
}

Pair_t CAN_get_pair_for_function(Function_t function) {
    if (function >= FUNCTION_COUNT) return PAIR_COUNT;
    return can_lookup_table[function].pair;
}
