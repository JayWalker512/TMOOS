/* (C) 2013 Brandon Foltz

This module handles all control of the CPU (such as scaling) and accurate time
keeping. Since scaling the cpu speed changes time keeping, we need to compensate
for it here. */

#include "time.h"
#include "../hardware/hardware.h"

#ifdef DEBUG
#include "../debug/debug.h"
#include "../avr_common.h"
#endif

#define CPU_SET_FREQ(n) (CLKPR = 0x80, CLKPR = (n))
//#define CPU_16MHZ       0x00
//#define CPU_8MHZ        0x01
//#define CPU_4MHZ        0x02
//#define CPU_2MHZ        0x03
//#define CPU_1MHZ        0x04
//#define CPU_500KHZ      0x05
//#define CPU_250KHZ      0x06
//#define CPU_125KHZ      0x07
//#define CPU_62KHZ       0x08

//static char TME_ScaleCpu(const char freq);

static unsigned char m_timekeepingMultiplier;
//static unsigned char m_curFreq;

static unsigned long m_lastMicros;
static unsigned long m_lastMillis;

static unsigned long m_lastAccurateMicros;
static unsigned long m_lastAccurateMillis;

char 
TME_Init(void)
{
	//need to include hardware/arduino header for these
	HRD_Init();
	
	//we start at 16mhz no matter what. see main() in os.c
	m_timekeepingMultiplier = 1;
	//m_timekeepingMultiplier = 2; //teensy seems to be starting at 8mhz...
	
	m_lastMicros = HRD_GetMicros();
	m_lastMillis = HRD_GetMillis();
	
	m_lastAccurateMicros = HRD_GetMicros();
	m_lastAccurateMillis = HRD_GetMillis();
	
	//TME_ScaleCpu(CPU_16MHZ); //must init at 16mhz
	//m_curFreq = CPU_16MHZ;
	
	return 1;
}

//TODO change this back to defines to save memory... (assuming enums eat it)
char
TME_ScaleCpu(enum CPU_FREQS freq)
{
	//this also needs to adjust vars for accurate timekeeping
	if (freq == CPU_16MHZ)
	{
		m_timekeepingMultiplier = 1;
	}
	else if (freq == CPU_8MHZ)
	{
		m_timekeepingMultiplier = 2;
	}
	else if (freq == CPU_4MHZ)
	{
		m_timekeepingMultiplier = 4;
	}
	else if (freq == CPU_2MHZ)
	{
		m_timekeepingMultiplier = 8;
	}
	else if (freq == CPU_1MHZ)
	{
		m_timekeepingMultiplier = 16;
	}
	TME_GetAccurateMicros(); //need to call these to update m_lastAccurate vars
	TME_GetAccurateMillis();
	
	//TODO return to previous interrupt state rather than global enable/disable
	//unsigned char sreg_backup;

	//sreg_backup = SREG;   /* save interrupt enable/disable state */
	cli();
	CPU_SET_FREQ(freq);
	sei();
	//SREG = sreg_backup;
	
	//m_curFreq = freq;
	return freq;
}

/* Trying to think of the best way to utilize this and the above function. Guess
I'll have to wait and see what I need, and prune the excess later. */
unsigned char 
TME_GetCpuClockMhz(void)
{
	return (16 / m_timekeepingMultiplier);
}

unsigned long 
TME_GetAccurateMicros(void)
{
	unsigned long curMicros = HRD_GetMicros();
	m_lastAccurateMicros += ((curMicros - m_lastMicros) * m_timekeepingMultiplier);
	m_lastMicros = curMicros;
	
	return m_lastAccurateMicros;
}

unsigned long 
TME_GetAccurateMillis(void)
{
	unsigned long curMillis = HRD_GetMillis();
	m_lastAccurateMillis += ((curMillis - m_lastMillis) * m_timekeepingMultiplier);
	m_lastMillis = curMillis;
	
	return m_lastAccurateMillis;

	return 0;
}

/* We could call OS_Update() from these in practice but I want to use these as
actual process stalling delays for now. */
void 
TME_DelayRealMicros(unsigned int micros)
{
	unsigned long endTime = TME_GetAccurateMicros() + micros;
	while (TME_GetAccurateMicros() < endTime);
}

void 
TME_DelayRealMillis(unsigned int millis)
{
	unsigned long endTime = TME_GetAccurateMillis() + millis;
	while (TME_GetAccurateMillis() < endTime);
}


