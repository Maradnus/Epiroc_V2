#ifndef EEPROM_H
#define EEPROM_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"

// EEPROM address definitions
#define EEPROM_MODE_PAIR1     0x09
#define EEPROM_MODE_PAIR6     0x0E
#define EEPROM_MAGIC_ADDR     0xFF
#define EEPROM_MAGIC_VALUE    0xA5

// Function prototypes
void EEPROM_init(void);
uint8_t EEPROM_read_byte(uint8_t addr);
void EEPROM_write_byte(uint8_t addr, uint8_t data);
bool EEPROM_is_initialized(void);

#endif // EEPROM_H