#include "gamelib.h"
#include "gamemath.h"
#include "../common/avr.h"
#include "../common/binary.h"
#include "display/display.h"
#include "input/input.h"
#include "console/console.h"

#define SHIP_WIDTH 3
#define SHIP_HEIGHT 2
#define WEAPON_COOLDOWN_MS 800
#define MAX_PLAYER_PROJECTILES 2 /*The enum will need updated if we change this.
	Very sloppy way to save a few bytes. */
#define PLAYER_PROJECTILE_YSPEED 0.020f;

enum e_InvadersStates 
{
	SOME_STATE = 0,
};

typedef struct Projectile_s 
{
	Vec2Df_t pos;
	float ySpeed;
} Projectile_t;

typedef struct AlienShip_s 
{
	Vec2Df_t pos;
} AlienShip_t;

Projectile_t g_playerBullets[MAX_PLAYER_PROJECTILES];

unsigned char g_InvadersState;
char g_shipX;
PROGMEM unsigned char g_shipGlyph = B01011100;

int g_playerWeaponCooldown;

void HandleInvadersInput(void);
void UpdateProjectiles(Projectile_t *projectileList, unsigned char num);
void FirePlayerProjectile(char x, Projectile_t *projectileList, unsigned char num);
void RenderInvaders(void);

char
InitInvaders(void)
{
	g_shipX = 0;
	g_playerWeaponCooldown = 0;
	g_InvadersState = 0;
	
	//set projectiles way away  and .y < 0 when they're "inactive"
	unsigned char i;
	for (i = 0; i < MAX_PLAYER_PROJECTILES; i++)
	{
		g_playerBullets[i].pos.x = -50; 
		g_playerBullets[i].pos.y = -1;
	}
	
	return 1;
}

char
InvadersLoop(void)
{
	/* TODO clearly we need a dt variable here... Projectile speeds? Duh? */
	
	HandleInvadersInput();
	UpdateProjectiles(g_playerBullets, MAX_PLAYER_PROJECTILES);
	RenderInvaders();
	
	return 1;
}

void
HandleInvadersInput(void)
{
	g_shipX = GLIB_GetWheelRegion(13);
	unsigned char events = INP_PollEvents();
	if (GetBit(&events, INPUT_PB0_DOWN))
		FirePlayerProjectile(g_shipX + 1, g_playerBullets, MAX_PLAYER_PROJECTILES);
}

void 
UpdateProjectiles(Projectile_t *projectileList, unsigned char num)
{
	unsigned char i;
	for (i = 0; i < num; i++)
	{
		Projectile_t *cProj = (projectileList+i);
		cProj->pos.y -= cProj->ySpeed;	
		
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
			CON_SendString(PSTR("Firing!"));
			cProj->pos.y = DISPLAY_HEIGHT - SHIP_HEIGHT;
			cProj->pos.x = x;
			cProj->ySpeed = PLAYER_PROJECTILE_YSPEED;
			return; //found a bullet and fired. Leave now!
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
	
	GFX_SwapBuffers();
}
