#include "gamelib.h"
#include "../display/display.h" //remove this.
#include "../os.h"

#ifdef DEBUG
#include "../debug/debug.h"
#endif

static unsigned long endTime = 0; //used multiple times

void 
GameMain(void)
{
	GFX_Clear(0);
	
	GFX_DrawRect(0,0,8,8, GFX_BORDERED_RECT);
	
	//draw face
	GFX_PutPixel(2,2,1);
	GFX_PutPixel(5,2,1);
	
	GFX_PutPixel(2,5,1);
	GFX_PutPixel(3,5,1);
	GFX_PutPixel(4,5,1);
	GFX_PutPixel(5,5,1);
	
	GFX_PutPixel(1,4,1);
	GFX_PutPixel(6,4,1);
	
	GFX_SwapBuffers();
}

static char lastChar = 0;
static unsigned int fps = 0;
void 
GameInputTest(void)
{	
	unsigned char wheelPos = GLIB_GetInput(GLIB_WHEEL);
	unsigned char pb0State = GLIB_GetInput(GLIB_PB0);
	unsigned char pb1State = GLIB_GetInput(GLIB_PB1);
	unsigned char pb2State = GLIB_GetInput(GLIB_PB2);

	GFX_Clear(0);
	
	char characterSelect = wheelPos / 10;
	if (characterSelect > 35)
		characterSelect = 35;
		
	if (lastChar != characterSelect)
	{
		SND_Beep(1710, 25);
		lastChar = characterSelect;
	}
	
	if (pb0State)
	{
		GFX_PutPixel(3,0,1);
	}
	
	if (pb1State)
	{
		GFX_PutPixel(4,0,1);
	}
	
	if (pb2State)
	{
		GFX_PutPixel(5,0,1);
	}
	
	GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 0, 0);

	GFX_SwapBuffers();
}

void
GameSlidingWheelChar(void)
{
	unsigned char x = 0;
	unsigned char wheelPos = GLIB_GetInput(GLIB_WHEEL);

	x = (wheelPos / 42);

	GFX_Clear(0);
	GFX_BitBLT(&g_alphaNumGlyphs[0], 3, 5, x, 0);

	GFX_SwapBuffers(); 
}

char textX = 0;
unsigned long scrollTime = 0;
#define SCROLL_RATE 200;
void GameScrollText(void)
{
	GFX_Clear(0);
	
	GFX_DrawText("HELLO", textX, 0);
	GFX_DrawLine(0, 7, 3, 5);
	GFX_DrawLine(7, 7, 4, 5);
		
	GFX_SwapBuffers();
	
	if (GLIB_GetGameMillis() > scrollTime)
	{
		textX--;
		if (textX == -20)
			textX = 0;
		
		scrollTime = GLIB_GetGameMillis() + SCROLL_RATE;
	}
}