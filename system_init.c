#include "system_init.h"
#include "led.h"
#include "can.h"
#include "mode_controller.h"
#include "solenoid.h"
#include "error_handler.h"
#include "debug.h"

void Sys_init_power(void) {

    LED_set(LED_POWER, LED_ON,0);
    LED_set(LED_CAN, LED_ON, 0);
    LED_set(LED_OUTPUT, LED_ON, 0);
    LED_set(LED_MODE, LED_ON, 0);
    _delay_ms(800);
    LED_set(LED_POWER, LED_OFF,0);
    LED_set(LED_CAN, LED_OFF, 0);
    LED_set(LED_OUTPUT, LED_OFF, 0);
    LED_set(LED_MODE, LED_OFF, 0);
    _delay_ms(800);
    LED_set(LED_POWER, LED_ON,0);
    DEBUG_PRINTLN("System Power initialized");
    // Initialize power management (placeholder)
    // In a real system, would set up power monitoring, etc.
}

void Sys_init_CAN(void) {
    CAN_init();
    
    DEBUG_PRINTLN("CAN initialized");
}

void Sys_init_mode(void) {
    Mode_init();
    Mode_check_startup_key();
    DEBUG_PRINTLN("System Mode initialized");
}

void Sys_init_solenoid(void) {
    Sol_init();
    DEBUG_PRINTLN("System Solenoid initialized");
}

void Sys_init_monitor(void) {
    // Initialize current sensor ADC
    DDRF &= ~(1 << PF1);  // Ensure PF1 is input
    PORTF &= ~(1 << PF1); // No pull-up
    
    Err_init();
    DEBUG_PRINTLN("ADC initialized for signal on PF1 (ADC1)");
}
