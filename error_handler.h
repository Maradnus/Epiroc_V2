#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <avr/io.h>
#include <stdint.h>

// Error codes
typedef enum {
    ERR_NONE = 0,
    ERR_CAN_INIT_FAIL,
    ERR_CAN_COMM_FAIL,
    ERR_OVERCURRENT,
    ERR_TOO_MANY_CHANNELS,
    ERR_EEPROM_FAIL,
    ERR_SYSTEM_FAIL
} Error_Code_t;

// Function prototypes
void Err_Init(void);
Error_Code_t Err_DetectSysError(void);
void Err_LogError(Error_Code_t error, uint8_t module);
void Err_RecoverFromError(Error_Code_t error);
void Err_TriggerErrProtocol(Error_Code_t error);

#endif /* ERROR_HANDLER_H */