#ifndef HARDWARE_H
#define HARDWARE_H

#include <avr/interrupt.h>

int HRD_Init(void);

int HRD_SetPin(char charport, char pin, char val);

int (*HRD_GetPinAnalog)(uint8_t pin);

char (*HRD_GetPinDigital)(uint8_t pin);

unsigned long (*HRD_GetMicros)(void);

unsigned long (*HRD_GetMillis)(void);

#endif
