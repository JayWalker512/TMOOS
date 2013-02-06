#ifndef OS_H
#define OS_H

/* (C) 2013 Brandon Foltz

This file should contain extern prototypes of all system functions that may
need to be accessed from a game. Games should ONLY include this file and any
necessary files from sharedlib. */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdint.h>

//the enums below are used for accessing bitwise variables most efficiently.
enum e_FATAL_ERRORS {
	INIT_ERROR,
	};

enum e_OSParameter {
	OS_CPU_SCALING_ENABLED = 0,
	OS_SOUND_ENABLED = 1,
	OS_DISPLAY_ENABLED = 2,
	OS_INPUT_ENABLED = 3,
	OS_STATE = 8,
	OS_DEBUGLEVELS = 9,
	};
	
/* No global "debug enabled", each subsys just checks against its respective 
 bit so we can see only what we want. */
enum e_OSDebugLevels {
	DEBUG_OS,
	DEBUG_DSP,
	DEBUG_SND,
	DEBUG_INP,
	DEBUG_DSK,
	DEBUG_TME,
	DEBUG_CNS,
	DEBUG_GAME,
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
