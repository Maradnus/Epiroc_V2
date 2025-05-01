#ifndef CAN_H
#define CAN_H

#include "common.h"
#include "config.h"

#define CAN_BUFFER_SIZE 2

typedef struct {
    uint32_t id;
    uint8_t data[8];
    uint8_t length;
} CAN_Message_t;

void CAN_init(void);
Status_t CAN_process_message(void);
Status_t CAN_extract(CAN_Message_t *msg);

#endif // CAN_H