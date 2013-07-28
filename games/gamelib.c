#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../input/input.h"
#include "../sound/sound.h"
#include "../disk/disk.h"
#include "../os.h"
#include "gamelib.h"
#include <math.h>
#include <string.h>

#define NUM_GAME_ITEMS 6
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
	strcpy(m_gameDataTable[0].name, "INP");
	m_gameDataTable[0].initFunc = &GameInputTest;
	m_gameDataTable[0].loopFunc = &GameInputTest;
	
	strcpy(m_gameDataTable[1].name, "SMLY");
	m_gameDataTable[1].initFunc = &GameSmiley;
	m_gameDataTable[1].loopFunc = &GameSmiley;
	
	strcpy(m_gameDataTable[2].name, "STXT");
	m_gameDataTable[2].initFunc = &InitGameScrollText;
	m_gameDataTable[2].loopFunc = &GameScrollText;
	
	strcpy(m_gameDataTable[3].name, "BATT");
	m_gameDataTable[3].initFunc = &BatteryLevelTest;
	m_gameDataTable[3].loopFunc = &BatteryLevelTest;
	
	strcpy(m_gameDataTable[4].name, "WEEL");
	m_gameDataTable[4].initFunc = &GameWheelRegionTest;
	m_gameDataTable[4].loopFunc = &GameWheelRegionTest;
	
	strcpy(m_gameDataTable[5].name, "PONG");
	m_gameDataTable[5].initFunc = &InitPongGame;
	m_gameDataTable[5].loopFunc = &PongGameLoop;
	
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

//unsigned char //if we drop below ~4 fps, this will roll over. 
//GLIB_GetDeltaMillis(void)