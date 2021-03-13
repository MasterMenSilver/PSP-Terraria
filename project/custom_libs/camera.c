#include "libs/graphics2d.h"
#include "camera.h"

int offsetX,offsetY;

void draw(Image* img, int x, int y)
{	
	drawImage(img, x+offsetX, y+offsetY, 0, 255);
}

void drawPart(Image* img, int x, int y, int sx,
	int sy, int w, int h)
{
	drawImagePart(img,x+offsetX,y+offsetY,sx,sy,w,h,0,255);
}

void focus(Image* img, int x, int y, int x0, int y0, int w0, int h0)
{	
	int w=img->width;
	int h=img->height;
	
	offsetX=(480-w)/2;
	offsetY=(272-h)/2;
	
	int dx=x-x0;
	int dy=y-y0;
	
	if(dx<offsetX){
		offsetX=dx;
	}else if(w0-w-dx<offsetX){
		offsetX=480+dx-w0;
	}
	if(dy<offsetY){ 
		offsetY=dy;
	}else if(h0-h-dy<offsetY){
		offsetY=272+dy-h0;
	}
	
	offsetX=offsetX-x;
	offsetY=offsetY-y;
}

void focus0(int w, int h, int x, int y, int x0, int y0, int w0, int h0)
{
	offsetX=(480-w)/2;
	offsetY=(272-h)/2;
	
	int dx=x-x0;
	int dy=y-y0;
	
	if(dx<offsetX){
		offsetX=dx;
	}else if(w0-w-dx<offsetX){
		offsetX=480+dx-w0;
	}
	if(dy<offsetY){ 
		offsetY=dy;
	}else if(h0-h-dy<offsetY){
		offsetY=272+dy-h0;
	}
	
	offsetX=offsetX-x;
	offsetY=offsetY-y;
}