/**
 * @file can_ctrl.c
 * @brief CAN controller implementation
 * @author Claude
 * @date April 25, 2025
 */

#include "can_ctrl.h"
#include "error_handler.h"
#include "LED_ctrl.h"
#include "eeprom_ctrl.h"
#include <avr/interrupt.h>
#include <avr/io.h>

// Default CAN ID to filter
#define DEFAULT_CAN_ID 0x14FFFFB0

// Macro to extract bit from byte
#define BIT_IS_SET(byte, bit) (((byte) >> (bit)) & 0x01)

// CAN message buffer
static volatile CAN_Message_t can_message;
static volatile uint8_t message_received = 0;

// Signal variables
volatile uint8_t signal_C = 0, signal_D = 0, signal_E = 0, signal_F = 0;
volatile uint8_t signal_G = 0, signal_H = 0, signal_M = 0, signal_N = 0;
volatile uint8_t signal_A = 0, signal_P = 0, signal_J = 0, signal_L = 0;

/**
 * @brief Initialize CAN controller
 */
void CAN_Init(void)
{
    uint32_t can_id;
    uint8_t baud_rate;
    
    // Read settings from EEPROM
    can_id = EEPROM_ReadCANID();
    baud_rate = EEPROM_ReadCANBaud();
    
    // If EEPROM values are invalid, use defaults
    if (can_id == 0xFFFFFFFF) {
        can_id = DEFAULT_CAN_ID;
    }
    
    // Reset CAN controller
    CANGCON |= (1 << SWRES);
    
    // Set baud rate (250kbps @ 16MHz - default)
    switch (baud_rate) {
        case 0: // 125 kbps
            CANBT1 = 0x0E;
            CANBT2 = 0x0C;
            CANBT3 = 0x37;
            break;
        case 1: // 250 kbps
            CANBT1 = 0x06;
            CANBT2 = 0x0C;
            CANBT3 = 0x37;
            break;
        case 2: // 500 kbps
            CANBT1 = 0x02;
            CANBT2 = 0x0C;
            CANBT3 = 0x37;
            break;
        case 3: // 1 Mbps
            CANBT1 = 0x00;
            CANBT2 = 0x0C;
            CANBT3 = 0x37;
            break;
        default: // Default to 250 kbps
            CANBT1 = 0x06;
            CANBT2 = 0x0C;
            CANBT3 = 0x37;
            break;
    }
    
    // Enable CAN interrupts
    CANGIE = (1 << ENIT) | (1 << ENRX);
    
    // Configure MOb0 for reception
    CANPAGE = 0x00;   // Select MOb0
    CANCDMOB = 0x00;  // Clear control register
    
    // Set ID filter for specified CAN ID
    CANIDT1 = (uint8_t)(can_id >> 21);
    CANIDT2 = (uint8_t)(can_id >> 13);
    CANIDT3 = (uint8_t)(can_id >> 5);
    CANIDT4 = (uint8_t)(can_id << 3);
    
    // Set mask to match exact ID
    CANIDM1 = 0xFF;
    CANIDM2 = 0xFF;
    CANIDM3 = 0xFF;
    CANIDM4 = 0xF8;
    
    // Enable reception mode for MOb0
    CANCDMOB = (1 << CONMOB1);
    CANIE2 = (1 << IEMOB0);
    
    // Enable CAN controller
    CANGCON = (1 << ENASTB);
}

/**
 * @brief Process received CAN message
 */
void CAN_ProcessMessage(void)
{
    if (message_received) {
        // Process the message here if needed
        LED_Set(LED2_GREEN, BLINK, 100); // Blink CAN LED to indicate reception
        
        // Clear message flag
        message_received = 0;
    }
}

/**
 * @brief Extract signals from CAN message data
 * @param data Pointer to store extracted data
 * @return True if successful extraction, false otherwise
 */
bool CAN_Extract(uint8_t* data)
{
    if (!message_received) {
        return false;
    }
    
    // Decode signals based on lookup table
    signal_M = BIT_IS_SET(can_message.data[6], 6);  // Byte 6, bit 6
    signal_N = BIT_IS_SET(can_message.data[6], 4);  // Byte 6, bit 4
    signal_A = BIT_IS_SET(can_message.data[6], 2);  // Byte 6, bit 2
    signal_P = BIT_IS_SET(can_message.data[6], 0);  // Byte 6, bit 0
    
    signal_J = BIT_IS_SET(can_message.data[2], 6);  // Byte 2, bit 6
    signal_L = BIT_IS_SET(can_message.data[2], 4);  // Byte 2, bit 4
    signal_C = BIT_IS_SET(can_message.data[2], 2);  // Byte 2, bit 2
    signal_D = BIT_IS_SET(can_message.data[2], 0);  // Byte 2, bit 0
    
    signal_E = BIT_IS_SET(can_message.data[3], 0);  // Byte 3, bit 0
    signal_F = BIT_IS_SET(can_message.data[3], 1);  // Byte 3, bit 1
    signal_G = BIT_IS_SET(can_message.data[3], 2);  // Byte 3, bit 2
    signal_H = BIT_IS_SET(can_message.data[3], 3);  // Byte 3, bit 3
    
    // Copy to data array if provided
    if (data != NULL) {
        data[0] = signal_C;
        data[1] = signal_D;
        data[2] = signal_E;
        data[3] = signal_F;
        data[4] = signal_G;
        data[5] = signal_H;
        data[6] = signal_M;
        data[7] = signal_N;
        data[8] = signal_A;
        data[9] = signal_P;
        data[10] = signal_J;
        data[11] = signal_L;
    }
    
    return true;
}

/**
 * @brief Check if new message is received
 * @return True if new message is available, false otherwise
 */
bool CAN_IsMessageReceived(void)
{
    return message_received ? true : false;
}

/**
 * @brief Clear message received flag
 */
void CAN_ClearMessageFlag(void)
{
    message_received = 0;
}

/**
 * @brief CAN interrupt handler
 */
ISR(CANIT_vect)
{
    // Select MOb0
    CANPAGE = 0x00;
    
    if (CANSIT2 & (1 << SIT0)) {  // Check if MOb0 has received a message
        // Read received ID
        can_message.id = ((uint32_t)CANIDT1 << 21) |
                         ((uint32_t)CANIDT2 << 13) |
                         ((uint32_t)CANIDT3 << 5) |
                         ((uint32_t)CANIDT4 >> 3);
        
        // Read message length
        can_message.dlc = (CANCDMOB & 0x0F);
        
        // Read message data
        for (uint8_t i = 0; i < 8; i++) {
            CANPAGE = (i << INDX0);  // Select data byte
            can_message.data[i] = CANMSG;
        }
        
        // Set message received flag
        message_received = 1;
        
        // Toggle CAN LED
        LED_Set(LED2_BLUE, BLINK, 50);
    }
    
    // Clear interrupt flags
    CANSTMOB = 0x00;  // Clear MOb status
    CANGIT |= (1 << CANIT);  // Clear general interrupt
    
    // Re-enable reception
    CANCDMOB = (1 << CONMOB1);
}
