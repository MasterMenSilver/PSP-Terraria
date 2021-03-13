#ifndef __CAMERA_H
#define __CAMERA_H
#ifdef __cplusplus
extern "C" {
#endif

void draw(Image* img, int x, int y);
void drawPart(Image* img, int x, int y, int sx, int sy, int w, int h);
void focus(Image* img, int x, int y, int x0, int y0, int w0, int h0);
void focus0(int w, int h, int x, int y, int x0, int y0, int w0, int h0);

#ifdef __cplusplus
}
#endif
#endif