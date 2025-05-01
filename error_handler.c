#include "error_handler.h"
#include <avr/eeprom.h>
#include <avr/io.h>
#include <util/delay.h>
#include "led.h"
#include "debug.h"
#define CURRENT_SAMPLES 16  // Number of samples for averaging

static Error_t current_error = ERROR_NONE;
static bool output_active = false;  // Flag to indicate if any output is active

// Initialize ADC for current sensing on PF1 (ADC1)
static void ADC_init(void) {
    // Set PF1 as input (ADC1)
    DDRF &= ~(1 << PF1);
    
    // AVCC with external capacitor at AREF pin
    ADMUX = (1 << REFS0);
    
    // Enable ADC and set prescaler to 128 (16MHz/128 = 125kHz)
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    DEBUG_PRINTLN("ADC initialized for current sensing on PF1 (ADC1)");
}

// Read ADC value from current sensor on PF1 (ADC1)
static uint16_t ADC_read(void) {
    // Select ADC1 channel (PF1)
    ADMUX = (ADMUX & 0xF0) | (CURRENT_SENSOR_ADC_CHANNEL & 0x0F);
    
    // Start single conversion
    ADCSRA |= (1 << ADSC);
    
    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC));
    
    // Return ADC value
    return ADC;
}

// Read current in Amps
float Err_read_current(void) {
    uint16_t adc_value = ADC_read();
    
    // Convert ADC value to voltage
    float voltage = (adc_value * ADC_REF_VOLTAGE) / ADC_RESOLUTION;
    
    // Convert voltage to current (A)
    float current = (voltage - (ADC_REF_VOLTAGE/2)) / CURRENT_SENSOR_SENSITIVITY;
    
    DEBUG_PRINT("Current Sensor (PF1) - Raw: ");
    DEBUG_PRINT_NUM(adc_value);
    DEBUG_PRINT(" (");
    DEBUG_PRINT_NUM((int)(voltage * 1000)); // mV
    DEBUG_PRINT("mV) Current: ");
    DEBUG_PRINT_NUM((int)(current * 1000)); // mA
    DEBUG_PRINTLN(" mA");
    
    return current;
}

float Err_read_current_filtered(void) {
    uint32_t adc_sum = 0;
    
    // Take multiple samples for averaging
    for (uint8_t i = 0; i < CURRENT_SAMPLES; i++) {
        adc_sum += ADC_read();
        _delay_us(100);  // Small delay between samples
    }
    
    uint16_t adc_avg = adc_sum / CURRENT_SAMPLES;
    
    // Convert to current
    float voltage = (adc_avg * ADC_REF_VOLTAGE) / ADC_RESOLUTION;
    float current = (voltage - (ADC_REF_VOLTAGE/2)) / CURRENT_SENSOR_SENSITIVITY;
    
    DEBUG_PRINT("Filtered Current (PF1) - Avg Raw: ");
    DEBUG_PRINT_NUM(adc_avg);
    DEBUG_PRINT(" (");
    DEBUG_PRINT_NUM((int)(voltage * 1000));
    DEBUG_PRINT("mV) Current: ");
    DEBUG_PRINT_NUM((int)(current * 1000));
    DEBUG_PRINTLN(" mA");
    
    return current;
}

void Err_init(void) {
    current_error = ERROR_NONE;
    output_active = false;
    ADC_init();  // Initialize ADC for current sensing
    DEBUG_PRINTLN("Error handler initialized");
}

void Err_detect_sys_error(void) {
    // Only check current if outputs are active
    if (output_active) {
        float current = Err_read_current_filtered();
        
        if (current > (MAX_TOTAL_CURRENT / 1000.0)) {
            Err_trigger_err_protocol(ERROR_OVER_CURRENT);
        }
    }
    
    // Add other error detection logic here
}

void Err_log_error(Error_t error, const char* module) {
    if (error >= ERROR_COUNT) return;
    
    current_error = error;
    
    DEBUG_PRINT("Error in ");
    DEBUG_PRINT(module);
    DEBUG_PRINT(": ");
    
    switch (error) {
        case ERROR_CAN_COMM:
            DEBUG_PRINTLN("CAN communication error");
            LED_set(LED_CAN, LED_BLINK, 200);
            break;
        case ERROR_OVER_CURRENT: {
            float current = Err_read_current();
            DEBUG_PRINT("OVER CURRENT: ");
            DEBUG_PRINT_NUM((int)(current * 1000));
            DEBUG_PRINTLN(" mA");
            LED_set(LED_POWER, LED_BLINK, 200);
            break;
        }
        case ERROR_CHANNEL_CONFLICT:
            DEBUG_PRINTLN("Channel conflict detected");
            LED_set(LED_OUTPUT, LED_BLINK, 200);
            break;
        case ERROR_EEPROM:
            DEBUG_PRINTLN("EEPROM error");
            break;
        default:
            DEBUG_PRINTLN("Unknown error");
            break;
    }
}

void Err_recover_from_error(Error_t error) {
    switch (error) {
        case ERROR_OVER_CURRENT:
            // Turn off all outputs
            DEBUG_PRINTLN("Recovering from over-current - disabling outputs");
            output_active = false;
            // You would call solenoid module to turn everything off here
            break;
        default:
            DEBUG_PRINT("Recovering from error: ");
            DEBUG_PRINT_NUM(error);
            DEBUG_PRINTLN("");
            break;
    }
    
    current_error = ERROR_NONE;
}

void Err_trigger_err_protocol(Error_t error) {
    Err_log_error(error, "SYSTEM");
    Err_recover_from_error(error);
}

// Set the output active flag
void Err_set_output_active(bool active) {
    output_active = active;
    if (active) {
        DEBUG_PRINTLN("Outputs active, enabling current monitoring");
    } else {
        DEBUG_PRINTLN("All outputs inactive, disabling current monitoring");
    }
}