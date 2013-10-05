#include "gamelib.h"

#define TEENSY_MEMORY 2560

char MemViewer(void)
{
	unsigned char region = GLIB_GetWheelRegion(TEENSY_MEMORY / 32);
	
	GFX_Clear(0);
	GFX_BitBLT((const char * const)(region * 32), 16,16,0,0);
	GFX_SwapBuffers();
	
	return 1;
}