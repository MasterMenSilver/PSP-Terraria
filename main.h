#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <string.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <math.h>
#include "libs/intraFont.h"
#include "libs/graphics2d.h"
#include "libs/pge/pgeWav.h"
#include "libs/pge/pgeControls.h"
#include "libs/pge/pgeTimer.h"

#include "libs/sound_utils/oslib.h"

#define PI 3.1415926535f
#define GTR(a) a*PI/180.0f
#define RTG(a) a*180.0f/PI
#define Array(t,n) (t*)malloc(sizeof(t)*n)
#define ScaleArray(s,t,n) (t*)realloc(s,sizeof(t)*n)

#define showStatusf(text,...) startDrawing();\
	clearScreen(GU_RGBA(4,4,4,255));\
	intraFontPrintf(Andy, 480/2, 272/2-25, text, __VA_ARGS__);\
	endDrawing();\
	flipScreen();
	
//idk about this...
#define showStatus(text) startDrawing();\
	clearScreen(GU_RGBA(4,4,4,255));\
	intraFontPrint(Andy, 480/2, 272/2-25, text);\
	endDrawing();\
	flipScreen();
	
#define SetStr(t,n) t=Array(char,sizeof(n));\
t=n;

#ifdef __cplusplus
extern "C" {
#endif

struct clrs_t{
	u32 red,blue,green,black,white,gray,yellow,purple,cyan,pink;
};

double fixAngle(double angle);

u32 getFreeMemory(void);
void FreeImageArray(Image**, u8);
void initEngine(void);
void shutdownEngine(void);

extern u8 worldSizeType;
extern u8 nosol;//new or save or load
extern char* worldName;
extern int version;

extern u8 globalState;
extern struct clrs_t clrs;
extern intraFont* Andy;

#ifdef __cplusplus
}
#endif
#endif