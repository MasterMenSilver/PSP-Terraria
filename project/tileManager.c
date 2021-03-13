#include "main.h"
#include "game.h"
#include "blocks.h"
#include "walls.h"
#include "project/custom_libs/camera.h"
#include "tileManager.h"

u16 convertile(u16 id, u16 convId){
	if(convId==id)return convId;
	switch(id){
		case 0:return 0;break;//air
		case 1://dirt
			switch(convId){
				case 2:case 3:return 1;
				default:return 0;break;
			}
		break;
		case 2://grass
			switch(convId){
				case 7:return 2;
				case 1:case 3:return 1;
				default:return 0;break;
			}
		break;
		case 3://stone
			switch(convId){
				case 7:return 3;
				case 1:case 2:return 1;
				default:return 0;break;
			}
		break;
		case 4://tree
			switch(convId){
				case 1:case 2:return 1;
				case 4:case 5:case 6:return 4;
				default:return 0;break;
			}
		break;
		case 5:
			switch(convId){
				case 4:return 4;
				default:return 0;break;
			}
		break;
		case 6:
			switch(convId){
				case 4:return 4;
				default:return 0;break;
			}
		break;
		case 7://glass
			switch(convId){
				case 0:case 1:case 4:
				case 5:case 6:return 0;
				default:return 7;break;
			}
		break;
	}
	return 0;
}

void setTile(Image* tile, int x, int y){
	int chX,chY;
	int i=(x+y*WORLD_W)%3;
	chX=9+i;
	chY=3;
	
	float sox=0.0f;//setOffsetX(blocks)
	float soy=0.0f;
	
	u16 c=*getBlock(x,y);
	Block cBlock=getBlockInfo(c);
	u16 l=convertile(c,*getBlock(x-1,y));
	u16 r=convertile(c,*getBlock(x+1,y));
	u16 u=convertile(c,*getBlock(x,y-1));
	u16 d=convertile(c,*getBlock(x,y+1));
	
	switch(cBlock.tile_type){
		case 0:return;break;
		case 1://dirt
			{//dirt-not dirt
			if (u != 0 && d != 0 && l != 0 && r != 0)//1111
			{
				chX=1+i;
				chY=1;
			}
			else if (u == 0 && d == 0 && l == 0 && r == 0)//0000
			{
				chX=9+i;
				chY=3;
			}
			else if (u == 0 && d == 0 && l == 0 && r != 0)//0001
			{
				chX=9;
				chY=i;
			}
			else if (u == 0 && d == 0 && l != 0 && r == 0)//0010
			{
				chX=12;
				chY=i;
			}
			else if (u == 0 && d != 0 && l == 0 && r == 0)//0100
			{
				chX=6+i;
				chY=0;
			}
			else if (u != 0 && d == 0 && l == 0 && r == 0)//1000
			{
				chX=6+i;
				chY=3;
			}
			else if (u == 0 && d == 0 && l != 0 && r != 0)//0011
			{
				chX=6+i;
				chY=4;
			}
			else if (u == 0 && d != 0 && l == 0 && r != 0)//0101
			{
				chX=0+i*2;
				chY=3;
			}
			else if (u == 0 && d != 0 && l != 0 && r == 0)//0110
			{
				chX=1+i*2;
				chY=3;
			}
			else if (u == 0 && d != 0 && l != 0 && r != 0)//0111
			{
				chX=1+i;
				chY=0;
			}
			else if (u != 0 && d == 0 && l == 0 && r != 0)//1001
			{
				chX=0+i*2;
				chY=4;
			}
			else if (u != 0 && d == 0 && l != 0 && r == 0)//1010
			{
				chX=1+i*2;
				chY=4;
			}
			else if (u != 0 && d == 0 && l != 0 && r != 0)//1011
			{
				chX=1+i;
				chY=2;
			}
			else if (u != 0 && d != 0 && l == 0 && r == 0)//1100
			{
				chX=5;
				chY=i;
			}
			else if (u != 0 && d != 0 && l == 0 && r != 0)//1101
			{
				chX=0;
				chY=i;
			}
			else if (u != 0 && d != 0 && l != 0 && r == 0)//1110
			{
				chX=4;
				chY=i;
			}
			}
		break;
		case 2://grass (builded with self html/js builder)
			if(l==c){
				if(r==c){
					if(u==c){
						if(d==c){
							chX=8+i*3;chY=13;
						}else if(d==1){
							chX=6+i;chY=1;
						}else{
							chX=8+i*3;chY=14;
						}
					}else if(u==1){
						if(d==c){
							chX=6+i;chY=2;
						}else if(d==1){
							chX=8+i;chY=10;
						}else{
							chX=8+i*3;chY=14;
						}
					}else{
						if(d==c){
							chX=8+i*3;chY=12;
						}else if(d==1){
							chX=1+i;chY=0;
						}else{
							chX=6+i;chY=4;
						}
					}
				}else if(r==1){
					if(u==c){
						if(d==c){
							chX=10;chY=0+i;
						}else if(d==1){
							chX=3;chY=6+i*2;
						}else{
							chX=8+i*3;chY=14;
						}
					}else if(u==1){
						if(d==c){
							chX=3;chY=5+i*2;
						}else if(d==1){
							chX=11+i;chY=17;
						}else{
							chX=3+i;chY=12;
						}
					}else{
						if(d==c){
							chX=8+i*3;chY=12;
						}else if(d==1){
							chX=3+i;chY=11;
						}else{
							chX=3+i;chY=14;
						}
					}
				}else{
					if(u==c){
						if(d==c){
							chX=9+i*3;chY=13;
						}else if(d==1){
							chX=9+i*3;chY=13;
						}else{
							chX=9+i*3;chY=14;
						}
					}else if(u==1){
						if(d==c){
							chX=9+i*3;chY=13;
						}else if(d==1){
							chX=9+i*3;chY=13;
						}else{
							chX=1+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=9+i*3;chY=12;
						}else if(d==1){
							chX=1+i*2;chY=3;
						}else{
							chX=12;chY=0+i;
						}
					}
				}
			}else if(l==1){
				if(r==c){
					if(u==c){
						if(d==c){
							chX=11;chY=0+i;
						}else if(d==1){
							chX=2;chY=6+i*2;
						}else{
							chX=8+i*3;chY=14;
						}
					}else if(u==1){
						if(d==c){
							chX=2;chY=5+i*2;
						}else if(d==1){
							chX=8+i;chY=17;
						}else{
							chX=0+i;chY=12;
						}
					}else{
						if(d==c){
							chX=8+i*3;chY=12;
						}else if(d==1){
							chX=0+i;chY=11;
						}else{
							chX=0+i;chY=14;
						}
					}
				}else if(r==1){
					if(u==c){
						if(d==c){
							chX=10;chY=7+i;
						}else if(d==1){
							chX=11+i;chY=16;
						}else{
							chX=8+i*3;chY=14;
						}
					}else if(u==1){
						if(d==c){
							chX=11+i;chY=15;
						}else if(d==1){
							chX=1+i;chY=1;
						}else{
							chX=2+i;chY=16;
						}
					}else{
						if(d==c){
							chX=8+i*3;chY=12;
						}else if(d==1){
							chX=2+i;chY=15;
						}else{
							chX=9+i;chY=11;
						}
					}
				}else{
					if(u==c){
						if(d==c){
							chX=9+i*3;chY=13;
						}else if(d==1){
							chX=5;chY=6+i;
						}else{
							chX=1+i*2;chY=4;
						}
					}else if(u==1){
						if(d==c){
							chX=5;chY=8+i;
						}else if(d==1){
							chX=1;chY=15+i;
						}else{
							chX=1+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=1+i*2;chY=3;
						}else if(d==1){
							chX=1+i*2;chY=3;
						}else{
							chX=0+i;chY=13;
						}
					}
				}
			}else{
				if(r==c){
					if(u==c){
						if(d==c){
							chX=7+i*3;chY=13;
						}else if(d==1){
							chX=7+i*3;chY=13;
						}else{
							chX=7+i*3;chY=14;
						}
					}else if(u==1){
						if(d==c){
							chX=7+i*3;chY=13;
						}else if(d==1){
							chX=7+i*3;chY=13;
						}else{
							chX=0+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=7+i*3;chY=12;
						}else if(d==1){
							chX=0+i*2;chY=3;
						}else{
							chX=9;chY=0+i;
						}
					}
				}else if(r==1){
					if(u==c){
						if(d==c){
							chX=7+i*3;chY=13;
						}else if(d==1){
							chX=4;chY=5+i;
						}else{
							chX=0+i*2;chY=4;
						}
					}else if(u==1){
						if(d==c){
							chX=4;chY=8+i;
						}else if(d==1){
							chX=0;chY=15+i;
						}else{
							chX=0+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=0+i*2;chY=3;
						}else if(d==1){
							chX=0+i*2;chY=3;
						}else{
							chX=3+i;chY=13;
						}
					}
				}else{
					if(u==c){
						if(d==c){
							chX=5;chY=0+i;
						}else if(d==1){
							chX=7;chY=5+i;
						}else{
							chX=6+i;chY=3;
						}
					}else if(u==1){
						if(d==c){
							chX=7;chY=8+i;
						}else if(d==1){
							chX=6;chY=12+i;
						}else{
							chX=6;chY=8+i;
						}
					}else{
						if(d==c){
							chX=6+i;chY=0;
						}else if(d==1){
							chX=6;chY=5+i;
						}else{
							chX=9+i;chY=3;
						}
					}
				}
			}
		break;
		case 3://stone
			if(l==c){
				if(r==c){
					if(u==c){
						if(d==c){
							chX=1+i;chY=1;
						}else if(d==1){
							chX=8+i;chY=5;
						}else{
							chX=1+i;chY=2;
						}
					}else if(u==1){
						if(d==c){
							chX=8+i;chY=6;
						}else if(d==1){
							chX=8+i;chY=10;
						}else{
							chX=13+i;chY=1;
						}
					}else{
						if(d==c){
							chX=1+i;chY=0;
						}else if(d==1){
							chX=13+i;chY=0;
						}else{
							chX=6+i;chY=4;
						}
					}
				}else if(r==1){
					if(u==c){
						if(d==c){
							chX=8;chY=7+i;
						}else if(d==1){
							chX=3;chY=6+i*2;
						}else{
							chX=3+i;chY=12;
						}
					}else if(u==1){
						if(d==c){
							chX=0;chY=8+i;
						}else if(d==1){
							chX=12;chY=8+i;
						}else{
							chX=13+i;chY=1;
						}
					}else{
						if(d==c){
							chX=3+i;chY=11;
						}else if(d==1){
							chX=13+i;chY=0;
						}else{
							chX=3+i;chY=14;
						}
					}
				}else{
					if(u==c){
						if(d==c){
							chX=4;chY=0+i;
						}else if(d==1){
							chX=5;chY=5+i;
						}else{
							chX=1+i*2;chY=4;
						}
					}else if(u==1){
						if(d==c){
							chX=5;chY=8+i;
						}else if(d==1){
							chX=4;chY=0+i;
						}else{
							chX=1+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=1+i*2;chY=3;
						}else if(d==1){
							chX=1+i*2;chY=3;
						}else{
							chX=12;chY=0+i;
						}
					}
				}
			}else if(l==1){
				if(r==c){
					if(u==c){
						if(d==c){
							chX=9;chY=7+i;
						}else if(d==1){
							chX=2;chY=6+i*2;
						}else{
							chX=0+i;chY=12;
						}
					}else if(u==1){
						if(d==c){
							chX=1;chY=6+i*2;
						}else if(d==1){
							chX=12;chY=5+i;
						}else{
							chX=13;chY=1;
						}
					}else{
						if(d==c){
							chX=0+i;chY=11;
						}else if(d==1){
							chX=13+i;chY=0;
						}else{
							chX=0+i;chY=14;
						}
					}
				}else if(r==1){
					if(u==c){
						if(d==c){
							chX=10;chY=7+i;
						}else if(d==1){
							chX=11;chY=8+i;
						}else{
							chX=1+i;chY=2;
						}
					}else if(u==1){
						if(d==c){
							chX=11;chY=5+i;
						}else if(d==1){
							chX=6+i;chY=11;
						}else{
							chX=13+i;chY=1;
						}
					}else{
						if(d==c){
							chX=1+i;chY=1;
						}else if(d==1){
							chX=13+i;chY=0;
						}else{
							chX=10;chY=7+i;
						}
					}
				}else{
					if(u==c){
						if(d==c){
							chX=13+i;chY=3;
						}else if(d==1){
							chX=13+i;chY=3;
						}else{
							chX=1+i*2;chY=4;
						}
					}else if(u==1){
						if(d==c){
							chX=13+i;chY=3;
						}else if(d==1){
							chX=13+i;chY=3;
						}else{
							chX=1+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=1+i*2;chY=3;
						}else if(d==1){
							chX=1+i*2;chY=3;
						}else{
							chX=0+i;chY=13;
						}
					}
				}
			}else{
				if(r==c){
					if(u==c){
						if(d==c){
							chX=0;chY=0+i;
						}else if(d==1){
							chX=4;chY=5+i;
						}else{
							chX=0+i*2;chY=4;
						}
					}else if(u==1){
						if(d==c){
							chX=0;chY=0+i;
						}else if(d==1){
							chX=0;chY=0+i;
						}else{
							chX=0+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=0+i*2;chY=3;
						}else if(d==1){
							chX=0+i*2;chY=3;
						}else{
							chX=9;chY=0+i;
						}
					}
				}else if(r==1){
					if(u==c){
						if(d==c){
							chX=13+i;chY=2;
						}else if(d==1){
							chX=13+i;chY=2;
						}else{
							chX=0+i*2;chY=4;
						}
					}else if(u==1){
						if(d==c){
							chX=13+i;chY=2;
						}else if(d==1){
							chX=13+i;chY=2;
						}else{
							chX=0+i*2;chY=4;
						}
					}else{
						if(d==c){
							chX=0+i*2;chY=3;
						}else if(d==1){
							chX=0+i*2;chY=3;
						}else{
							chX=3+i;chY=13;
						}
					}
				}else{
					if(u==c){
						if(d==c){
							chX=5;chY=0+i;
						}else if(d==1){
							chX=7;chY=5+i;
						}else{
							chX=6+i;chY=3;
						}
					}else if(u==1){
						if(d==c){
							chX=7;chY=8+i;
						}else if(d==1){
							chX=6;chY=12+i;
						}else{
							chX=6;chY=8+i;
						}
					}else{
						if(d==c){
							chX=6+i;chY=0;
						}else if(d==1){
							chX=6;chY=5+i;
						}else{
							chX=9+i;chY=3;
						}
					}
				}
			}
		break;
		case 4://tree
			if(l==c){
				if(r==c){//it's tree
					if(u==c){
						if(d==c){
							chX=5;chY=3+i;
						}else{
							chX=4;chY=6+i;
						}
					}else{
						if(d==c){
							chX=6;chY=6+i;
						}else{
							chX=4;chY=6+i;
						}
					}
				}else{//root/branch?
					if(u==c){//no
						if(d==c){
							chX=2;chY=0+i;
						}else{
							chX=3;chY=6+i;
						}
					}else{//?
						if(d==c){//no
							chX=6;chY=0+i;
						}else{
							chX=1;chY=6+i;//root(right)
							if(*getBlock(x-1,y+1)==4){//branch(right)
								chX=4;chY=3+i;
							}
						}
					}
				}
			}else{
				if(r==c){//root/branch?
					if(u==c){//no
						if(d==c){
							chX=3;chY=3+i;
						}else{
							chX=0;chY=6+i;
						}
					}else{//?
						if(d==c){//no
							chX=6;chY=3+i;
						}else{
							chX=2;chY=6+i;//root(left)
							if(*getBlock(x+1,y+1)==4){//branch(left)
								chX=3;chY=i;
							}
						}
					}
				}else{//no
					if(u==c){
						if(d==c){
							chX=5;chY=3+i;
						}else{
							chX=0;chY=0+i;
						}
					}else{
						if(d==c){
							chX=6;chY=0+i;
						}else{
							chX=5;chY=0+i;
						}
					}
				}
			}
		break;
		case 5:
			chX=i;
			chY=0;
			sox=-1.85f;
			soy=-4.0f;
		break;
		case 6:
			if(l==4){
				chX=1;
				chY=i;
				sox=0.0f;
				soy=-1.0f;
			}else{
				chX=0;
				chY=i;
				sox=-1.35f;
				soy=-1.0f;
			}
		break;
	}
	
	int ww=(int)(cBlock.tile_size*BLOCK_SIZE/16);
	
	chX*=ww+(int)(BLOCK_SIZE/8);
	chY*=ww+(int)(BLOCK_SIZE/8);
	
	drawPart(tile,(x+sox)*BLOCK_SIZE,(y+soy)*BLOCK_SIZE,chX,chY,ww,ww);
}

void setWall(Image* wall, int x, int y){
	u16 c=*getWall(x,y);
	Wall cWall=getWallInfo(c);
	if(cWall.can_over&&!(getBlockInfo(*getBlock(x,y)).transparent||
		getBlockInfo(*getBlock(x-1,y)).transparent||
		getBlockInfo(*getBlock(x+1,y)).transparent||
		getBlockInfo(*getBlock(x,y-1)).transparent||
		getBlockInfo(*getBlock(x,y+1)).transparent||
		getBlockInfo(*getBlock(x-1,y-1)).transparent||
		getBlockInfo(*getBlock(x+1,y-1)).transparent||
		getBlockInfo(*getBlock(x-1,y+1)).transparent||
		getBlockInfo(*getBlock(x+1,y+1)).transparent))return;
	int chX,chY;
	int i=(x+y*WORLD_W)%3;
	chX=9+i;
	chY=3;
	
	u16 l=*getWall(x-1,y);
	u16 r=*getWall(x+1,y);
	u16 u=*getWall(x,y-1);
	u16 d=*getWall(x,y+1);
	
	switch(cWall.wall_type){
		case 0:return;break;
		case 1://dirt,stone
			{
			if (u == c && d == c && l == c && r == c)//1111
			{
				chX=1+i;
				chY=1;
			}
			else if (u == 0 && d == 0 && l == 0 && r == 0)//0000
			{
				chX=9+i;
				chY=3;
			}
			else if (u == 0 && d == 0 && l == 0 && r == c)//0001
			{
				chX=9;
				chY=i;
			}
			else if (u == 0 && d == 0 && l == c && r == 0)//0010
			{
				chX=12;
				chY=i;
			}
			else if (u == 0 && d == c && l == 0 && r == 0)//0100
			{
				chX=6+i;
				chY=0;
			}
			else if (u == c && d == 0 && l == 0 && r == 0)//1000
			{
				chX=6+i;
				chY=3;
			}
			else if (u == 0 && d == 0 && l == c && r == c)//0011
			{
				chX=6+i;
				chY=4;
			}
			else if (u == 0 && d == c && l == 0 && r == c)//0101
			{
				chX=0+i*2;
				chY=3;
			}
			else if (u == 0 && d == c && l == c && r == 0)//0110
			{
				chX=1+i*2;
				chY=3;
			}
			else if (u == 0 && d == c && l == c && r == c)//0111
			{
				chX=1+i;
				chY=0;
			}
			else if (u == c && d == 0 && l == 0 && r == c)//1001
			{
				chX=0+i*2;
				chY=4;
			}
			else if (u == c && d == 0 && l == c && r == 0)//1010
			{
				chX=1+i*2;
				chY=4;
			}
			else if (u == c && d == 0 && l == c && r == c)//1011
			{
				chX=1+i;
				chY=2;
			}
			else if (u == c && d == c && l == 0 && r == 0)//1100
			{
				chX=5;
				chY=i;
			}
			else if (u == c && d == c && l == 0 && r == c)//1101
			{
				chX=0;
				chY=i;
			}
			else if (u == c && d == c && l == c && r == 0)//1110
			{
				chX=4;
				chY=i;
			}
			}
		break;
		case 2:
		break;
	}
	
	chX*=WALL_SIZE+(int)(WALL_SIZE/8);
	chY*=WALL_SIZE+(int)(WALL_SIZE/8);
	
	drawPart(wall,x*BLOCK_SIZE,y*BLOCK_SIZE,chX,chY,WALL_SIZE,WALL_SIZE);
}
