#ifndef GAMES_H
#define GAMES_H

#include "../gfx/gfx.h"
#include "../common/glyphs.h"
#include "../sound/sound.h"

typedef struct ProgData_s 
{
	char name[8]; //TODO this should point to a string in flash memory
	char (*initFunc)(void);
	char (*loopFunc)(void);
} ProgData_t;

enum e_ProgDataTableEnums 
{
	DATA_GAMES = 0,
};

//these need to stay in sync with what's in input.h
enum e_GLIBenums 
{
	GLIB_PB0 = 0,
	GLIB_PB1 = 1,
	GLIB_PB2 = 2,
	GLIB_WHEEL = 3,
};

char GLIB_Init(void);

ProgData_t *GLIB_GetProgDataTable(enum e_ProgDataTableEnums table);

void (*GLIB_Idle)(void);

//This could be handled function pointer style...
unsigned char (*GLIB_GetInput)(enum e_GLIBenums device);
unsigned char (*GLIB_GetWheelRegion)(unsigned char regions);

unsigned long GLIB_GetGameMillis(void);

void (*GLIB_Beep)(unsigned int Hz, unsigned long ms);

/* Everything below will need refactored... not sure how im handling multiple 
games yet. Play around with it. */

char GameSmiley(void);

char GameInputTest(void);

char GameSlidingWheelChar(void);

char InitGameScrollText(void);
char GameScrollText(void);

char GameWheelRegionTest(void);

#endif
