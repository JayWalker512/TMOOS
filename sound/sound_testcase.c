#include "sound.h"
#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../avr_common.h"
#include "../debug/debug.h"
#include "../sharedlib/binary.h"

int main()
{
	TME_Init();
	SND_Init();
	usb_init();
	_delay_ms(1000);

	unsigned long endTime;
	SND_Beep(220, 1000);
	endTime = TME_GetAccurateMillis + 1000;
	while(TME_GetAccurateMillis() < endTime)
	{
		SND_Update();
	}
	
	SND_Beep(880, 2000);
	endTime = TME_GetAccurateMillis + 2000;
	while(TME_GetAccurateMillis() < endTime)
	{
		SND_Update();
	}
	
	SND_Beep(110, 3000);
	endTime = TME_GetAccurateMillis + 3000;
	while(TME_GetAccurateMillis() < endTime)
	{
		SND_Update();
	}
	
	
	
	return 0;
}

