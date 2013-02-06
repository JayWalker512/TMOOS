#include "input.h"

#ifndef TESTCASE
#include "../os.h"
#endif

#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../sharedlib/binary.h"

#ifdef DEBUG
#include "../debug/debug.h"
#include "../avr_common.h"
#endif

unsigned char m_inputUpdateInterval; //max of 255ms which is ~4hz
unsigned long m_timeToUpdate;
unsigned int m_calibMin, m_calibMax;
unsigned char m_wheelPos;
unsigned char m_buttonStates;
unsigned char m_inputPins[3];

/* TODO Maybe we should add functions to allow calibrating the input wheel via the
interactive console, but for now it will be hardcoded. */
char 
INP_Init(void) //TODO init settings (poll rate, calib) should be passed here from OS
{
	m_inputUpdateInterval = 40;
	m_timeToUpdate = 0;
	
	m_buttonStates = 0;
	m_wheelPos = 0;
	
	m_inputPins[INPUT_WHEEL] = 2; //Arduino analog pin # scheme
	m_inputPins[INPUT_PB1] = 18;	
	m_inputPins[INPUT_PB2] = 12;
	
	/*TODO These values will need loaded from EEPROM eventually*/
	m_calibMin = 150;
	m_calibMax = 850;
	
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
	if (HRD_GetPinDigital(m_inputPins[INPUT_PB1]) == 0) //active low, remember!
		SetBit(&m_buttonStates, INPUT_PB1);
	else
		ClearBit(&m_buttonStates, INPUT_PB1);
		
	if (HRD_GetPinDigital(m_inputPins[INPUT_PB2]) == 0)
		SetBit(&m_buttonStates, INPUT_PB2);
	else
		ClearBit(&m_buttonStates, INPUT_PB2);
}


unsigned long 
INP_GetInputState(enum e_InputDevice device)
{
	switch (device)
	{
		case INPUT_WHEEL:
			return m_wheelPos;
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

void INP_Calibrate(enum e_InputParameters parameter)
{
	if (parameter == INPUT_LBOUND)
		m_calibMin = HRD_GetPinAnalog(m_inputPins[INPUT_WHEEL]);
	else if (parameter == INPUT_UBOUND)
		m_calibMax = HRD_GetPinAnalog(m_inputPins[INPUT_WHEEL]);
	
	return 1;
}

void INP_SetConfig(enum e_InputParameters parameter,
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

int INP_GetConfig(enum e_InputParameters parameter)
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