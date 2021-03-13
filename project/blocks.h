#ifndef __BLOCKS_H__
#define __BLOCKS_H__
#ifdef __cplusplus
extern "C" {
#endif

#define BLOCKS_COUNT 8

typedef struct Block_t
{
	u16 id;//short
	u8 tile_type;//byte
	u8 tile_size;//byte
	u8 collision;//bool
	u8 transparent;//bool
} Block;

Block getBlockInfo(u16 id);

Block block_air(void);//1
Block block_dirt(void);//2
Block block_grass(void);//3
Block block_stone(void);//4
Block block_tree(void);
Block block_tree_top(void);
Block block_tree_branch(void);
Block block_glass(void);//8

#ifdef __cplusplus
}
#endif
#endif