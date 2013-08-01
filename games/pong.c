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

t_PongBall g_pongBall;
t_PongPlayer g_player, g_comp;

unsigned long g_lastTime;

enum e_GameState  
{
	PONG_PLAYING = 0, //will be 1 if regular game running, 0 if other
	PONG_WHOSTURN, //0 for player turn, 1 for comp turn.
	PONG_SCORESCREEN,
	PONG_BALLLAUNCHED, //beginning of each turn, ball stays at paddle
	PONG_COMPDECIDED, //comp decided where to launch ball from
};

unsigned char g_pongState; //variable for storing bitwise states above

void RenderPong(unsigned long dt);
void DrawScoreScreen(t_PongPlayer *top, t_PongPlayer *bottom);
void CheckForCollisions(void);
void UpdateBall(t_PongBall *ball, unsigned long dt);
void UpdatePlayerPaddle(t_PongPlayer *playerPaddle, unsigned long dt);
void UpdateCompPaddle(t_PongPlayer *compPaddle, unsigned long dt);
void HandleInput(void);
float GetRandFloat(float min, float max);

char 
InitPongGame(void)
{
	g_player.x = 6;
	g_player.score = 0;
	g_comp.x = 6;
	g_comp.score = 0;
	
	g_pongState = 0; 
	SetBit(&g_pongState, PONG_PLAYING); //start a regular game
	/* ball isn't launched, it's players turn. */
	
	g_lastTime = GLIB_GetGameMillis();
	srand(g_lastTime);
}

char 
PongGameLoop(void)
{
	unsigned long dt;
	unsigned long thisTime;
	thisTime = GLIB_GetGameMillis();
	dt = thisTime - g_lastTime;
	g_lastTime = thisTime;
	
	UpdateBall(&g_pongBall, dt);
	UpdatePlayerPaddle(&g_player, dt);
	UpdateCompPaddle(&g_comp, dt);
	HandleInput();
	CheckForCollisions();
	//RenderPong(dt);
	DrawScoreScreen(&g_comp, &g_player);
}

void 
RenderPong(unsigned long dt)
{
	GFX_Clear(0);
	
	//draw ball
	GFX_PutPixel(floor(g_pongBall.x), floor(g_pongBall.y), 1);
	
	//draw player paddle
	GFX_DrawLine(g_player.x, DISPLAY_HEIGHT - 1, 
		g_player.x + PADDLE_WIDTH, DISPLAY_HEIGHT - 1);
	
	//draw comp paddle
	GFX_DrawLine(g_comp.x, 0, g_comp.x + PADDLE_WIDTH, 0);
	
	GFX_SwapBuffers();
}

void DrawScoreScreen(t_PongPlayer *top, t_PongPlayer *bottom)
{
	GFX_Clear(0);
	
	
	GFX_DrawLine(0,7,15,7);
	GFX_DrawLine(0,8,15,8);
	
	/* Maybe have a "DrawChar" func for single chars so we don't 
	 have to use this little trick to deal with the omitted null
	 terminator? */
	char scoreText[1];
	scoreText[1] = 0;
	scoreText[0] = (top->score)+48;
	GFX_DrawText(scoreText, 8, 1);
	
	scoreText[0] = (bottom->score)+48;
	GFX_DrawText(scoreText, 8, 10);
	
	
	GFX_SwapBuffers();
}

void 
CheckForCollisions(void)
{
	//bounce off walls
	if (g_pongBall.x >= DISPLAY_WIDTH)
	{
		g_pongBall.x = DISPLAY_WIDTH;
		g_pongBall.xSpeed = -g_pongBall.xSpeed;
	}
	
	if (g_pongBall.x <= 0)
	{
		g_pongBall.x = 0;
		g_pongBall.xSpeed = -g_pongBall.xSpeed;
	}
	
	//bounce ball off paddles
	if (g_pongBall.y >= 15 && g_pongBall.y <= DISPLAY_HEIGHT)
	{
		if (g_pongBall.x >= g_player.x && 
			g_pongBall.x <= g_player.x + PADDLE_WIDTH)
		{
			g_pongBall.ySpeed = -g_pongBall.ySpeed;
			g_pongBall.y = 15;
		}
	}
	else if (g_pongBall.y <= 1 && g_pongBall.y >= 0)
	{
		if (g_pongBall.x >= g_comp.x && 
			g_pongBall.x <= g_comp.x + PADDLE_WIDTH)
		{
			g_pongBall.ySpeed = -g_pongBall.ySpeed;
			g_pongBall.y = 1;
		}	
	}
	
	/* below here we're checking for goals. */
	
	if (g_pongBall.y < 0) //score for player
	{
		g_player.score++;
		ClearBit(&g_pongState, PONG_WHOSTURN); //players turn
		ClearBit(&g_pongState, PONG_BALLLAUNCHED);
	}
	else if (g_pongBall.y > DISPLAY_HEIGHT)
	{
		g_comp.score++;
		SetBit(&g_pongState, PONG_WHOSTURN); //comps turn
		ClearBit(&g_pongState, PONG_BALLLAUNCHED);
		ClearBit(&g_pongState, PONG_COMPDECIDED);
	}
	
}

void 
UpdateBall(t_PongBall *ball, unsigned long dt)
{
	if (GetBit(&g_pongState, PONG_PLAYING))
	{
		if (GetBit(&g_pongState, PONG_BALLLAUNCHED))
		{
			ball->x += ball->xSpeed * dt;
			ball->y += ball->ySpeed * dt;
		}
		else
		{
			if (GetBit(&g_pongState, PONG_WHOSTURN))
			{
				//comps turn
				ball->x = g_comp.x + (PADDLE_WIDTH / 2);
				ball->y = 2;
			}
			else
			{
				//players turn
				ball->x = g_player.x + (PADDLE_WIDTH / 2);
				ball->y = 13;
			}
		}
	}
}

void 
UpdatePlayerPaddle(t_PongPlayer *playerPaddle, unsigned long dt)
{
	playerPaddle->x = GLIB_GetWheelRegion(DISPLAY_HEIGHT - PADDLE_WIDTH);
}

//This is the computer players AI essentially.
void 
UpdateCompPaddle(t_PongPlayer *compPaddle, unsigned long dt)
{
	static unsigned char launchDestX = 0;
	static unsigned char elapsed = 0;
	elapsed += dt;
	
	if (elapsed <= 100) //comp updates at 10hz
		return;
	
	elapsed = 0;
	
	if (GetBit(&g_pongState, PONG_BALLLAUNCHED))
	{
		if (g_pongBall.x < compPaddle->x)
		{
			if (compPaddle->x > 0)
				compPaddle->x--;
		}
		else if (g_pongBall.x > compPaddle->x + PADDLE_WIDTH)
		{
			if (compPaddle->x + PADDLE_WIDTH < DISPLAY_WIDTH - 1)
				compPaddle->x++;
		}
	}
	
	//it's comps turn, decide where to launch ball (and launch it)
	if (GetBit(&g_pongState, PONG_WHOSTURN) && 
		!GetBit(&g_pongState, PONG_COMPDECIDED)) //comps turn, undecided
	{
		launchDestX = floor(GetRandFloat(0, 
			DISPLAY_WIDTH - PADDLE_WIDTH - 1));
		SetBit(&g_pongState, PONG_COMPDECIDED);
	}
	
	if (GetBit(&g_pongState, PONG_COMPDECIDED) && //comp has decided 
		!GetBit(&g_pongState, PONG_BALLLAUNCHED) && //ball isn't launched
		GetBit(&g_pongState, PONG_WHOSTURN)) //comps turn
	{
		if (compPaddle->x > launchDestX)
			compPaddle->x--;
		else if (compPaddle->x < launchDestX)
			compPaddle->x++;
			
		if (compPaddle->x == launchDestX)
		{
			//launch ball (duplicate code, could be function
			SetBit(&g_pongState, PONG_BALLLAUNCHED);
			char direction = GLIB_GetGameMillis() % 2;
			g_pongBall.xSpeed = GetRandFloat(PONGBALL_MINSPEED,
					PONGBALL_MAXSPEED);
			
			if (direction == 0) //left
				g_pongBall.xSpeed = -g_pongBall.xSpeed;
			//otherwise, change nothing.
				
			g_pongBall.ySpeed = GetRandFloat(PONGBALL_MINSPEED,
				PONGBALL_MAXSPEED);
		}
	}
}

void 
HandleInput(void)
{
	if (GetBit(&g_pongState, PONG_BALLLAUNCHED) == 0 &&
		GetBit(&g_pongState, PONG_WHOSTURN) == 0)
	{
		if (GLIB_GetInput(GLIB_PB2))
		{
			SetBit(&g_pongState, PONG_BALLLAUNCHED);
			char direction = GLIB_GetGameMillis() % 2;
			g_pongBall.xSpeed = GetRandFloat(PONGBALL_MINSPEED,
					PONGBALL_MAXSPEED);
			
			if (direction == 0) //left
				g_pongBall.xSpeed = -g_pongBall.xSpeed;
			//otherwise, change nothing.
				
			g_pongBall.ySpeed = -GetRandFloat(PONGBALL_MINSPEED,
				PONGBALL_MAXSPEED);
		}
	}
}

/* TODO put me in gamelib */
float 
GetRandFloat(float min, float max)
{
	return min + (float)rand()/((float)RAND_MAX/max);
}