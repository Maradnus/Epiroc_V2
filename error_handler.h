#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include "common.h"
#include <stdbool.h>

typedef enum {
    ERROR_NONE = 0,
    ERROR_CAN_COMM,
    ERROR_OVER_CURRENT,
    ERROR_CHANNEL_CONFLICT,
    ERROR_EEPROM,
    ERROR_COUNT
} Error_t;

void Err_init(void);
void Err_detect_sys_error(void);
void Err_log_error(Error_t error, const char* module);
void Err_recover_from_error(Error_t error);
void Err_trigger_err_protocol(Error_t error);
float Err_read_current(void);  
float Err_read_current_filtered(void);
void Err_set_output_active(bool active);  // New function to indicate if outputs are active

#endif // ERROR_HANDLER_H