#include "../debug/debug.h"
#include "../avr_common.h"
#include "disk.h"

int main()
{
	usb_init();
	_delay_ms(1000);
	print("Disk testcase started!\n");
	DSK_Init();
	DSK_WriteByte(0x04, 42);
	char test = DSK_ReadByte(0x04);
	phex16(test);
	print("\n");
	_delay_ms(1000);
	while(1);
}
