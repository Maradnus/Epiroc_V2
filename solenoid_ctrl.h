/**
 * @file solenoid_ctrl.h
 * @brief Solenoid control header
 * @author Claude
 * @date April 26, 2025
 */

#ifndef SOLENOID_CTRL_H
#define SOLENOID_CTRL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "mode_ctrl.h"

// Pin mapping for solenoids
typedef enum {
    SOL_C = 0,  // PA0
    SOL_D = 1,  // PA1
    SOL_E = 2,  // PA2
    SOL_F = 3,  // PA3
    SOL_G = 4,  // PA4
    SOL_H = 5,  // PA5
    SOL_M = 6,  // PA6
    SOL_N = 7,  // PA7
    SOL_A = 8,  // PC0
    SOL_P = 9,  // PC1
    SOL_J = 10, // PC2
    SOL_L = 11, // PC3
    SOL_COUNT
} Solenoid_t;

// Function prototypes
void Sol_Init(void);
void Sol_setPinState(Solenoid_t pin, bool state);
bool Sol_readPinState(Solenoid_t pin);
void Sol_setOutput(void);
void Sol_Update(void);
void Sol_ProcessCANSignals(void);

#endif /* SOLENOID_CTRL_H */