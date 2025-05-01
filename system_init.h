#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include "common.h"

void Sys_init_power(void);
void Sys_init_CAN(void);
void Sys_init_mode(void);
void Sys_init_solenoid(void);
void Sys_init_monitor(void);

#endif // SYSTEM_INIT_H