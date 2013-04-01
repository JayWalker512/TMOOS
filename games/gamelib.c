#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../input/input.h"
#include "../sound/sound.h"
#include "../disk/disk.h"
#include "../os.h"
#include "gamelib.h"
#include <math.h>

char 
GLIB_Init(void)
{
	GFX_Init();
	GLIB_Idle = &OS_Update;
	GLIB_Beep = &SND_Beep;
	GLIB_GetInput = &INP_GetInputState;
	GLIB_GetWheelRegion = &INP_GetWheelRegion;
	return 1;
}

//this could be handled function pointer style...
/*unsigned char 
GLIB_GetInput(enum e_GLIBenums device)
{
	//would be nice if we could just map these enums directly to the input equivalent without branches...
	switch (device)
	{
		case GLIB_WHEEL:
			return INP_GetInputState(INPUT_WHEEL);
			break;
		case GLIB_PB0:
			return INP_GetInputState(INPUT_PB0);
			break;
		case GLIB_PB1:
			return INP_GetInputState(INPUT_PB1);
			break;
		case GLIB_PB2:
			return INP_GetInputState(INPUT_PB2);
			break;
		default:
			break;
	}
	return 0;
}*/

unsigned long 
GLIB_GetGameMillis(void)
{
	return g_OSIdleLoopTimeMs;
}