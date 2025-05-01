#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <stdint.h>
#include "config.h"
void system_timer_init(void);
uint32_t system_timer_get_ms(void);
void system_timer_delay_ms(uint32_t ms);

#endif // SYSTEM_TIMER_H