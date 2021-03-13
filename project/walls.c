#include "main.h"
#include "walls.h"

Wall getWallInfo(wts id){
	switch(id){
		case 0:return wall_air();break;
		case 1:return wall_dirt();break;
		case 2:return wall_stone();break;
	}
	return wall_air();
}

Wall wall_air(void){
	//tile_type,collision
	Wall rez={
		.id=0,
		.wall_type=0,
		.can_over=0
	};
	return rez;
}
Wall wall_dirt(void){
	Wall rez={
		.id=1,
		.wall_type=1,
		.can_over=1
	};
	return rez;
}
Wall wall_stone(void){
	Wall rez={
		.id=2,
		.wall_type=1,
		.can_over=1
	};
	return rez;
}