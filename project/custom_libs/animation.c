#include "animation.h"
#define Array(t,n) (t*)malloc(sizeof(t)*n)

Image* animFrame(Animation* anim)
{	
	anim->ms0+=pgeTimerPeekDeltaTime(anim->time);
	if(anim->ms0>anim->ms){
		anim->i++;
		if(anim->i>anim->i1)anim->i=anim->i0;
		anim->ms0=0;
		pgeTimerUpdate(anim->time);
	}
	return anim->collection[anim->i];
}

int animId(Animation* anim)
{	
	anim->ms0+=pgeTimerPeekDeltaTime(anim->time);
	if(anim->ms0>anim->ms){
		anim->i++;
		if(anim->i>anim->i1)anim->i=anim->i0;
		pgeTimerUpdate(anim->time);
	}
	return anim->i;
}

Animation* animLoad(const char* path, int i0, int i1, float ms)
{	
	Animation* o=(Animation*)malloc(sizeof(Animation));
	o->ms=ms;
	o->ms0=0.0f;
	o->time=pgeTimerCreate();
	pgeTimerUpdate(o->time);
	o->i0=0;
	o->i=0;
	o->i1=i1-i0;
	o->collection=Array(Image*,i1-i0);
	int i;
	for(i=0;i<=i1-i0;i++){
		char* rez;
		asprintf(&rez,"%s%i.png",path,i);
		o->collection[i]=loadImage(rez);
		free(rez);
	}
	return o;
}

void animFree(Animation* anim)
{
	if(anim!=NULL){
		if(anim->collection!=NULL){
			int i=0;
			for(i = 0; i<=anim->i1; i++)
				if(anim->collection[i]!=NULL)
					freeImage(anim->collection[i]);
			free(anim->collection);anim->collection=NULL;
		}
		if(anim->time!=NULL)
			pgeTimerDestroy(anim->time);
		free(anim);
		anim=NULL;
	}
}

void animStretch(Animation* anim, float xScale, float yScale)
{
	int i;
	for(i=0;i<=anim->i1;i++){
		stretchImage(anim->collection[i], xScale, yScale);
	}
}

void animSetZ(Animation* anim, float z)
{
	int i;
	for(i=0;i<=anim->i1;i++){
		setImageZ(anim->collection[i], z);
	}
}









