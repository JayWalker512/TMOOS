#include "gamelib.h"

#define PADDLE_WIDTH 4

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
void UpdatePaddles(unsigned long dt);

char 
InitPongGame(void)
{
	pongBall.x = 7;
	pongBall.y = 7;
	pongBall.xSpeed = 0;
	pongBall.ySpeed = 0;
	
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
	
	RenderPong();
}

void 
RenderPong(void)
{
	
}