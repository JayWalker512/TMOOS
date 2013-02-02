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

#ifdef PIN_OFFSET_TABLE

/* This could be optimized further still by removing address additions from 
 set/get functions, but that would obfuscate things a bit. */
const unsigned char m_arduinoPinsTable[][2] = { {0,0}, {0,1}, {0,2}, {0,3},
{0,7}, {6,0}, {6,1}, {6,2}, {6,3}, {3,6}, {3,7}, {6,6}, {6,7}, {0,4}, {0,5},
{0,6}, {12,7}, {12,6}, {12,5}, {12,4}, {12,1}, {12,0}, {6,4}, {6,5}, {9,6} }; 

#elif PIN_OFFSET_CALC
const unsigned char m_arduinoPinsTable[] = { 0, 1, 2, 3, 7, 20, 21, 22, 23, 16, 
	17, 26, 27, 4, 5, 6, 47, 46, 45, 44, 41, 40, 24, 25, 36 };
#endif

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
	
	return 1;
}

#ifdef PIN_OFFSET_TABLE
char
HRD_SetPinDigital(const unsigned char ardPin, unsigned char value)
{
	/* This function is only written for the Atmel Mega 32u4 */
	
	//set the pin
	if (value == 0)
	{
		// make the pin an output (DDRx)
		SetBit((unsigned char *)(&DDRB + m_arduinoPinsTable[ardPin][0]), m_arduinoPinsTable[ardPin][1]);
		// drive it low (PORTx)
		ClearBit((unsigned char *)(&PORTB + m_arduinoPinsTable[ardPin][0]), m_arduinoPinsTable[ardPin][1]);
		
	}
	else
	{
		// make the pin an output
		SetBit((unsigned char *)(&DDRB + m_arduinoPinsTable[ardPin][0]), m_arduinoPinsTable[ardPin][1]);
		// drive it high
		SetBit((unsigned char *)(&PORTB + m_arduinoPinsTable[ardPin][0]), m_arduinoPinsTable[ardPin][1]);
	}
	
	return 1;
}

unsigned char 
HRD_GetPinDigital(const unsigned char ardPin)
{	
	ClearBit((unsigned char *)(&PORTB + m_arduinoPinsTable[ardPin][0]), m_arduinoPinsTable[ardPin][1]); //pullup resistor mode
	return GetBit((unsigned char *)(&PINB + m_arduinoPinsTable[ardPin][0]), m_arduinoPinsTable[ardPin][1]);
}
#endif

#ifdef PIN_OFFSET_CALC
char 
HRD_SetPinDigital(const unsigned char ardPin, unsigned char value)
{
	/* This function is only written for the Atmel Mega 32u4 */
	unsigned char port = 0;
	unsigned char pin = 0;

	port = (m_arduinoPinsTable[ardPin] / 10) * 3;
	pin = (m_arduinoPinsTable[ardPin] % 10);
	
	//set the pin
	if (value == 0)
	{
		// make the pin an output (DDRx)
		//*(unsigned char *)(&DDRB + port) |= (1 << pin);
		SetBit((unsigned char *)(&DDRB + port), pin);
		// drive it low (PORTx)
		//*(unsigned char *)(&PORTB + port) &= ~(1 << pin);
		ClearBit((unsigned char *)(&PORTB + port), pin);
		
	}
	else
	{
		// make the pin an output
		//*(unsigned char *)(&DDRB + port) |= (1 << pin);
		SetBit((unsigned char *)(&DDRB + port), pin);
		// drive it high
		//*(unsigned char *)(&PORTB + port) |= (1 << pin);
		SetBit((unsigned char *)(&PORTB + port), pin);
	}
	
	return 1;
}

unsigned char 
HRD_GetPinDigital(const unsigned char ardPin)
{
	unsigned char port = 0;
	unsigned char pin = 0;
	
	port = (m_arduinoPinsTable[ardPin] / 10) * 3;
	pin = (m_arduinoPinsTable[ardPin] % 10);
	
	ClearBit((unsigned char *)(&PORTB + port), pin); //pullup resistor mode
	return GetBit((unsigned char *)(&PINB + port), pin);
}
#endif



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