#include "mode_controller.h"
#include <avr/eeprom.h>
#include "led.h"
#include "can_lookup.h"
#include "debug.h"
#include "eeprom.h"
#include "system_timer.h"

static Output_Mode_t pair_modes[PAIR_COUNT];
static uint32_t mode_led_start_time = 0;
static const uint16_t MODE_LED_TIMEOUT = 300; // 30 seconds in deciseconds

void Mode_init(void) {
    DEBUG_PRINTLN("Initializing mode controller");
    EEPROM_init();
    
    // Read modes from EEPROM for pair 1 and pair BY DEFAULT
    pair_modes[PAIR_1] = eeprom_read_byte((uint8_t*)EEPROM_MODE_PAIR1);
    pair_modes[PAIR_6] = eeprom_read_byte((uint8_t*)EEPROM_MODE_PAIR6);
    
    // Default all other pairs to MOMENTARY mode
    for (uint8_t i = 1; i < PAIR_COUNT - 1; i++) {
        pair_modes[i] = MOMENTARY;
    }
    
    DEBUG_PRINT("Pair 1 mode: ");
    DEBUG_PRINTLN(pair_modes[PAIR_1] ? "LATCH" : "MOMENTARY");
    DEBUG_PRINT("Pair 6 mode: ");
    DEBUG_PRINTLN(pair_modes[PAIR_6] ? "LATCH" : "MOMENTARY");
    
    // Default to momentary if EEPROM is uninitialized
    if (pair_modes[PAIR_1] > LATCH) {
        DEBUG_PRINTLN("Resetting pair 1 to default");
        pair_modes[PAIR_1] = MOMENTARY;
        EEPROM_write_byte(EEPROM_MODE_PAIR1, MOMENTARY);
    }
    if (pair_modes[PAIR_6] > LATCH) {
        DEBUG_PRINTLN("Resetting pair 6 to default");
        pair_modes[PAIR_6] = MOMENTARY;
        EEPROM_write_byte(EEPROM_MODE_PAIR6, MOMENTARY);
    }
    
    // Initialize timer for LED
    mode_led_start_time = 0;
}

void Mode_check_startup_key(void) {
    bool key_detected = false;
    
    // Check for pair 1 keys (C and D)
    if (FUNCTION_C || FUNCTION_D) {
        // If C is pressed, set MOMENTARY
        if (FUNCTION_C) {
            DEBUG_PRINTLN("C key detected at startup - setting PAIR_1 to MOMENTARY");
            Mode_set_momentary(PAIR_1);
            key_detected = true;
        }
        // If D is pressed, set LATCH
        else if (FUNCTION_D) {
            DEBUG_PRINTLN("D key detected at startup - setting PAIR_1 to LATCH");
            Mode_set_latch(PAIR_1);
            key_detected = true;
        }
    }
    
    // Check for pair 6 keys (J and L)
    if (FUNCTION_J || FUNCTION_L) {
        // If L is pressed, set MOMENTARY
        if (FUNCTION_L) {
            DEBUG_PRINTLN("L key detected at startup - setting PAIR_6 to MOMENTARY");
            Mode_set_momentary(PAIR_6);
            key_detected = true;
        }
        // If J is pressed, set LATCH
        else if (FUNCTION_J) {
            DEBUG_PRINTLN("J key detected at startup - setting PAIR_6 to LATCH");
            Mode_set_latch(PAIR_6);
            key_detected = true;
        }
    }
    
    // If any key detected, turn on mode LED for 30 seconds
    if (key_detected) {
        LED_set(LED_MODE, LED_ON, 0);
        mode_led_start_time = system_timer_get_ms();
    } else {
        // No startup key detected, just read from EEPROM (already done in Mode_init)
        DEBUG_PRINTLN("No startup key detected, using stored modes from EEPROM");
    }
}

void Mode_set_latch(Pair_t pair) {
    DEBUG_PRINT("Setting pair ");
    DEBUG_PRINT_NUM(pair);
    DEBUG_PRINTLN(" to LATCH");
    if (pair >= PAIR_COUNT) return;
    
    pair_modes[pair] = LATCH;
    
    // Update LED to indicate mode change
    LED_set(LED_MODE, LED_ON, 0);
    mode_led_start_time = system_timer_get_ms();
    
    // Store to EEPROM if this is pair 1 or 6
    if (pair == PAIR_1) {
        EEPROM_write_byte(EEPROM_MODE_PAIR1, LATCH);
    } else if (pair == PAIR_6) {
        EEPROM_write_byte(EEPROM_MODE_PAIR6, LATCH);
    }
}

void Mode_set_momentary(Pair_t pair) {
    DEBUG_PRINT("Setting pair ");
    DEBUG_PRINT_NUM(pair);
    DEBUG_PRINTLN(" to MOMENTARY");
    
    if (pair >= PAIR_COUNT) return;
    
    pair_modes[pair] = MOMENTARY;
    
    // Update LED to indicate mode change
    LED_set(LED_MODE, LED_ON, 0);
    mode_led_start_time = system_timer_get_ms();
    
    // Store to EEPROM if this is pair 1 or 6
    if (pair == PAIR_1) {
        EEPROM_write_byte(EEPROM_MODE_PAIR1, MOMENTARY);
    } else if (pair == PAIR_6) {
        EEPROM_write_byte(EEPROM_MODE_PAIR6, MOMENTARY);
    }
}

void Mode_store_prev(void) {
    // Check if mode LED timeout has occurred
    if (mode_led_start_time > 0) {
        uint32_t current_time = system_timer_get_ms();
        if ((current_time - mode_led_start_time) >= (MODE_LED_TIMEOUT * 100)) {
            LED_set(LED_MODE, LED_OFF, 0);
            mode_led_start_time = 0;
        }
    }
}

Output_Mode_t Mode_get_pair_mode(Pair_t pair) {
    if (pair < PAIR_COUNT) {
        return pair_modes[pair];
    }
    return MOMENTARY; // Default to momentary mode
}