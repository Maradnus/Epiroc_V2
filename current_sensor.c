/**
 * @file current_sensor.c
 * @brief Current sensor implementation
 * @author Claude
 * @date April 26, 2025
 */

#include "current_sensor.h"
#include "error_handler.h"
#include <avr/io.h>
#include <avr/interrupt.h>

// ADC reference and channel
#define ADC_CHANNEL     1       // PF1
#define OVERCURRENT_THRESHOLD 900  // Adjust based on actual system requirements

// Current measurement variables
static volatile uint16_t current_value = 0;
static volatile bool adc_conversion_complete = false;

// Initialize current sensor
void CurrentSensor_Init(void)
{
    // Configure ADC
    
    // Set ADC reference to AVCC
    ADMUX = (1 << REFS0);
    
    // Select channel ADC1 (PF1)
    ADMUX |= ADC_CHANNEL;
    
    // Enable ADC, enable interrupt, prescaler 128 (125kHz @ 16MHz)
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // Configure PF1 as input
    DDRF &= ~(1 << PF1);
    
    // Start first conversion
    ADCSRA |= (1 << ADSC);
}

// Get current value
uint16_t CurrentSensor_GetCurrent(void)
{
    // Start new conversion if not already in progress
    if (!(ADCSRA & (1 << ADSC))) {
        ADCSRA |= (1 << ADSC);
    }
    
    return current_value;
}

// Check if current is over the threshold
bool CurrentSensor_IsOverC
