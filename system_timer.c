#include "system_timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

volatile static uint32_t system_ticks = 0;

// Initialize Timer1 for 1ms interrupts
void system_timer_init(void) {
    // Configure Timer0 for 1ms interrupts at 16MHz
    TCCR1A = 0;                   // CTC mode
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10); // CTC mode, prescaler 64
    
    // Calculate compare value for 1ms
    OCR1A = (F_CPU / 64 / 1000) - 1;
    
    // Enable compare match interrupt
    TIMSK1 = (1 << OCIE1A);
    
    // Enable global interrupts
    sei();
}

/* Timer1 compare match interrupt
ISR(TIMER1_COMPA_vect) {
    system_ticks++;
}
*/
// Get current ticks in milliseconds
uint32_t system_timer_get_ms(void) {
    uint32_t ticks;
    
    cli();
    ticks = system_ticks;
    sei();
    
    return ticks;
}

// Busy wait delay
void system_timer_delay_ms(uint32_t ms) {
    uint32_t start = system_timer_get_ms();
    while ((system_timer_get_ms() - start) < ms);
}
