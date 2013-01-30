/* (C) 2013 Brandon Foltz

This is the main operating system file. This program has access to all 
peripheral subsystems through their respective interfaces. */

#include "avr_common.h"

#include "hardware/hardware.h"
#include "console/console.h"
#include "disk/disk.h"
#include "display/display.h"
#include "input/input.h"
#include "sound/sound.h"
#include "time/time.h"
#include "games/gamelib.h"

//#include "sharedlib/string.h"
#include "sharedlib/binary.h"
#include "sharedlib/glyphs.h"
#include "os.h"

#ifdef DEBUG
#include "debug/debug.h"
#endif

//may need this here... gonna leave it here for now.
#define CPU_SET_FREQ(n) (CLKPR = 0x80, CLKPR = (n))

//These shouldn't go above ~2 seconds to avoid integer overflows in load calc
#define LOAD_REFRESH_MS 1000
#define LOAD_REFRESH_US 1000000

void OS_Update(void);
static char OS_Init(void);
static char OS_InitSubsystems(void);
static void OS_FatalError(enum e_FATAL_ERRORS error);
static void OS_CPUScaleByLoad(void);
static void OS_CPULoadCalc(enum e_TimerParams parameter);

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

int 
main(void)
{
	CPU_SET_FREQ(CPU_16MHZ); //start @ 16mhz no matter what.

	if(!OS_InitSubsystems())
		OS_FatalError(INIT_ERROR);
		
	if (!OS_Init())
		OS_FatalError(INIT_ERROR);

	SND_Beep(1710, 80); //startup OK notification
	
	//Game3Init();
	while(1)
	{
		OS_Update();
		
		//GameMain();
		Game2Main();
		//Game3Main();
	}
	return 0;
}

static char
OS_InitSubsystems(void)
{
	#ifdef DEBUG
	usb_init();
	_delay_us(100000);
	#endif

	if (!TME_Init()) //start at 16mhz
		return 0;
		
	if (!DSP_Init())
		return 0;
		
	if (!INP_Init())
		return 0;
		
	if (!SND_Init())
		return 0;
		
	if (!GLIB_Init())
		return 0;
		
	/*if (!CON_Init())
		return 0;
	
	if (!DSK_Init(512))
		return 0;*/
	
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
	
	OS_SetConfig(CPU_SCALING_ENABLED, 0);
	OS_SetConfig(SOUND_ENABLED, 1);
	
	DSP_SetConfig(DSP_REFRESH_HZ, 50);
	
	return 1;
}

void 
OS_Update(void)
{
	g_OSIdleLoopTimeMs = TME_GetAccurateMillis();
	/* This is the OS idle loop. Any time spent in here is counted as time 
	spent idling. */
	OS_CPULoadCalc(TIMER_START);
	
	//CON_Update();
	INP_Update();
	DSP_Refresh();
	
	if (GetBit(&g_OSState, SOUND_ENABLED))
		SND_Update();
		
	/*UpdateGlobals(); undecided on this one. probably should use this to
	handle updated cvars */
	
	OS_CPULoadCalc(TIMER_STOP);
	
	#ifdef DEBUG
	/*print("m_idleTimeStart: ");
	phex16(m_idleTimeStart);
	print("\n");
	
	print(" m_idleTimeEnd: ");
	phex16(m_idleTimeEnd);
	print("\n");*/
	//_delay_us(10000);
	#endif
}

static void 
OS_FatalError(enum e_FATAL_ERRORS error)
{
	//CPU_SET_FREQ(0x00);
	/*blink some pattern on D6 or on display to signify something went
	VERY wrong during startup. Perhaps beep would be better. */
	
	if (error == INIT_ERROR)
	{
		#ifdef DEBUG 
		print("Well shit, something went wrong during OS_Init()\n");
		#endif
		
		HRD_SetPin('D', 6, 1);
		while(1); //loop forever
	}
}

char 
OS_SetConfig(enum e_OSParameter parameter, const char newValue)
{
	switch ( parameter )
	{
		case CPU_SCALING_ENABLED:
			if (newValue)
				SetBit(&g_OSState, CPU_SCALING_ENABLED);
			else
			{
				ClearBit(&g_OSState, CPU_SCALING_ENABLED);
				//TME_ScaleCpu(CPU_16MHZ);
			}
			break;
		case SOUND_ENABLED:
			if (newValue)
				SetBit(&g_OSState, SOUND_ENABLED);
			else
				ClearBit(&g_OSState, SOUND_ENABLED);
			break;
		default:
			return 0;
	}
	
	return 1;
}

char 
OS_GetConfig(enum e_OSParameter parameter)
{
	//TODO write this function...
	return 0;
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
	#ifdef DEBUG 
	print("    Cpu scaled to: 0x");
	phex16(TME_GetCpuClockMhz());
	print(" Mhz\n");
	SND_Beep(440, 100); //TODO remove this soon enough....
	#endif
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
		print("Cpu load: 0x");
		phex16(m_sysLoad);
		print(" @ 0x");
		phex16(TME_GetCpuClockMhz());
		print(" Mhz\n");
		#endif
		
		if (GetBit(&g_OSState, CPU_SCALING_ENABLED))
			OS_CPUScaleByLoad();
	}		
}

	/*unsigned long curMillis = TME_GetAccurateMillis();
	m_idleTimeStart = curMillis; 
	
	if (m_endCycle <= curMillis)
	{
		m_endCycle = curMillis + LOAD_REFRESH_MS;
		//m_sysLoad = 100 * ((float)m_idleTime / (float)LOAD_REFRESH_MS);
		
		//shouldnt it be...
		m_sysLoad = 100 - (100 * m_idleTime) / LOAD_REFRESH_MS;

		if (m_sysLoad > 100)
			m_sysLoad = 100;
		else if (m_sysLoad < 0)
			m_sysLoad = 0;

		m_idleTime = 0;
		
		#ifdef DEBUG 
		print("Cpu load: 0x");
		phex16(m_sysLoad);
		print(" @ 0x");
		phex16(TME_GetCpuClockMhz());
		print(" Mhz\n");
		#endif
		
		if (GetBit(&g_OSState, CPU_SCALING_ENABLED))
		{
			OS_CPUScaleByLoad();
		}
	}*/
