#ifndef INPUT_H
#define INPUT_H

#define INPUT_WHEEL_RANGE 255

#define INPUT_WHEEL_MAX 255 //are these two even used?
#define INPUT_WHEEL_MIN 0


enum e_InputDevice 
{
	INPUT_PB0 = 0,
	INPUT_PB1 = 1,
	INPUT_PB2 = 2,
	INPUT_WHEEL = 3,
};

enum e_InputParameters 
{
	INPUT_LBOUND = 0,
	INPUT_UBOUND,
	INPUT_POLLINTERVALMS,
};

enum e_InputEvents
{
	INPUT_PB0_DOWN = 0,
	INPUT_PB1_DOWN = 1,
	INPUT_PB2_DOWN = 2,
	INPUT_PB0_UP = 3,
	INPUT_PB1_UP = 4,
	INPUT_PB2_UP = 5,
};

char INP_Init(void);

void INP_Update(void);

//returns a uchar with event list
unsigned char INP_PollEvents(void);

unsigned char INP_GetInputState(enum e_InputDevice device);

unsigned char INP_GetWheelRegion(unsigned char regions);

void INP_Calibrate(enum e_InputParameters parameter);

void INP_SetConfig(enum e_InputParameters parameter,
			    unsigned char newValue);

int INP_GetConfig(enum e_InputParameters parameter);

#endif
