#include "main.h"
#include "system_init.h"
#include "can.h"
#include "solenoid.h"
#include "led.h"
#include "error_handler.h"
#include <avr/interrupt.h>
#include "debug.h"
#include "system_timer.h"
#include "mode_controller.h"

void system_init(void) {
    // Initialize all subsystems
    //system_timer_init();  // Initialize timer first
    debug_init();
    DEBUG_PRINTLN("System starting...");
    // Rest of the Initialization
    // Initialize LEDs last so we can show status
    LED_init();
    Sys_init_power();
    Sys_init_CAN();
    Sys_init_mode();
    Sys_init_solenoid();
    Sys_init_monitor();
    
    DEBUG_PRINTLN("System initialized");
    // Enable global interrupts
    sei();
}

void main_loop(void) {
    CAN_Message_t msg;
    uint32_t last_current_check = 0;
    uint32_t last_can_check = 0;
    uint32_t last_led_update = 0;
    uint32_t last_mode_update = 0;
    uint32_t current_time;
    bool output_changed = false;

    while (1) {
        current_time = system_timer_get_ms();
        
        // Process CAN messages every 10ms
        if (current_time - last_can_check >= 100) {
            if (CAN_process_message() == SUCCESS) {
                while (CAN_extract(&msg) == SUCCESS) {
                    // Process each received message
                    for (uint8_t i = 0; i < 8; i++) {
                        Function_t func = CAN_get_function_from_data(i, msg.data[i]);
                        if (func != FUNCTION_COUNT) {
                            bool current_state = Sol_read_pin_state(func);
                            Sol_set_pin_state(func, !current_state);
                            output_changed = true;
                        }
                    }
                    if (output_changed) {
                        Sol_set_output();
                        // Check if any outputs are active and notify error handler
                        bool any_active = false;
                        for (uint8_t j = 0; j < FUNCTION_COUNT; j++) {
                            if (Sol_read_pin_state((Function_t)j)) {
                                any_active = true;
                                break;
                            }
                        }
                        Err_set_output_active(any_active);
                        output_changed = false;
                    }
                }
            }
            last_can_check = current_time;
        }
        
        // Check current ONLY if any output is active, and every 100ms
        if ((current_time - last_current_check >= 100)) {
            // Error handler will only check current if outputs are active
            Err_detect_sys_error();
            last_current_check = current_time;
        }
        
        // Update LED states every 50ms
        if (current_time - last_led_update >= 50) {
            LED_status_update();
            last_led_update = current_time;
        }
        
        // Update mode controller state every 100ms to check LED timeout
        if (current_time - last_mode_update >= 100) {
            Mode_store_prev();
            last_mode_update = current_time;
        }
    }
}

int main(void) {
    system_init();
    main_loop();
    return 0;
}