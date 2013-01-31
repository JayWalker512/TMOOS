#include "../avr_common.h"
#include "libarduino/Arduino.h"
#include "hardware.h"
#include "analog.h"

int 
HRD_Init(void)
{
	ARD_Init();
	HRD_GetMicros = &micros;
	HRD_GetMillis = &millis;
	HRD_GetPinAnalog = &analogRead;
	HRD_GetPinDigital = &digitalRead;
	
	pinMode(A7, INPUT);
	return 1;
}

int 
HRD_SetPin(char charport, char pin, char val)
{
	uint8_t port = 0;

	//check for invalid values, return 0 if they are invalid.
	if (charport < 'A' || charport > 'F')
		return 0;
	
	if (pin < 0 || pin > 7)
		return 0; 
		
	port = charport - 'A';
	
	//set the pin
	if (val == 0)
	{
		// make the pin an output
		*(uint8_t *)(0x21 + port * 3) |= (1 << pin);
		// drive it low
		*(uint8_t *)(0x22 + port * 3) &= ~(1 << pin);
	}
	else
	{
		// make the pin an output
		*(uint8_t *)(0x21 + port * 3) |= (1 << pin);
		// drive it high
		*(uint8_t *)(0x22 + port * 3) |= (1 << pin);
	}
	
	return 1;
}


