/**
 * @file eeprom_ctrl.c
 * @brief EEPROM controller implementation
 * @author Claude
 * @date April 26, 2025
 */

#include "eeprom_ctrl.h"
#include "error_handler.h"
#include <avr/eeprom.h>
#include <avr/io.h>

// Magic value to check if EEPROM has been initialized
#define EEPROM_MAGIC_VALUE 0xA5
#define EEPROM_ADDR_MAGIC  0xFF

// Default CAN ID and baud rate
#define DEFAULT_CAN_ID     0x14FFFFB0
#define DEFAULT_CAN_BAUD   1           // 250 kbps

// Initialize EEPROM controller
void EEPROM_Init(void)
{
    // Check if EEPROM has been initialized
    uint8_t magic = eeprom_read_byte((uint8_t*)EEPROM_ADDR_MAGIC);
    
    // If not initialized, set default values
    if (magic != EEPROM_MAGIC_VALUE) {
        // Set default CAN ID
        EEPROM_WriteCANID(DEFAULT_CAN_ID);
        
        // Set default baud rate
        EEPROM_WriteCANBaud(DEFAULT_CAN_BAUD);
        
        // Set default modes (all momentary)
        for (uint8_t i = 0; i < PAIR_COUNT; i++) {
            EEPROM_WriteMode((Channel_Pair_t)i, MODE_MOMENTARY);
        }
        
        // Write magic value to indicate initialization
        eeprom_write_byte((uint8_t*)EEPROM_ADDR_MAGIC, EEPROM_MAGIC_VALUE);
    }
}

// Write mode for a channel pair
void EEPROM_WriteMode(Channel_Pair_t pair, Mode_t mode)
{
    uint8_t address;
    
    // Calculate EEPROM address based on pair
    switch (pair) {
        case PAIR_1:
            address = EEPROM_ADDR_MODE_PAIR1;
            break;
        case PAIR_2:
            address = EEPROM_ADDR_MODE_PAIR2;
            break;
        case PAIR_3:
            address = EEPROM_ADDR_MODE_PAIR3;
            break;
        case PAIR_4:
            address = EEPROM_ADDR_MODE_PAIR4;
            break;
        case PAIR_5:
            address = EEPROM_ADDR_MODE_PAIR5;
            break;
        case PAIR_6:
            address = EEPROM_ADDR_MODE_PAIR6;
            break;
        default:
            return;
    }
    
    // Write mode to EEPROM
    eeprom_write_byte((uint8_t*)address, (uint8_t)mode);
}

// Read mode for a channel pair
Mode_t EEPROM_ReadMode(Channel_Pair_t pair)
{
    uint8_t address;
    
    // Calculate EEPROM address based on pair
    switch (pair) {
        case PAIR_1:
            address = EEPROM_ADDR_MODE_PAIR1;
            break;
        case PAIR_2:
            address = EEPROM_ADDR_MODE_PAIR2;
            break;
        case PAIR_3:
            address = EEPROM_ADDR_MODE_PAIR3;
            break;
        case PAIR_4:
            address = EEPROM_ADDR_MODE_PAIR4;
            break;
        case PAIR_5:
            address = EEPROM_ADDR_MODE_PAIR5;
            break;
        case PAIR_6:
            address = EEPROM_ADDR_MODE_PAIR6;
            break;
        default:
            return MODE_MOMENTARY; // Default to momentary mode
    }
    
    // Read mode from EEPROM
    uint8_t mode = eeprom_read_byte((uint8_t*)address);
    
    // Validate and return mode
    if (mode <= MODE_LATCH) {
        return (Mode_t)mode;
    } else {
        // If invalid, return default and fix the value
        EEPROM_WriteMode(pair, MODE_MOMENTARY);
        return MODE_MOMENTARY;
    }
}

// Write CAN ID
void EEPROM_WriteCANID(uint32_t can_id)
{
    // Write CAN ID (4 bytes)
    eeprom_write_byte((uint8_t*)EEPROM_ADDR_CAN_ID_HIGH, (uint8_t)(can_id >> 24));
    eeprom_write_byte((uint8_t*)(EEPROM_ADDR_CAN_ID_HIGH + 1), (uint8_t)(can_id >> 16));
    eeprom_write_byte((uint8_t*)(EEPROM_ADDR_CAN_ID_HIGH + 2), (uint8_t)(can_id >> 8));
    eeprom_write_byte((uint8_t*)(EEPROM_ADDR_CAN_ID_HIGH + 3), (uint8_t)can_id);
}

// Read CAN ID
uint32_t EEPROM_ReadCANID(void)
{
    // Read CAN ID (4 bytes)
    uint32_t can_id = 0;
    
    can_id = (uint32_t)eeprom_read_byte((uint8_t*)EEPROM_ADDR_CAN_ID_HIGH) << 24;
    can_id |= (uint32_t)eeprom_read_byte((uint8_t*)(EEPROM_ADDR_CAN_ID_HIGH + 1)) << 16;
    can_id |= (uint32_t)eeprom_read_byte((uint8_t*)(EEPROM_ADDR_CAN_ID_HIGH + 2)) << 8;
    can_id |= (uint32_t)eeprom_read_byte((uint8_t*)(EEPROM_ADDR_CAN_ID_HIGH + 3));
    
    // Validate CAN ID
    if (can_id == 0 || can_id == 0xFFFFFFFF) {
        // If invalid, return default and fix the value
        EEPROM_WriteCANID(DEFAULT_CAN_ID);
        return DEFAULT_CAN_ID;
    }
    
    return can_id;
}

// Write CAN baud rate
void EEPROM_WriteCANBaud(uint8_t baud_rate)
{
    // Validate baud rate (0-3 are valid)
    if (baud_rate > 3) {
        baud_rate = DEFAULT_CAN_BAUD;
    }
    
    // Write baud rate
    eeprom_write_byte((uint8_t*)EEPROM_ADDR_CAN_BAUD, baud_rate);
}

// Read CAN baud rate
uint8_t EEPROM_ReadCANBaud(void)
{
    // Read baud rate
    uint8_t baud_rate = eeprom_read_byte((uint8_t*)EEPROM_ADDR_CAN_BAUD);
    
    // Validate baud rate
    if (baud_rate > 3) {
        // If invalid, return default and fix the value
        EEPROM_WriteCANBaud(DEFAULT_CAN_BAUD);
        return DEFAULT_CAN_BAUD;
    }
    
    return baud_rate;
}
