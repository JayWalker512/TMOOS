#include "console.h"
#include "usb_serial.h"
#include "../sharedlib/binary.h"
#include "../avr_common.h"

void CON_SendString(const char *s);
uint8_t CON_RecieveString(char *buf, uint8_t size);

static char m_consoleState;

int 
CON_Init(void)
{
	m_consoleState = 0;
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
			"Format is: command arg1 arg2 arg3\r\n"));
	}
	else
	{
		/*do all our console in/out stuff here. will need some kind of 
		 * time out perhaps incase the usb cable is yanked, we can stop
		 * taking console input. */
		
		int n = usb_serial_getchar();
		if (n >= 0) usb_serial_putchar(n);
	}
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
