#ifndef LED_H
#define LED_H

#include "common.h"

typedef enum {
    LED_POWER = 0,
    LED_CAN,
    LED_OUTPUT,
    LED_MODE,
    LED_COUNT
} LED_t;

typedef enum {
    LED_OFF = 0,
    LED_ON,
    LED_BLINK
} LED_State_t;

void LED_init(void);
void LED_set(LED_t led, LED_State_t state, uint16_t blink_period);
void LED_status_update(void);

#endif // LED_H