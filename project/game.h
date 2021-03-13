#ifndef __GAME_H__
#define __GAME_H__

#include "items.h"
#include "custom_libs/animation.h"

#ifdef __cplusplus
extern "C" {
#endif

struct Rect{
	int x,y,w,h;
};

struct Player{
	float x,y;
	int w,h;
	int dir, mv;
	float acc, spd;
	int hp, mp, hpMax, mpMax;
	Item** inv;
	Animation** image;
};

extern int WORLD_W;extern int WORLD_H;extern int WORLD_SIZE;
extern int BLOCK_SIZE;extern int WALL_SIZE;extern int inv_size;

//need for save
extern u16* blocks;
extern u8* walls;
extern struct Player player;

void game(void);
void generator(void);
void init(int w,int h);
u8 r_r(struct Rect,struct Rect);
u8 collisionBXY(int x, int y);
u8 collisionB(void);
void move(float dx, float dy, float spd);

u16* getBlock(int x, int y);
u8* getWall(int x, int y);//wts == u8 at now
u8 getBlockCollision(int x, int y);

int buildBlock(int x, int y, u16 id);
int buildWall(int x, int y, u8 id);

#ifdef __cplusplus
}
#endif
#endif