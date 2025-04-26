#ifndef CAN_CTRL_H
#define CAN_CTRL_H

#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

// CAN message structure
typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t dlc;
} CAN_Message_t;

// Function prototypes
void CAN_Init(void);
void CAN_ProcessMessage(void);
bool CAN_Extract(uint8_t* data);
bool CAN_IsMessageReceived(void);
void CAN_ClearMessageFlag(void);

// External variables for decoded signals
extern volatile uint8_t signal_C, signal_D, signal_E, signal_F;
extern volatile uint8_t signal_G, signal_H, signal_M, signal_N;
extern volatile uint8_t signal_A, signal_P, signal_J, signal_L;

#endif /* CAN_CTRL_H */