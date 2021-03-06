#include "profiling.h"
#include "../hardware/hardware.h"

void 
PRO_StartTimer(Timer_t *timer)
{
	timer->endTime = 0;
	timer->elapsedTime = 0;
	timer->startTime = HRD_GetMicros();
}

unsigned long 
PRO_StopTimer(Timer_t *timer)
{
	timer->endTime = HRD_GetMicros();
	timer->elapsedTime = timer->endTime - timer->startTime;
	return timer->elapsedTime;
}