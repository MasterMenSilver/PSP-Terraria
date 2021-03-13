// graphics2d.h
//--------------------------------------------------------------------
// Author: the_darkside_986
// Date Created: Jun 17, 2007
// Date Modified: Jun 17, 2007
//--------------------------------------------------------------------
// Purpose: 
//	Contains all struct and method declarations for the Easy
// 	Accelerated Image lib.
//--------------------------------------------------------------------

#ifndef GRAPHICS_2D_H
#define GRAPHICS_2D_H

#include <psptypes.h>

// structs
//
typedef struct Vertex_t
{
	float u,v; // tex coords
	unsigned int color; // 32-bit color
	float x,y,z;
}
Vertex;

Vertex getVertex(float u,float v,u32 color,float x, float y,float z);

typedef struct Image_t
{
	int width, height, texWidth, texHeight;
	float scX, scY;
	int origWidth, origHeight; // to restore after scaling
	int colorMode;
	unsigned char alpha;
	unsigned char* data;
	unsigned char* data2;
	unsigned char swizzled; // true or false value
	
	Vertex* vertices; // always will be an array of 4 or NULL
	
} Image;

// newImage - create solid color image
//--------------------------------------------------------
Image* newImage(int width, int height, unsigned int color);

// loadImage - load image from a png file
//-----------------------------------------
Image* loadImage(const char fileName[]);

// swizzleImage - swizzles the texture
//----------------------------------------
void swizzleImage(Image* img);

// unswizzleImage - reverse of swizzle
//------------------------------------------
void unswizzleImage(Image* img);

// createImageVerts
//-------------------------------------
void createImageVerts(Image* img);

// freeImage - free image from memory
//---------------------------------------
void freeImage(Image* img);

// drawImage - draws the image to the screen by using the shape
// 			primitive GU_TRIANGLE_STRIP with the texture on it
//------------------------------------------------------------
void drawImage(Image* img, int x, int y, float radians, u8 alpha);//original was without "alpha"

// drawImagePart - draws part of the image.
//----------------------------------------------------
void drawImagePart(Image* img, int dx, int dy, int srcX,
					int srcY, int srcW, int srcH, float radians, u8 alpha);//original was with ", float radians"
					
//added by another people
void setAlphaCopy(Image* img, u8 alpha);//change alpha of data2
void initAlphaCopy(Image* img);//copy data to data2

void setPixelImage(Image* img, int x,int y, u32 clr);
u32 getPixelImage(Image* img, int x, int y);

void clearImage(Image* img, u32 clr);
void changeColorImage(Image* img, u32 clr, u32 clr2);
void changeNearColorImage(Image* img, int x, int y, u32 clr, u32 clr2);
void swapColorImage(Image* img, u32 clr, u32 clr2);

void lineImage(Image* img, int x, int y, int w, int h, u32 clr);
void fillCircleImage(Image* img, int x, int y, int r, u32 clr);
void strokeCircleImage(Image* img, int x0, int y0, int r, u32 clr);
double get_angle(int centerX, int centerY, int pointX, int pointY);
void arcImage(Image* img, int x0, int y0, int r, double startAngle, double sweepAngle, u32 color);
void fillRectImage(Image* img, int x, int y, int w, int h, u32 clr);
void strokeRectImage(Image* img, int x, int y, int w, int h, u32 clr);

// drawImageToImage - copies pixels from an unswizzled image to
//		another unswizzled images (not implemented yet). Unswizzling
//		is automatically done as necessary.
//---------------------------------------------------------------
void blitImage(Image* srcImg, Image* dstImg, int dx, int dy,
					int srcX, int srcY, int srcW, int srcH);
					
// setImageZ - sets the z location of the image. The image
//	with the lowest Z is drawn below other images. The Z
//	value MUST be between -1 and 1. It can be a fraction, such as
//	0.31, -0.6, etc., of course.
//----------------------------------------------------------------
void setImageZ(Image* img, float z);

// stretchImage - stretches the image by modifying the coord
//		locations.
//----------------------------------------------------------
void stretchImage(Image* img, float xScale, float yScale);

// restoreImage - restores to original scale after scaling
//--------------------------------------------------------
void restoreImage(Image* img);

// getImagePixel - gets the specified pixel from an image. The image
//		will first be unswizzled and then re-swizzled if necessary
//-----------------------------------------------------------------
u32 getImagePixelAt(Image* img, int x, int y);

// setImagePixel - sets the color value at pixel (x,y) to the value
//		of "color." Swizzled images will be unswizzled and then
//		reswizzled.
//--------------------------------------------------------------
void setImagePixelAt(Image* img, int x,int y, u32 color);

// nextPowOf2 - used to find a suitable texture height that is a power
//	of two.
//-------------------------------------------------------------------
int nextPowOf2(int dimension);

// graphics functions and macros
//

#define BUF_WIDTH (512)
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)
#define	PSP_LINE_SIZE 512

// initGraphics - this must be called before using the functions
//--------------------------------------------------------------
void initGraphics();

// endGraphics - call this at the end of the program
//--------------------------------------------------
void endGraphics();

// startDrawing - all calls to drawImage and drawImagePart and 
//		printTextScreen must go between startDrawing and endDrawing
//-----------------------------------------------------------------
void startDrawing();

// endDrawing - all calls to drawImage and drawImagePart and 
//		printTextScreen must go between startDrawing and endDrawing
//-----------------------------------------------------------------
void endDrawing();

// clearScreen - ignore this because the screen is always cleared
//		by default in startDrawing().
//--------------------------------------------------------------
void clearScreen(u32 clr);

// flipScreen - this must be called after endDrawing so that the
//		images will appear on the screen
//--------------------------------------------------------------
void flipScreen();

// printTextScreen - prints text at a specified location on the screen
//		this also should be between start and end Drawing()
//------------------------------------------------------------------
void printTextScreen(const char* text, int x, int y, u32 color);

// printTextImage - prints text to an unswizzled image. If the image
//		is swizzled, it must be unswizzled first and then re-swizzled.
//------------------------------------------------------------------
void printTextImage(const char*, int x, int y, u32, Image* image);

#endif