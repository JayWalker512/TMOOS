#ifndef PROFILING_H
#define PROFILING_H

typedef struct Timer_s 
{
	unsigned long startTime;
	unsigned long endTime;
	unsigned long elapsedTime;
} Timer_t;

/* The timer functions only deal with microseconds since they're for precise
 profiling. */
void PRO_StartTimer(Timer_t *timer);
unsigned long PRO_StopTimer(Timer_t *timer);

#endif
