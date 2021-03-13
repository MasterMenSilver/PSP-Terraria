#include "main.h"
#include "blocks.h"

Block getBlockInfo(u16 id){
	switch(id){
		case 0:return block_air();break;
		case 1:return block_dirt();break;
		case 2:return block_grass();break;
		case 3:return block_stone();break;
		case 4:return block_tree();break;
		case 5:return block_tree_top();break;
		case 6:return block_tree_branch();break;
		case 7:return block_glass();break;
	}
	return block_air();
}

Block block_air(void){
	//tile_type,collision
	Block rez={
		.id=0,
		.tile_type=0,
		.collision=0,
		.transparent=1,
		.tile_size=0
	};
	return rez;
}
Block block_dirt(void){
	Block rez={
		.id=1,
		.tile_type=1,
		.collision=1,
		.transparent=0,
		.tile_size=16
	};
	return rez;
}
Block block_grass(void){
	Block rez={
		.id=2,
		.tile_type=2,
		.collision=1,
		.transparent=0,
		.tile_size=16
	};
	return rez;
}
Block block_stone(void){
	Block rez={
		.id=3,
		.tile_type=3,
		.collision=1,
		.transparent=0,
		.tile_size=16
	};
	return rez;
}
Block block_tree(void){
	Block rez={
		.id=4,
		.tile_type=4,
		.collision=0,
		.transparent=1,
		.tile_size=20
	};
	return rez;
}
Block block_tree_top(void){
	Block rez={
		.id=5,
		.tile_type=5,//single(dx) tile
		.collision=0,
		.transparent=1,
		.tile_size=80
	};
	return rez;
}
Block block_tree_branch(void){
	Block rez={
		.id=6,
		.tile_type=6,
		.collision=0,
		.transparent=1,
		.tile_size=40
	};
	return rez;
}
Block block_glass(void){
	Block rez={
		.id=7,
		.tile_type=1,
		.collision=1,
		.transparent=1,
		.tile_size=16
	};
	return rez;
}