#ifndef GAMES_H
#define GAMES_H

#include "../gfx/gfx.h"
#include "../common/glyphs.h"
#include "../sound/sound.h"
#include "../common/binary.h"
#include "../common/avr.h"
#include <math.h>

typedef struct ProgData_s 
{
	char *name; //TODO this should point to a string in flash memory
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
size_t GLIB_GetProgDataTableSize(enum e_ProgDataTableEnums table);

void (*GLIB_Idle)(void);

unsigned char (*GLIB_GetInput)(enum e_GLIBenums device);
unsigned char (*GLIB_GetWheelRegion)(unsigned char regions);

unsigned long GLIB_GetGameMillis(void);
unsigned char GLIB_GetDeltaMillis(void);

void (*GLIB_Beep)(unsigned int Hz, unsigned long ms);

/* Everything below will need refactored... not sure how im handling multiple 
games yet. Play around with it. */

char InitSmiley(void);
char GameSmiley(void);
char GameInputTest(void);
char GameSlidingWheelChar(void);
char InitGameScrollText(void);
char GameScrollText(void);
char GameWheelRegionTest(void);
char ProgMemTest(void);
char BatteryLevelTest(void);

//pong.c
char InitPongGame(void);
char PongGameLoop(void);

//memviewer.c
char MemViewer(void);

//invaders.c
char InitInvaders(void);
char InvadersLoop(void);

//gameoflife.c
char GameOfLifeInit(void);
char GameOfLifeLoop(void);

#endif
