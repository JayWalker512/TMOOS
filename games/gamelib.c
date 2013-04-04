#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../input/input.h"
#include "../sound/sound.h"
#include "../disk/disk.h"
#include "../os.h"
#include "gamelib.h"
#include <math.h>

ProgData_t GameDataTable[NUM_GAME_ITEMS];

char 
GLIB_Init(void)
{
	GFX_Init();
	GLIB_Idle = &OS_Update;
	GLIB_Beep = &SND_Beep;
	GLIB_GetInput = &INP_GetInputState;
	GLIB_GetWheelRegion = &INP_GetWheelRegion;
	
	//next set up game data table
	strcpy(&GameDataTable[0].name, "IN");
	GameDataTable[0].initFunc = &GameInputTest;
	GameDataTable[0].loopFunc = &GameInputTest;
	
	strcpy(&GameDataTable[1].name, "SM");
	GameDataTable[1].initFunc = &GameSmiley;
	GameDataTable[1].loopFunc = &GameSmiley;
	
	strcpy(&GameDataTable[2].name, "ST");
	GameDataTable[2].initFunc = &GameScrollText;
	GameDataTable[2].loopFunc = &GameScrollText;
	
	return 1;
}

ProgData_t *GLIB_GetProgDataTable(enum e_ProgDataTableEnums table)
{
	if (table == DATA_GAMES)
		return &GameDataTable;
}

unsigned long 
GLIB_GetGameMillis(void)
{
	return g_OSIdleLoopTimeMs;
}