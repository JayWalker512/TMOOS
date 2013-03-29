/* (C) 2013 Brandon Foltz

This is the main operating system file. This program has access to all 
peripheral subsystems through their respective interfaces. */

#include "os.h"
#include "hardware/hardware.h"
#include "console/console.h"
#include "disk/disk.h"
#include "display/display.h"
#include "input/input.h"
#include "sound/sound.h"
#include "time/time.h"
#include "games/gamelib.h"

#include "common/binary.h"
#include "common/glyphs.h"
#include "common/print.h"
#include "avr_common.h"

#ifdef DEBUG
#include "debug/debug.h"
#endif

#ifdef PROFILING
#include "common/profiling.h"
#endif

//may need this here... gonna leave it here for now.
#define CPU_SET_FREQ(n) (CLKPR = 0x80, CLKPR = (n))

//These shouldn't go above ~2 seconds to avoid integer overflows in load calc
#define LOAD_REFRESH_MS 1000
#define LOAD_REFRESH_US 1000000

enum e_OSConfigFileSystem {
    FILE_OS_STATE = 0x00, //1 byte
    FILE_OS_DEBUGLEVELS = 0x01, //1 byte
    
    FILE_INP_CALIBMIN = 0x02, //2 bytes
    FILE_INP_CALIBMAX = 0x04, //2 bytes
    FILE_INP_POLLRATE = 0x06, //1 byte
    
    FILE_DSP_STATE = 0x07, //1 byte
    FILE_DSP_REFRESHRATE = 0x08, //1 byte
};

void OS_Update(void);
static char OS_Init(void);
static char OS_InitSubsystems(void);
static void OS_FatalError(enum e_FATAL_ERRORS error);
static void OS_CPUScaleByLoad(void);
static void OS_CPULoadCalc(enum e_TimerParams parameter);
static char OS_SaveSysConfig(void);
static char OS_RestoreSysConfig(void);

//cached version of TME_GetAccurateMillis() from beginning of OS_Update() for non-time-critical use only. Good for games.
unsigned long g_OSIdleLoopTimeMs;
unsigned char g_OSState;
unsigned char g_OSDebugLevels; /*each bit corresponds to a subsystem. If bit enabled,
that subsystem will print it's debug info to terminal. Bits are above. */

static unsigned long m_endCycle;
static unsigned char m_sysLoad;
static unsigned long m_idleTimeStart;
static unsigned long m_idleTimeEnd;
static unsigned long m_idleTime;

#ifdef PROFILING
	t_Timer profilerTimer;
	t_Timer conTimer;
	t_Timer inpTimer;
	t_Timer dspTimer;
	t_Timer sndTimer;
	t_Timer gameTimer;
	unsigned long conSum = 0;
	unsigned long inpSum = 0;
	unsigned long dspSum = 0;
	unsigned long sndSum = 0;
	unsigned long gameSum = 0;
	unsigned int nLoops;
#endif

int 
main(void)
{
	CPU_SET_FREQ(CPU_16MHZ); //start @ 16mhz no matter what.

	if(!OS_InitSubsystems())
		OS_FatalError(INIT_ERROR);
		
	if (!OS_Init())
		OS_FatalError(INIT_ERROR);
	
	//if (!OS_RestoreSysConfig())
		//OS_FatalError(INIT_ERROR);

#ifdef PROFILING
	PRO_StartTimer(&profilerTimer);
#endif
	//Game3Init();
	while(1)
	{
		OS_Update();
		
#ifdef PROFILING
		PRO_StartTimer(&gameTimer);
#endif
		//GameScrollText();
		GameInputTest();
		
#ifdef PROFILING
		gameSum += PRO_StopTimer(&gameTimer);
		nLoops++;
		
		//here is where we calculate averages and print
		if (PRO_StopTimer(&profilerTimer) >= LOAD_REFRESH_US)
		{
			PRO_StartTimer(&profilerTimer);
			unsigned long conAvg = conSum / nLoops;
			unsigned long inpAvg = inpSum / nLoops;
			unsigned long dspAvg = dspSum / nLoops;
			unsigned long sndAvg = sndSum / nLoops;
			unsigned long gameAvg = gameSum / nLoops;
			//TODO print averages.
			
			CON_SendString(PSTR("----Profiler Data----\r\n"));
			
			CON_SendString(PSTR("Console Avg Time: "));
			printInt(conAvg, VAR_UNSIGNED);
			CON_SendString(PSTR("\r\n"));
			
			CON_SendString(PSTR("Input Avg Time: "));
			printInt(inpAvg, VAR_UNSIGNED);
			CON_SendString(PSTR("\r\n"));
			
			/*CON_SendString(PSTR("Display Avg Time: "));
			printInt(dspAvg, VAR_UNSIGNED);
			CON_SendString(PSTR("\r\n"));*/
			
			CON_SendString(PSTR("Sound Avg Time: "));
			printInt(sndAvg, VAR_UNSIGNED);
			CON_SendString(PSTR("\r\n"));
			
			CON_SendString(PSTR("Game Avg Time: "));
			printInt(gameAvg, VAR_UNSIGNED);
			CON_SendString(PSTR("\r\n"));
			
			CON_SendString(PSTR("Cycles/Sec: "));
			printInt(nLoops, VAR_UNSIGNED);
			CON_SendString(PSTR("\r\n"));
			
			CON_SendString(PSTR("---------------------\r\n"));
			
			conAvg = inpAvg = dspAvg = sndAvg = gameAvg = 0;
			conSum = inpSum = dspSum = sndSum = gameSum = 0;
			nLoops = 0;
		}
#endif
	}
	return 0;
}

static char
OS_InitSubsystems(void)
{
	if (!TME_Init()) 
		return 0;
	
	if (!CON_Init())
		return 0;
		
	if (!DSP_Init())
		return 0;
		
	if (!INP_Init())
		return 0;
		
	if (!SND_Init())
		return 0;
	
	if (!DSK_Init())
		return 0;
	
	if (!GLIB_Init())
		return 0;
	
	return 1;
}

static char 
OS_Init(void)
{
	g_OSState = 0;
	g_OSDebugLevels = 0;
	m_endCycle = 0;
	m_sysLoad = 0;	
	m_idleTimeStart = 0;
	m_idleTimeEnd = 0;
	m_idleTime = 0;

	/* TODO These should be loaded from EEPROM on startup*/
	OS_SetConfig(OS_CPU_SCALING_ENABLED, 0);
	OS_SetConfig(OS_SOUND_ENABLED, 0);
	OS_SetConfig(OS_DISPLAY_ENABLED, 1);
	OS_SetConfig(OS_INPUT_ENABLED, 1);

	DSP_SetConfig(DSP_VSYNC, 0);
	DSP_SetConfig(DSP_REFRESH_HZ, 60);
	
	return 1;
}

void 
OS_Update(void)
{
	g_OSIdleLoopTimeMs = TME_GetAccurateMillis();
	/* This is the OS idle loop. Any time spent in here is counted as time 
	spent idling. */
	OS_CPULoadCalc(TIMER_START);

#ifdef PROFILING
	PRO_StartTimer(&conTimer);
#endif
	CON_Update();
#ifdef PROFILING
	conSum += PRO_StopTimer(&conTimer);
#endif	

	if (GetBit(&g_OSState, OS_INPUT_ENABLED))
	{
#ifdef PROFILING
		PRO_StartTimer(&inpTimer);
#endif
		INP_Update();
#ifdef PROFILING
		inpSum += PRO_StopTimer(&inpTimer);
#endif
	}
		
	if (GetBit(&g_OSState, OS_DISPLAY_ENABLED))
	{
#ifdef PROFILING
		PRO_StartTimer(&dspTimer);
#endif
		//DSP_Refresh();
		/* Being driven by interrupt in hardware.c
		 * disregard profiler output for display until timing
		 * points are moved. If we should even do that. Display
		 overhead should be constant. */
#ifdef PROFILING
		dspSum += PRO_StopTimer(&dspTimer);
#endif
	}
	
	if (GetBit(&g_OSState, OS_SOUND_ENABLED))
	{
#ifdef PROFILING
		PRO_StartTimer(&sndTimer);
#endif
		SND_Update();
#ifdef PROFILING
		sndSum += PRO_StopTimer(&sndTimer);
#endif
	}
		
	OS_CPULoadCalc(TIMER_STOP);
}

static void 
OS_FatalError(enum e_FATAL_ERRORS error)
{
	//CPU_SET_FREQ(0x00);
	/*blink some pattern on D6 or on display to signify something went
	VERY wrong during startup. Perhaps beep would be better. */
	
	if (error == INIT_ERROR)
	{
		unsigned int overflow = 0;
		while(1)
		{
			if (overflow < 32768)
				HRD_SetPinDigital(11, 1);
			else
				HRD_SetPinDigital(11, 0);
			
			overflow++;
			
		}; //loop forever
	}
}

char 
OS_SetConfig(enum e_OSParameter parameter, const char newValue)
{
	switch ( parameter )
	{
		case OS_CPU_SCALING_ENABLED:
			if (newValue)
				SetBit(&g_OSState, OS_CPU_SCALING_ENABLED);
			else
				ClearBit(&g_OSState, OS_CPU_SCALING_ENABLED);
			break;
		case OS_SOUND_ENABLED:
			if (newValue)
				SetBit(&g_OSState, OS_SOUND_ENABLED);
			else
				ClearBit(&g_OSState, OS_SOUND_ENABLED);
			break;
		case OS_DISPLAY_ENABLED:
			if (newValue)
				SetBit(&g_OSState, OS_DISPLAY_ENABLED);
			else
				ClearBit(&g_OSState, OS_DISPLAY_ENABLED);
			break;
		case OS_INPUT_ENABLED:
			if (newValue)
				SetBit(&g_OSState, OS_INPUT_ENABLED);
			else
				ClearBit(&g_OSState, OS_INPUT_ENABLED);
			break;
		case OS_STATE:
			g_OSState = newValue;
			break;
		case OS_DEBUGLEVELS:
			g_OSDebugLevels = newValue;
			break;
		default:
			return 0;
	}
	
	return 1;
}

char 
OS_GetConfig(enum e_OSParameter parameter)
{
	switch ( parameter )
	{
		case OS_CPU_SCALING_ENABLED:
			return GetBit(&g_OSState, OS_CPU_SCALING_ENABLED);
			break;
		case OS_SOUND_ENABLED:
			return GetBit(&g_OSState, OS_SOUND_ENABLED);
			break;
		case OS_DISPLAY_ENABLED:
			return GetBit(&g_OSState, OS_DISPLAY_ENABLED);
			break;
		case OS_INPUT_ENABLED:
			return GetBit(&g_OSState, OS_INPUT_ENABLED);
			break;
		case OS_STATE:
			return g_OSState;
			break;
		case OS_DEBUGLEVELS:
			return g_OSDebugLevels;
			break;
		default:
			return -1;
	}
}

void
OS_CPUScaleByLoad(void)
{	
	static unsigned char curClock = 255;
	if (m_sysLoad >= 66)
	{
		if (curClock == CPU_16MHZ)
			return;
		curClock = TME_ScaleCpu(CPU_16MHZ);
	}
	else if (m_sysLoad < 66 && m_sysLoad >= 44)
	{
		if (curClock == CPU_8MHZ)
			return;
		curClock = TME_ScaleCpu(CPU_8MHZ);
	}
	else if (m_sysLoad < 44 && m_sysLoad >= 25)
	{
		if (curClock == CPU_4MHZ)
			return;
		curClock = TME_ScaleCpu(CPU_4MHZ);
	}
	else if (m_sysLoad < 25 && m_sysLoad >= 0)
	{
		if (curClock == CPU_1MHZ)
			return;
		curClock = TME_ScaleCpu(CPU_1MHZ);

	}
}

static void 
OS_CPULoadCalc(enum e_TimerParams parameter)
{
	unsigned long curMicros = TME_GetAccurateMicros();
	
	if (parameter == TIMER_START)
		m_idleTimeStart = curMicros; 
	else if (parameter == TIMER_STOP)
	{
		m_idleTimeEnd = TME_GetAccurateMicros();
		m_idleTime += (m_idleTimeEnd - m_idleTimeStart);
	}
	
	if (m_endCycle <= curMicros)
	{
		m_endCycle = curMicros + LOAD_REFRESH_US;
		//m_sysLoad = 100 - (100 * ((float)m_idleTime / (float)LOAD_REFRESH_US));
		
		//shouldnt it be...
		m_sysLoad = 100 - (100 * m_idleTime) / LOAD_REFRESH_US;

		if (m_sysLoad > 100)
			m_sysLoad = 100;
		else if (m_sysLoad < 0)
			m_sysLoad = 0;

		m_idleTime = 0;
		
		#ifdef DEBUG 
		/*if (GetBit(&g_OSDebugLevels, DEBUG_OS))
		{
			print("Cpu load: 0x");
			phex16(m_sysLoad);
			print(" @ 0x");
			phex16(TME_GetCpuClockMhz());
			print(" Mhz\n");
		}*/
		#endif
		
		if (GetBit(&g_OSState, OS_CPU_SCALING_ENABLED))
			OS_CPUScaleByLoad();
	}		
}

static char 
OS_SaveSysConfig(void)
{
	//save OS config
	DSK_WriteByte(FILE_OS_STATE, g_OSState);
	DSK_WriteByte(FILE_OS_DEBUGLEVELS, g_OSDebugLevels);
	
	//save input config
	DSK_WriteWord(FILE_INP_CALIBMIN, INP_GetConfig(INPUT_LBOUND));
	DSK_WriteWord(FILE_INP_CALIBMAX, INP_GetConfig(INPUT_UBOUND));
	DSK_WriteByte(FILE_INP_POLLRATE, INP_GetConfig(INPUT_POLLINTERVALMS));
	
	//save dsp config
	DSK_WriteByte(FILE_DSP_STATE, DSP_GetConfig(DSP_STATE_BITS));
	DSK_WriteByte(FILE_DSP_REFRESHRATE, DSP_GetConfig(DSP_REFRESH_HZ));
	return 1;
}

static char 
OS_RestoreSysConfig(void)
{
	//restore OS
	OS_SetConfig(OS_STATE, DSK_ReadByte(FILE_OS_STATE));
	OS_SetConfig(OS_DEBUGLEVELS, DSK_ReadByte(FILE_OS_DEBUGLEVELS));
	
	//restore input
	INP_SetConfig(INPUT_LBOUND, DSK_ReadWord(FILE_INP_CALIBMIN));
	INP_SetConfig(INPUT_UBOUND, DSK_ReadByte(FILE_INP_CALIBMAX));
	INP_SetConfig(INPUT_POLLINTERVALMS, DSK_ReadByte(FILE_INP_POLLRATE));
	
	//restore dsp
	DSP_SetConfig(DSP_STATE_BITS, DSK_ReadByte(FILE_DSP_STATE));
	DSP_SetConfig(DSP_REFRESH_HZ, DSK_ReadByte(FILE_DSP_REFRESHRATE));
	return 1;
}