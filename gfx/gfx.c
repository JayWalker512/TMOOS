#include "../os.h"
#include "gfx.h"
#include "../display/display.h"
#include "../common/binary.h"
#include <math.h>
#include <string.h>
#include "../common/glyphs.h"

unsigned char m_GFXState;

void 
GFX_Init(void)
{
	m_GFXState = 0;
	GFX_PutPixel = &DSP_PutPixel;
	GFX_GetPixel = &DSP_GetPixel;
	GFX_BitBLT = &DSP_BitBLT;
	GFX_Clear = &DSP_Clear;
	GFX_SwapBuffers = &DSP_SwapBuffers;
}

void 
GFX_Enable(enum e_GFXState parameter)
{
	SetBit(&m_GFXState, parameter);
}

void 
GFX_Disable(enum e_GFXState parameter)
{
	ClearBit(&m_GFXState, parameter);
}

void 
GFX_DrawRect(char x, char y, 
		  const char width, 
		  const char height, enum e_GFXState parameter)
{
	if (x >= DISPLAY_WIDTH || y >= DISPLAY_HEIGHT)
		return;
	
	/*Don't need to add height on every call, cache it.
	Don't know if the compiler will make that optimization,
	so let's do it explicitly. */
	char xLimit = x + width;
	char yLimit = y + height;
	/*if (xLimit >= DISPLAY_WIDTH)
		xLimit = DISPLAY_WIDTH;
	if (yLimit >= DISPLAY_HEIGHT)
		yLimit = DISPLAY_HEIGHT;*/
	
	if (parameter == GFX_FILLED_RECT ||
	    parameter == GFX_BLANK_DESTRUCTIVE_RECT)
	{	
		if (y < 0)
			y = 0;
		if (x < 0)
			x = 0;
		
		char fillState;
		if (parameter == GFX_FILLED_RECT)
			fillState = 1;
		else
			fillState = 0;
		
		for (char iY = y; iY < yLimit; iY++)
		{
			for (char iX = x; iX < xLimit; iX++)
			{
				DSP_PutPixel(iX, iY, fillState);
			}
		}
	}
	else if (parameter == GFX_BORDERED_RECT)
	{		
		if (y < 0)
			y = -1; /*-1 so we don't draw OOB lines but save time on 
				 * coords preceeding -1 */
		if (x < 0)
			x = -1;
		
		//why doesn't this draw properly with the following optimization?
		//char xOppositeBorder = yLimit - 1;
		//char yOppositeBorder = xLimit - 1;
		for (char iY = y; iY < yLimit; iY++)
		{
			DSP_PutPixel(x, iY, 1);
			DSP_PutPixel(xLimit - 1, iY, 1);
		}
		for (char iX = x; iX < xLimit; iX++)
		{
			DSP_PutPixel(iX, y, 1);
			DSP_PutPixel(iX, yLimit - 1, 1);
		}
	}	
}
		  
void 
GFX_DrawLine(const char x1, const char y1,
		const char x2, const char y2)
{
	/* Bresenhams line algorithm.
	http://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm#Simplification */
	char xS = x1;
	char yS = y1;
	char xD = x2;
	char yD = y2;
	char dX = abs(xD - xS);
	char dY = abs(yD - yS);
	char err = dX - dY;
	
	char stepX;
	char stepY;
	if (xS < xD)
		stepX = 1;
	else
		stepX = -1;
	
	if (yS < yD)
		stepY = 1;
	else
		stepY = -1;
	
	while (1)
	{
		DSP_PutPixel(xS, yS, 1);
		if (xS == xD && yS == yD)
			break;
		
		char e2 = 2 * err;
		if (e2 > -dY)
		{
			err = err - dY;
			xS = xS + stepX;
		}
		if (e2 < dX)
		{
			err = err + dX;
			yS = yS + stepY;
		}
	}
}

void GFX_DrawText(const char *text, const char x, const char y)
{
	char len = strlen(text);
	char spacingMp = 3; //spacing multiplier
	char step = 0;
	for (char xLoc = x; step <= len; xLoc++)
	{
		if (*(text+step) >= 'A' && *(text+step) <= 'Z')
		{
			DSP_BitBLT(&g_alphaNumGlyphs[*(text+step) - 'A'],
				3, 5, 
				xLoc + (step * spacingMp), y);
		}
		else if (*(text+step) >= '0' && *(text+step) <= '9')
		{
			DSP_BitBLT(&g_alphaNumGlyphs[*(text+step) - '0' + 26],
				3, 5, 
				xLoc + (step * spacingMp), y);
		}
		else
			DSP_BitBLT(&g_testGlyph, 2, 3, xLoc + (step * spacingMp), y);
		
		step++;
		OS_Update(); //calling this here because this function is SLOOOWWW
	}
}
