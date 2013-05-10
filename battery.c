#include "battery.h"
#include <hardware/hardware.h>
#include <math.h>

#define BATTERY_INPUT_PIN 1

static unsigned int m_calibMin, m_calibMax;

char 
OS_BatteryInit(void)
{
	m_calibMax = 307;
	m_calibMin = 204;
	return 1;
}

int
OS_GetBatteryLevel(void)
{
	/*TODO: Will use floats for now. Fix to use only ints later. */
	//unsigned int battLevel;
	//battLevel = HRD_GetPinAnalog(BATTERY_INPUT_PIN);
	//battLevel -= m_calibMin;
	//return 100 * floor( (float)battLevel / (float)(m_calibMax - m_calibMin));
	return HRD_GetPinAnalog(BATTERY_INPUT_PIN);
}

void 
OS_BatteryCalibrate(enum e_BatteryParameters parameter)
{
	
}