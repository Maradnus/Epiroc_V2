#ifndef LED_CTRL_H
#define LED_CTRL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

// LED states
typedef enum {
    OFF = 0,
    ON = 1,
    BLINK = 2
} LED_State_t;

// LED definitions
typedef enum {
    LED1_RED = 0,
    LED1_GREEN,
    LED1_BLUE,
    LED2_RED,
    LED2_GREEN,
    LED2_BLUE,
    LED3_RED,
    LED3_GREEN,
    LED3_BLUE,
    LED4_RED,
    LED4_GREEN,
    LED4_BLUE,
    LED_COUNT
} LED_t;

/**
 * @brief Initialize LED module
 * @return None
 */
void LED_Init(void);

/**
 * @brief Set LED state
 * @param led LED to control
 * @param state ON, OFF, or BLINK
 * @param blink_period Blink period in ms (used only if state is BLINK, 0 for ON/OFF states)
 * @return None
 */
void LED_Set(LED_t led, LED_State_t state, uint16_t blink_period);

/**
 * @brief Update function to be called periodically (for blink handling)
 * @return None
 */
void LED_Update(void);

#endif /* LED_CTRL_H */