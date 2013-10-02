#include "input.h"

#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../common/binary.h"
#include "../common/print.h"
#include "../common/pins.h"

static unsigned char m_inputUpdateInterval; //max of 255ms which is ~4hz
static unsigned long m_timeToUpdate;
static unsigned int m_calibMin, m_calibMax;
static unsigned char m_wheelPos;
static unsigned char m_buttonStates;
static unsigned char m_inputPins[4]; 

//for event list
static unsigned char m_inputEvents;
static unsigned char m_oldButtonStates;

/* TODO Maybe we should add functions to allow calibrating the input wheel via the
interactive console, but for now it will be hardcoded. */
char 
INP_Init(void) //TODO init settings (poll rate, calib) should be passed here from OS
{
	m_inputUpdateInterval = 60;
	m_timeToUpdate = 0;
	
	m_buttonStates = 0;
	m_wheelPos = 0;
	
	m_inputPins[INPUT_WHEEL] = INPUT_WHEEL_PIN; //== A2 Arduino analog pin # scheme
	m_inputPins[INPUT_PB0] = INPUT_PB0_PIN;	//regular ard # scheme
	m_inputPins[INPUT_PB1] = INPUT_PB1_PIN;
	m_inputPins[INPUT_PB2] = INPUT_PB2_PIN;
	
	/*TODO These values will need loaded from EEPROM eventually*/
	m_calibMin = 100;
	m_calibMax = 900;
	
	//init events list
	m_inputEvents = 0;
	
	return 1;
}

void 
INP_Update(void)
{	
	unsigned long curTime = TME_GetAccurateMillis();
	if (curTime < m_timeToUpdate)
		return;
		
	m_timeToUpdate = curTime + m_inputUpdateInterval;

	//update wheel 
	unsigned long tempState = 0;
	tempState = HRD_GetPinAnalog(m_inputPins[INPUT_WHEEL]);
	
	
	if (tempState > m_calibMax)
		tempState = m_calibMax;
	else if (tempState < m_calibMin)
		tempState = m_calibMin;
		
	m_wheelPos = (unsigned long)(tempState - m_calibMin) * 255 / (m_calibMax - m_calibMin);
	
	//update buttons
	if (!HRD_GetPinDigital(m_inputPins[INPUT_PB0])) //active low, remember!
		SetBit(&m_buttonStates, INPUT_PB0);
	else
		ClearBit(&m_buttonStates, INPUT_PB0);
		
	if (!HRD_GetPinDigital(m_inputPins[INPUT_PB1]))
		SetBit(&m_buttonStates, INPUT_PB1);
	else
		ClearBit(&m_buttonStates, INPUT_PB1);
	
	if (!HRD_GetPinDigital(m_inputPins[INPUT_PB2]))
		SetBit(&m_buttonStates, INPUT_PB2);
	else
		ClearBit(&m_buttonStates, INPUT_PB2);
}

unsigned char INP_PollEvents(void)
{
	if (m_oldButtonStates == m_buttonStates)
	{
		m_inputEvents = 0;
		return m_inputEvents; //nothing has changed!
	}
	
	m_inputEvents = 0; //reset events
		
	if (GetBit(&m_buttonStates, INPUT_PB0) > GetBit(&m_oldButtonStates, INPUT_PB0))
		SetBit(&m_inputEvents, INPUT_PB0_DOWN);
	else if (GetBit(&m_buttonStates, INPUT_PB0) < GetBit(&m_oldButtonStates, INPUT_PB0))
		SetBit(&m_inputEvents, INPUT_PB0_UP);
	
	if (GetBit(&m_buttonStates, INPUT_PB1) > GetBit(&m_oldButtonStates, INPUT_PB1))
		SetBit(&m_inputEvents, INPUT_PB1_DOWN);
	else if (GetBit(&m_buttonStates, INPUT_PB1) < GetBit(&m_oldButtonStates, INPUT_PB1))
		SetBit(&m_inputEvents, INPUT_PB1_UP);
	
	if (GetBit(&m_buttonStates, INPUT_PB2) > GetBit(&m_oldButtonStates, INPUT_PB2))
		SetBit(&m_inputEvents, INPUT_PB2_DOWN);
	else if (GetBit(&m_buttonStates, INPUT_PB2) < GetBit(&m_oldButtonStates, INPUT_PB2))
		SetBit(&m_inputEvents, INPUT_PB2_UP);
	
	m_oldButtonStates = m_buttonStates;
	return m_inputEvents;
}



unsigned char
INP_GetInputState(enum e_InputDevice device)
{
	switch (device)
	{
		case INPUT_WHEEL:
			return m_wheelPos;
			break;
		case INPUT_PB0:
			return GetBit(&m_buttonStates, INPUT_PB0);
			break;
		case INPUT_PB1:
			return GetBit(&m_buttonStates, INPUT_PB1);
			break;
		case INPUT_PB2:
			return GetBit(&m_buttonStates, INPUT_PB2);
			break;
		default:
			return 0;
	}
	return 0;
}

unsigned char 
INP_GetWheelRegion(unsigned char regions)
{
	unsigned char wheelPos = INP_GetInputState(INPUT_WHEEL);
	unsigned char compliment = INPUT_WHEEL_RANGE - wheelPos;
	unsigned char regionSize = INPUT_WHEEL_RANGE / regions;
	if (wheelPos < INPUT_WHEEL_RANGE)
		return floor( wheelPos / regionSize);
	else
		return regions - 1;
}

void 
INP_Calibrate(enum e_InputParameters parameter)
{
	if (parameter == INPUT_LBOUND)
		m_calibMin = HRD_GetPinAnalog(m_inputPins[INPUT_WHEEL]);
	else if (parameter == INPUT_UBOUND)
		m_calibMax = HRD_GetPinAnalog(m_inputPins[INPUT_WHEEL]);
	
	return 1;
}

void 
INP_SetConfig(enum e_InputParameters parameter,
			    unsigned char newValue)
{
	switch (parameter)
	{
		case INPUT_LBOUND:
			m_calibMin = newValue;
			break;
		case INPUT_UBOUND:
			m_calibMax = newValue;
			break;
		case INPUT_POLLINTERVALMS:
			m_inputUpdateInterval = newValue;
			break;
		default:
			break;		
	}
}

int 
INP_GetConfig(enum e_InputParameters parameter)
{
	switch ( parameter )
	{
		case INPUT_LBOUND:
			return m_calibMin;
			break;
		case INPUT_UBOUND:
			return m_calibMax;
			break;
		case INPUT_POLLINTERVALMS:
			return m_inputUpdateInterval;
			break;
		default:
			return -1;
	}
}