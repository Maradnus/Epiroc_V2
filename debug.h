#ifndef DEBUG_H
#define DEBUG_H

#include "config.h"
#include <avr/io.h>
#include <util/delay.h>

void debug_init(void);
void debug_print(const char *str);
void debug_println(const char *str);
void debug_print_number(uint32_t num);
void debug_print_hex(uint8_t value);

// Macro for conditional debug printing
#define DEBUG_PRINT(msg) (DEBUG_ENABLED ? debug_print(msg) : (void)0)
#define DEBUG_PRINTLN(msg) (DEBUG_ENABLED ? debug_println(msg) : (void)0)
#define DEBUG_PRINT_NUM(num) (DEBUG_ENABLED ? debug_print_number(num) : (void)0)
#define DEBUG_PRINT_HEX(hex) (DEBUG_ENABLED ? debug_print_hex(hex) : (void)0)

#endif // DEBUG_H