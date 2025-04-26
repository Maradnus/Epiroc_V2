#ifndef SYSTEM_INIT_H
#define SYSTEM_INIT_H

#include <avr/io.h>
#include <stdint.h>

// Function prototypes
void Sys_Init(void);
void Sys_InitPower(void);
void Sys_InitCAN(void);
void Sys_InitMode(void);
void Sys_InitSolenoid(void);
void Sys_InitMonitor(void);

#endif /* SYSTEM_INIT_H */