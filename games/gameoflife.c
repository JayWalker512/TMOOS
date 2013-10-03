#include "gamelib.h"
#include "gamemath.h"
#include <stdlib.h>
#include "../display/display.h"
#include "time/time.h"
#include "console/console.h"
#include "common/print.h"

char NumLivingNeighbors(char x, char y);
char Wrap4BitUInt(char fourBitInt);

char 
GameOfLifeInit(void)
{
	GFX_Clear(0);
	srand(GLIB_GetGameMillis());
	GFX_BitBLT(RandLong(0,2304),16,16,0,0);
	GFX_SwapBuffers();
}

char 
GameOfLifeLoop(void)
{
	unsigned char x = 0;
	unsigned char y = 0;
	GFX_Clear(0);
	for (y = 0; y <= 15; y++)
	{
		for (x = 0; x <= 15; x++) //rules applied in this inner loop
		{
			char numNeighbors = NumLivingNeighbors(x,y);
			
			/*if (y < 1 && x < 3)
			{
				CON_SendString(PSTR("LN: "));
				printInt(numNeighbors, VAR_SIGNED);
				CON_SendString(PSTR("\r\n"));
			}*/
			
			char cellIsLiving = GFX_GetPixel(x,y);
			if (cellIsLiving)
			{
				//we subtract 1 to avoid including self
				if (numNeighbors - 1 >= 2 && numNeighbors - 1 <= 3)
				{
					GFX_PutPixel(x,y,1); //cell was alive, stays alive
				}	
				else if (numNeighbors - 1 < 2 || numNeighbors - 1 > 3)
				{
					GFX_PutPixel(x,y,0); //cell was alive, dies
				}
			}
			
			if (!cellIsLiving && numNeighbors == 3) //cell was dead and has 3 live neighbors
			{
				GFX_PutPixel(x,y,1); //living cell is born
			}
		}
	}
	
	//TME_DelayRealMillis(16);
	GFX_SwapBuffers();
}

char 
NumLivingNeighbors(char x, char y)
{
	char sumLivingNeighbors = 0;
	
	char i = 0;
	char j = 0;
	for (j = y - 1; j <= y + 1; j++)
	{
		for (i = x - 1; i <= x + 1; i++)
		{
			if (GFX_GetPixel(Wrap4BitUInt(i), Wrap4BitUInt(j)))
			{
				sumLivingNeighbors++;
			}
		}
	}
	
	return sumLivingNeighbors;
}

char 
Wrap4BitUInt(char fourBitInt)
{
	char outInt = fourBitInt;
	while (outInt > 15)
	{
		outInt -= 15;
	}
	while (outInt < 0)
	{
		outInt += 16;
	}
	
	return outInt;
}