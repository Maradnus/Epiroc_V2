/**
 * @file led.c
 * @brief LED control implementation
 * @author Claude
 * @date April 2, 2025
 */

#include "LED_ctrl.h"
#include "system_timer.h"
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

// Structure to hold LED configuration
typedef struct {
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t pin;
    LED_State_t state;
    uint16_t blink_period;
    uint32_t last_toggle;
    bool current_state;
} LED_Config_t;

// LED configurations
static LED_Config_t led_config[LED_COUNT];

// Get current time in milliseconds
static uint32_t get_current_time(void)
{
    return Timer_GetMillis();
}

// Initialize LED configurations
void LED_Init(void)
{
    // Configure LED1 (PORT C)
    led_config[LED1_RED].port = &PORTC;
    led_config[LED1_RED].ddr = &DDRC;
    led_config[LED1_RED].pin = PC4;
    
    led_config[LED1_GREEN].port = &PORTC;
    led_config[LED1_GREEN].ddr = &DDRC;
    led_config[LED1_GREEN].pin = PC5;
    
    led_config[LED1_BLUE].port = &PORTC;
    led_config[LED1_BLUE].ddr = &DDRC;
    led_config[LED1_BLUE].pin = PC6;
    
    // Configure LED2 (PORT B)
    led_config[LED2_RED].port = &PORTB;
    led_config[LED2_RED].ddr = &DDRB;
    led_config[LED2_RED].pin = PB0;
    
    led_config[LED2_GREEN].port = &PORTB;
    led_config[LED2_GREEN].ddr = &DDRB;
    led_config[LED2_GREEN].pin = PB1;
    
    led_config[LED2_BLUE].port = &PORTB;
    led_config[LED2_BLUE].ddr = &DDRB;
    led_config[LED2_BLUE].pin = PB2;
    
    // Configure LED3 (PORT B)
    led_config[LED3_RED].port = &PORTB;
    led_config[LED3_RED].ddr = &DDRB;
    led_config[LED3_RED].pin = PB3;
    
    led_config[LED3_GREEN].port = &PORTB;
    led_config[LED3_GREEN].ddr = &DDRB;
    led_config[LED3_GREEN].pin = PB4;
    
    led_config[LED3_BLUE].port = &PORTB;
    led_config[LED3_BLUE].ddr = &DDRB;
    led_config[LED3_BLUE].pin = PB5;
    
    // Configure LED4 (PORT B and PORT G)
    led_config[LED4_RED].port = &PORTB;
    led_config[LED4_RED].ddr = &DDRB;
    led_config[LED4_RED].pin = PB6;
    
    led_config[LED4_GREEN].port = &PORTB;
    led_config[LED4_GREEN].ddr = &DDRB;
    led_config[LED4_GREEN].pin = PB7;
    
    led_config[LED4_BLUE].port = &PORTG;
    led_config[LED4_BLUE].ddr = &DDRG;
    led_config[LED4_BLUE].pin = PG0;
    
    // Configure all LEDs as outputs and turn them off initially
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        *(led_config[i].ddr) |= (1 << led_config[i].pin);
        *(led_config[i].port) &= ~(1 << led_config[i].pin); // Active high logic
        led_config[i].state = OFF;
        led_config[i].blink_period = 0;
        led_config[i].last_toggle = 0;
        led_config[i].current_state = false;
    }
}

// Set LED state
void LED_Set(LED_t led, LED_State_t state, uint16_t blink_period)
{
    if (led >= LED_COUNT) {
        return;
    }
    
    led_config[led].state = state;
    
    switch (state) {
        case OFF:
            *(led_config[led].port) &= ~(1 << led_config[led].pin);
            led_config[led].current_state = false;
            break;
        case ON:
            *(led_config[led].port) |= (1 << led_config[led].pin);
            led_config[led].current_state = true;
            break;
        case BLINK:
            led_config[led].blink_period = blink_period;
            led_config[led].last_toggle = get_current_time();
            led_config[led].current_state = true;
            *(led_config[led].port) |= (1 << led_config[led].pin);
            break;
        default:
            break;
    }
}

// Update LED states (to be called regularly)
void LED_Update(void)
{
    uint32_t current_time = get_current_time();
    
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        if (led_config[i].state == BLINK && led_config[i].blink_period > 0) {
            if ((current_time - led_config[i].last_toggle) >= led_config[i].blink_period) {
                // Toggle LED
                if (led_config[i].current_state) {
                    *(led_config[i].port) &= ~(1 << led_config[i].pin);
                    led_config[i].current_state = false;
                } else {
                    *(led_config[i].port) |= (1 << led_config[i].pin);
                    led_config[i].current_state = true;
                }
                led_config[i].last_toggle = current_time;
            }
        }
    }
}