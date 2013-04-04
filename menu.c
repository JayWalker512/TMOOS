#include "menu.h"
#include "gfx/gfx.h"
#include <string.h>

#define ESCAPE_SEQUENCE_DELAY 1500
#define ESCAPE_KEY1 GLIB_PB1
#define ESCAPE_KEY2 GLIB_PB2

#define MENU_SCROLL_START_DELAY 750

static char m_escapeKeyState;
static unsigned long m_escapeKeyTime;
static char m_bRunning;
static char m_progIndex;

static char MENU_Loop(ProgData_t *dataTable, size_t numElements);
static char MENU_CheckEscapeSequence(void);

char
MENU_Init(void)
{
        m_escapeKeyState = 0;
        m_escapeKeyTime = 0;
	
	m_bRunning = 0;
	m_progIndex = -1;
	
	return 1;
}

void 
MENU_LauncherLoop(void)
{
	ProgData_t *gameDataTable = GLIB_GetProgDataTable(DATA_GAMES);
	size_t numElements = GLIB_GetProgDataTableSize(DATA_GAMES);
	
	if (!m_bRunning)
		m_progIndex = MENU_Loop(gameDataTable, numElements);
	else //running game
	{
		if (0 == gameDataTable[m_progIndex].loopFunc())
		{
			//game returns 0, we quit
			m_bRunning = 0;
			m_progIndex = -1;
		}
	}

	if (m_progIndex != -1 && !m_bRunning)
	{
		//prog index just changed from MENU_Loop(), need to run
		m_bRunning = 1;
		gameDataTable[m_progIndex].initFunc();
	}
	
	if (MENU_CheckEscapeSequence())
	{
		m_bRunning = 0; 
		m_progIndex = -1;
	}
}

char
MENU_Loop(ProgData_t *dataTable, size_t numElements)
{
	//menu input handling
	unsigned char menuIndex = GLIB_GetWheelRegion(numElements);
	
	if (GLIB_GetInput(GLIB_PB0))
		return menuIndex;
	
	//menu rendering
	GFX_Clear(0);
	GFX_DrawText(dataTable[menuIndex].name, 0, 0);
	GFX_SwapBuffers();
	
	return -1; //return -1 unless a program was run. Return its index otherwise.
}

char
MENU_CheckEscapeSequence(void)
{
        if (GLIB_GetInput(ESCAPE_KEY1) && GLIB_GetInput(ESCAPE_KEY2))
        {
		if (m_escapeKeyState == 1 )
		{
			if (GLIB_GetGameMillis() >= m_escapeKeyTime + ESCAPE_SEQUENCE_DELAY)
			{
				m_escapeKeyState = 0;
				m_escapeKeyTime = 0;
				return 1;
			}
			return 0;
		}
		m_escapeKeyState = 1;
		m_escapeKeyTime = GLIB_GetGameMillis();
		return 0;
        }
	else
	{
		m_escapeKeyTime = GLIB_GetGameMillis();
		m_escapeKeyState = 0;
		return 0;
	}
	return 0;
}