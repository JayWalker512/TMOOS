#include "../avr_common.h"
#include "libarduino/Arduino.h"
#include "hardware.h"
#include "../sharedlib/binary.h"

#ifdef DEBUG
#include "../debug/debug.h"
#endif

int main()
{
	//HRD_SetPinDigital(11, 1);
	while (1)
	{
		_delay_ms(50);
		if (!HRD_GetPinDigital(14))
			HRD_SetPinDigital(11, 1);
		else
			HRD_SetPinDigital(11, 0);
	}
	return 0;
}