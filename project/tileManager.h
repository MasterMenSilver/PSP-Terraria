#ifndef __TILE_MANAGER_H__
#define __TILE_MANAGER_H__
#ifdef __cplusplus
extern "C" {
#endif

u16 convertile(u16 id, u16 convId);
void setTile(Image*,int x, int y);
void setWall(Image*,int x, int y);

#ifdef __cplusplus
}
#endif
#endif