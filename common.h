#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    SUCCESS = 0,
    ERROR,
    INVALID_PARAM,
    NOT_READY,
    BUSY
} Status_t;

#endif // COMMON_H