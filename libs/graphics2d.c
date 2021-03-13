#include "graphics2d.h"
#include "main.h"

#include <string.h>
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <png.h>
#include <stdlib.h>
#include <math.h>

#define RGBA8888(col,r,g,b,a)	{ \
		a=(col>>24)&0xFF;\
		b=(col>>16)&0xFF;\
		g=(col>>8)&0xFF;\
		r=(col&0xFF);\
		}

#define IS_ALPHA(color) (((color)&0xff000000)==0xff000000?0:1)
#define PI 3.1415926535f
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#define png_set_gray_1_2_4_to_8 png_set_expand_gray_1_2_4_to_8
#define png_bytep_NULL (png_bytep)NULL

//unsigned int __attribute__((aligned(16))) g_dlist[262144];
void* g_dlist;

extern u8 msx[]; // change to extern "C" u8 msx[]; for use with C++

#define FRAMEBUFFER_SIZE (PSP_LINE_SIZE*SCR_HEIGHT*4)

int frames;

int g_dispNumber;
u32 *g_vram_base = (u32*) (0x40000000 | 0x04000000);

void *fbp0 = NULL;
void *fbp1;
void *zbp;

Vertex getVertex(float u,float v,u32 color,float x, float y,float z)
{
	Vertex vert;
	vert.u = u;
	vert.v = v;
	vert.color = color;
	vert.x = x;
	vert.y = y;
	vert.z = z;
	return vert;
}

Image* newImage(int width, int height, unsigned int color)
{
	Image* img = (Image*)malloc(sizeof(Image));
	
	if( img==NULL || width < 1 || height < 1 )
		return NULL;
		
	img->width = width;
	img->height = height;
	img->origWidth = width;
	img->origHeight = height;
	img->texWidth = nextPowOf2(width);
	img->texHeight = nextPowOf2(height);
	img->scX = 1.0f;
	img->scY = 1.0f;
	
	img->data = (unsigned char*)malloc(img->texWidth*img->height*sizeof(u32));
	if( img->data == NULL ){
		free(img);
		return NULL;
	}
	
	int ix, iy;
	u32* data = (u32*)(img->data);
	
	for(ix = 0; ix < width; ix++){
		for(iy=0; iy < img->height; iy++){
			data[iy * img->texWidth + ix] = color;
		}
	}
	
	if( (height % 8) == 0 )
		swizzleImage(img);
	else
		img->swizzled = 0;
		
	createImageVerts(img);
	
	return img;
}

Image* loadImage(const char fileName[])
{
	Image* img = NULL;
	
	png_structp pngPtr;
	png_infop infoPtr;
	unsigned int sigRead = 0;
	png_uint_32 width, height;
	int bitDepth, colorType, interlaceType, x, y;
	u32* line;
	FILE *fileIn;

	fileIn = fopen(fileName, "rb");
	if (fileIn == NULL){
		//char* str;
		//sprintf(str,"No Texture \"%s\"",fileName);
		//throwError(str);
		sceKernelExitGame();
		return NULL;
	}

	pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (pngPtr == NULL) 
	{
		fclose(fileIn);
		return NULL;
	}

	png_set_error_fn(pngPtr, (png_voidp) NULL, (png_error_ptr) NULL, NULL);
	infoPtr = png_create_info_struct(pngPtr);
	if (infoPtr == NULL) 
	{
		fclose(fileIn);
		png_destroy_read_struct(&pngPtr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

	png_init_io(pngPtr, fileIn);
	png_set_sig_bytes(pngPtr, sigRead);
	png_read_info(pngPtr, infoPtr);
	png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorType, 
					&interlaceType, int_p_NULL, int_p_NULL);

	// can't allow images with width or height greater than 512 or it will crash
	// the system.
	//
	if (width > 512 || height > 512) 
	{
		fclose(fileIn);
		png_destroy_read_struct(&pngPtr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

	// attempt to allocate memory for new image
	//
	img = (Image*)malloc(sizeof(Image));
	if( img == NULL )
		return NULL;

	img->swizzled = 0;

	img->scX = 1.0f;
	img->scY = 1.0f;
	img->width = width;
	img->height = height;
	img->origWidth = width;
	img->origHeight = height;
	img->texWidth = nextPowOf2(width);
	img->texHeight = nextPowOf2(height);
	png_set_strip_16(pngPtr);
	png_set_packing(pngPtr);

	if (colorType == PNG_COLOR_TYPE_PALETTE) 
		png_set_palette_to_rgb(pngPtr);

	if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8) 
		png_set_gray_1_2_4_to_8(pngPtr);

	if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) 
		png_set_tRNS_to_alpha(pngPtr);

	png_set_filler(pngPtr, 0xff, PNG_FILLER_AFTER);

	//img->data = malloc(width * height * sizeof(u32));
	img->data = (unsigned char*)malloc( img->texWidth * img->height * sizeof(u32));
	
	if (!img->data){
		fclose(fileIn);
		png_destroy_read_struct(&pngPtr, png_infopp_NULL, png_infopp_NULL);
		free(img);
		return NULL;
	}

	line = (u32*) malloc(width * 4);

	if (!line){
		fclose(fileIn);
		png_destroy_read_struct(&pngPtr, png_infopp_NULL, png_infopp_NULL);
		return NULL;
	}

	u32* data = (u32*)img->data;

	for (y = 0; y < height; y++){
		png_read_row(pngPtr, (u8*) line, png_bytep_NULL);
		for (x = 0; x < width; x++){
			u32 color = line[x];
			data[x + y * img->texWidth] =  color;
		}
	}

	// Png's are 32-bit color? so...
	//
	img->colorMode = 32 / 8; // i'm not sure about this

	free(line);
	png_read_end(pngPtr, infoPtr);
	png_destroy_read_struct(&pngPtr, &infoPtr, png_infopp_NULL);
	fclose(fileIn);

	if( (img->height % 8) == 0 )
		swizzleImage(img);
	createImageVerts(img);
	return img;

}

// swizzle code from
// http://www.psp-programming.com/forums/index.php?topic=1043.0
// Thanks to Raphael
void swizzleImage(Image* img)
{
	if(img==NULL||img->swizzled||(img->origHeight%8)!= 0)
		return;
	/*
	int bytewidth = img->texWidth*sizeof(u32);
	long size = bytewidth*img->height;
	int height = size / bytewidth;
	int rowblocks = (bytewidth>>4);
	int rowblocksadd = (rowblocks-1)<<7;
	unsigned int blockaddress = 0;
	unsigned int *src = (unsigned int*) img->data;
	unsigned char *data = NULL;
	data = malloc(size);
	if(!data)return;
	int j;
	for(j = 0; j < height; j++, blockaddress += 16)
	{
		unsigned int *block;
		block=(unsigned int*)&data[blockaddress];
		int i;
		for(i = 0; i < rowblocks; i++)
		{
			*block++ = *src++;
			*block++ = *src++;
			*block++ = *src++;
			*block++ = *src++;
			block += 28;
		}
		if((j & 0x7) == 0x7)
			blockaddress+=rowblocksadd;
	}
	free(img->data);
	img->data = data;
	img->swizzled = 1;*/
	unsigned int pitch = (img->texWidth*sizeof(u32));
	unsigned int i,j;
	unsigned int rowblocks = (pitch / 16);
	long size = pitch  * img->height;
      
	unsigned char* out = (unsigned char*)malloc( size*sizeof(unsigned char) );
    
	for (j = 0; j < img->origHeight; ++j)
	{
		for (i = 0; i < pitch; ++i)
		{
			unsigned int blockx = i / 16;
            unsigned int blocky = j / 8;
   
            unsigned int x = (i - blockx*16);
            unsigned int y = (j - blocky*8);
            unsigned int block_index = blockx + ((blocky) * rowblocks);
            unsigned int block_address = block_index * 16 * 8;
   
            out[block_address + x + y * 16] = img->data[i+j*pitch];
         }
	}
	free( img->data );
	img->data = out;
	img->swizzled = 1;
}

void unswizzleImage(Image* img)
{
	if(img==NULL||!img->swizzled||(img->origHeight%8)!=0)
		return;

	unsigned int pitch = (img->texWidth*sizeof(u32));
	unsigned int i,j;
	unsigned int rowblocks = (pitch / 16);
	long size = img->texWidth * img->texHeight * sizeof(u32);
      
	unsigned char* out = (unsigned char*)malloc( size );
	if(!out)return;
	sceKernelDcacheWritebackAll();
    
	for (j = 0; j < img->origHeight; ++j)
	{
		for (i = 0; i < pitch; ++i)
		{
			unsigned int blockx = i / 16;
            unsigned int blocky = j / 8;
   
            unsigned int x = (i - blockx*16);
            unsigned int y = (j - blocky*8);
            unsigned int block_index = blockx + ((blocky) * rowblocks);
            unsigned int block_address = block_index * 16 * 8;
   
            out[i+j*pitch] = img->data[block_address + x + y * 16];
         }
	}
	free( img->data );
	img->data = out;
	img->swizzled = 0;
}

void createImageVerts(Image* img)
{
	img->vertices = (Vertex*)malloc( 4 * sizeof(Vertex) );
	
	float hPercent = (float)img->height / (float)img->texHeight;
	float wPercent = (float)img->width / (float)img->texWidth;
	
	if( img->vertices )
	{
		img->vertices[0] = getVertex(0.0f,0.0f,0xffFFFFFF, // top-left
			-img->width/2,-img->height/2,0.0f);
		img->vertices[1] = getVertex(0.0f,hPercent,0xffFFFFFF, // bl
			-img->width/2, img->height/2,0.0f);
		img->vertices[2] = getVertex(wPercent,0.0f,0xffFFFFFF, // tr
			 img->width/2,-img->height/2,0.0f);
		img->vertices[3] = getVertex(wPercent,hPercent,0xffFFFFFF,// br
			 img->width/2, img->height/2,0.0f);
	}
}

void freeImage(Image* img)
{
	free(img->data);
	img->data = NULL;
	free(img);
	img = NULL;
}

void drawImage(Image* img, int x, int y, float radians, u8 alpha)
{	
	if(x>480||y>272||x+img->width<0||y+img->height<0||alpha==0||img==NULL)return;
	sceGumPushMatrix();

	ScePspFVector3 loc = {x+img->width/2,y+img->height/2,0.0f};
	sceGumTranslate(&loc);
	if((int)radians!=0)sceGumRotateZ(radians);
	
	if(!alpha)return;
	if(alpha != 255)setAlphaCopy(img, alpha);
	
	sceGuTexMode(GU_PSM_8888,0,0,img->swizzled);
	sceGuTexImage(0,img->texWidth,img->texHeight,img->texWidth,alpha==255?img->data:img->data2);
	sceGumDrawArray(GU_TRIANGLE_STRIP,GU_COLOR_8888 | 
		GU_TEXTURE_32BITF |	GU_VERTEX_32BITF | GU_TRANSFORM_3D, 
		4, 0, img->vertices
		);
	
	sceGumPopMatrix();
	frames++;
	if(frames>800){
		endDrawing();
		startDrawing();
	}
}

void drawImagePart(Image* img, int dx, int dy, int srcX,
	int srcY, int srcW, int srcH, float rads, u8 alpha)
{
	if(img==NULL)return;
	float scaleX = (float)img->width/(float)img->origWidth;
	float scaleY = (float)img->height/(float)img->origHeight;
	srcX*=scaleX;srcY*=scaleY;
	if(dx>480||dy>272||dx+srcW*scaleX<0||dy+srcH*scaleY<0||alpha==0||srcW==0||srcH==0)return;
	const unsigned int WHITE=0xffFFFFFF;
	Vertex *verts = (Vertex*)sceGuGetMemory(sizeof(Vertex) * 4);
	
	float u_Left= ((float)(srcX)/(float)(img->texWidth)) / scaleX;
	float v_Top = ((float)(srcY)/(float)(img->texHeight))/ scaleY;
	float u_Right=((float)(srcX+srcW)/(float)(img->texWidth))/ scaleX;
	float v_Bottom=((float)(srcY+srcH)/(float)(img->texHeight))/scaleY;
	
	int width = srcW;
	int height = srcH;
	float z = img->vertices[0].z;
	
	// -- top left --
	verts[0]=getVertex(u_Left,v_Top,WHITE,-width/2,-height/2,z);
	// -- bottom left --
	verts[1]=getVertex(u_Left,v_Bottom,WHITE,-width/2,height/2,z);
	// -- top right --
	verts[2]=getVertex(u_Right,v_Top,WHITE,width/2,-height/2,z);
	// -- bottom right -- 
	verts[3]=getVertex(u_Right,v_Bottom,WHITE,width/2,height/2,z);
	
	// render the image
	if(!alpha)return;
	if(alpha != 255)setAlphaCopy(img, alpha);
	
	sceGumPushMatrix();
	{
	ScePspFVector3 pos={dx+width/2,dy+height/2,0};
	sceGumTranslate(&pos);
	if((int)rads!=0)sceGumRotateZ(rads);
	sceGuTexMode(GU_PSM_8888,0,0,img->swizzled);
	sceGuTexImage(0,img->texWidth,img->texHeight,
		img->texWidth,alpha==255?img->data:img->data2);
	sceGumDrawArray(GU_TRIANGLE_STRIP,GU_COLOR_8888|GU_TEXTURE_32BITF 
		 |GU_VERTEX_32BITF|GU_TRANSFORM_3D,4, 0, verts);
	}
	sceGumPopMatrix();
	frames++;
	if(frames>800){
		endDrawing();
		startDrawing();
	}
}

// borrowed from graphics.c. i will replace it with something
// that uses memcpy.
//
void blitImage(Image* srcImg, Image* dstImg, int dx, int dy,
					int sx, int sy, int sw, int sh)
{
	int wi=dstImg->origWidth; int hi=dstImg->origHeight;
	if(dstImg==NULL||srcImg==NULL||dx+sw<0||dy+sh<0||dx>wi||dy>hi)return;
	if(dx<0){sx-=dx; sw+=dx; dx=0;}
	if(dy<0){sy-=dy; sh+=dy; dy=0;}
	if(dx+sw>wi){sw=wi-dx;}
	if(dy+sh>hi){sh=hi-dy;}
	
	u32* dst = (u32*)dstImg->data;
	u32* src = (u32*)srcImg->data;
	int x,y;
	
	for(y = 0; y < sh; y++)
	{
		for(x = 0; x < sw; x++)
		{
			dst[(u32)((x+dx) + dstImg->texWidth * (y+dy))] =
				src[(u32)((x+sx) + srcImg->texWidth * (y+sy))]; 
		}
	}
}

void initAlphaCopy(Image* img){
	img->data2=img->data;
}


void setAlphaCopy(Image* img, u8 alpha){
	if(alpha!=img->alpha)img->alpha=alpha;
	unswizzleImage(img);
	u8* addr=(u8*)(img->data)+3;
	int i;
	for(i=3; i<img->origWidth*4+img->origHeight*img->texWidth*4; i+=4, addr+=4)
		*addr=alpha;
	swizzleImage(img);
}

void lineImage(Image* img, int x, int y, int x2, int y2, u32 clr){
	const int w=img->origWidth;
	const int h=img->origHeight;
	const int dx=abs(x2-x);
	const int dy=abs(y2-y);
	const int signx=x<x2?1:-1;
	const int signy=y<y2?1:-1;
	int err=dx-dy;
	setPixelImage(img,x2,y2,clr);
	if((x>=0&&x<w)||(y>=0&&y<h)||(x2>=0&&x2<w)||(y2>=0&&y2<h))
		while((x!=x2||y!=y2)){
			setPixelImage(img,x,y,clr);
			const int err2=err*2;
			if(err2>-dy){
				err-=dy;
				x+=signx;
			}
			if(err2<dx){
				err+=dx;
				y+=signy;
			}
		}
}

void fillCircleImage(Image* img, int x, int y, int r, u32 clr){
	if(r<1)return;
	int wi,w,h;
	wi=img->texWidth;
	w=img->origWidth;
	h=img->origHeight;
	
	int x1=x-r;
	if(x1<0)x1=0;
	int x2=x+r;
	if(x2>=w)x2=w-1;
	int y1=y-r;
	if(y1<0)y1=0;
	int y2=y+r;
	if(y2>=h)y2=h-1;
	if(x2<x1||y2<y1)return;
	
	int rr=r*r+r;
	int dxdx0=(x1-x)*(x1-x);
	int ab0=2*(x1-x)+1;
	int i,d;
	for(i=y1; i<=y2; i++){
		int dd=(i-y)*(i-y)+dxdx0;
		int ab=ab0;
		u32* x0=((u32*)(img->data)+x1+i*wi);
		for(d=x1-x2; d<=0; d++){
			if(dd<rr)*x0=clr;
			x0++;
			dd+=ab;
			ab+=2;
		}
	}
}

void strokeCircleImage(Image* img, int x0, int y0, int r, u32 clr){
	int x=0;
	int y=r;
	int delta=1-2*r;
	int error=0;
	while(y>=0){
		setPixelImage(img, x0+x, y0+y, clr);
		setPixelImage(img, x0+x, y0-y, clr);
		setPixelImage(img, x0-x, y0+y, clr);
		setPixelImage(img, x0-x, y0-y, clr);
		error=2*(delta+y)-1;
		if(delta<0 && error<=0){
			++x;
			delta+=2*x+1;
			continue;
		}
		error=2*(delta-x)-1;
		if(delta>0 && error>0){
			--y;
			delta+=1-2*y;
			continue;
		}
		++x;delta+=2*(x-y);--y;
	}
}

double get_angle(int centerX, int centerY, int pointX, int pointY){
  int x=pointX-centerX; 
  int y=pointY-centerY; 
  if(x==0)return (y>0)?180:0; 
  double a=atan2(y,x)*180/PI; 
  a+=90;
  if(x<0 && y<0)a+=360;
  return a; 
}

void arcImage(Image* img, int x0, int y0, int r, double startAngle, double sweepAngle, u32 color){
  startAngle=abs(startAngle);
  sweepAngle=abs(sweepAngle);
  if(startAngle>=360)startAngle=0;
  if(sweepAngle>360)sweepAngle=360;
  if(startAngle+sweepAngle>360){
    arcImage(img, x0, y0, r, 0, startAngle+sweepAngle-360, color);
    sweepAngle=360-startAngle;
  }
  int x=0;
  int y=r;
  int gap=0;
  int delta=(2-2*r);
  double angle;
  while(y>=0){
    angle=get_angle(x0,y0,x0+x,y0-y);
    if(angle>=startAngle && angle<=startAngle+sweepAngle){
      if(0<=angle && angle<=90)setPixelImage(img, x0+x, y0-y, color);
    }
    angle=get_angle(x0, y0, x0+x, y0+y);
    if(angle>=startAngle && angle<=startAngle+sweepAngle){
      if(90<angle && angle<=180)setPixelImage(img, x0+x, y0+y, color);
    }
    angle=get_angle(x0, y0, x0-x, y0+y);
    if(angle >= startAngle && angle <= startAngle + sweepAngle){
      if(180<angle && angle<=270)setPixelImage(img, x0-x, y0+y, color);
    }
    angle=get_angle(x0, y0, x0-x, y0-y);
    if(angle>=startAngle && angle<=startAngle+sweepAngle){
      if(270<angle && angle<=360)setPixelImage(img, x0-x, y0-y, color);
    }

    gap=2*(delta+y)-1;
    if(delta<0 && gap<=0){
      x++;
      delta+=2*x+1;
      continue;
    }
    if(delta>0 && gap>0){
      y--;
      delta-=2*y+1;
      continue;
    }
    x++;delta+=2*(x-y);y--;
  }
}

/*
void flipImage(Image* img, u8 dx, u8 dy){
	int i,x,y;
	unswizzleImage(img);
	Image* data;
	if(dx){
		data=img;
		for(y=0;y<img->texHeight;y++)
			for(i=0,x=img->texWidth-1;x>=0;x--,i++)
				*((u32*)img->data+i+y*img->texWidth)=*((u32*)data->data+x+y*img->texWidth);
	}
	if(dy){
		data=img;
		for(i=0,y=img->texHeight-1;y>=0;y--,i++)
			for(x=0;x<img->texWidth;x++)
				*((u32*)img->data+x+i*img->texWidth)=*((u32*)data->data+x+y*img->texWidth);
	}
	freeImage(data);
	swizzleImage(img);
}*/

void clearImage(Image* img, u32 clr){
	u32* addr=(u32*)(img->data);
	int i;
	for(i=0; i<img->origWidth+img->origHeight*img->texWidth; i++, addr++)
		*addr=clr;
}

void changeColorImage(Image* img, u32 clr, u32 clr2){
	if(clr==clr2)return;
	u32* addr=(u32*)(img->data);
	int i;
	for(i=0; i<img->origWidth+img->origHeight*img->texWidth; i++, addr++)
		if(*addr==clr)*addr=clr2;
}

void changeNearColorImage(Image* img, int x, int y, u32 clr, u32 clr2){
	//*
	if(*((u32*)(img->data)+x+y*img->texWidth)==clr2||clr==clr2||x<0||y<0||x>=img->origWidth||y>=img->origHeight)return;
	int** stack = Array(int*,480*272);
	stack[0]=Array(int,2);
	stack[0][0]=x;
	stack[0][1]=y; 
	int spanLeft = 0; 
	int spanRight = 0;
	int size=1;
	
	while(size!=0){
		x=stack[size-1][0];
		y=stack[size-1][1];
		free(stack[size-1]);stack[size-1]=NULL;size--;
		int y1=y;
		
		while(y1>=0 && *((u32*)(img->data)+x+y1*img->texWidth)==clr)
			y1=y1-1;
		y1=y1+1;
		spanLeft = 0; spanRight = 0;
		
		while(y1<img->origHeight && *((u32*)(img->data)+x+y1*img->texWidth)==clr){
			*((u32*)(img->data)+x+y1*img->texWidth)=clr2;
			if(spanLeft==0 && x>0 && *((u32*)(img->data)+x-1+y1*img->texWidth)==clr){
				stack[size]=Array(int,2);
				stack[size][0]=x-1;
				stack[size][1]=y1;
				size++;
				spanLeft=1;
			}else if(spanLeft==1 && x>0 && *((u32*)(img->data)+x-1+y1*img->texWidth)!=clr)
					spanLeft = 0;
			
			if(spanRight == 0 && x<img->origWidth && *((u32*)(img->data)+x+1+y1*img->texWidth)==clr){
				stack[size]=Array(int,2);
				stack[size][0]=x+1;
				stack[size][1]=y1;
				size++;
				spanRight=1;
			}else if(spanRight==1 && x<img->origWidth && *((u32*)(img->data)+x+1+y1*img->texWidth)!=clr)
				spanRight = 0;
			
			y1=y1+1;
		}
	}
}

void swapColorImage(Image* img, u32 clr, u32 clr2){
	if(clr==clr2)return;
	u32* addr=(u32*)(img->data);
	int i;
	for(i=0; i<img->origWidth+img->origHeight*img->texWidth; i++, addr++){
		if(*addr==clr)*addr=clr2;
		else if(*addr==clr2)*addr=clr;
	}
}

void fillRectImage(Image* img, int x, int y, int w, int h, u32 clr){
	int wi=img->origWidth,hi=img->origHeight;
	if(w<1||h<1||x>wi||x+w<0||y>hi||y+h<0)return;
	int x2=x,y2=y,w2=w,h2=h;
	if(x+w>wi)w2=wi-x;if(y+h>hi)h2=hi-y;
	if(y<0){h2+=y2;y2=0;}if(x<0){w2+=x2;x2=0;}
	int i, i2, skipX=img->texWidth-w2;
	u32* addr=(u32*)(img->data)+x2+y2*img->texWidth;
	for(i2=0; i2<h2; i2++, addr+=skipX)
		for(i=0; i<w2; i++, addr++)
			*addr=clr;
}

void strokeRectImage(Image* img, int x, int y, int w, int h, u32 clr){
	w--;h--;
	int wi=img->origWidth,hi=img->origHeight;
	if(w<1||h<1||x>wi||x+w<0||y>hi||y+h<0)return;
	int x2=x,y2=y,w2=w,h2=h;
	if(x+w>wi)w2=wi-x;if(y+h>hi)h2=hi-y;
	if(y<0){h2+=y2;y2=0;}if(x<0){w2+=x2;x2=0;}
	int i, skipX=img->texWidth;
	
	//[0,0]->[1,0]
	u32* addr=(u32*)(img->data)+x2+y2*img->texWidth;//[0,0]
	if(y==y2)for(i=0; i<w2; i++, addr++)*addr=clr;//[1,0]
	else addr+=w2;
	
	//[1,0]->[1,1]
	if(w==w2||x!=x2)for(i=0; i<h2; i++, addr+=skipX)*addr=clr;//[1,1]
	else addr+=skipX*h2;
	
	//[1,1]->[0,1]
	if(h==h2||y!=y2)for(i=0; i<w2+(x!=x2); i++, addr--)*addr=clr;//[0,1]
	else addr-=w2;
	
	//[0,1]->[0,0]
	if(x==x2)for(i=0; i<h2+(y!=y2); i++, addr-=skipX)*addr=clr;//[0,0]
}

void setPixelImage(Image* img, int x, int y, u32 clr){
	if(x<0||y<0||x>=img->origWidth||y>=img->origHeight)return;
	*((u32*)(img->data)+x+y*img->texWidth)=clr;
}

u32 getPixelImage(Image* img, int x, int y){
	if(x<0||y<0||x>=img->origWidth||y>=img->origHeight)return 0xFFFFFFFF;//white
	return *((u32*)(img->data)+x+y*img->texWidth);
}

void setImageZ(Image* img, float z)
{
	if(img==NULL) 
		return;
		
	img->vertices[0].z = z;
	img->vertices[1].z = z;
	img->vertices[2].z = z;
	img->vertices[3].z = z;
}

void stretchImage(Image* img, float xScale, float yScale)
{
	int i;
	
	if(img==NULL)
		return;
		
	for(i=0; i<4; i++)
	{
		img->vertices[i].x = img->vertices[i].x * xScale;
		img->vertices[i].y = img->vertices[i].y * yScale;
	}
	
	// as long as all tex operations deal with texWidth/Height then
	// this should be ok to do
	//
	img->width = img->width * xScale;
	img->height = img->height * yScale;
}

void restoreImage(Image* img)
{
	if( img == NULL )
		return;
	img->width = img->origWidth;
	img->height = img->origHeight;
	
	// restore the vertices
	
	img->vertices[0].x = -img->width/2; // top left
	img->vertices[0].y = -img->height/2; //
	
	img->vertices[1].x = -img->width/2; // bottom left
	img->vertices[1].y = img->height/2; //
	
	img->vertices[2].x = img->width/2; // top right
	img->vertices[2].y = -img->height/2; //
	
	img->vertices[3].x = img->width/2; // bottom right
	img->vertices[3].y = img->height/2; //
	
	
}

u32 getImagePixelAt(Image* img, int x, int y)
{
	u32 pixel=0;
	u32 *data;
	unswizzleImage(img);
	data = (u32*)(img->data);
	pixel = data[y*img->texWidth + x];
	swizzleImage(img);
	return pixel;
}

void setImagePixelAt(Image* img, int x, int y, u32 color)
{
	u32* data = NULL;
	
	unswizzleImage(img);
	
	data = (u32*)(img->data);
	data[(y * (img->texWidth)) + x] = color;
	
	swizzleImage(img);
}

// taken from graphics.c
//
int nextPowOf2(int dimension)
{
	int b = dimension;
	int n;
	for (n = 0; b != 0; n++) b >>= 1;
	b = 1 << n;
	if (b == 2 * dimension) b >>= 1;
	return b;
}

void initGraphics()
{	
	g_dlist = malloc(262144);
	g_dispNumber = 0;
	g_vram_base = (u32*) (0x40000000 | 0x04000000);
	fbp0 = NULL;
	fbp1 = (void*)0x88000;
	
	sceGuInit();
 
	sceGuStart(GU_DIRECT,g_dlist);
	sceGuDrawBuffer( GU_PSM_8888, fbp0, BUF_WIDTH );
	sceGuDispBuffer( SCR_WIDTH, SCR_HEIGHT, fbp1, BUF_WIDTH);
	sceGuDepthBuffer( (void*)0x110000, BUF_WIDTH );
	
	sceGuOffset(2048 - (SCR_WIDTH/2),2048 - (SCR_HEIGHT/2));
	sceGuViewport(2048,2048,SCR_WIDTH,SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,SCR_WIDTH,SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuFrontFace(GU_CCW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuAlphaFunc(GU_GREATER,0,0xff);
	sceGuEnable(GU_CULL_FACE); 
	sceGuEnable(GU_DEPTH_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_ALPHA_TEST);
	sceGuEnable(GU_TEXTURE_2D);
	
	sceGuTexMode(GU_PSM_8888,0,0,1);
	sceGuTexFunc(GU_TFX_REPLACE,GU_TCC_RGBA);
	sceGuTexFilter(GU_NEAREST,GU_NEAREST);
	sceGuTexWrap(GU_CLAMP,GU_CLAMP);
	sceGuTexScale(1,1);
	sceGuTexOffset(0,0);
	sceGuAmbientColor(0xffffffff);
	
	//for font working
	sceGuEnable(GU_BLEND);
	sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
	
	sceGumMatrixMode(GU_PROJECTION);
	sceGumLoadIdentity();
	sceGumOrtho(0,480,272,0,-1,1);
	
	sceGumMatrixMode(GU_VIEW);
	sceGumLoadIdentity();

	sceGumMatrixMode(GU_MODEL);
	sceGumLoadIdentity();
	
	sceGuFinish();
	sceGuSync(0,0);
	sceDisplayWaitVblankStart();
	sceGuDisplay(1);
	frames=0;
}

void endGraphics()
{
	sceGuTerm();
}

void startDrawing()
{
	sceGuStart(GU_DIRECT,g_dlist);
	
	//sceGuClearColor(0);
	//sceGuClearDepth(0);
	//sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
}

void endDrawing()
{
	sceGuFinish();
	sceGuSync(0,0);
	frames=0;
}

void clearScreen(u32 clr)
{
	sceGuClearColor(clr);
	sceGuClearDepth(0);
	sceGuClear(GU_COLOR_BUFFER_BIT|GU_DEPTH_BUFFER_BIT);
}

void flipScreen()
{
	fbp0 = sceGuSwapBuffers();
	g_dispNumber ^= 1;
}

u32* getVramDrawBuffer()
{
	u32* vram = (u32*) g_vram_base;
	if (g_dispNumber== 0) 
		vram += FRAMEBUFFER_SIZE / sizeof(u32);
	return vram;
}

// adapted from graphics.c
//
void printTextScreen(const char* text, int x, int y, u32 color)
{
	int c, i, j, l;
	u8 *font;
	u32 *vram_ptr;
	u32 *vram;

	sceGuFinish();
	sceGuSync(0,0);

	sceGuStart(GU_DIRECT,g_dlist);

	for (c = 0; (unsigned)c < strlen(text); c++) 
	{
		if (x < 0 || x + 8 > SCR_WIDTH || y < 0 || y + 8 > SCR_HEIGHT) 
			break;
		char ch = text[c];
		
		//vram = getVramDrawBuffer() //+(unsigned int)(fbp0) 
			//	+ x + y * PSP_LINE_SIZE;
		if( g_dispNumber == 0 )
			vram = g_vram_base + (unsigned long)(fbp0)
				+ x + (y * PSP_LINE_SIZE);
		else
			vram = g_vram_base + FRAMEBUFFER_SIZE / sizeof(u32)
				+ x + (y * PSP_LINE_SIZE);
		
		font = &msx[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) 
		{
			vram_ptr  = vram;
			for (j = 0; j < 8; j++) 
			{
				if ((*font & (128 >> j))) *vram_ptr = color;
				vram_ptr++;
			}
			vram += PSP_LINE_SIZE;
		}
		x += 8;
	}

}

// also borrowed from graphics.c
//
void printTextImage(const char* text, int x, int y, u32 color, 
					Image* img)
{
	int c, i, j, l;
	u8 *font;
	u32 *data_ptr;
	u32 *data;
	
	unswizzleImage(img);

	for (c = 0; c < strlen(text); c++) 
	{
		
		if (x < 0 || (x + 8) > img->origWidth || y < 0 
			|| y + 8 > (img->origHeight)) 
		{
			break;
		}
		
		char ch = text[c];
		data = (u32*)img->data + x + y * img->texWidth;
		
		font = &msx[ (int)ch * 8];
		for (i = l = 0; i < 8; i++, l += 8, font++) 
		{
			data_ptr  = data;
			for (j = 0; j < 8; j++) {
				if ((*font & (128 >> j))) *data_ptr = color;
				data_ptr++;
			}
			data += img->texWidth;
		}
		x += 8;
	}
	
	swizzleImage(img);
}