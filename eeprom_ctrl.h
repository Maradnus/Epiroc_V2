#ifndef EEPROM_CTRL_H
#define EEPROM_CTRL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>
#include "mode_ctrl.h"

// EEPROM address definitions
#define EEPROM_ADDR_CAN_BAUD       0x00
#define EEPROM_ADDR_CAN_ID_HIGH    0x01
#define EEPROM_ADDR_CAN_ID_LOW     0x05
#define EEPROM_ADDR_MODE_PAIR1     0x09
#define EEPROM_ADDR_MODE_PAIR2     0x0A
#define EEPROM_ADDR_MODE_PAIR3     0x0B
#define EEPROM_ADDR_MODE_PAIR4     0x0C
#define EEPROM_ADDR_MODE_PAIR5     0x0D
#define EEPROM_ADDR_MODE_PAIR6     0x0E

// Function prototypes
void EEPROM_Init(void);
void EEPROM_WriteMode(Channel_Pair_t pair, Mode_t mode);
Mode_t EEPROM_ReadMode(Channel_Pair_t pair);
void EEPROM_WriteCANID(uint32_t can_id);
uint32_t EEPROM_ReadCANID(void);
void EEPROM_WriteCANBaud(uint8_t baud_rate);
uint8_t EEPROM_ReadCANBaud(void);

#endif /* EEPROM_CTRL_H */