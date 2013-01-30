#ifndef INPUT_H
#define INPUT_H

#define INPUT_WHEEL_MAX 255
#define INPUT_WHEEL_MIN 0

enum e_InputDevice {
	INPUT_PB1 = 0,
	INPUT_PB2,
	INPUT_WHEEL,
};

char INP_Init(void);

void INP_Update(void);

unsigned long INP_GetInputState(enum e_InputDevice device);

#endif
