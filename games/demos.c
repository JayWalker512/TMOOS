#include "gamelib.h"
#include "../os.h"
#include "../battery.h"

static unsigned long endTime = 0; //used multiple times

char
GameSmiley(void)
{
	GFX_Clear(0);
	
	//eyes
	GFX_DrawRect(2,2,4,4,GFX_BORDERED_RECT);
	GFX_DrawRect(10,2,4,4,GFX_BORDERED_RECT);
	
	//nose
	GFX_DrawRect(7,7,2,2,GFX_BORDERED_RECT);
	
	//mouth
	GFX_DrawLine(2,12,13,12);
	GFX_DrawLine(1,10,1,11);
	GFX_DrawLine(14,10,14,11);
	
	GFX_SwapBuffers();
	return 1;
}

static char lastChar = 0;
static unsigned int fps = 0;
char
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
	return 1;
}

char
GameSlidingWheelChar(void)
{
	unsigned char x = 0;
	unsigned char wheelPos = GLIB_GetInput(GLIB_WHEEL);

	x = (wheelPos / 42);

	GFX_Clear(0);
	GFX_BitBLT(&g_alphaNumGlyphs[0], 3, 5, x, 0);

	GFX_SwapBuffers(); 
	return 1;
}

char m_textX;
unsigned long m_scrollTime;
unsigned char m_scrollRate;
char 
InitGameScrollText(void)
{
	m_textX = 16;
	m_scrollTime = 0;
	m_scrollRate = GLIB_GetInput(GLIB_WHEEL);
}

char 
GameScrollText(void)
{
	GFX_Clear(0);
	
	GFX_DrawText("HELLO", m_textX, 0);
	GFX_DrawLine(7, 7, 0, 15);
	GFX_DrawLine(8, 7, 15, 15);
		
	GFX_SwapBuffers();
	
	if (GLIB_GetGameMillis() > m_scrollTime)
	{
		m_textX--;
		if (m_textX == -20)
			m_textX = 16;
		
		m_scrollRate = GLIB_GetInput(GLIB_WHEEL);
		m_scrollTime = GLIB_GetGameMillis() + m_scrollRate;
	}
	return 1;
}

char
GameWheelRegionTest(void)
{
	GFX_Clear(0);
	char region = GLIB_GetWheelRegion(9);
	GFX_BitBLT(&g_alphaNumGlyphs[26 + region], 3, 5, 0, 0);
	GFX_SwapBuffers();
	return 1;
}

char
BatteryLevelTest(void)
{
	GFX_Clear(0);
	char xDest = 16 * ((float)OS_GetBatteryLevel() / 1023);
	GFX_DrawLine(0,0, xDest, 0);
	GFX_SwapBuffers();
	return 1;
}
