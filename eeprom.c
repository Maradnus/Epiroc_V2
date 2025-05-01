#include "eeprom.h"
#include <avr/eeprom.h>
#include "debug.h"

void EEPROM_init(void) {
    DEBUG_PRINTLN("Initializing EEPROM");
    
    // Check if EEPROM has been initialized
    if (!EEPROM_is_initialized()) {
        DEBUG_PRINTLN("EEPROM not initialized, setting default values");
        
        // Set default mode values (MOMENTARY = 0)
        EEPROM_write_byte(EEPROM_MODE_PAIR1, 0);
        EEPROM_write_byte(EEPROM_MODE_PAIR6, 0);
        
        // Write magic value to indicate initialization
        EEPROM_write_byte(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VALUE);
        DEBUG_PRINTLN("Magic value written to EEPROM");
        uint8_t magic_val = EEPROM_read_byte(EEPROM_MAGIC_ADDR);
        DEBUG_PRINT("Read back magic value: 0x");
        DEBUG_PRINT_HEX(magic_val);
        DEBUG_PRINTLN("");
    } else {
        DEBUG_PRINTLN("EEPROM already initialized");
    }
}

uint8_t EEPROM_read_byte(uint8_t addr) {
    return eeprom_read_byte((uint8_t*)addr);
}

void EEPROM_write_byte(uint8_t addr, uint8_t data) {
    eeprom_update_byte((uint8_t*)addr, data);
}

bool EEPROM_is_initialized(void) {
    return (EEPROM_read_byte(EEPROM_MAGIC_ADDR) == EEPROM_MAGIC_VALUE);
}
