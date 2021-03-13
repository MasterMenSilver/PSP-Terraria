#include "main.h"
#include "items.h"

Item* getItemInfo(u16 id){
	switch(id){
		case 0:return item_none();break;
		case 1:return item_dirt();break;
		case 2:return item_stone();break;
		case 3:return item_glass();break;
	}
	return item_none();
}

Item* item_none(void){
	Item* rez=(Item*)malloc(sizeof(Item));
	rez->id=0;
	rez->type=0;//block/wall (1/0)
	rez->id0=0;//real id (for block/wall)
	rez->stackable=0;
	rez->count=0;
	rez->name="";
	return rez;
}
Item* item_dirt(void){
	Item* rez=(Item*)malloc(sizeof(Item));
	rez->id=1;
	rez->type=1;
	rez->id0=1;
	rez->stackable=1;
	rez->count=1;
	rez->name="dirt";
	return rez;
}
Item* item_stone(void){
	Item* rez=(Item*)malloc(sizeof(Item));
	rez->id=2;
	rez->type=1;
	rez->id0=3;
	rez->stackable=1;
	rez->count=1;
	rez->name="stone";
	return rez;
}
Item* item_glass(void){
	Item* rez=(Item*)malloc(sizeof(Item));
	rez->id=3;
	rez->type=1;
	rez->id0=7;
	rez->stackable=1;
	rez->count=1;
	rez->name="glass";
	return rez;
}

