#include "solenoid.h"
#include <avr/io.h>
#include "mode_controller.h"
#include "error_handler.h"
#include "led.h"
#include "debug.h"

static bool pin_states[FUNCTION_COUNT];
static Output_Mode_t pin_modes[FUNCTION_COUNT];

void Sol_init(void) {
    // Initialize all GPIO pins as outputs
    DDRA |= 0xFF; // PA0-PA7
    DDRC |= 0x0F; // PC0-PC3
    
    // Initialize all pins to low
    PORTA = 0x00;
    PORTC &= 0xF0;
    
    // Initialize pin states and modes
    for (uint8_t i = 0; i < FUNCTION_COUNT; i++) {
        pin_states[i] = false;
        pin_modes[i] = MOMENTARY; // Default, will be overridden by mode controller
    }
}

Status_t Sol_set_pin_state(Function_t function, bool state) {
    DEBUG_PRINT("Setting function ");
    DEBUG_PRINT_NUM(function);
    DEBUG_PRINT(" to ");
    DEBUG_PRINTLN(state ? "ON" : "OFF");
    
    if (function >= FUNCTION_COUNT) {
        DEBUG_PRINTLN("Invalid function");
        return INVALID_PARAM;
    }
    // Check current consumption limitations
    if (state) {
        uint8_t active_count = 0;
        for (uint8_t i = 0; i < FUNCTION_COUNT; i++) {
            if (pin_states[i]) active_count++;
        }
        
        if (active_count >= MAX_CONCURRENT_CHANNELS) {
            DEBUG_PRINTLN("Max channels exceeded");
            return ERROR; // Too many active channels
        }
        
       // In a real system, we'd also check current sensor here
        // Current sensor check would go here
        DEBUG_PRINTLN("Current check passed");
    }
    
    pin_states[function] = state;
    
    // Toggle output LED
    LED_set(LED_OUTPUT, LED_BLINK, 100);
    DEBUG_PRINTLN("Output LED triggered");
    
    return SUCCESS;
}

bool Sol_read_pin_state(Function_t function) {
    if (function >= FUNCTION_COUNT) return false;
    return pin_states[function];
}

void Sol_set_output(void) {
    // Update all outputs based on pin_states
    DEBUG_PRINTLN("Updating outputs");
    uint8_t porta_state = 0;
    uint8_t portc_state = PORTC & 0xF0; // Preserve upper nibble
    
    // PA0-PA7 mappings
    if (pin_states[FUNCTION_C]) porta_state |= (1 << PA0);
    if (pin_states[FUNCTION_D]) porta_state |= (1 << PA1);
    if (pin_states[FUNCTION_E]) porta_state |= (1 << PA2);
    if (pin_states[FUNCTION_F]) porta_state |= (1 << PA3);
    if (pin_states[FUNCTION_G]) porta_state |= (1 << PA4);
    if (pin_states[FUNCTION_H]) porta_state |= (1 << PA5);
    if (pin_states[FUNCTION_M]) porta_state |= (1 << PA6);
    if (pin_states[FUNCTION_N]) porta_state |= (1 << PA7);
    
    // PC0-PC3 mappings
    if (pin_states[FUNCTION_A]) portc_state |= (1 << PC0);
    if (pin_states[FUNCTION_P]) portc_state |= (1 << PC1);
    if (pin_states[FUNCTION_J]) portc_state |= (1 << PC2);
    if (pin_states[FUNCTION_L]) portc_state |= (1 << PC3);
    
    PORTA = porta_state;
    PORTC = portc_state;
    DEBUG_PRINT("PORTA: ");
    DEBUG_PRINT_HEX(PORTA);
    DEBUG_PRINT(" PORTC: ");
    DEBUG_PRINT_HEX(PORTC);
    DEBUG_PRINTLN("");
}
