#ifndef GAMES_H
#define GAMES_H

#include "../gfx/gfx.h"
#include "../common/glyphs.h"

#include "../sound/sound.h"

enum e_GLIBenums {
	GLIB_WHEEL,
	GLIB_PB1,
	GLIB_PB2
};

char GLIB_Init(void);

void (*GLIB_Idle)(void);

unsigned char GLIB_GetInput(enum e_GLIBenums device);

unsigned long GLIB_GetGameMillis(void);

void (*GLIB_Beep)(unsigned int Hz, unsigned long ms);


/* Everything below will need refactored... not sure how im handling multiple 
games yet. Play around with it. */


void GameMain(void);

void GameInputTest(void);

void GameSlidingWheelChar(void);

void GameScrollText(void);

#endif
