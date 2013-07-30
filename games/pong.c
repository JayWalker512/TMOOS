#include "gamelib.h"
#include <stdlib.h>
#include "../display/display.h"

#define PADDLE_WIDTH 3
#define PONGBALL_MINSPEED 0.003
#define PONGBALL_MAXSPEED 0.012

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

enum e_GameState  
{
	PONG_PLAYING = 0, //will be 1 if regular game running, 0 if other
	PONG_WHOSTURN, //0 for player turn, 1 for comp turn.
	PONG_SCORESCREEN,
	PONG_BALLLAUNCHED, //beginning of each turn, ball stays at paddle
};

unsigned char pongState; //variable for storing bitwise states above

void RenderPong(void);
void CheckForCollisions(void);
void UpdateBall(unsigned long dt);
void UpdatePlayerPaddle(unsigned long dt);
void HandleInput(void);
float GetRandFloat(float min, float max);

char 
InitPongGame(void)
{
	pongBall.x = 7;
	pongBall.y = 7;
	pongBall.xSpeed = GetRandFloat(PONGBALL_MINSPEED, PONGBALL_MAXSPEED);
	pongBall.ySpeed = GetRandFloat(PONGBALL_MINSPEED, PONGBALL_MAXSPEED);
	
	player.x = 6;
	player.score = 0;
	comp.x = 6;
	comp.score = 0;
	
	pongState = 0; 
	SetBit(&pongState, PONG_PLAYING); //start a regular game
	/* ball isn't launched, it's players turn. */
	
	lastTime = GLIB_GetGameMillis();
	srand(lastTime);
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
	HandleInput();
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
	
	//This block bounces ball off the goals
	/*if (pongBall.y >= DISPLAY_HEIGHT)
	{
		pongBall.y = DISPLAY_HEIGHT;
		pongBall.ySpeed = -pongBall.ySpeed; 
	}
	
	if (pongBall.y <= 0)
	{
		pongBall.y = 0;
		pongBall.ySpeed = -pongBall.ySpeed;
	}*/
	
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
	
	/* below here we're checking for goals. */
	
	if (pongBall.y < 0) //score for player
	{
		player.score++;
		ClearBit(&pongState, PONG_WHOSTURN); //players turn
		ClearBit(&pongState, PONG_BALLLAUNCHED);
	}
	else if (pongBall.y > DISPLAY_HEIGHT)
	{
		comp.score++;
		SetBit(&pongState, PONG_WHOSTURN); //comps turn
		ClearBit(&pongState, PONG_BALLLAUNCHED);
	}
	
}

void UpdateBall(unsigned long dt)
{
	if (GetBit(&pongState, PONG_PLAYING))
	{
		if (GetBit(&pongState, PONG_BALLLAUNCHED))
		{
			pongBall.x += pongBall.xSpeed * dt;
			pongBall.y += pongBall.ySpeed * dt;
		}
		else
		{
			if (GetBit(&pongState, PONG_WHOSTURN))
			{
				//comps turn
				pongBall.x = comp.x + (PADDLE_WIDTH / 2);
				pongBall.y = 2;
			}
			else
			{
				//players turn
				pongBall.x = player.x + (PADDLE_WIDTH / 2);
				pongBall.y = 13;
			}
		}
	}
}

void UpdatePlayerPaddle(unsigned long dt)
{
	player.x = GLIB_GetWheelRegion(DISPLAY_HEIGHT - PADDLE_WIDTH);
}

void HandleInput(void)
{
	if (GetBit(&pongState, PONG_BALLLAUNCHED) == 0 &&
		GetBit(&pongState, PONG_WHOSTURN) == 0)
	{
		if (GLIB_GetInput(GLIB_PB2))
		{
			SetBit(&pongState, PONG_BALLLAUNCHED);
			char direction = GLIB_GetGameMillis() % 2;
			pongBall.xSpeed = GetRandFloat(PONGBALL_MINSPEED,
					PONGBALL_MAXSPEED);
			
			if (direction == 0) //left
				pongBall.xSpeed = -pongBall.xSpeed;
			//otherwise, change nothing.
				
			pongBall.ySpeed = -GetRandFloat(PONGBALL_MINSPEED,
				PONGBALL_MAXSPEED);
		}
	}
}

/* TODO put me in gamelib */
float GetRandFloat(float min, float max)
{
	return min + (float)rand()/((float)RAND_MAX/max);
}