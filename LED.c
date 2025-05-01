#include "led.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "system_timer.h"

typedef struct {
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    uint8_t pin;
    LED_State_t state;
    uint16_t blink_period;
    uint32_t last_toggle_time;
    bool current_state;
} LED_Config_t;

static LED_Config_t led_config[LED_COUNT];

void LED_init(void) {
    // Power LED (PC4)
    led_config[LED_POWER].port = &PORTC;
    led_config[LED_POWER].ddr = &DDRC;
    led_config[LED_POWER].pin = PC5;
    
    // CAN LED (PB0)
    led_config[LED_CAN].port = &PORTB;
    led_config[LED_CAN].ddr = &DDRB;
    led_config[LED_CAN].pin = PB0;
    
    // Output LED (PB3)
    led_config[LED_OUTPUT].port = &PORTB;
    led_config[LED_OUTPUT].ddr = &DDRB;
    led_config[LED_OUTPUT].pin = PB3;
    
    // Mode LED (PB6)
    led_config[LED_MODE].port = &PORTB;
    led_config[LED_MODE].ddr = &DDRB;
    led_config[LED_MODE].pin = PB6;
    
    // Initialize all LEDs
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        *(led_config[i].ddr) |= (1 << led_config[i].pin);
        *(led_config[i].port) &= ~(1 << led_config[i].pin);
        led_config[i].state = LED_OFF;
        led_config[i].blink_period = 0;
        led_config[i].last_toggle_time = 0;
        led_config[i].current_state = false;
    }
    
    
}

void LED_set(LED_t led, LED_State_t state, uint16_t blink_period) {
    if (led >= LED_COUNT) return;
    
    cli();
    led_config[led].state = state;
    led_config[led].blink_period = blink_period;
    
    switch (state) {
        case LED_OFF:
            *(led_config[led].port) &= ~(1 << led_config[led].pin);
            led_config[led].current_state = false;
            break;
        case LED_ON:
            *(led_config[led].port) |= (1 << led_config[led].pin);
            led_config[led].current_state = true;
            break;
        case LED_BLINK:
            led_config[led].last_toggle_time = system_timer_get_ms();
            *(led_config[led].port) |= (1 << led_config[led].pin);
            led_config[led].current_state = true;
            break;
    }
    sei();
}

void LED_status_update(void) {
    uint32_t current_time = system_timer_get_ms();
    
    for (uint8_t i = 0; i < LED_COUNT; i++) {
        if (led_config[i].state == LED_BLINK && led_config[i].blink_period > 0) {
            if ((current_time - led_config[i].last_toggle_time) >= led_config[i].blink_period) {
                if (led_config[i].current_state) {
                    *(led_config[i].port) &= ~(1 << led_config[i].pin);
                } else {
                    *(led_config[i].port) |= (1 << led_config[i].pin);
                }
                led_config[i].current_state = !led_config[i].current_state;
                led_config[i].last_toggle_time = current_time;
            }
        }
    }
}
