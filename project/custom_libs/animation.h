#ifndef __ANIMATION_H
#define __ANIMATION_H

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Animation_t
{
	Image** collection;
	int i0,i,i1;
	float ms,ms0;
	pgeTimer* time;
} Animation;

Image* animFrame(Animation* anim);
int animId(Animation* anim);
Animation* animLoad(const char* path, int i0, int i1, float ms);
void animFree(Animation* anim);
void animStretch(Animation* anim, float xScale, float yScale);
void animSetZ(Animation* anim, float z);

#ifdef __cplusplus
}
#endif
#endif