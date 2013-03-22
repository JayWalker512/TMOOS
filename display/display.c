
#ifndef TESTCASE
#include "../os.h"
#else
#include "../avr_common.h"
#include "../debug/print.h"
#endif

#ifdef DEBUG
#include "../debug/debug.h"
#include "../avr_common.h"
#include "../console/console.h"
#endif

#include "display.h"
#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../common/binary.h"
#include <math.h>

#define DEFAULT_REFRESH_RATE 60

#ifndef NO_DOUBLE_BUFFER_SAVE_MEMORY
#define NUM_BUFFERS 2
#endif

#ifdef NO_DOUBLE_BUFFER_SAVE_MEMORY
#define NUM_BUFFERS 1
#endif

#define FRAMEBUFFER_ROWS 9 //TODO change this to FRAMEBUFFER_SIZE_BYTES and test
#define DISPLAY_COLUMNS 8
#define DISPLAY_ROWS 8


static void DSP_ConfigureDriver(const unsigned char refreshRate);

static void DSP_RefreshDriver0(void); //sync driver
static void DSP_RefreshDriver1(void); //async driver

char DSP_GetPixelMem(const char * const src,
			const unsigned char srcWidth,
			const unsigned char srcHeight,
			const unsigned char srcX,
			const unsigned char srcY);


#define FRAMEBUFFER_TYPE unsigned char
#define DISPLAY_NUMPIXELS DISPLAY_ROWS * DISPLAY_COLUMNS
#define FRAMEBUFFER_SIZE_BYTES ((DISPLAY_NUMPIXELS) / (sizeof(FRAMEBUFFER_TYPE) * 8)) + 1
#define FRAMEBUFFER_TYPE_BITS (sizeof(FRAMEBUFFER_TYPE) * 8)
static FRAMEBUFFER_TYPE m_frameBuffer[NUM_BUFFERS][FRAMEBUFFER_SIZE_BYTES];


static FRAMEBUFFER_TYPE *m_backBuffer, *m_frontBuffer;

static unsigned long m_scanlineDelayUs;

//relevant to DSP_RefreshDriver1()
static unsigned long m_nextScanlineTime;
static char m_curRow;

static unsigned char m_DSPState;

//shouldnt initialize here, but doing it anyway for now
//static unsigned char m_anodePins[DISPLAY_ROWS] = { 16, 20, 8, 17, 2, 7, 4, 9 };
//static unsigned char m_cathodePins[DISPLAY_COLUMNS] = { 3, 14, 15, 21, 13, 24, 1, 0 };

#define DSP_POWER_PIN 12

#define ANODE_DATA 1
#define ANODE_OE 2
#define ANODE_LATCH 3
#define ANODE_CLOCK 4
#define ANODE_CLEAR 5

#define CATHODE_DATA 16
#define CATHODE_OE 15
#define CATHODE_LATCH 17
#define CATHODE_CLOCK 14
#define CATHODE_CLEAR 13

int 
DSP_Init(void) //TODO init settings (refresh rate, double buffer) should be passed here from OS
{
	for(char b = 0; b < 2; b++)
	{
		for(char r = 0; r < FRAMEBUFFER_ROWS; r++)
		{
			m_frameBuffer[b][r] = 0x00;
		}
	}

	#ifndef NO_DOUBLE_BUFFER_SAVE_MEMORY
	m_frontBuffer = &m_frameBuffer[0];
	m_backBuffer = &m_frameBuffer[1];
	#endif
	
	#ifdef NO_DOUBLE_BUFFER_SAVE_MEMORY
	m_frontBuffer = m_backBuffer = &m_frameBuffer;
	#endif

	m_curRow = 0; //for driver1
	m_nextScanlineTime = 0;
	
	DSP_ConfigureDriver(DEFAULT_REFRESH_RATE);
	DSP_SetConfig(DSP_VSYNC, 0);
	DSP_SetConfig(DSP_DESTRUCTIVE_BITBLT, 1);
	
	/* FIXME disabled double buffer is broken right now... */
	DSP_SetConfig(DSP_DOUBLE_BUFFER, 1);
	
	DSP_Power(1);
	
	//should these go in DSP_Power()?
	HRD_SetPinDigital(ANODE_CLEAR, 1);
	HRD_SetPinDigital(CATHODE_CLEAR, 1);
	HRD_SetPinDigital(CATHODE_OE, 0);
	
	#ifdef DEBUG
	CON_SendString(PSTR("Framebuffer size (BITS): "));
	printInt(sizeof(m_frameBuffer[1]) * 8, VAR_UNSIGNED);
	CON_SendString(PSTR("\r\n"));
	CON_SendString(PSTR("Framebuffer size (Bytes): "));
	printInt(sizeof(m_frameBuffer[1]), VAR_UNSIGNED);
	CON_SendString(PSTR("\r\n"));
	#endif
	
	return 1;
}

char DSP_Power(const char state)
{
	/*TODO finish this. Refresh cycle needs to start from the beginning
	 when display is powered on, so that the image isn't corrupted by 
	 shift registers starting to get data part way through a refresh. */
	if (state == 1)
	{
		HRD_SetPinDigital(DSP_POWER_PIN, 1);
		SetBit(&m_DSPState, DSP_POWERED);
		
		ClearBit(&m_DSPState, DSP_CURRENTLY_REFRESHING);
		m_curRow = 0;
		m_nextScanlineTime = 0;
		return 1;
	}
	else if (state == 0)
	{
		HRD_SetPinDigital(DSP_POWER_PIN, 0);
		ClearBit(&m_DSPState, DSP_POWERED);
		
		ClearBit(&m_DSPState, DSP_CURRENTLY_REFRESHING);
		return 0;
	}
	return -1;
}

static void
DSP_ConfigureDriver(const unsigned char refreshRate)
{
	m_scanlineDelayUs = 1000000 / (DISPLAY_ROWS * refreshRate);
}

void 
DSP_SwapBuffers(void)
{
	if (0 == GetBit(&m_DSPState, DSP_DOUBLE_BUFFER))
		return;
	
	#ifndef NO_DOUBLE_BUFFER_SAVE_MEMORY
	if (m_frontBuffer == &m_frameBuffer[0])
	{
		m_frontBuffer = &m_frameBuffer[1];
		m_backBuffer = &m_frameBuffer[0];
	}
	else
	{
		m_frontBuffer = &m_frameBuffer[0];
		m_backBuffer = &m_frameBuffer[1];
	}
	#endif
}

//sync driver
static void 
DSP_RefreshDriver0(void)
{
#if 0
	/* TODO 
	The display gets ever so slightly brighter or dimmer when the CPU is
	clocked up or down. May need to profile how much time is spent calling
	OS_Update, and subtract that from the delay for continuing to the next 
	row. Or maybe add a delay. This needs more research, because we need to
	keep refresh rate consisten and accurate with settings. */

	if (GetBit(&m_DSPState, DSP_CURRENTLY_REFRESHING))
		return;
	
	SetBit(&m_DSPState, DSP_CURRENTLY_REFRESHING);
	
	unsigned long endTime = TME_GetAccurateMicros();
	char curRow = 0;
	
	while(curRow < DISPLAY_ROWS)
	{	
		if (TME_GetAccurateMicros() >= endTime)
		{
			endTime = TME_GetAccurateMicros() + m_scanlineDelayUs;
			
			//set previous row anode low
			if (curRow == 0)
				HRD_SetPinDigital(m_anodePins[DISPLAY_ROWS - 1], 0);
			else
				HRD_SetPinDigital(m_anodePins[curRow - 1], 0);
			
			//setting active cathodes low. Do this first to avoid ghosting
			for (char x = 0; x < DISPLAY_COLUMNS; x++)
			{				
				if (DSP_GetPixel(x, curRow)) //can inline this, using function now for clarity
				{	
					HRD_SetPinDigital(m_cathodePins[x], 0);
				}
				else
				{
					HRD_SetPinDigital(m_cathodePins[x], 1);
				}
				#ifndef TESTCASE
				OS_Update();
				#endif
			}
			
			//set current row anode high
			HRD_SetPinDigital(m_anodePins[curRow], 1);
			
			curRow++; 
		}
		#ifndef TESTCASE
		OS_Update();
		#endif
	}
	
	/* Maybe rather than this, keep a member variable tracking when the display was last updated, 
	so we don't spend too long or short a time on last row. */
	endTime = TME_GetAccurateMicros() + m_scanlineDelayUs;
	
	#ifdef TESTCASE
	while (TME_GetAccurateMicros() <= endTime);
	#endif
	
	#ifndef TESTCASE
	while (TME_GetAccurateMicros() <= endTime) 
		OS_Update();
	#endif
	
	ClearBit(&m_DSPState, DSP_CURRENTLY_REFRESHING);
#endif
}

/* driver for old pinout. commented for reference atm
//async driver
static void 
DSP_RefreshDriver1(void)
{
	if (TME_GetAccurateMicros() < m_nextScanlineTime)
		return;
			
	//set previous row anode low
	if (m_curRow == 0)
		HRD_SetPinDigital(m_anodePins[DISPLAY_ROWS - 1], 0);
	else
		HRD_SetPinDigital(m_anodePins[m_curRow - 1], 0);

	//setting active cathodes low. Do this first to avoid ghosting
	for (char x = 0; x < DISPLAY_COLUMNS; x++)
	{				
		if (DSP_GetPixel(x, m_curRow)) //can inline this, using function now for clarity
		{	
			HRD_SetPinDigital(m_cathodePins[x], 0);
		}
		else
		{
			HRD_SetPinDigital(m_cathodePins[x], 1);
		}
	}

	//set current row anode high
	HRD_SetPinDigital(m_anodePins[m_curRow], 1);
	
	m_curRow++; 
	if (m_curRow >= DISPLAY_ROWS)
		m_curRow = 0;
	m_nextScanlineTime = TME_GetAccurateMicros() + m_scanlineDelayUs;
}
*/

static void 
DSP_RefreshDriver1(void)
{
	if (TME_GetAccurateMicros() < m_nextScanlineTime)
		return;
	
	//firstly, disable anode output for now.
	HRD_SetPinDigital(ANODE_OE, 1);
	
	if (m_curRow == 0)
	{
		HRD_SetPinDigital(ANODE_DATA, 1);
		HRD_CycleClockPin(ANODE_CLOCK);
		HRD_CycleClockPin(ANODE_LATCH);
		HRD_SetPinDigital(ANODE_DATA, 0);
	}
	else
	{
		//note above, we already set ANODE_DATA to 0 so we just shift it in
		HRD_CycleClockPin(ANODE_CLOCK);
		HRD_CycleClockPin(ANODE_LATCH);
	}

	for (char x = DISPLAY_COLUMNS; x > 0; x--)
	{				
		if (DSP_GetPixel(x - 1, m_curRow)) //can inline this, using function now for clarity
		{		
			HRD_SetPinDigital(CATHODE_DATA, 0);
			HRD_CycleClockPin(CATHODE_CLOCK);
		}
		else
		{
			HRD_SetPinDigital(CATHODE_DATA, 1);
			HRD_CycleClockPin(CATHODE_CLOCK);
		}
	}
	HRD_CycleClockPin(CATHODE_LATCH);
	HRD_SetPinDigital(ANODE_OE, 0);
	
	m_curRow++; 
	if (m_curRow >= DISPLAY_ROWS)
		m_curRow = 0;
	m_nextScanlineTime = TME_GetAccurateMicros() + m_scanlineDelayUs;
}

void 
DSP_PutPixel(const char x, const char y, const char state)
{
	//TODO just return if not in bounds, saves indenting. looks nicer.
	if (x >= 0 && x < DISPLAY_COLUMNS)
	{
		if (y >= 0 && y < DISPLAY_ROWS)
		{
			unsigned char accessedBit = (y * DISPLAY_COLUMNS) + x;
			unsigned char offset = floor(accessedBit / FRAMEBUFFER_TYPE_BITS);
	
			if (state)
				SetBit( (m_backBuffer+offset), FRAMEBUFFER_TYPE_BITS - (accessedBit % FRAMEBUFFER_TYPE_BITS) - 1);
			else
				ClearBit( (m_backBuffer+offset), FRAMEBUFFER_TYPE_BITS - (accessedBit % FRAMEBUFFER_TYPE_BITS) - 1);
		}
	}
}

char 
DSP_GetPixel(const char x, const char y)
{
	if (x >= 0 && x < DISPLAY_COLUMNS)
	{
		if (y >= 0 && y < DISPLAY_ROWS)
		{
			unsigned char accessedBit = (y * DISPLAY_COLUMNS) + x;
			unsigned char offset = floor(accessedBit / FRAMEBUFFER_TYPE_BITS);
		
			return GetBit((m_frontBuffer+offset), FRAMEBUFFER_TYPE_BITS - (accessedBit % FRAMEBUFFER_TYPE_BITS) - 1);
		}
	}
	return 0;
}

//maybe inline?
char 
DSP_GetPixelMem(const char * const src,
		const unsigned char srcWidth,
		const unsigned char srcHeight,
		const unsigned char srcX,
		const unsigned char srcY)
{
	/* Don't need any special bounds checking because this function will only
	be called from inside this file, where we hope it will be properly used. */

	unsigned char srcBits = 8;
	unsigned char accessedBit = (srcY * srcWidth) + srcX;
	unsigned char offset = floor(accessedBit / srcBits);
	
	return GetBit((src+offset), srcBits - (accessedBit % srcBits) - 1);
}

char 
DSP_BitBLT(const char * const src, 
	const unsigned char srcWidth, 
	const unsigned char srcHeight,
	const char dstX,
	const char dstY)
{
	/* TODO Add some checks so that if drawing destination is outside display
	area (dst < 0), only area inside is looped through. Also check that if the display 
	destination is > DISPLAY_WIDTH or DISPLAY_HEIGHT, we just return because
	nothing will be drawn anyway. */
	
	if (!!((m_DSPState) & (1 << DSP_DESTRUCTIVE_BITBLT) ))
	{
		for (char iY = 0; iY < srcHeight; iY++)
		{
			for (char iX = 0; iX < srcWidth; iX++)
			{
				DSP_PutPixel(dstX + iX, dstY + iY, 
					DSP_GetPixelMem(src, srcWidth, srcHeight, iX, iY)
					);
			}
		}
	}
	else
	{	
		for (char iY = 0; iY < srcHeight; iY++)
		{
			for (char iX = 0; iX < srcWidth; iX++)
			{
				char pixSet = DSP_GetPixelMem(src, srcWidth, srcHeight, iX, iY);
				
				if (pixSet)
					DSP_PutPixel(dstX + iX, dstY + iY, pixSet);
			}
		}
	}
	
	//maybe write so that 1 is only returned if anything was actually copied to frameBuffer
	return 1;
}

void 
DSP_Clear(const char state)
{
	for (char y = 0; y < FRAMEBUFFER_SIZE_BYTES; y++)
	{
		if (state == 0)
			*(m_backBuffer+y) = 0;
		else
			*(m_backBuffer+y) = ~((*m_backBuffer+y) & 0);
	}
}

char 
DSP_SetConfig(enum e_DSPParameter parameter, const unsigned char newValue)
{
	switch ( parameter )
	{
		case DSP_REFRESH_HZ:
			DSP_ConfigureDriver(newValue);
			break;
		case DSP_VSYNC:
			if (newValue == 1)
                        {
				DSP_Refresh = &DSP_RefreshDriver0;
                                SetBit(&m_DSPState, DSP_VSYNC);
                        }
			else if (newValue == 0)
                        {
				DSP_Refresh = &DSP_RefreshDriver1;	
                                ClearBit(&m_DSPState, DSP_VSYNC);
                        }
			break;
		case DSP_DESTRUCTIVE_BITBLT:
			if (newValue == 0)
				ClearBit(m_DSPState, DSP_DESTRUCTIVE_BITBLT);
			else if (newValue == 1)
				SetBit(&m_DSPState, DSP_DESTRUCTIVE_BITBLT);
			break;
		case DSP_DOUBLE_BUFFER:
			if (newValue == 0)
			{
				ClearBit(m_DSPState, DSP_DOUBLE_BUFFER);
				m_frontBuffer = m_backBuffer = &m_frameBuffer[0];
			}
			else if (newValue == 1)
				SetBit(&m_DSPState, DSP_DOUBLE_BUFFER);
			break;
		case DSP_STATE_BITS:
			m_DSPState = newValue;
			ClearBit(&m_DSPState, DSP_DESTRUCTIVE_BITBLT);
			ClearBit(&m_DSPState, DSP_CURRENTLY_REFRESHING);
			break;
		default:
			return 0;
	}
	
	return 1;
}

char 
DSP_GetConfig(enum e_DSPParameter parameter)
{
	switch (parameter)
	{
		case DSP_REFRESH_HZ:
			return (m_scanlineDelayUs * 1000000) / DISPLAY_ROWS;
			break;
		case DSP_VSYNC:
                        //this is set in m_DSPState as well, but this is literal check
			if (DSP_Refresh == &DSP_RefreshDriver0)
				return 1;
			else
				return 0;
			break;
		case DSP_DESTRUCTIVE_BITBLT:
			return GetBit(&m_DSPState, DSP_DESTRUCTIVE_BITBLT);
			break;
		case DSP_DOUBLE_BUFFER:
			return GetBit(&m_DSPState, DSP_DOUBLE_BUFFER);
			break;
		case DSP_STATE_BITS:
			return m_DSPState;
		default:
			return -1;
			break;
	}
	return -1;
}

#ifdef TESTCASE
void
DSP_DBG_PrintFrontBufBin(void)
{
	for(char y = 0; y < FRAMEBUFFER_SIZE_BYTES; y++)
	{
		printBin8Bit(*(m_frontBuffer+y));
		print("\n");
		_delay_us(10000);
	}
}
#endif
