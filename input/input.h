#ifndef INPUT_H
#define INPUT_H

#define INPUT_WHEEL_MAX 255
#define INPUT_WHEEL_MIN 0

enum e_InputDevice {
	INPUT_PB1 = 0,
	INPUT_PB2 = 1,
	INPUT_WHEEL = 8,
};

enum e_InputParameters {
	INPUT_LBOUND = 0,
	INPUT_UBOUND,
	INPUT_POLLINTERVALMS,
};

char INP_Init(void);

void INP_Update(void);

unsigned long INP_GetInputState(enum e_InputDevice device);

void INP_Calibrate(enum e_InputParameters parameter);

void INP_SetConfig(enum e_InputParameters parameter,
			    unsigned char newValue);

int INP_GetConfig(enum e_InputParameters parameter);

#endif
