/* This module was implemented with great haste, so if you're looking for something to 
optimize then this is a good place to start. */

#include "sound.h"
#include "../hardware/hardware.h"
#include "../time/time.h"

#ifdef DEBUG
#include "../debug/debug.h"
#include "../avr_common.h"
#endif

unsigned int m_cycleTime;
unsigned long m_cycleEndTime;
unsigned long m_playStop;

//combine to one byte?
unsigned long m_bPlaying;
unsigned char m_pinState;

/* TODO implement some kind of tone queue so I can line up some sounds to be
played in succession. Would be more interesting than simple beeps or held tones. */

/* Maybe give different functions that will automatically change sounds. Such as
supplying a starting freq, end freq, delta time, ascend or descend and whether it
should move linearly or log from start to end freq. */ 

char SND_Init(void)
{
	m_cycleTime = 0;
	m_cycleEndTime = 0;
	m_pinState = 0;
	m_playStop = 0;
	m_bPlaying = 0;
	return 1;
}

void SND_Update(void)
{
	if (m_bPlaying)
	{
		unsigned long curTime = TME_GetAccurateMicros();
		if (curTime >= m_cycleEndTime)
		{
			if (m_pinState == 0)
			{
				m_pinState = 1;
				HRD_SetPinDigital(18, 1);
			}
			else
			{
				m_pinState = 0;
				HRD_SetPinDigital(18, 0);	
			}
		
			m_cycleEndTime = curTime + m_cycleTime;
		}	
	
		if (TME_GetAccurateMillis() >= m_playStop)
			m_bPlaying = 0;
	}
}

void SND_Beep(unsigned int Hz, unsigned long ms)
{
	m_cycleTime = 1000000 / Hz;
	m_playStop = TME_GetAccurateMillis() + ms;
	m_cycleEndTime = TME_GetAccurateMicros() + m_cycleTime;
	m_bPlaying = 1;
}
