/**
 * @file mode_ctrl.c
 * @brief Mode controller implementation
 * @author Claude
 * @date April 25, 2025
 */

#include "mode_ctrl.h"
#include "can_ctrl.h"
#include "eeprom_ctrl.h"
#include "LED_ctrl.h"
#include "system_timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// Mode configuration for each channel pair
static Mode_t pair_modes[PAIR_COUNT];

// Flag to indicate if mode change has occurred
static bool mode_changed = false;

// Timer for MODE LED indication
static uint32_t mode_led_start_time = 0;
static const uint16_t MODE_LED_TIMEOUT = 30000; // 30 seconds in ms

/**
 * @brief Initialize mode controller
 */
void Mode_Init(void)
{
    // Load modes from EEPROM
    for (uint8_t i = 0; i < PAIR_COUNT; i++) {
        pair_modes[i] = EEPROM_ReadMode((Channel_Pair_t)i);
    }
    
    // Initialize mode changed flag
    mode_changed = false;
    
    // Initialize timer
    mode_led_start_time = 0;
}

/**
 * @brief Check for startup key presses
 */
void Mode_CheckStartupKey(void)
{
    // Check key presses at startup
    // This function should be called soon after power on
    bool mode_key_pressed = false;
    
    // Check for D key press (change to latch mode for pair 1)
    if (signal_D) {
        Mode_SetLatch(PAIR_1);
        mode_key_pressed = true;
    }
    
    // Check for J key press (change to latch mode for pair 6)
    if (signal_J) {
        Mode_SetLatch(PAIR_6);
        mode_key_pressed = true;
    }
    
    // Check for C key press (change to momentary mode for pair 1)
    if (signal_C) {
        Mode_SetMomentary(PAIR_1);
        mode_key_pressed = true;
    }
    
    // Check for L key press (change to momentary mode for pair 6)
    if (signal_L) {
        Mode_SetMomentary(PAIR_6);
        mode_key_pressed = true;
    }
    
    // If any mode key was pressed, turn on mode LED for 30 seconds
    if (mode_key_pressed) {
        LED_Set(LED4_GREEN, ON, 0);
        mode_led_start_time = Timer_GetMillis();
        mode_changed = true;
    }
}

/**
 * @brief Set latch mode for specified pair
 * @param pair Channel pair to set to latch mode
 */
void Mode_SetLatch(Channel_Pair_t pair)
{
    if (pair < PAIR_COUNT) {
        pair_modes[pair] = MODE_LATCH;
        mode_changed = true;
        
        // Store to EEPROM immediately
        EEPROM_WriteMode(pair, MODE_LATCH);
    }
}

/**
 * @brief Set momentary mode for specified pair
 * @param pair Channel pair to set to momentary mode
 */
void Mode_SetMomentary(Channel_Pair_t pair)
{
    if (pair < PAIR_COUNT) {
        pair_modes[pair] = MODE_MOMENTARY;
        mode_changed = true;
        
        // Store to EEPROM immediately
        EEPROM_WriteMode(pair, MODE_MOMENTARY);
    }
}

/**
 * @brief Store current modes to EEPROM
 */
void Mode_StorePrev(void)
{
    // Only store if changes occurred
    if (mode_changed) {
        for (uint8_t i = 0; i < PAIR_COUNT; i++) {
            EEPROM_WriteMode((Channel_Pair_t)i, pair_modes[i]);
        }
        mode_changed = false;
    }
    
    // Update mode LED status
    if (mode_led_start_time > 0) {
        uint32_t current_time = Timer_GetMillis();
        if ((current_time - mode_led_start_time) >= MODE_LED_TIMEOUT) {
            LED_Set(LED4_GREEN, OFF, 0);
            mode_led_start_time = 0;
        }
    }
}

/**
 * @brief Get mode for specified pair
 * @param pair Channel pair to query
 * @return Current mode for the pair
 */
Mode_t Mode_GetPairMode(Channel_Pair_t pair)
{
    if (pair < PAIR_COUNT) {
        return pair_modes[pair];
    }
    return MODE_MOMENTARY; // Default to momentary mode
}
