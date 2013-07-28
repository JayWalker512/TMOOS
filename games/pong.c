#include "gamelib.h"
#include "../display/display.h"

#define PADDLE_WIDTH 3

typedef struct s_PongPlayer {
	char x;
	char score;
} t_PongPlayer;

typedef struct s_PongBall {
	float x;
	float y;
	float xSpeed;
	float ySpeed;
} t_PongBall;

t_PongBall pongBall;
t_PongPlayer player, comp;

unsigned long lastTime;

void RenderPong(void);
void CheckForCollisions(void);
void UpdateBall(unsigned long dt);
void UpdatePlayerPaddle(unsigned long dt);

char 
InitPongGame(void)
{
	pongBall.x = 7;
	pongBall.y = 7;
	pongBall.xSpeed = 0.010;
	pongBall.ySpeed = 0.007;
	
	player.x = 6;
	player.score = 0;
	comp.x = 6;
	comp.score = 0;
	
	lastTime = GLIB_GetGameMillis();
}

char 
PongGameLoop(void)
{
	unsigned long dt;
	unsigned long thisTime;
	thisTime = GLIB_GetGameMillis();
	dt = thisTime - lastTime;
	lastTime = thisTime;
	
	UpdateBall(dt);
	UpdatePlayerPaddle(dt);
	CheckForCollisions();
	RenderPong();
}

void 
RenderPong(void)
{
	GFX_Clear(0);
	
	//draw ball
	GFX_PutPixel(floor(pongBall.x), floor(pongBall.y), 1);
	
	//draw player paddle
	GFX_DrawLine(player.x, DISPLAY_HEIGHT - 1, 
		player.x + PADDLE_WIDTH, DISPLAY_HEIGHT - 1);
	
	//draw comp paddle
	GFX_DrawLine(comp.x, 0, comp.x + PADDLE_WIDTH, 0);
	
	GFX_SwapBuffers();
}

void CheckForCollisions(void)
{
	//bounce off walls
	if (pongBall.x >= DISPLAY_WIDTH)
	{
		pongBall.x = DISPLAY_WIDTH;
		pongBall.xSpeed = -pongBall.xSpeed;
	}
	
	if (pongBall.x <= 0)
	{
		pongBall.x = 0;
		pongBall.xSpeed = -pongBall.xSpeed;
	}
	
	if (pongBall.y >= DISPLAY_HEIGHT)
	{
		pongBall.y = DISPLAY_HEIGHT;
		pongBall.ySpeed = -pongBall.ySpeed; 
	}
	
	if (pongBall.y <= 0)
	{
		pongBall.y = 0;
		pongBall.ySpeed = -pongBall.ySpeed;
	}
	
	//bounce ball off paddles
	if (pongBall.y >= 15 && pongBall.y <= DISPLAY_HEIGHT)
	{
		if (pongBall.x >= player.x && 
			pongBall.x <= player.x + PADDLE_WIDTH)
		{
			pongBall.ySpeed = -pongBall.ySpeed;
			pongBall.y = 15;
		}
		else if (pongBall.x >= comp.x && 
			pongBall.x <= comp.x + PADDLE_WIDTH)
		{
			pongBall.ySpeed = -pongBall.ySpeed;
			pongBall.y = 15;
		}	
	}
	else if (pongBall.y <= 1 && pongBall.y >= 0)
	{
		if (pongBall.x >= player.x && 
			pongBall.x <= player.x + PADDLE_WIDTH)
		{
			pongBall.ySpeed = -pongBall.ySpeed;
			pongBall.y = 1;
		}
		else if (pongBall.x >= comp.x && 
			pongBall.x <= comp.x + PADDLE_WIDTH)
		{
			pongBall.ySpeed = -pongBall.ySpeed;
			pongBall.y = 1;
		}	
	}
}

void UpdateBall(unsigned long dt)
{
	pongBall.x += pongBall.xSpeed * dt;
	pongBall.y += pongBall.ySpeed * dt;
}

void UpdatePlayerPaddle(unsigned long dt)
{
	player.x = GLIB_GetWheelRegion(DISPLAY_HEIGHT - PADDLE_WIDTH);
}
