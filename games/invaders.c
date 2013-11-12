//Brace yourself for shitty code, I wrote most of this late one night rushing
//to get some semblance of a game working for a game showcase.
#include "gamelib.h"
#include "gamemath.h"
#include "../common/avr.h"
#include "../common/binary.h"
#include "display/display.h"
#include "input/input.h"
#include "console/console.h"

#include <math.h> //should probably include from gamemath.h
#include <stdlib.h>

#define SHIP_WIDTH 3
#define SHIP_HEIGHT 2
#define MAX_PLAYER_PROJECTILES 2 
#define PLAYER_PROJECTILE_YSPEED 0.020f
#define ALIEN_DESCENT_SPEED 0.005f
#define MAX_ALIENS 2
#define MAX_AMP 3
#define MIN_AMP 1.25f
#define MAX_DIV 8.0f
#define MIN_DIV 5.0f
#define DART_PROBABILITY 0.010f

#define FRAME_STEP_MS 17

enum e_InvadersStates 
{
	SOME_STATE = 0,
	//I guess I wrote this for "scoreboard" state or something?
};

typedef struct Projectile_s 
{
	Vec2Df_t pos;
	float ySpeed;
} Projectile_t;

typedef struct AlienShip_s 
{
	Vec2Df_t pos;
	float dartXSpeed; //for when they *maybe* pick a direction below the midpoint
	float xCenter; //axis sin wave is generated about. Set to initial pos.x
	float amplitude; //height of sin wave
	float wavDiv; //divider for width of sin wave
} AlienShip_t;

Projectile_t g_playerBullets[MAX_PLAYER_PROJECTILES];
AlienShip_t g_alienShips[MAX_ALIENS];

unsigned char g_InvadersState;
char g_shipX;
PROGMEM const char g_shipGlyph = B01011100;

void HandleInvadersInput(void);
void UpdateProjectiles(Projectile_t *projectileList, unsigned char num);
void FirePlayerProjectile(char x, Projectile_t *projectileList, unsigned char num);
void UpdateAliens(AlienShip_t *alienList, unsigned char num);
void SpawnRandAlien(AlienShip_t *alienList, unsigned char num);
void RenderInvaders(void);

char
InitInvaders(void)
{
	g_shipX = 0;
	g_InvadersState = 0;
	
	//set projectiles way away and .y < 0 when they're "inactive"
	unsigned char i;
	for (i = 0; i < MAX_PLAYER_PROJECTILES; i++)
	{
		g_playerBullets[i].pos.x = -50; 
		g_playerBullets[i].pos.y = -1;
	}
	
	for (i = 0; i < MAX_ALIENS; i++)
	{
		g_alienShips[i].pos.x = -50;
		g_alienShips[i].pos.y = -1;
	}
	
	srand(GLIB_GetGameMillis());
	
	return 1;
}

static unsigned long endTime = 0;

char
InvadersLoop(void)
{
	unsigned long now = GLIB_GetGameMillis();
	if (now < endTime)
		return 1;
	else
		endTime = now + FRAME_STEP_MS;
	
	HandleInvadersInput();
	UpdateProjectiles(g_playerBullets, MAX_PLAYER_PROJECTILES);
	UpdateAliens(g_alienShips, MAX_ALIENS);
	RenderInvaders();
	
	return 1;
}

void
HandleInvadersInput(void)
{
	g_shipX = GLIB_GetWheelRegion(13);
	unsigned char events = INP_PollEvents();
	if (GetBitUInt8(&events, INPUT_PB0_DOWN))
		FirePlayerProjectile(g_shipX + 1, g_playerBullets, MAX_PLAYER_PROJECTILES);
	
	if (GetBitUInt8(&events, INPUT_PB1_DOWN))
		SpawnRandAlien(g_alienShips, MAX_ALIENS);
}

void 
UpdateProjectiles(Projectile_t *projectileList, unsigned char num)
{
	unsigned char i;
	for (i = 0; i < num; i++)
	{
		Projectile_t *cProj = (projectileList+i);
		cProj->pos.y -= cProj->ySpeed * FRAME_STEP_MS;	
		
		//if it goes off screen, becomes "inactive"
		if (cProj->pos.y < 0)
		{
			cProj->ySpeed = 0;
			cProj->pos.x = -50;
		}	
	}
}

void 
FirePlayerProjectile(char x, Projectile_t *projectileList, unsigned char num)
{
	unsigned char i = 0;
	for (i = 0; i < num; i++)
	{
		Projectile_t *cProj = (projectileList+i);
		if (cProj->pos.y < 0) //we can use it!
		{
			cProj->pos.y = DISPLAY_HEIGHT - SHIP_HEIGHT;
			cProj->pos.x = x;
			cProj->ySpeed = PLAYER_PROJECTILE_YSPEED;
			return; //found a bullet and fired. Leave now!
		}
	}
}

void 
UpdateAliens(AlienShip_t *alienList, unsigned char num)
{
	unsigned char i;
	for (i = 0; i < num; i++)
	{
		AlienShip_t *cAlien = (alienList+i);
		
		//sin wave code in here
		if (cAlien->pos.y > 0 && cAlien->pos.y < DISPLAY_HEIGHT &&
			cAlien->dartXSpeed == 0)
		{
			//move straight down
			cAlien->pos.y += ALIEN_DESCENT_SPEED * FRAME_STEP_MS;
			cAlien->pos.x = cAlien->xCenter + 
				(cAlien->amplitude * sin(cAlien->pos.y + 
				(cAlien->pos.y / cAlien->wavDiv)));
		}	
		//end sin wave code
		
		//code for probabilistically deciding to dart in one direction
		if (RandFloat(0.0, 1.0) < DART_PROBABILITY && 
			cAlien->dartXSpeed == 0 &&
			cAlien->pos.y >= (DISPLAY_HEIGHT / 3))
		{
			cAlien->dartXSpeed = RandFloat(-ALIEN_DESCENT_SPEED * 1.25, 
				ALIEN_DESCENT_SPEED * 1.25);
		}
		
		//code for moving in the darted direction.
		//not sure what the FP error is like on here, so I provide a 
		//small error range.
		if (cAlien->dartXSpeed > 0.001f || 
			cAlien->dartXSpeed < -0.001f ||
			cAlien->dartXSpeed != 0.0f) 
		{
			cAlien->pos.x += cAlien->dartXSpeed * FRAME_STEP_MS;
			cAlien->pos.y += ALIEN_DESCENT_SPEED * FRAME_STEP_MS;
		}
		
		if (cAlien->pos.y < 0) 
		{
			cAlien->pos.x = -50;
		}
	}
}

void 
SpawnRandAlien(AlienShip_t *alienList, unsigned char num)
{
	unsigned char i = 0;
	for (i = 0; i < num; i++)
	{
		AlienShip_t *cAlien = (alienList+i);
		if (cAlien->pos.y < 0 || cAlien->pos.y > DISPLAY_HEIGHT) //we can use it!
		{
			cAlien->pos.y = 0.05f;
			cAlien->pos.x = RandFloat(2,13);
			cAlien->xCenter = cAlien->pos.x;
			cAlien->amplitude = RandFloat(MIN_AMP, MAX_AMP);
			cAlien->wavDiv = RandFloat(MIN_DIV, MAX_DIV);
			cAlien->dartXSpeed = 0.0;
			return; //Found a free alien and spawned it.
		}
	}
}

void
RenderInvaders(void)
{
	GFX_Clear(0);
	
	//draw player ship
	GFX_BitBLTF(&g_shipGlyph, 3, 2, g_shipX, 14);
	
	//draw projectiles
	unsigned char i;
	for (i = 0; i < MAX_PLAYER_PROJECTILES; i++)
		GFX_PutPixel(roundf(g_playerBullets[i].pos.x), 
			roundf(g_playerBullets[i].pos.y), 1);
	
	//alien ships drawn 1 pixel tall.
	for (i = 0; i < MAX_ALIENS; i++)
	{
		if (g_alienShips[i].pos.y > 0 && 
			g_alienShips[i].pos.y < 16)
		{
			GFX_PutPixel(roundf(g_alienShips[i].pos.x),
				roundf(g_alienShips[i].pos.y), 1);
			GFX_PutPixel(roundf(g_alienShips[i].pos.x),
				roundf(g_alienShips[i].pos.y - 1), 1);
		}
	}
	
	GFX_SwapBuffers();
}