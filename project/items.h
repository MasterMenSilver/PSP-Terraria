#ifndef __ITEMS_H__
#define __ITEMS_H__
#ifdef __cplusplus
extern "C" {
#endif

#define ITEMS_COUNT 4

typedef struct Item_t
{
	u16 id;//short
	u8 type;//bool
	u16 id0;//short
	u8 stackable;//bool
	u16 count;//when init
	char* name;
} Item;

Item* getItemInfo(u16 id);

Item* item_none(void);//1
Item* item_dirt(void);//2
Item* item_stone(void);//3
Item* item_glass(void);//4

#ifdef __cplusplus
}
#endif
#endif