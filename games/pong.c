#include "gamelib.h"
#include "gamemath.h"
#include <stdlib.h>
#include "../display/display.h"

#define PADDLE_WIDTH 3
#define PONGBALL_MINSPEED 0.003f
#define PONGBALL_MAXSPEED 0.012f
#define SCORE_SCREEN_DELAY 3000
#define ACCELERATION_COEFFICIENT 1.001f
#define NUDGE_MAX 0.003f
#define SCORELIMIT 10

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
void DrawScoreScreen(t_PongPlayer *top, t_PongPlayer *bottom, unsigned long dt);
void CheckForCollisions(void);
void PlayerScore(t_PongPlayer *player, char isAI);
void UpdateBall(t_PongBall *ball, unsigned long dt);
void UpdatePlayerPaddle(t_PongPlayer *playerPaddle, unsigned long dt);
void UpdateCompPaddle(t_PongPlayer *compPaddle, unsigned long dt);
void HandleInput(void);
void AccelerateBall(t_PongBall *ball);
void NudgeBall(t_PongBall *ball, char upOrDown); //0 down 1 up

char 
InitPongGame(void)
{
	g_player.x = 6;
	g_player.score = 0;
	g_comp.x = 6;
	g_comp.score = 0;
	
	g_pongState = 0; 
	SetBitUInt8(&g_pongState, PONG_PLAYING); //start a regular game
	/* ball isn't launched, it's players turn. */
	
	g_lastTime = GLIB_GetGameMillis();
	srand(g_lastTime);
	
	return 1;
}

char 
PongGameLoop(void)
{
	unsigned long dt;
	unsigned long thisTime;
	thisTime = GLIB_GetGameMillis();
	dt = thisTime - g_lastTime;
	g_lastTime = thisTime;
	
	if (GetBitUInt8(&g_pongState, PONG_PLAYING))
	{
		UpdateBall(&g_pongBall, dt);
		UpdatePlayerPaddle(&g_player, dt);
		UpdateCompPaddle(&g_comp, dt);
		HandleInput();
		CheckForCollisions();
	}
	RenderPong(dt);
	
	return 1;
}

void 
RenderPong(unsigned long dt)
{
	GFX_Clear(0);
	
	//draw ball
	GFX_PutPixel(roundf(g_pongBall.x), roundf(g_pongBall.y), 1);
	
	//draw player paddle
	GFX_DrawLine(g_player.x, DISPLAY_HEIGHT - 1, 
		g_player.x + PADDLE_WIDTH, DISPLAY_HEIGHT - 1);
	
	//draw comp paddle
	GFX_DrawLine(g_comp.x, 0, g_comp.x + PADDLE_WIDTH, 0);
	
	//decides for itself if it needs to be drawn.
	/* Glitchy screen bug introduced a while ago coming from here.
	 No fucking clue why. Heap corruption? Don't even know how to test
	 for that. */
	DrawScoreScreen(&g_comp, &g_player, dt); 
	
	GFX_SwapBuffers();
}

void 
DrawScoreScreen(t_PongPlayer *top, t_PongPlayer *bottom, unsigned long dt)
{
	static unsigned long elapsed = SCORE_SCREEN_DELAY + 100;
	
	if (GetBitUInt8(&g_pongState, PONG_SCORESCREEN))
	{
		elapsed = 0;
		ClearBitUInt8(&g_pongState, PONG_SCORESCREEN);
	}
	
	if (elapsed > SCORE_SCREEN_DELAY)
	{
		if (top->score >= SCORELIMIT || bottom->score >= SCORELIMIT)
			InitPongGame();
		
		return;
	}
	
	if (top->score < SCORELIMIT && bottom->score < SCORELIMIT)
	{
		GFX_Clear(0);
		
		GFX_DrawLine(0,7,15,7);
		GFX_DrawLine(0,8,15,8);

		/* Maybe have a "DrawChar" func for single chars so we don't 
		 have to use this little trick to deal with the omitted null
		 terminator? */
		char scoreText[1];
		scoreText[1] = 0;

		if (GetBitUInt8(&g_pongState, PONG_WHOSTURN) == 1 &&
			elapsed <= (SCORE_SCREEN_DELAY / 2) )
			scoreText[0] = (top->score)+48-1;
		else
			scoreText[0] = (top->score)+48;

		GFX_DrawText(scoreText, 8, 1);


		if (GetBitUInt8(&g_pongState, PONG_WHOSTURN) == 0 &&
			elapsed <= (SCORE_SCREEN_DELAY / 2) )
			scoreText[0] = (bottom->score)+48-1;
		else
			scoreText[0] = (bottom->score)+48;

		GFX_DrawText(scoreText, 8, 10);
	}
	else
	{
		GFX_Clear(0);
		
		if (top->score >= SCORELIMIT)
		{
			GFX_DrawTextF(PSTR("COMP"), 0,0);
			GFX_DrawTextF(PSTR("WINS"), 0,8);
		}
		else if (bottom->score >= SCORELIMIT)
		{
			GFX_DrawTextF(PSTR("YOU"), 0,0);
			GFX_DrawTextF(PSTR("WIN"), 0,8);
		}
	}	

	elapsed += dt;

	/* This way we don't set the bit EVERY time we test whether to
	 display the score screen above. */
	if (elapsed > SCORE_SCREEN_DELAY) 
		SetBitUInt8(&g_pongState, PONG_PLAYING);
}

void 
CheckForCollisions(void)
{
	//bounce off walls
	if (roundf(g_pongBall.x) > DISPLAY_WIDTH - 1)
	{
		g_pongBall.x = DISPLAY_WIDTH - 1;
		g_pongBall.xSpeed = -g_pongBall.xSpeed;
	}
	
	if (roundf(g_pongBall.x) < 0)
	{
		g_pongBall.x = 0;
		g_pongBall.xSpeed = -g_pongBall.xSpeed;
	}
	
	//bounce ball off paddles
	if (g_pongBall.y > 14 && g_pongBall.y < DISPLAY_HEIGHT - 1)
	{
		if (g_pongBall.x >= g_player.x && 
			g_pongBall.x <= g_player.x + PADDLE_WIDTH)
		{
			g_pongBall.ySpeed = -g_pongBall.ySpeed;
			g_pongBall.y = 14;
			AccelerateBall(&g_pongBall);
			NudgeBall(&g_pongBall, 1);
		}
	}
	else if (g_pongBall.y < 1 && g_pongBall.y > 0)
	{
		if (g_pongBall.x >= g_comp.x && 
			g_pongBall.x <= g_comp.x + PADDLE_WIDTH)
		{
			g_pongBall.ySpeed = -g_pongBall.ySpeed;
			g_pongBall.y = 1;
			AccelerateBall(&g_pongBall);
			NudgeBall(&g_pongBall, 0);
		}	
	}
	
	/* below here we're checking for goals. */
	
	if (g_pongBall.y < 0) //score for player
	{
		PlayerScore(&g_player, 0);
	}
	else if (g_pongBall.y > DISPLAY_HEIGHT)
	{
		PlayerScore(&g_comp, 1);
	}
	
}

void 
PlayerScore(t_PongPlayer *player, char isAI)
{
	if (isAI)
	{
		player->score++;
		SetBitUInt8(&g_pongState, PONG_WHOSTURN); //comps turn
		ClearBitUInt8(&g_pongState, PONG_BALLLAUNCHED);
	}
	else
	{
		player->score++;
		ClearBitUInt8(&g_pongState, PONG_WHOSTURN); //players turn
		ClearBitUInt8(&g_pongState, PONG_BALLLAUNCHED);
		ClearBitUInt8(&g_pongState, PONG_COMPDECIDED);
	}
	
	//everybody now!
	SetBitUInt8(&g_pongState, PONG_SCORESCREEN);
	ClearBitUInt8(&g_pongState, PONG_PLAYING);
}

void 
UpdateBall(t_PongBall *ball, unsigned long dt)
{
	if (GetBitUInt8(&g_pongState, PONG_BALLLAUNCHED))
	{
		ball->x += ball->xSpeed * dt;
		ball->y += ball->ySpeed * dt;
	}
	else
	{
		if (GetBitUInt8(&g_pongState, PONG_WHOSTURN))
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
	
	if (GetBitUInt8(&g_pongState, PONG_BALLLAUNCHED))
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
	if (GetBitUInt8(&g_pongState, PONG_WHOSTURN) && 
		!GetBitUInt8(&g_pongState, PONG_COMPDECIDED)) //comps turn, undecided
	{
		launchDestX = floor(RandFloat(0, 
			DISPLAY_WIDTH - PADDLE_WIDTH - 1));
		SetBitUInt8(&g_pongState, PONG_COMPDECIDED);
	}
	
	if (GetBitUInt8(&g_pongState, PONG_COMPDECIDED) && //comp has decided 
		!GetBitUInt8(&g_pongState, PONG_BALLLAUNCHED) && //ball isn't launched
		GetBitUInt8(&g_pongState, PONG_WHOSTURN)) //comps turn
	{
		if (compPaddle->x > launchDestX)
			compPaddle->x--;
		else if (compPaddle->x < launchDestX)
			compPaddle->x++;
			
		if (compPaddle->x == launchDestX)
		{
			//launch ball (duplicate code, could be function
			SetBitUInt8(&g_pongState, PONG_BALLLAUNCHED);
			char direction = GLIB_GetGameMillis() % 2;
			g_pongBall.xSpeed = RandFloat(PONGBALL_MINSPEED,
					PONGBALL_MAXSPEED);
			
			if (direction == 0) //left
				g_pongBall.xSpeed = -g_pongBall.xSpeed;
			//otherwise, change nothing.
				
			g_pongBall.ySpeed = RandFloat(PONGBALL_MINSPEED,
				PONGBALL_MAXSPEED);
		}
	}
}

void 
HandleInput(void)
{
	if (GetBitUInt8(&g_pongState, PONG_BALLLAUNCHED) == 0 &&
		GetBitUInt8(&g_pongState, PONG_WHOSTURN) == 0)
	{
		if (GLIB_GetInput(GLIB_PB2))
		{
			SetBitUInt8(&g_pongState, PONG_BALLLAUNCHED);
			char direction = GLIB_GetGameMillis() % 2;
			g_pongBall.xSpeed = RandFloat(PONGBALL_MINSPEED,
					PONGBALL_MAXSPEED);
			
			if (direction == 0) //left
				g_pongBall.xSpeed = -g_pongBall.xSpeed;
			//otherwise, change nothing.
				
			g_pongBall.ySpeed = -RandFloat(PONGBALL_MINSPEED,
				PONGBALL_MAXSPEED);
		}
	}
}


void 
AccelerateBall(t_PongBall *ball)
{
	ball->xSpeed = ball->xSpeed * ACCELERATION_COEFFICIENT;
	ball->ySpeed = ball->ySpeed * ACCELERATION_COEFFICIENT;
}

void 
NudgeBall(t_PongBall *ball, char upOrDown)
{
	ball->xSpeed += RandFloat(-NUDGE_MAX, NUDGE_MAX);
	
	if (upOrDown) // nudge up
		ball->ySpeed -= RandFloat(0, NUDGE_MAX);
	else // nudge down
		ball->ySpeed += RandFloat(0, NUDGE_MAX);
}