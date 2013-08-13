
#ifndef DISPLAY_H
#define DISPLAY_H

#define DISPLAY_WIDTH 16
#define DISPLAY_HEIGHT 16


/* Parameters 0-7 are stored bitwise in static m_DSPState */
enum e_DSPParameter 
{
	DSP_DESTRUCTIVE_BITBLT = 0, //should always be disabled on boot.
	DSP_CURRENTLY_REFRESHING = 1, //should always be disabled on boot.
	DSP_DOUBLE_BUFFER = 2,
        DSP_VSYNC = 3,
	DSP_POWERED = 7,
	DSP_REFRESH_HZ = 8,
	DSP_STATE_BITS = 9,
};

int DSP_Init(void);

char DSP_Power(const char state);

void DSP_SwapBuffers(void);

void (*DSP_Refresh)(void); //pointer to function so we can swap drivers on the fly

void DSP_PutPixel(const char x, const char y, char state);

char DSP_GetPixel(const char x, const char y);

char DSP_GetPixelMem(const char * const src,
			const unsigned char srcWidth,
			const unsigned char srcHeight,
			const unsigned char srcX,
			const unsigned char srcY);

char DSP_GetPixelMemF(const char * const src,
			const unsigned char srcWidth,
			const unsigned char srcHeight,
			const unsigned char srcX,
			const unsigned char srcY);

/* This form of BitBLT won't allow partial copies of src from arbitrary x/y, nor 
will it allow blitting to memory that is not the framebuffer. Maybe implement 
another function for these duties, but for now I don't see them as necessary. */
char DSP_BitBLT(const char * const src, 
		const unsigned char srcWidth, 
		const unsigned char srcHeight,
		const char dstX,
		const char dstY);

char DSP_BitBLTF(const char * const src, 
		const unsigned char srcWidth, 
		const unsigned char srcHeight,
		const char dstX,
		const char dstY);



void DSP_Clear(const char state);

//well teensy compiler is shitting the bed about enums... time for sleep
char DSP_SetConfig(enum e_DSPParameter parameter, const unsigned char newValue);

char DSP_GetConfig(enum e_DSPParameter parameter);

#ifdef TESTCASE
void DSP_DBG_PrintFrontBufBin(void);
#endif

#endif
