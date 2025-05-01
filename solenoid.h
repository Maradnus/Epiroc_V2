#ifndef SOLENOID_H
#define SOLENOID_H

#include "common.h"
#include "config.h"
#include "can_lookup.h"

void Sol_init(void);
Status_t Sol_set_pin_state(Function_t function, bool state);
bool Sol_read_pin_state(Function_t function);
void Sol_set_output(void);

#endif // SOLENOID_H