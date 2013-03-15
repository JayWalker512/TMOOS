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
Game2Main(void)
{
#ifdef DEBUG
	/*fps++;
	if (g_OSIdleLoopTimeMs >= endTime)
	{
		endTime = g_OSIdleLoopTimeMs + 1000;
		print("FPS: ");
		phex16(fps);
		print("\n");
		fps = 0;
	}*/
#endif
		
	unsigned char wheelPos = GLIB_GetInput(GLIB_WHEEL);
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
	
	if (pb1State)
	{
		GFX_DrawRect(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, GFX_FILLED_RECT);
		GFX_DrawRect(1, 1, 4, 3, GFX_BLANK_DESTRUCTIVE_RECT);
		GFX_DrawRect(2, 2, 2, 1, GFX_FILLED_RECT);
	}
	else
	{
		GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 0, 0);
		//GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 1, 0);
		//GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 2, 0);
		//GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 3, 0);
		//GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 4, 0);
		//GFX_BitBLT(&g_alphaNumGlyphs[characterSelect], 3, 5, 5, 0);
	}
	
	GFX_SwapBuffers();
}

static unsigned char curGlyph;
void
Game3Init(void)
{
	curGlyph = 36; //so it cycles back to A immediately
	endTime = 0;
}

void
Game3Main(void)
{
	/* XXX below is demo code, delete eventually */
	if (g_OSIdleLoopTimeMs >= endTime)
	{
		DSP_Clear(0);
		endTime = g_OSIdleLoopTimeMs + 1000;

		curGlyph++;
		if (curGlyph >= 36)
			curGlyph = 0;
		DSP_BitBLT(&g_alphaNumGlyphs[curGlyph], 3, 5, 2, 0); 
		DSP_SwapBuffers();		
	}
}


void
Game4Main(void)
{
	unsigned char x = 0;
	unsigned char wheelPos = GLIB_GetInput(GLIB_WHEEL);

	x = (wheelPos / 42);

	GFX_Clear(0);
	GFX_BitBLT(&g_alphaNumGlyphs[0], 3, 5, x, 0);

	GFX_SwapBuffers(); 
}