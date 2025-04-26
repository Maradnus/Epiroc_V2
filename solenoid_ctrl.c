/**
 * @file solenoid_ctrl.c
 * @brief Solenoid control implementation
 * @author Claude
 * @date April 26, 2025
 */

#include "solenoid_ctrl.h"
#include "mode_ctrl.h"
#include "can_ctrl.h"
#include "LED_ctrl.h"
#include "current_sensor.h"
#include "error_handler.h"
#include <avr/io.h>

// Solenoid pin configuration
typedef struct {
    volatile uint8_t *port;
    volatile uint8_t *ddr;
    volatile uint8_t *pin_reg;
    uint8_t pin;
    bool state;
    bool latched;
    Channel_Pair_t pair;
} Solenoid_Config_t;

// Pin configurations
static Solenoid_Config_t solenoid_config[SOL_COUNT];

// Initialize solenoid control
void Sol_Init(void)
{
    // Configure PORTA pins for solenoids C-H and M-N
    // PA0 - C
    solenoid_config[SOL_C].port = &PORTA;
    solenoid_config[SOL_C].ddr = &DDRA;
    solenoid_config[SOL_C].pin_reg = &PINA;
    solenoid_config[SOL_C].pin = 0;
    solenoid_config[SOL_C].pair = PAIR_1;
    
    // PA1 - D
    solenoid_config[SOL_D].port = &PORTA;
    solenoid_config[SOL_D].ddr = &DDRA;
    solenoid_config[SOL_D].pin_reg = &PINA;
    solenoid_config[SOL_D].pin = 1;
    solenoid_config[SOL_D].pair = PAIR_1;
    
    // PA2 - E
    solenoid_config[SOL_E].port = &PORTA;
    solenoid_config[SOL_E].ddr = &DDRA;
    solenoid_config[SOL_E].pin_reg = &PINA;
    solenoid_config[SOL_E].pin = 2;
    solenoid_config[SOL_E].pair = PAIR_2;
    
    // PA3 - F
    solenoid_config[SOL_F].port = &PORTA;
    solenoid_config[SOL_F].ddr = &DDRA;
    solenoid_config[SOL_F].pin_reg = &PINA;
    solenoid_config[SOL_F].pin = 3;
    solenoid_config[SOL_F].pair = PAIR_2;
    
    // PA4 - G
    solenoid_config[SOL_G].port = &PORTA;
    solenoid_config[SOL_G].ddr = &DDRA;
    solenoid_config[SOL_G].pin_reg = &PINA;
    solenoid_config[SOL_G].pin = 4;
    solenoid_config[SOL_G].pair = PAIR_3;
    
    // PA5 - H
    solenoid_config[SOL_H].port = &PORTA;
    solenoid_config[SOL_H].ddr = &DDRA;
    solenoid_config[SOL_H].pin_reg = &PINA;
    solenoid_config[SOL_H].pin = 5;
    solenoid_config[SOL_H].pair = PAIR_3;
    
    // PA6 - M
    solenoid_config[SOL_M].port = &PORTA;
    solenoid_config[SOL_M].ddr = &DDRA;
    solenoid_config[SOL_M].pin_reg = &PINA;
    solenoid_config[SOL_M].pin = 6;
    solenoid_config[SOL_M].pair = PAIR_4;
    
    // PA7 - N
    solenoid_config[SOL_N].port = &PORTA;
    solenoid_config[SOL_N].ddr = &DDRA;
    solenoid_config[SOL_N].pin_reg = &PINA;
    solenoid_config[SOL_N].pin = 7;
    solenoid_config[SOL_N].pair = PAIR_4;
    
    // Configure PORTC pins for solenoids A, P, J, L
    // PC0 - A
    solenoid_config[SOL_A].port = &PORTC;
    solenoid_config[SOL_A].ddr = &DDRC;
    solenoid_config[SOL_A].pin_reg = &PINC;
    solenoid_config[SOL_A].pin = 0;
    solenoid_config[SOL_A].pair = PAIR_5;
    
    // PC1 - P
    solenoid_config[SOL_P].port = &PORTC;
    solenoid_config[SOL_P].ddr = &DDRC;
    solenoid_config[SOL_P].pin_reg = &PINC;
    solenoid_config[SOL_P].pin = 1;
    solenoid_config[SOL_P].pair = PAIR_5;
    
    // PC2 - J
    solenoid_config[SOL_J].port = &PORTC;
    solenoid_config[SOL_J].ddr = &DDRC;
    solenoid_config[SOL_J].pin_reg = &PINC;
    solenoid_config[SOL_J].pin = 2;
    solenoid_config[SOL_J].pair = PAIR_6;
    
    // PC3 - L
    solenoid_config[SOL_L].port = &PORTC;
    solenoid_config[SOL_L].ddr = &DDRC;
    solenoid_config[SOL_L].pin_reg = &PINC;
    solenoid_config[SOL_L].pin = 3;
    solenoid_config[SOL_L].pair = PAIR_6;
    
    // Set all pins as outputs and initialize to off
    for (uint8_t i = 0; i < SOL_COUNT; i++) {
        *(solenoid_config[i].ddr) |= (1 << solenoid_config[i].pin);
        *(solenoid_config[i].port) &= ~(1 << solenoid_config[i].pin);
        solenoid_config[i].state = false;
        solenoid_config[i].latched = false;
    }
}

// Set pin state
void Sol_setPinState(Solenoid_t pin, bool state)
{
    if (pin < SOL_COUNT) {
        solenoid_config[pin].state = state;
        
        // Check if pin is in latch mode
        if (Mode_GetPairMode(solenoid_config[pin].pair) == MODE_LATCH) {
            if (state) {
                // In latch mode, remember the latched state
                solenoid_config[pin].latched = true;
                
                // Find the paired pin and turn it off
                for (uint8_t i = 0; i < SOL_COUNT; i++) {
                    if (i != pin && solenoid_config[i].pair == solenoid_config[pin].pair) {
                        solenoid_config[i].state = false;
                        solenoid_config[i].latched = false;
                        *(solenoid_config[i].port) &= ~(1 << solenoid_config[i].pin);
                    }
                }
            }
        }
        
        // Apply the state to the pin
        if (state) {
            *(solenoid_config[pin].port) |= (1 << solenoid_config[pin].pin);
        } else {
            *(solenoid_config[pin].port) &= ~(1 << solenoid_config[pin].pin);
        }
        
        // Toggle output LED to indicate operation
        LED_Set(LED3_GREEN, BLINK, 100);
    }
}

// Read pin state
bool Sol_readPinState(Solenoid_t pin)
{
    if (pin < SOL_COUNT) {
        return solenoid_config[pin].state;
    }
    return false;
}

// Set output based on current states and modes
void Sol_setOutput(void)
{
    // Update pins based on their states and modes
    for (uint8_t i = 0; i < SOL_COUNT; i++) {
        // For momentary mode, pin is active only when state is true
        if (Mode_GetPairMode(solenoid_config[i].pair) == MODE_MOMENTARY) {
            if (solenoid_config[i].state) {
                *(solenoid_config[i].port) |= (1 << solenoid_config[i].pin);
            } else {
                *(solenoid_config[i].port) &= ~(1 << solenoid_config[i].pin);
                solenoid_config[i].latched = false;
            }
        }
        // For latch mode, pin stays active until its paired pin is activated
        else if (Mode_GetPairMode(solenoid_config[i].pair) == MODE_LATCH) {
            if (solenoid_config[i].latched) {
                *(solenoid_config[i].port) |= (1 << solenoid_config[i].pin);
            } else {
                *(solenoid_config[i].port) &= ~(1 << solenoid_config[i].pin);
            }
        }
    }
    
    // Check for overcurrent
    if (CurrentSensor_IsOverCurrent()) {
        // Turn off all outputs and log error
        for (uint8_t i = 0; i < SOL_COUNT; i++) {
            *(solenoid_config[i].port) &= ~(1 << solenoid_config[i].pin);
            solenoid_config[i].state = false;
            solenoid_config[i].latched = false;
        }
        
        Err_LogError(ERR_OVERCURRENT, 4); // 4 = Solenoid module
        Err_TriggerErrProtocol(ERR_OVERCURRENT);
    }
}

// Update function to periodically check and apply solenoid states
void Sol_Update(void)
{
    // Check if any latched solenoids need to be released
    for (uint8_t i = 0; i < SOL_COUNT; i++) {
        // For latch mode, check if paired control was activated
        if (Mode_GetPairMode(solenoid_config[i].pair) == MODE_LATCH && solenoid_config[i].latched) {
            // Find the paired pin
            for (uint8_t j = 0; j < SOL_COUNT; j++) {
                if (j != i && solenoid_config[j].pair == solenoid_config[i].pair) {
                    // If the paired pin was activated, unlatch this pin
                    if (solenoid_config[j].state) {
                        solenoid_config[i].latched = false;
                        *(solenoid_config[i].port) &= ~(1 << solenoid_config[i].pin);
                    }
                }
            }
        }
    }
}

// Process CAN signals and update solenoid states
void Sol_ProcessCANSignals(void)
{
    // Only process if a new CAN message is received
    if (CAN_IsMessageReceived()) {
        uint8_t signal_data[12];
        
        // Extract signals from CAN message
        if (CAN_Extract(signal_data)) {
            // Update solenoid states based on signals
            Sol_setPinState(SOL_C, signal_data[0]);
            Sol_setPinState(SOL_D, signal_data[1]);
            Sol_setPinState(SOL_E, signal_data[2]);
            Sol_setPinState(SOL_F, signal_data[3]);
            Sol_setPinState(SOL_G, signal_data[4]);
            Sol_setPinState(SOL_H, signal_data[5]);
            Sol_setPinState(SOL_M, signal_data[6]);
            Sol_setPinState(SOL_N, signal_data[7]);
            Sol_setPinState(SOL_A, signal_data[8]);
            Sol_setPinState(SOL_P, signal_data[9]);
            Sol_setPinState(SOL_J, signal_data[10]);
            Sol_setPinState(SOL_L, signal_data[11]);
            
            // Apply the output states
            Sol_setOutput();
            
            // Clear message flag
            CAN_ClearMessageFlag();
        }
    }
}