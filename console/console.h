#ifndef CONSOLE_H
#define CONSOLE_H

enum e_ConsoleParams {
	CONSOLE_USB_CONFIGURED = 0,
	CONSOLE_CMD_DISPATCHED = 1,
};

int CON_Init(void);

void CON_Update(void);


//TODO these need more descriptive names
void CON_SendString(const char *s);
void CON_SendRAMString(const char *s);

#endif
