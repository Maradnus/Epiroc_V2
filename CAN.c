#include "can.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "can_lookup.h"
#include "debug.h"
#include "LED.h"

static CAN_Message_t can_buffer[CAN_BUFFER_SIZE];
static uint8_t buffer_head = 0;
static uint8_t buffer_tail = 0;
static uint8_t buffer_count = 0;

void CAN_init(void) {
    // Reset CAN controller
    CANGCON |= (1 << SWRES);
    
    // Set baud rate (250kbps @ 16MHz)
    CANBT1 = 0x06;
    CANBT2 = 0x0C;
    CANBT3 = 0x37;
    
    // Enable CAN interrupts
    CANGIE = (1 << ENIT) | (1 << ENRX);
    
    // Configure MOb0 for reception
    CANPAGE = 0x00;
    CANCDMOB = 0x00;
    CANIDT1 = 0x00;
    CANIDT2 = 0x00;
    CANIDT3 = 0x00;
    CANIDT4 = 0x00;
    CANIDM1 = 0x00;
    CANIDM2 = 0x00;
    CANIDM3 = 0x00;
    CANIDM4 = 0x00;
    
    // Enable reception mode for MOb0
    CANCDMOB = (1 << CONMOB1);
    CANIE2 = (1 << IEMOB0);
    
    // Enable CAN controller
    CANGCON = (1 << ENASTB);
}

Status_t CAN_process_message(void) {
    if (buffer_count >= CAN_BUFFER_SIZE) {
        DEBUG_PRINTLN("CAN buffer full");
        return BUSY;
    }
    
    // Check if message is available
    if (CANSIT2 & (1 << SIT0)) {
        DEBUG_PRINTLN("CAN message received");
        CANPAGE = 0x00;
        
        // Read received ID
        uint32_t received_id = ((uint32_t)CANIDT1 << 21) |
                             ((uint32_t)CANIDT2 << 13) |
                             ((uint32_t)CANIDT3 << 5) |
                             ((uint32_t)CANIDT4 >> 3);
        
        // Filter for our target ID
        if (received_id == CAN_MSG_ID) {
            // Read message data
            DEBUG_PRINT("Valid CAN ID: ");
            DEBUG_PRINT_HEX(received_id);
            DEBUG_PRINTLN("");
            for (uint8_t i = 0; i < 8; i++) {
                CANPAGE = (i << INDX0);
                can_buffer[buffer_head].data[i] = CANMSG;
                DEBUG_PRINT("Data[");
                DEBUG_PRINT_NUM(i);
                DEBUG_PRINT("]: ");
                DEBUG_PRINT_HEX(can_buffer[buffer_head].data[i]);
                DEBUG_PRINTLN("");
            }
            
            can_buffer[buffer_head].id = received_id;
            can_buffer[buffer_head].length = 8;
            
            buffer_head = (buffer_head + 1) % CAN_BUFFER_SIZE;
            buffer_count++;
            
            // Toggle CAN LED
            DEBUG_PRINTLN("CAN message stored in buffer");
            LED_set(LED_CAN, LED_BLINK, 100);
            DEBUG_PRINTLN("CAN LED triggered");
            
            return SUCCESS;
        }
        
        // Clear interrupt flags
        CANSTMOB = 0x00;
        CANGIT |= (1 << CANIT);
        
        // Re-enable reception
        CANCDMOB = (1 << CONMOB1);
    }
    
    return NOT_READY;
}

Status_t CAN_extract(CAN_Message_t *msg) {
    if (buffer_count == 0) {
        return NOT_READY;
    }
    
    *msg = can_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % CAN_BUFFER_SIZE;
    buffer_count--;
    
    return SUCCESS;
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
        LED_Set(LED_CAN, LED_BLINK, 50);
        DEBUG_PRINTLN("ISR_CAN done");
    }
    
    // Clear interrupt flags
    CANSTMOB = 0x00;  // Clear MOb status
    CANGIT |= (1 << CANIT);  // Clear general interrupt
    
    // Re-enable reception
    CANCDMOB = (1 << CONMOB1);
}