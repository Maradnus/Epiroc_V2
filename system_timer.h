#ifndef SYSTEM_TIMER_H
#define SYSTEM_TIMER_H

#include <avr/io.h>
#include <stdint.h>

// Function prototypes
void Timer_Init(void);
uint32_t Timer_GetMillis(void);
void Timer_Delay(uint16_t ms);

#endif /* SYSTEM_TIMER_H */