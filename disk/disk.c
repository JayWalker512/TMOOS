#include "disk.h"
#include <avr/eeprom.h>

char 
DSK_Init(void)
{
	DSK_ReadByte = &eeprom_read_byte;
	DSK_ReadWord = &eeprom_read_word;
	DSK_ReadDoubleWord = &eeprom_read_dword;
	DSK_WriteByte = &eeprom_write_byte;
	DSK_WriteWord = &eeprom_write_word;
	DSK_WriteDoubleWord = &eeprom_write_dword;
	return 1;
}