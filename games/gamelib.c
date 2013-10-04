#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../input/input.h"
#include "../sound/sound.h"
#include "../disk/disk.h"
#include "../os.h"
#include "../common/avr.h"
#include "gamelib.h"
#include <math.h>
#include <string.h>

#define NUM_GAME_ITEMS 9
ProgData_t m_gameDataTable[NUM_GAME_ITEMS];

char 
GLIB_Init(void)
{
	GFX_Init();
	GLIB_Idle = &OS_Update;
	GLIB_Beep = &SND_Beep;
	GLIB_GetInput = &INP_GetInputState;
	GLIB_GetWheelRegion = &INP_GetWheelRegion;
	
	//next set up game data table
	m_gameDataTable[0].name = PSTR("INP");
	m_gameDataTable[0].initFunc = &GameInputTest;
	m_gameDataTable[0].loopFunc = &GameInputTest;
	
	m_gameDataTable[1].name = PSTR("SMLY");
	m_gameDataTable[1].initFunc = &InitSmiley;
	m_gameDataTable[1].loopFunc = &GameSmiley;
	
	m_gameDataTable[2].name = PSTR("STXT");
	m_gameDataTable[2].initFunc = &InitGameScrollText;
	m_gameDataTable[2].loopFunc = &GameScrollText;

	m_gameDataTable[3].name = PSTR("BATT");
	m_gameDataTable[3].initFunc = &BatteryLevelTest;
	m_gameDataTable[3].loopFunc = &BatteryLevelTest;
	
	m_gameDataTable[4].name = PSTR("PMEM");
	m_gameDataTable[4].initFunc = &ProgMemTest;
	m_gameDataTable[4].loopFunc = &ProgMemTest;
	
	m_gameDataTable[5].name = PSTR("PONG");
	m_gameDataTable[5].initFunc = &InitPongGame;
	m_gameDataTable[5].loopFunc = &PongGameLoop;
	
	m_gameDataTable[6].name = PSTR("MEMV");
	m_gameDataTable[6].initFunc = &MemViewer;
	m_gameDataTable[6].loopFunc = &MemViewer;
	

	m_gameDataTable[7].name = PSTR("INVA");
	m_gameDataTable[7].initFunc = &InitInvaders;
	m_gameDataTable[7].loopFunc = &InvadersLoop;

	m_gameDataTable[8].name = PSTR("GOL");
	m_gameDataTable[8].initFunc = &GameOfLifeInit;
	m_gameDataTable[8].loopFunc = &GameOfLifeLoop;

	
	return 1;
}

ProgData_t *GLIB_GetProgDataTable(enum e_ProgDataTableEnums table)
{
	if (table == DATA_GAMES)
		return m_gameDataTable;
	
	return 0;
}

size_t GLIB_GetProgDataTableSize(enum e_ProgDataTableEnums table)
{
	if (table == DATA_GAMES)
		return (sizeof(m_gameDataTable) / sizeof(m_gameDataTable[0]));
	
	return 0;
}

unsigned long 
GLIB_GetGameMillis(void)
{
	return g_OSIdleLoopTimeMs;
}

/* Returns the number of millis that have passed since this function was 
 last called. Useful for frame delta time. */
unsigned char //if we drop below ~4 fps, this will roll over. 
GLIB_GetDeltaMillis(void)
{
	//going to wait to implement this... see if its really needed
	//or even a good idea.
}