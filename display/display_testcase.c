#define TESTCASE

#include "display.h"
#include "../time/time.h"
#include "../hardware/hardware.h"
#include "../avr_common.h"
#include "../debug/usb_debug_only.h"
#include "../debug/print.h"
#include "../sharedlib/binary.h"

#define CPU_SET_FREQ(n) (CLKPR = 0x80, CLKPR = (n))

/* 3x3 glyph looks like this when drawn:
010
101
111 */
const char glyph[] = { B01010111,
		       B10000000 }; 
		       
/* 4x3 right-pointing arrow 
0010
1111
0010*/	      
const char arrow[] = { B00101111,
		       B00100000 };
		       
/* 3x5 'A' character
010
101
111
101
101 */
const char AChar[] = { B01010111,
		       B11011010 };		 		       
		 
void Idle(void);

int 
main()
{
	CPU_SET_FREQ(0x00);
	TME_Init(); 
	usb_init();
	_delay_us(1000000);
	
	print("Starting test...\n");
	_delay_us(1000);
	print("\n");
	
	print("Size of framebuffer line is: ");
	phex16(sizeof(unsigned char) * 8);
	print("\n");

	HRD_SetPinDigital(11, 1);
	_delay_us(200000);
	HRD_SetPinDigital(11, 0);
	_delay_us(200000);

	DSP_Init();
	DSP_SetConfig(DSP_REFRESH_HZ, 50);
	/*DSP_PutPixel(1, 0, 1);
	DSP_PutPixel(3, 0, 1);
	DSP_PutPixel(2, 2, 1);
	DSP_PutPixel(0, 3, 1);
	DSP_PutPixel(4, 3, 1);
	DSP_PutPixel(1, 4, 1);
	DSP_PutPixel(2, 4, 1);
	DSP_PutPixel(3, 4, 1);*/
	DSP_BitBLT(&glyph, 3, 3, 0, 1);
	print("\n");
	//DSP_SwapBuffers();
	//DSP_Clear(1);
	
	print("Printing frontBuffer contents\n");
	_delay_us(100000);
	DSP_SwapBuffers();
	DSP_DBG_PrintFrontBufBin();
	_delay_us(100000);
	
	//phex16(DSP_GetPixel(1,1));
	/*if (DSP_GetPixelMem(&glyph, 3, 3, 1, 0))
		print("Glyph Pixel 1,1 IS set\n");
	else
		print("Glyph Pixel 1,1 IS NOT set\n");*/
	_delay_us(10000);
	
	//phex1(DSP_GetPixelMem(&glyph, 3, 3, 1, 0));
	_delay_us(1000);
	//print("\nAddress from Test: ");
	_delay_us(1000);
	//phex16(&glyph);
	_delay_us(1000);
	print("\n");	
	
	/*if (DSP_GetPixel(1,4))
		print("Pixel 1,4 IS set\n");
	else
		print("Pixel 1,4 IS NOT set\n");
		
	if (DSP_GetPixel(3,1))
		print("Pixel 3,1 IS set\n");
	else
		print("Pixel 3,1 IS NOT set\n");*/
		
	if (DSP_GetPixel(2,7))
		print("Pixel 2,7 IS set\n");
	else
		print("Pixel 2,7 IS NOT set\n");
	
	
	//TME_ScaleCpu(CPU_16MHZ);
	DSP_Clear(0);
	DSP_BitBLT(&AChar, 3, 5, 0, 0);
	
	//FIXME: destructive bitblt not working?
	DSP_SetConfig(DSP_DESTRUCTIVE_BITBLT, 1);
	DSP_SwapBuffers();
	#define ANIM_DELAY 250
	unsigned long endTime = TME_GetAccurateMillis() + ANIM_DELAY;
	char x = 0; 
	/* XXX XXX XXX WHAT THE FUCK? char defaults to UNSIGNED? 
		need to used signed to make things work... that's not okay compiler.
		edit: was a result of "CFLAGS += -funsigned-char" in makefile. */
		
	while (1)
	{
		/* TODO There needs to be some kind of forced-wait at the end of
		a refresh so the bottom row doesn't get turned off prematurely */
		//_delay_us(3000);
		if (endTime <= TME_GetAccurateMillis())
		{
			x++;
			if (x >= DISPLAY_WIDTH)
				x = 0;
		
			endTime = TME_GetAccurateMillis() + ANIM_DELAY;
			
			DSP_Clear(0);
			DSP_BitBLT(&AChar, 3, 5, x, 0);
			
			if (x > (DISPLAY_WIDTH - 3)) //draw it looping around
				DSP_BitBLT(&AChar, 3, 5, 0 - (DISPLAY_WIDTH - x), 0);
				
			DSP_SwapBuffers(); 
		}
		
		
		Idle();
	}	
	return 0;
}

void 
Idle(void)
{
	DSP_Refresh();
	
	/*print("Printing frontBuffer contents\n");
	_delay_us(100);
	DSP_DBG_PrintFrontBufBin();
	print("\n");*/
	//_delay_us(100);
}
