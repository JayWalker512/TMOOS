#include "hardware.h"
#include "../common/binary.h"
#include "../display/display.h" //for interrupt controlled refresh

#ifdef DEBUG
#include "../debug/debug.h"
#endif

//#if defined(__AVR_ATmega32U4__)

const unsigned char m_arduinoPinsTable[] = {'B',0, 'B',1, 'B',2, 'B',3, 'B', 7,
	'D',0, 'D',1, 'D',2, 'D',3, 'C',6, 'C',7, 'D',6, 'D',7, 'B',4, 'B',5, 'B',6,
	'F',7, 'F',6, 'F',5, 'F',4, 'F',1, 'F',0, 'D',4, 'D',5, 'E',6};

//#elif defined(__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__)
	//todo teensy 2.0++
//#endif

inline unsigned char 
clockCyclesPerMicrosecond(void) {
	return F_CPU / 1000000L;
}

inline unsigned long 
clockCyclesToMicroseconds(unsigned long cycles) {
	return cycles / clockCyclesPerMicrosecond();
}

inline unsigned long 
microsecondsToClockCycles(unsigned long micros) {
	return micros * clockCyclesPerMicrosecond();
}

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

volatile unsigned long m_timer0OverflowCount;
volatile unsigned long m_timer0Millis;
static unsigned char m_timer0Fract;

volatile unsigned char m_displayCounter;
#define DISPLAY_DIVISOR 3

static unsigned int HRD_ADCRead(unsigned char mux);
static void HRD_InterruptInit(void);

ISR(TIMER0_OVF_vect)
{
    	// copy these to local variables so they can be stored in registers
	// (volatile variables must be read from memory on every access)
	unsigned long m = m_timer0Millis;
	unsigned char f = m_timer0Fract;

	m += MILLIS_INC;
	f += FRACT_INC;
	if (f >= FRACT_MAX) {
		f -= FRACT_MAX;
		m += 1;
	}

	m_timer0Fract = f;
	m_timer0Millis = m;
	m_timer0OverflowCount++;
	
	//display interrupt code starts here
	m_displayCounter++;
	unsigned char curDisplayCount = m_displayCounter;
	if (curDisplayCount >= DISPLAY_DIVISOR)
	{
		DSP_Refresh();
		m_displayCounter = 0;
	}
}

int 
HRD_Init(void)
{
	HRD_InterruptInit();
	
	m_timer0OverflowCount = 0;
	m_timer0Millis = 0;
	m_timer0Fract = 0;
	m_displayCounter = 0;
	
	//let's set all pins low as part of the initialization. (teensy 2.0)
	for (char i = 0; i <= 24; i++)
		HRD_SetPinDigital(i, 0);
	
	return 1;
}

void 
HRD_InterruptInit(void)
{
	//Yes, this is copypasta'd from Arduino lib
	cli();

	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
#if defined(TCCR0A) && defined(WGM01)
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
#endif  

	// set timer 0 prescale factor to 64
#if defined(__AVR_ATmega128__)
	// CPU specific: different values for the ATmega128
	sbi(TCCR0, CS02);
#elif defined(TCCR0) && defined(CS01) && defined(CS00)
	// this combination is for the standard atmega8
	sbi(TCCR0, CS01);
	sbi(TCCR0, CS00);
#elif defined(TCCR0B) && defined(CS01) && defined(CS00)
	// this combination is for the standard 168/328/1280/2560
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
#elif defined(TCCR0A) && defined(CS01) && defined(CS00)
	// this combination is for the __AVR_ATmega645__ series
	sbi(TCCR0A, CS01);
	sbi(TCCR0A, CS00);
#else
	//#error Timer 0 prescale factor 64 not set correctly
#endif

	// enable timer 0 overflow interrupt
#if defined(TIMSK) && defined(TOIE0)
	sbi(TIMSK, TOIE0);
#elif defined(TIMSK0) && defined(TOIE0)
	sbi(TIMSK0, TOIE0);
#else
	//#error	Timer 0 overflow interrupt not set correctly
#endif
	
	sei();
}

char 
HRD_SetPinDigital(const unsigned char ardPin, unsigned char value)
{
	if (ardPin < 0 || ardPin > 24) //hardcoded for teensy 2.0
		return 0;

	unsigned char port, pin;
	port = m_arduinoPinsTable[ardPin * 2];
	port = port - 'A';
	pin = m_arduinoPinsTable[ardPin * 2 + 1];
	
	unsigned char offset = port * 3;
	//set the pin
	if (value == 0)
	{
		// make the pin an output
		*(uint8_t *)(0x21 + offset) |= (1 << pin);
		// drive it low
		*(uint8_t *)(0x22 + offset) &= ~(1 << pin);
	}
	else
	{
		// make the pin an output
		*(uint8_t *)(0x21 + offset) |= (1 << pin);
		// drive it high
		*(uint8_t *)(0x22 + offset) |= (1 << pin);
	}
	
	return 1;
}

unsigned char 
HRD_GetPinDigital(const unsigned char ardPin)
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

void 
HRD_CycleClockPin(const unsigned char ardPin)
{
	HRD_SetPinDigital(ardPin, 1);
	HRD_SetPinDigital(ardPin, 0);
}

/* These functions straight from the PJRC site, compatible with 2.0 and 2.0++ */
static uint8_t aref = (1<<REFS0); // default to AREF = Vcc

void 
HRD_AnalogReference(unsigned char mode)
{
	aref = mode & 0xC0;
}

// Arduino compatible pin input
unsigned int 
HRD_GetPinAnalog(unsigned char pin)
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
static unsigned int 
HRD_ADCRead(unsigned char mux)
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

unsigned long 
HRD_GetMillis(void)
{
	unsigned long m;
	uint8_t oldSREG = SREG;

	// disable interrupts while we read timer0_millis or we might get an
	// inconsistent value (e.g. in the middle of a write to timer0_millis)
	cli();
	m = m_timer0Millis;
	SREG = oldSREG;

	return m;
}

unsigned long HRD_GetMicros(void) {
	unsigned long m;
	uint8_t oldSREG = SREG, t;
	
	cli();
	m = m_timer0OverflowCount;
#if defined(TCNT0)
	t = TCNT0;
#elif defined(TCNT0L)
	t = TCNT0L;
#else
	//#error TIMER 0 not defined
#endif

  
#ifdef TIFR0
	if ((TIFR0 & _BV(TOV0)) && (t < 255))
		m++;
#else
	if ((TIFR & _BV(TOV0)) && (t < 255))
		m++;
#endif

	SREG = oldSREG;
	
	return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}