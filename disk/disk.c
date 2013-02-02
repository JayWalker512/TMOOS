#include "disk.h"
#include <avr/eeprom.h>

char 
DSK_Init(void)
{
	DSK_ReadByte = &eeprom_read_byte;
	DSK_ReadWord = &eeprom_read_word;
	DSK_ReadDoubleWord = &eeprom_read_dword;
	return 1;
}