#ifndef __WALLS_H__
#define __WALLS_H__
#ifdef __cplusplus
extern "C" {
#endif

#define WALLS_COUNT 3

typedef unsigned char wts;//wall type size (u8, byte)

typedef struct Wall_t
{
	wts id;//byte
	u8 wall_type;//byte
	u8 can_over;//byte
} Wall;

Wall getWallInfo(wts id);

Wall wall_air(void);
Wall wall_dirt(void);
Wall wall_stone(void);

#ifdef __cplusplus
}
#endif
#endif