#ifndef OS_H
#define OS_H

/* (C) 2013 Brandon Foltz

This file should contain extern prototypes of all system functions that may
need to be accessed from a game. Games should ONLY include this file and any
necessary files from sharedlib. */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

enum e_FATAL_ERRORS {
	INIT_ERROR,
	};
	
enum e_OSParameter {
	CPU_SCALING_ENABLED = 0,
	SOUND_ENABLED = 1,
	};
	
enum e_OSDebugLevels {
	DEBUG_OS = 0,
	DEBUG_DSP,
	DEBUG_SND,
	DEBUG_INP,
	DEBUG_DSK,
	DEBUG_TME,
	DEBUG_CNS
};

enum e_TimerParams {
	TIMER_START = 0,
	TIMER_STOP,
};
	
//cached version of TME_GetAccurateMillis() from beginning of OS_Update() for non-time-critical use only. Good for games.
extern unsigned long g_OSIdleLoopTimeMs;
extern unsigned char g_OSState;
extern unsigned char g_OSDebugLevels; /*each bit corresponds to a subsystem. If bit enabled,
that subsystem will print it's debug info to terminal. Bits are above. */

/* System idle loop. Handles peripherals such as display, sound, input, etc.
Should be called FREQUENTLY! Preferably in every loop that may take any 
significant amount of time to complete. This shouldn't affect your code speed 
much, as it will return quickly if not enough time has passed for anything to 
need to be done. */
void OS_Update(void);
	
char OS_SetConfig(enum e_OSParameter parameter, const char newValue);

char OS_GetConfig(enum e_OSParameter parameter);


#endif
