#ifndef MODE_CONTROLLER_H
#define MODE_CONTROLLER_H

#include "common.h"
#include "can_lookup.h"

typedef enum {
    MOMENTARY = 0,
    LATCH
} Output_Mode_t;

void Mode_init(void);
void Mode_check_startup_key(void);
void Mode_set_latch(Pair_t pair);
void Mode_set_momentary(Pair_t pair);
void Mode_store_prev(void);
Output_Mode_t Mode_get_pair_mode(Pair_t pair);

#endif // MODE_CONTROLLER_H