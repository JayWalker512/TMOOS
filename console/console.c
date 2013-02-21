#include "console.h"
#include "usb_serial.h"
#include "../sharedlib/binary.h"
#include "../avr_common.h"
#include <string.h> //for working with strings in regular RAM

void CON_SendString(const char *s);
uint8_t CON_RecieveString(char *buf, uint8_t size);

static char CON_BufferInput(char *buffer, const unsigned char size);
static char CON_ParseArgcArgv(char **argv, const char bufSize, const char *cmdString);

//static void CON_PushBuffer(const char c);
#define CMD_BUFFER_SIZE 32

static char m_consoleState;
static char m_cmdBufferString[CMD_BUFFER_SIZE];
static char m_cmdBufferIndex;

int 
CON_Init(void)
{
	m_consoleState = 0;
	m_cmdBufferIndex = 0;
	usb_init();
	return 1;
}

void
CON_Update(void)
{
	if (!GetBit(&m_consoleState, CONSOLE_USB_CONFIGURED))
	{
		if(!usb_configured())
			return;
	
		if(!(usb_serial_get_control() & USB_SERIAL_DTR))
			return;
	
		usb_serial_flush_input();
		SetBit(&m_consoleState, CONSOLE_USB_CONFIGURED);
		CON_SendString(PSTR("\r\nBMOS Terminal\r\n"
			"Format is: command arg1 arg2 arg3\r\n>"));
	}
	else
	{
		/*do all our console in/out stuff here. will need some kind of 
		 * time out perhaps incase the usb cable is yanked, we can stop
		 * taking console input. */
		
		char in;
		
		/*TODO only take input if previously dispatched command
		 * has completed. */
		in = CON_BufferInput(&m_cmdBufferString, CMD_BUFFER_SIZE);
		
		if (in == 0)
			ClearBit(&m_consoleState, CONSOLE_USB_CONFIGURED);
		else if (in == 4)
		{
			//TODO parse argc, argv
			
			
			
			m_cmdBufferIndex = 0; //only after parse/dispatch
		}
	}
}

char 
CON_BufferInput(char *buffer, const unsigned char size)
{
	char input = usb_serial_getchar();
	if (m_cmdBufferIndex < size)
	{
		if (input >= ' ' && input <= '~')
		{
			*(buffer+m_cmdBufferIndex) = input;
			m_cmdBufferIndex++;
			usb_serial_putchar(input);
		}
	}
	if (input == '\r' || input == '\n')
	{
		*(buffer+m_cmdBufferIndex) = '\0'; //dat null terminator
		CON_SendString(PSTR("\r\n>"));
		return 4; //end of transmission
	}
	
	if (!usb_configured() ||
		!(usb_serial_get_control() & USB_SERIAL_DTR)) 
	{
		// user no longer connected
		return 0;
	}
	return input;
}

// Send a string to the USB serial port.  The string must be in
// flash memory, using PSTR
//
void 
CON_SendString(const char *s)
{
	char c;
	while (1) {
		c = pgm_read_byte(s++);
		if (!c) break;
		usb_serial_putchar(c);
	}
}

// Receive a string from the USB serial port.  The string is stored
// in the buffer and this function will not exceed the buffer size.
// A carriage return or newline completes the string, and is not
// stored into the buffer.
// The return value is the number of characters received, or 255 if
// the virtual serial connection was closed while waiting.
//
uint8_t 
CON_RecieveString(char *buf, uint8_t size)
{
	/*XXX: this function is BLOCKING and not appropriate for our use*/
	int16_t r;
	uint8_t count=0;

	while (count < size) {
		r = usb_serial_getchar();
		if (r != -1) {
			if (r == '\r' || r == '\n') return count;
			if (r >= ' ' && r <= '~') {
				*buf++ = r;
				usb_serial_putchar(r);
				count++;
			}
		} else {
			if (!usb_configured() ||
			  !(usb_serial_get_control() & USB_SERIAL_DTR)) {
				// user no longer connected
				return 255;
			}
			// just a normal timeout, keep waiting
		}
	}
	return count;
}

char
CON_ParseArgcArgv(char **argv, const char bufSize, const char *cmdString)
{
	/* XXX this function is not working properly yet. Look into the strncpy
	 function. */
	char curSpace = 0, prevSpace = 0;
	char argc = 0;
	
	unsigned char i = 0;
	while (*(cmdString+i) != '\0')
	{
		if (*(cmdString+i) == ' ')
		{
			if (curSpace == 0)
				curSpace = i;
			else
			{
				prevSpace = curSpace;
				curSpace = i;
			}
			//TODO bufsize not taken into account!
			strncpy(&argv[argc], cmdString, curSpace - prevSpace);
			argc++;
		}
		i++;
	}
	return argc;
}