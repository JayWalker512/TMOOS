
#ifndef TESTCASE
#include "../os.h"
#endif

#include "display.h"
#include "../hardware/hardware.h"
#include "../time/time.h"
#include "../sharedlib/binary.h"
#include <math.h>

#ifdef DEBUG
#include "../debug/debug.h"
#include "../avr_common.h"
#endif

/* There are two display models available. Model 0 is (slightly) faster, but 
wastes a fair amount of memory. Model 1 is slower but uses the absolute minimum
required memory. Since pixel data is stored in memory differently for each of these,
they require different code for setting/getting pixels, clearing framebuffer, and
doing bitblit operations. */
#define DISPLAYMODEL_1

#define DEFAULT_REFRESH_RATE 50

#ifndef NO_DOUBLE_BUFFER_SAVE_MEMORY
#define NUM_BUFFERS 2
#endif

#ifdef NO_DOUBLE_BUFFER_SAVE_MEMORY
#define NUM_BUFFERS 1
#endif

//hardcoded stuff for current display
#ifdef TENXTEN
#define FRAMEBUFFER_ROWS 10
#define DISPLAY_COLUMNS 10
#define DISPLAY_ROWS 10
#endif

#ifdef FIVEXFIVE
#define FRAMEBUFFER_ROWS 5
#define DISPLAY_COLUMNS 5
#define DISPLAY_ROWS 5
#endif

#ifdef SIXXFIVE
#define FRAMEBUFFER_ROWS 5
#define DISPLAY_COLUMNS 6
#define DISPLAY_ROWS 5
#endif

static void DSP_ConfigureDriver(const char refreshRate);

static void DSP_RefreshDriver0(void);
static void DSP_RefreshDriver1(void);

char DSP_GetPixelMem(const char * const src,
			const unsigned char srcWidth,
			const unsigned char srcHeight,
			const unsigned char srcX,
			const unsigned char srcY);

/* TODO Display Model 0 soon to be deprecated... don't think I'll be using it. 
Remove these functions whenever. */
#ifdef DISPLAYMODEL_0
#define FRAMEBUFFER_TYPE unsigned short int
static FRAMEBUFFER_TYPE m_frameBuffer[NUM_BUFFERS][FRAMEBUFFER_ROWS];
#endif

#ifdef DISPLAYMODEL_1
#define FRAMEBUFFER_TYPE unsigned char
#define DISPLAY_NUMPIXELS DISPLAY_ROWS * DISPLAY_COLUMNS
#define FRAMEBUFFER_SIZE_BYTES (DISPLAY_NUMPIXELS) / (sizeof(FRAMEBUFFER_TYPE) * 8) + 1
#define FRAMEBUFFER_TYPE_BITS (sizeof(FRAMEBUFFER_TYPE) * 8)
static FRAMEBUFFER_TYPE m_frameBuffer[NUM_BUFFERS][FRAMEBUFFER_SIZE_BYTES];
#endif

static FRAMEBUFFER_TYPE *m_backBuffer, *m_frontBuffer;

static unsigned int m_scanlineDelayUs;
static unsigned char m_DSPState;

//shouldnt initialize here, but doing it anyway for now
static char m_anodePins[DISPLAY_ROWS * 2] = {	'C', 6,
						'D', 3,
						'D', 2,
						'D', 1,
						'D', 0 };
static char m_cathodePins[DISPLAY_COLUMNS * 2] = {	'B', 7,
							'B', 3,
							'B', 2,
							'B', 1,
							'B', 0,
							'F', 0 };

int 
DSP_Init(void)
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
	
	DSP_ConfigureDriver(DEFAULT_REFRESH_RATE);
	DSP_SetConfig(DSP_VSYNC, 1);
	DSP_SetConfig(DSP_DESTRUCTIVE_BITBLT, 1);
	
	/* FIXME no double buffer is broken right now... */
	DSP_SetConfig(DSP_DOUBLE_BUFFER, 1);
	
	#ifdef DEBUG
	_delay_us(15000);
	print("Framebuffer size (BITS): ");
	//phex16((DISPLAY_ROWS * DISPLAY_COLUMNS) / sizeof(FRAMEBUFFER_TYPE));
	phex16(sizeof(m_frameBuffer[1]) * 8);
	print("\n");
	_delay_us(15000);
	print("Framebuffer type size (BITS): ");
	//phex16((DISPLAY_ROWS * DISPLAY_COLUMNS) / sizeof(FRAMEBUFFER_TYPE));
	phex16(sizeof(FRAMEBUFFER_TYPE) * 8);
	print("\n");
	_delay_us(15000);
	#endif
	
	return 1;
}

static void
DSP_ConfigureDriver(const char refreshRate)
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

static void 
DSP_RefreshDriver0(void)
{
	/* TODO 
	The display gets ever so slightly brighter or dimmer when the CPU is
	clocked up or down. May need to profile how much time is spent calling
	OS_Update, and subtract that from the delay for continuing to the next 
	row. Or maybe add a delay. This needs more research, because we need to
	keep refresh rate consisten and accurate with settings. */

	if (GetBit(&m_DSPState, DSP_CURRENTLY_REFRESHING))
		return;
	
	SetBit(&m_DSPState, DSP_CURRENTLY_REFRESHING);
	
	#ifdef DEBUG 
	/*print("Refreshing...");
	phex16(TME_GetAccurateMicros());
	_delay_us(1000);
	print("\n");
	phex16(TME_GetAccurateMicros());*/
	#endif
	
	unsigned long endTime = TME_GetAccurateMicros();
	char curRow = 0;
	
	while(curRow < DISPLAY_ROWS)
	{	
		if (TME_GetAccurateMicros() >= endTime)
		{
			endTime = TME_GetAccurateMicros() + m_scanlineDelayUs;
			
			//set previous row anode low
			if (curRow == 0)
				HRD_SetPin(m_anodePins[(DISPLAY_ROWS - 1) * 2], m_anodePins[(DISPLAY_ROWS - 1) * 2 + 1], 0);
			else
				HRD_SetPin(m_anodePins[(curRow - 1) * 2], m_anodePins[(curRow - 1) * 2 + 1], 0);
			
			//setting active cathodes low. Do this first to avoid ghosting
			for (int x = 0; x < DISPLAY_COLUMNS; x++)
			{				
				if (DSP_GetPixel(x, curRow)) //can inline this, using function now for clarity
				{	
					HRD_SetPin(m_cathodePins[x * 2], m_cathodePins[x * 2 + 1], 0);
				}
				else
				{
					HRD_SetPin(m_cathodePins[x * 2], m_cathodePins[x * 2 + 1], 1);
				}
				#ifndef TESTCASE
				OS_Update();
				#endif
			}
			
			//set current row anode high
			HRD_SetPin(m_anodePins[curRow * 2], m_anodePins[curRow * 2 + 1], 1);
			
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
}

static void 
DSP_RefreshDriver1(void)
{
	/* Implement an asynchronious updating display driver here. Rather than 
	idling in the refresh loop until it's time to light the next row, keep local
	time information and return when it's not time to update the next row yet.
	May be slightly inconsistent row timing, but that's why it's ASYNC! Also
	games aren't locked at displays refresh rate. */
}

#ifdef DISPLAYMODEL_0
void 
DSP_PutPixel(const char x, const char y, const char state)
{
	if (x >= 0 && x < DISPLAY_COLUMNS)
	{
		if (y >= 0 && y < DISPLAY_ROWS)
		{
			if (state)
				SetBit((m_backBuffer+y), sizeof(FRAMEBUFFER_TYPE) * 8 - (x+1));
			else
				ClearBit((m_backBuffer+y), sizeof(FRAMEBUFFER_TYPE) * 8 - (x+1));
		}
	}
	
	//old code for reference
	/*if (state)
		*(m_backBuffer+y) |= (1 << (sizeof(FRAMEBUFFER_TYPE) * 8 - (x+1) )); //works as expected now
	else
		*(m_backBuffer+y) &= ~(1 << (sizeof(FRAMEBUFFER_TYPE) * 8 - (x+1) ));*/
}

char 
DSP_GetPixel(const char x, const char y)
{
	//may be tricky behavior... should we check back or front buffer?
	//checking against the FRONT buffer for now, seems most intuitive
	if (x >= 0 && x < DISPLAY_COLUMNS)
	{
		if (y >= 0 && y < DISPLAY_ROWS)
		{
			//return !!( *(m_frontBuffer+y) & (1 << (sizeof(FRAMEBUFFER_TYPE) * 8 - (x+1) )) );
			return GetBit((m_frontBuffer+y), (sizeof(FRAMEBUFFER_TYPE) * 8 - (x - 1)) );	
		}
	}
	
	return 0;
}

char 
DSP_BitBLT(const char * const src, 
		const unsigned char srcWidth, 
		const unsigned char srcHeight,
		const int dstX,
		const int dstY)
{

	//maybe write so that 1 is only returned if anything was actually copied to frameBuffer
	return 1;
}

void 
DSP_Clear(const char state)
{
	for (char y = 0; y < DISPLAY_ROWS; y++)
	{
		if (!state)
			*(m_backBuffer+y) = 0;
		else
			*(m_backBuffer+y) = ~((*m_backBuffer+y) & 0);
	}
}
#endif

#ifdef DISPLAYMODEL_1
/* DISPLAYMODEL_1 uses memory more efficiently by storing pixel data in a linear array of bits.
This is slightly slower, but memory is a scarcer resource in this environment. */


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
		
			//return !!( *(m_frontBuffer+offset) & (1 << FRAMEBUFFER_TYPE_BITS - (accessedBit % FRAMEBUFFER_TYPE_BITS) - 1) );
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
	
	//return !!( *(src+offset) & (1 << srcBits - (accessedBit % srcBits) - 1) );
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
	
	#ifdef DEBUG 
		//phex1(DSP_GetPixelMem(&src, srcWidth, srcHeight, iX, iY));
		/*_delay_us(1000);
		print("\nAddress from BitBLT: ");
		_delay_us(1000);
		phex16(src);
		print("\n");
		_delay_us(1000);*/
	#endif
	
	//maybe write so that 1 is only returned if anything was actually copied to frameBuffer
	return 1;
}

void 
DSP_Clear(const char state)
{
	for (char y = 0; y < FRAMEBUFFER_SIZE_BYTES; y++)
	{
		if (!state)
			*(m_backBuffer+y) = 0;
		else
			*(m_backBuffer+y) = ~((*m_backBuffer+y) & 0);
	}
}
#endif

char 
DSP_SetConfig(enum e_DSPParameter parameter, const char newValue)
{
	switch ( parameter )
	{
		case DSP_REFRESH_HZ:
			DSP_ConfigureDriver(newValue);
			break;
		case DSP_VSYNC:
			if (newValue == 1)
				DSP_Refresh = &DSP_RefreshDriver0;
			else if (newValue == 0)
				DSP_Refresh = &DSP_RefreshDriver1;	
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
		default:
			return 0;
	}
	
	return 1;
}

char 
DSP_GetConfig(enum e_DSPParameter parameter)
{
	return 0;
}

#ifdef DEBUG

#ifdef DISPLAYMODEL_0
void 
DSP_DBG_PrintFrontBufBin(void)
{
	for(char y = 0; y < DISPLAY_ROWS; y++)
	{
		printBin16Bit(*(m_frontBuffer+y));
		print("\n");
		_delay_us(10000);
	}
}
#endif 

#ifdef DISPLAYMODEL_1
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

#endif
