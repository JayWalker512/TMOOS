#ifndef PROFILING_H
#define PROFILING_H

typedef struct s_Timer 
{
	unsigned long startTime;
	unsigned long endTime;
	unsigned long elapsedTime;
} t_Timer;

/* The timer functions only deal with microseconds since they're for precise
 profiling. */
void PRO_StartTimer(t_Timer *timer);
unsigned long PRO_StopTimer(t_Timer *timer);

#endif
