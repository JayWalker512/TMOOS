
#ifndef DISPLAY_H
#define DISPLAY_H

#define SIXXFIVE

#ifdef TENXTEN
#define DISPLAY_WIDTH 10
#define DISPLAY_HEIGHT 10
#endif

#ifdef FIVEXFIVE
#define DISPLAY_WIDTH 5
#define DISPLAY_HEIGHT 5
#endif

#ifdef SIXXFIVE
#define DISPLAY_WIDTH 6
#define DISPLAY_HEIGHT 5
#endif

/* Parameters 0-7 are stored bitwise in static m_DSPState */
enum e_DSPParameter {
	DSP_DESTRUCTIVE_BITBLT = 0,
	DSP_CURRENTLY_REFRESHING = 1,
	DSP_DOUBLE_BUFFER = 2,
	DSP_REFRESH_HZ = 9,
	DSP_VSYNC = 10,
};

int DSP_Init(void);

void DSP_SwapBuffers(void);

void (*DSP_Refresh)(void); //pointer to function so we can swap drivers on the fly

void DSP_PutPixel(const char x, const char y, char state);

char DSP_GetPixel(const char x, const char y);

char DSP_GetPixelMem(const char * const src,
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

void DSP_Clear(const char state);

//well teensy compiler is shitting the bed about enums... time for sleep
char DSP_SetConfig(enum e_DSPParameter parameter, const char newValue);

char DSP_GetConfig(enum e_DSPParameter parameter);

#ifdef DEBUG
void DSP_DBG_PrintFrontBufBin(void);
#endif

#endif
