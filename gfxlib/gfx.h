#ifndef GFXLIB_H
#define GFXLIB_H

enum e_GFXState {
	GFX_FILLED_RECT = 8,
	GFX_BORDERED_RECT = 9,
	GFX_BLANK_DESTRUCTIVE_RECT = 10,
};

void GFX_Init(void);

void (*GFX_Clear)(char);
void (*GFX_SwapBuffers)(void);

void GFX_Enable(enum e_GFXState parameter);
void GFX_Disable(enum e_GFXState parameter);

void GFX_DrawRect(char x, char y, 
		  const char width, 
		  const char height,
		  enum e_GFXState parameter);
		  
void GFX_DrawLine(const char x1, const char y1,
		  const char x2, const char y2);
		  
void (*GFX_PutPixel)(const char x, const char y, const char state);
void (*GFX_GetPixel)(const char x, const char y);

unsigned char (*GFX_BitBLT)(const char * const src, 
			const unsigned char srcWidth, 
			const unsigned char srcHeight,
			const char dstX,
			const char dstY);

#endif
