#ifndef CONSOLE_H
#define CONSOLE_H

enum e_ConsoleParams {
	CONSOLE_USB_CONFIGURED = 0,
	CONSOLE_CMD_DISPATCHED = 1,
};

int CON_Init(void);

void CON_Update(void);

#endif
