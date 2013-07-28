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

void RenderPong(void);

char 
InitPongGame(void)
{
	playerScore = compScore = 0;
	playerX = compX = 6
	
	pongBall.x = 7;
	pongBall.y = 7;
	pongBall.xSpeed = 0;
	pongBall.ySpeed = 0;
	
	player.x = 6;
	player.score = 0;
	comp.x = 6;
	comp.score = 0;
}

char 
PongGameLoop(void)
{
	
	RenderPong();
}

void 
RenderPong(void)
{
	
}