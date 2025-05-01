#include "debug.h"

void debug_init(void) {
    // Set baud rate
    UBRR1H = (F_CPU/(DEBUG_UART_BAUD*16L)-1) >> 8;
    UBRR1L = (F_CPU/(DEBUG_UART_BAUD*16L)-1);
    
    // Enable transmitter
    UCSR1B = (1 << TXEN1);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
    
    DEBUG_PRINTLN("Debug UART initialized");
}

void debug_print(const char *str) {
    while (*str) {
        while (!(UCSR1A & (1 << UDRE1)));
        UDR1 = *str++;
    }
}

void debug_println(const char *str) {
    debug_print(str);
    debug_print("\r\n");
}

void debug_print_number(uint32_t num) {
    char buffer[10];
    uint8_t i = 0;
    
    if (num == 0) {
        debug_print("0");
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        while (!(UCSR1A & (1 << UDRE1)));
        UDR1 = buffer[--i];
    }
}

void debug_print_hex(uint8_t value) {
    char nibble;
    
    debug_print("0x");
    
    // Upper nibble
    nibble = (value >> 4) & 0x0F;
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
    
    // Lower nibble
    nibble = value & 0x0F;
    while (!(UCSR1A & (1 << UDRE1)));
    UDR1 = (nibble < 10) ? ('0' + nibble) : ('A' + nibble - 10);
}
