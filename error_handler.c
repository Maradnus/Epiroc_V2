/**
 * @file error_handler.c
 * @brief Error handler implementation
 * @author Claude
 * @date April 26, 2025
 */

#include "error_handler.h"
#include "LED_ctrl.h"
#include "system_timer.h"
#include <avr/io.h>
#include <avr/eeprom.h>

// Define module IDs for error logging
#define MODULE_SYSTEM   0
#define MODULE_CAN      1
#define MODULE_LED      2
#define MODULE_MODE     3
#define MODULE_SOLENOID 4
#define MODULE_EEPROM   5
#define MODULE_SENSOR   6

// Maximum number of errors to log
#define MAX_ERROR_LOG   10

// Error log structure
typedef struct {
    Error_Code_t error_code;
    uint8_t module;
    uint32_t timestamp;
} Error_Log_t;

// Error log array
static Error_Log_t error_log[MAX_ERROR_LOG];
static uint8_t error_log_index = 0;
static uint8_t error_count = 0;

// Current system error status
static Error_Code_t current_error = ERR_NONE;

// Initialize error handler
void Err_Init(void)
{
    // Clear error log
    for (uint8_t i = 0; i < MAX_ERROR_LOG; i++) {
        error_log[i].error_code = ERR_NONE;
        error_log[i].module = 0;
        error_log[i].timestamp = 0;
    }
    
    error_log_index = 0;
    error_count = 0;
    current_error = ERR_NONE;
}

// Detect system errors
Error_Code_t Err_DetectSysError(void)
{
    // Check current sensor for overcurrent
    if ((ADMUX & 0x0F) == 0x01 && (ADCSRA & (1 << ADIF))) {
        // Read ADC value
        uint16_t adc_value = ADC;
        
        // Check if current is too high (threshold example)
        if (adc_value > 900) { // Adjust threshold based on actual system
            return ERR_OVERCURRENT;
        }
    }
    
    // Check CAN communication status
    if ((CANGSTA & (1 << TXBSY)) && (Timer_GetMillis() % 1000 == 0)) {
        // If TX has been busy for a long time, consider it a failure
        return ERR_CAN_COMM_FAIL;
    }
    
    return ERR_NONE;
}

// Log an error
void Err_LogError(Error_Code_t error, uint8_t module)
{
    if (error != ERR_NONE) {
        // Log error details
        error_log[error_log_index].error_code = error;
        error_log[error_log_index].module = module;
        error_log[error_log_index].timestamp = Timer_GetMillis();
        
        // Update index
        error_log_index = (error_log_index + 1) % MAX_ERROR_LOG;
        
        // Update error count
        if (error_count < MAX_ERROR_LOG) {
            error_count++;
        }
        
        // Store current error
        current_error = error;
    }
}

// Try to recover from an error
void Err_RecoverFromError(Error_Code_t error)
{
    switch (error) {
        case ERR_CAN_INIT_FAIL:
        case ERR_CAN_COMM_FAIL:
            // Reset CAN module
            CANGCON |= (1 << SWRES);
            Timer_Delay(10);
            // Re-initialize
            CANGCON = (1 << ENASTB);
            break;
            
        case ERR_OVERCURRENT:
            // Turn off all outputs and wait
            PORTA &= 0x00;  // Clear all port A outputs
            PORTC &= 0xF0;  // Clear lower 4 bits of port C
            Timer_Delay(1000);  // Wait 1 second
            break;
            
        case ERR_EEPROM_FAIL:
            // No specific recovery, just log
            break;
            
        case ERR_SYSTEM_FAIL:
            // System reset - in a real implementation, consider watchdog
            Timer_Delay(100);
            // In real code: use watchdog to reset
            break;
            
        default:
            break;
    }
    
    // Clear current error if recovery was attempted
    current_error = ERR_NONE;
}

// Trigger appropriate error protocol
void Err_TriggerErrProtocol(Error_Code_t error)
{
    switch (error) {
        case ERR_CAN_INIT_FAIL:
        case ERR_CAN_COMM_FAIL:
            // Set LED2 to red blinking to indicate CAN error
            LED_Set(LED2_RED, BLINK, 500);
            break;
            
        case ERR_OVERCURRENT:
            // Set LED3 to red blinking to indicate output error
            LED_Set(LED3_RED, BLINK, 200);
            break;
            
        case ERR_EEPROM_FAIL:
            // Set LED4 to red to indicate configuration error
            LED_Set(LED4_RED, ON, 0);
            break;
            
        case ERR_SYSTEM_FAIL:
            // Set all LEDs to red
            LED_Set(LED1_RED, ON, 0);
            LED_Set(LED2_RED, ON, 0);
            LED_Set(LED3_RED, ON, 0);
            LED_Set(LED4_RED, ON, 0);
            break;
            
        default:
            break;
    }
    
    // Try to recover from error
    Err_RecoverFromError(error);
}
