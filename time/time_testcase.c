#include "time.h"
#include "../hardware/hardware.h"
#include "../common/avr.h"
#include "../debug/debug.h"

int main()
{
	usb_init();
	TME_Init();	
	_delay_us(1000000);
	print("Starting test at 16mhz\n");
	HRD_SetPin('D', 6, 1);	
	_delay_us(50000);
	
	unsigned long endTime = 0;
	
	//starting at 16mhz
	endTime = TME_GetAccurateMicros() + 1000000;
	while (TME_GetAccurateMicros() < endTime)
	HRD_SetPin('D', 6, 0);
	
	print("Setting 4mhz\n");
	TME_ScaleCpu(CPU_4MHZ);	
	endTime = TME_GetAccurateMicros() + 1000000;
	while (TME_GetAccurateMicros() < endTime);
	HRD_SetPin('D', 6, 1);
	
	print("Setting 8mhz\n");
	TME_ScaleCpu(CPU_8MHZ);	
	endTime = TME_GetAccurateMicros() + 1000000;
	while (TME_GetAccurateMicros() < endTime);
	HRD_SetPin('D', 6, 0);
	
	print("Setting 1mhz\n");
	TME_ScaleCpu(CPU_1MHZ);	
	endTime = TME_GetAccurateMicros() + 1000000;
	while (TME_GetAccurateMicros() < endTime);
	HRD_SetPin('D', 6, 1);
	
	print("Setting 2mhz\n");
	TME_ScaleCpu(CPU_2MHZ);	
	endTime = TME_GetAccurateMicros() + 1000000;
	while (TME_GetAccurateMicros() < endTime);
	HRD_SetPin('D', 6, 0);
	
	print("Setting 16mhz\n");
	TME_ScaleCpu(CPU_16MHZ);	
	endTime = TME_GetAccurateMicros() + 1000000;
	while (TME_GetAccurateMicros() < endTime);
	HRD_SetPin('D', 6, 1);
	
	print("Now testing millis at 16mhz...\n");
	
	endTime = TME_GetAccurateMillis() + 1000;
	while (TME_GetAccurateMillis() < endTime)
	HRD_SetPin('D', 6, 0);
	
	print("Setting 4mhz\n");
	TME_ScaleCpu(CPU_4MHZ);	
	endTime = TME_GetAccurateMillis() + 1000;
	while (TME_GetAccurateMillis() < endTime)
	HRD_SetPin('D', 6, 1);
	
	print("Setting 8mhz\n");
	TME_ScaleCpu(CPU_8MHZ);	
	endTime = TME_GetAccurateMillis() + 1000;
	while (TME_GetAccurateMillis() < endTime)
	HRD_SetPin('D', 6, 1);
	
	print("Setting 1mhz\n");
	TME_ScaleCpu(CPU_1MHZ);	
	endTime = TME_GetAccurateMillis() + 1000;
	while (TME_GetAccurateMillis() < endTime)
	HRD_SetPin('D', 6, 0);
	
	print("Setting 16mhz\n");
	TME_ScaleCpu(CPU_16MHZ);	
	endTime = TME_GetAccurateMillis() + 1000;
	while (TME_GetAccurateMillis() < endTime)
	HRD_SetPin('D', 6, 1);
	
	return 0;
}
