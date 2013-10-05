#include "gamelib.h"
#include "gamemath.h"
#include <stdlib.h>
#include <stdint.h>
#include "../display/display.h"
#include "time/time.h"
#include "console/console.h"
#include "common/print.h"
#include "input/input.h"

//#define NEIGHBORS_LOOPS

char NumLivingNeighbors(char x, char y);
char Wrap4BitUInt(char fourBitInt);

unsigned long endTime;
unsigned char gens;

char 
GameOfLifeInit(void)
{
	endTime = GLIB_GetGameMillis() + 1000;
	gens = 0;
	
	GFX_Clear(0);
	srand(GLIB_GetGameMillis()); 
	GFX_BitBLT((const char * const)(intptr_t)RandLong(0,2304),16,16,0,0);
	GFX_SwapBuffers();
	return 1;
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
			
			char cellIsLiving = GFX_GetPixel(x,y);
			if (cellIsLiving)
			{
				//we subtract 1 to avoid including self
				if (numNeighbors >= 2 && numNeighbors <= 3)
				{
					GFX_PutPixel(x,y,1); //cell was alive, stays alive
				}	
				else if (numNeighbors < 2 || numNeighbors > 3)
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
	
	//if 3rd button is pressed, reset the game
	unsigned char events = INP_PollEvents();
	if (GetBitUInt8(&events, INPUT_PB2_DOWN))
	{
		GFX_Clear(0);
		srand(GLIB_GetGameMillis()); 
		GFX_BitBLT((const char * const)(intptr_t)RandLong(0,2304),16,16,0,0);	
	}

	GFX_SwapBuffers();
	
	gens++;
	if (GLIB_GetGameMillis() >= endTime)
	{
		endTime = GLIB_GetGameMillis() + 1000;
		CON_SendString(PSTR("Gen/s: "));
		printInt((long)gens, VAR_UNSIGNED);
		CON_SendString(PSTR("\r\n"));
		gens = 0;
	}
	
	return 1;
}

#ifdef NEIGHBORS_LOOPS
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
	
	if (GFX_GetPixel(x,y) == 1)
		sumLivingNeighbors--; //subtract self
	
	return sumLivingNeighbors;
}
#else
char 
NumLivingNeighbors(char x, char y)
{
	char sumLivingNeighbors = 0;
	
	//check upper-left
	if (GFX_GetPixel(Wrap4BitUInt(x-1),Wrap4BitUInt(y-1)) == 1)
		sumLivingNeighbors++;
	
	//top-center
	if (GFX_GetPixel(x,Wrap4BitUInt(y-1)) == 1)
		sumLivingNeighbors++;
	
	//upper-right
	if (GFX_GetPixel(Wrap4BitUInt(x+1),Wrap4BitUInt(y-1)) == 1)
		sumLivingNeighbors++;
	
	//left
	if (GFX_GetPixel(Wrap4BitUInt(x-1),y) == 1)
		sumLivingNeighbors++;
	
	//right
	if (GFX_GetPixel(Wrap4BitUInt(x+1),y) == 1)
		sumLivingNeighbors++;
	
	//bottom-left
	if (GFX_GetPixel(Wrap4BitUInt(x-1),Wrap4BitUInt(y+1)) == 1)
		sumLivingNeighbors++;
	
	//bottom
	if (GFX_GetPixel(x,Wrap4BitUInt(y+1)) == 1)
		sumLivingNeighbors++;
	
	//bottom-right
	if (GFX_GetPixel(Wrap4BitUInt(x+1),Wrap4BitUInt(y+1)) == 1)
		sumLivingNeighbors++;
	
	return sumLivingNeighbors;
}
#endif

char 
Wrap4BitUInt(char fourBitInt)
{
	char outInt = fourBitInt;
	while (outInt > 15)
	{
		outInt -= 16;
	}
	while (outInt < 0)
	{
		outInt += 16;
	}
	
	return outInt;
}