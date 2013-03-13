#include "../avr_common.h"
#include "libarduino/Arduino.h"
#include "hardware.h"
#include "../sharedlib/binary.h"

#ifdef DEBUG
#include "../debug/debug.h"
#endif

#define PIN_OFFSET_TABLE

/* TODO Arduino Pin Compatibility
 
 So we'd have an array with the # of pins onboard like the one below. The array 
 * index of course indicates pin #, and the following operations give you the
 * offsets corresponding to port and pin (bit) number.
 * 
 * eg. 26 corresponds to pin D6. 
 * Take the number and divide by 10. This gives you the port with 0 being B and 
 * 4 being F. Then modulus the value by 10, which will give you the pin # and
 * likewise the bit offset. 
 * 
 unsigned char m_arduinoPins[25]; */

#if defined(__AVR_ATmega32U4__)

const unsigned char m_arduinoPinsTable[] = {'B',0, 'B',1, 'B',2, 'B',3, 'B', 7,
	'D',0, 'D',1, 'D',2, 'D',3, 'C',6, 'C',7, 'D',6, 'D',7, 'B',4, 'B',5, 'B',6,
	'F',7, 'F',6, 'F',5, 'F',4, 'F',1, 'F',0, 'D',4, 'D',5, 'E',6};

#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	//todo teensy 2.0++
#endif

static unsigned int HRD_ADCRead(unsigned char mux);

int 
HRD_Init(void)
{
	ARD_Init();
	HRD_GetMicros = &micros;
	HRD_GetMillis = &millis;
	//these are the only 2 functions still depending on Arduino library
	
	//let's set all pins low as part of the initialization. (teensy 2.0)
	for (char i = 0; i <= 24; i++)
		HRD_SetPinDigital(i, 0);
	
	return 1;
}

char HRD_SetPinDigital(const unsigned char ardPin, unsigned char value)
{
	if (ardPin < 0 || ardPin > 24)
		return 0;

	unsigned char port, pin;
	port = m_arduinoPinsTable[ardPin * 2];
	port = port - 'A';
	pin = m_arduinoPinsTable[ardPin * 2 + 1];
	//set the pin
	if (value == 0)
	{
		// make the pin an output
		*(uint8_t *)(0x21 + port * 3) |= (1 << pin);
		// drive it low
		*(uint8_t *)(0x22 + port * 3) &= ~(1 << pin);
	}
	else
	{
		// make the pin an output
		*(uint8_t *)(0x21 + port * 3) |= (1 << pin);
		// drive it high
		*(uint8_t *)(0x22 + port * 3) |= (1 << pin);
	}
	
	return 1;
}

unsigned char HRD_GetPinDigital(const unsigned char ardPin)
{
	if (ardPin < 0 || ardPin > 24)
		return 0;

	unsigned char port, pin;
	port = m_arduinoPinsTable[ardPin * 2];
	port = port - 'A';
	pin = m_arduinoPinsTable[ardPin * 2 + 1];
	
	//set pin to input mode
	*(uint8_t *)(0x21 + port * 3) &= ~(1 << pin);
	// read the pin
	return *(uint8_t *)(0x20 + port * 3) & (1 << pin);
}

/* These functions straight from the PJRC site, compatible with 2.0 and 2.0++ */
static uint8_t aref = (1<<REFS0); // default to AREF = Vcc

void HRD_AnalogReference(unsigned char mode)
{
	aref = mode & 0xC0;
}

// Arduino compatible pin input
unsigned int HRD_GetPinAnalog(unsigned char pin)
{
#if defined(__AVR_ATmega32U4__)
	static const uint8_t PROGMEM pin_to_mux[] = {
		0x00, 0x01, 0x04, 0x05, 0x06, 0x07,
		0x25, 0x24, 0x23, 0x22, 0x21, 0x20};
	if (pin >= 12) return 0;
	return HRD_ADCRead(pgm_read_byte(pin_to_mux + pin));
#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	if (pin >= 8) return 0;
	return HRD_ADCRead(pin);
#else
	return 0;
#endif
}

// Mux input
static unsigned int HRD_ADCRead(unsigned char mux)
{
#if defined(__AVR_AT90USB162__)
	return 0;
#else
	uint8_t low;

	ADCSRA = (1<<ADEN) | ADC_PRESCALER;		// enable ADC
	ADCSRB = (1<<ADHSM) | (mux & 0x20);		// high speed mode
	ADMUX = aref | (mux & 0x1F);			// configure mux input
	ADCSRA = (1<<ADEN) | ADC_PRESCALER | (1<<ADSC);	// start the conversion
	while (ADCSRA & (1<<ADSC)) ;			// wait for result
	low = ADCL;					// must read LSB first
	return (ADCH << 8) | low;			// must read MSB only once!
#endif
}
