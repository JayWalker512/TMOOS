#include "gamemath.h"
#include <stdlib.h>

float 
RandFloat(float min, float max)
{
	return min + (float)rand()/((float)RAND_MAX/max);
}

long 
RandLong(long min, long max)
{
	return min + (long)rand()/((long)RAND_MAX/max);
}