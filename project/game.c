#include "main.h"
#include "game.h"
#include "blocks.h"
#include "walls.h"
#include "custom_libs/camera.h"
#include "tileManager.h"
#include "saveManager.h"

//8400 x 2400 (77MB)

//44MB,22MB,5MB
//6400//4200//1750
//1800//1200//900
int WORLD_W;int WORLD_H;int WORLD_SIZE;
int BLOCK_SIZE;int WALL_SIZE;
int inv_size=40;
static int inv_cur=0;
static float inv_scale=0.35f;//52x0.35f
u16* blocks;u8* walls;
Image** tiles;Image** tiles2;
Image** backs;Image** backs2;
Image** items;

struct Player player;

struct{
	float x,y,spd;
	Image* img;
}cursor;

u16* getBlock(int x, int y){
	if(x<0||y<0||x>WORLD_W||y>WORLD_H)
		return (u16*)(blocks)+WORLD_W+WORLD_H*WORLD_W-2;
	return (u16*)(blocks)+x+y*WORLD_W;
}
u8 getBlockCollision(int x, int y){
	return getBlockInfo(*getBlock(x,y)).collision;
}
wts* getWall(int x, int y){
	if(x<0||y<0||x>WORLD_W||y>WORLD_H)
		return (wts*)(walls)+WORLD_W+WORLD_H*WORLD_W-2;
	return (wts*)(walls)+x+y*WORLD_W;
}

void generator(void){
	int i,x,y;
	for(i=0;i<WORLD_SIZE; i++){//init world
		*((u16*)(blocks)+i)=0;
		*((wts*)(walls)+i)=0;
	}
	
	int r_h_dirt=WORLD_H*(1.0f-0.82f);
	int r_h_stone=WORLD_H*(1.0f-0.66f);
	int r=r_h_dirt;
	int dr=0;
	int tree_close=0;
	int tree_count=1;
	int** tree_points=Array(int*,tree_count);
	showStatusf("free ram:%iKB\n\nwidth:%i blocks\n\nheight:%i blocks\n\ntotal:%i blocks\n\ngenerating 82%% ground...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
	for(x=0; x<WORLD_W-1; x++){//ground generator
		*getBlock(x,r)=2;
		tree_close++;
		if(x>3&&x<WORLD_W-10&&tree_close>3&&rand()%20==0){//save tree points
			tree_points=ScaleArray(tree_points,int*,tree_count);
			tree_points[tree_count-1]=Array(int,2);
			tree_points[tree_count-1][0]=x;
			tree_points[tree_count-1][1]=r;
			tree_count++;
			tree_close=0;
		}
		
		for(y=r+1; y<WORLD_H; y++){//ground&wall
			*getBlock(x,y)=1;
			*getWall(x,y)=1;
		}
		if(dr){//some grass at corners
			if(dr>0)
				*getBlock(x,r+1)=2;
			else
				*getBlock(x-1,r)=2;
			dr=0;
		}
		switch(rand()%12){//change height
			case 0:r++;dr=-1;break;
			case 1:r--;dr=1;break;
		}
		if(r<r_h_dirt-20){
			r=r_h_dirt-20;
		}else if(r>r_h_dirt+20){
			r=r_h_dirt+20;
		}
	}
	tree_count--;
	showStatusf("free ram:%iKB\n\nwidth:%i blocks\n\nheight:%i blocks\n\ntotal:%i blocks\n\ngrowing trees...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
	for(i=0;i<tree_count;i++){//tree generator
		x=tree_points[i][0];
		r=tree_points[i][1];
		int h=r-7-rand()%17;
		for(y=h; y<r; y++){
			*getBlock(x,y)=4;
			if(y<r-2&&y%2==0){
				if(rand()%4==0){
					if(rand()%2){
						*getBlock(x+1,y)=6;
					}else{
						*getBlock(x+1,y)=4;
					}
				}
				if(rand()%4==0){
					if(rand()%2){
						*getBlock(x-1,y)=6;
					}else{
						*getBlock(x-1,y)=4;
					}
				}
			}
		}
		*getBlock(x,h-1)=5;
		if(*getBlock(x+1,r)!=0&&rand()%2==0){
			*getBlock(x+1,r-1)=4;
			if(*getBlock(x-1,r)!=0&&rand()%8==0){
				*getBlock(x-1,r-1)=4;
			}
		}
		else if(*getBlock(x-1,r)!=0){
			*getBlock(x-1,r-1)=4;
			if(*getBlock(x+1,r)!=0&&rand()%8==0){
				*getBlock(x+1,r-1)=4;
			}
		}
		free(tree_points[i]);
	}
	free(tree_points);
	
	showStatusf("free ram:%iKB\n\nwidth:%i blocks\n\nheight:%i blocks\n\ntotal:%i blocks\n\ngenerating 66%% stones...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
	r=r_h_stone;
	for(x=0; x<WORLD_W-1; x++){//stone generator
		for(y=r; y<WORLD_H; y++){//block & wall
			*getBlock(x,y)=3;
			*getWall(x,y)=2;
		}
		switch(rand()%20){
			case 0:r++;break;
			case 1:r--;break;
		}
		if(r<r_h_stone-10){
			r=r_h_stone-10;
		}else if(r>r_h_stone+10){
			r=r_h_stone+10;
		}
	}
}

void init(int width, int height){
	int i;
	BLOCK_SIZE=16;WALL_SIZE=32;
	showStatusf("free ram:%iKB\n\nmemory allocation[images]...",getFreeMemory()/1024);
	tiles=Array(Image*, BLOCKS_COUNT);
	tiles2=Array(Image*, BLOCKS_COUNT);
	backs=Array(Image*, WALLS_COUNT);
	backs2=Array(Image*, WALLS_COUNT);
	items=Array(Image*, ITEMS_COUNT);
	if(nosol==2)
		showStatusf("free ram:%iKB\n\nLoading World...",getFreeMemory()/1024);
	if(nosol==2&&loadGame()==1){
		player.w=32;
		player.h=50;
		player.mv=0;
	}else{
		WORLD_W=width;WORLD_H=height;//init global vars
		WORLD_SIZE=WORLD_W*WORLD_H;
		
		showStatusf("free ram:%iKB\n\nwidth:%i blocks(блоков)\n\nheight:%i blocks(блоков)\n\ntotal:%i blocks(блоков)\n\nmemory allocation[blocks]...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
		blocks=Array(u16, WORLD_SIZE);
		showStatusf("free ram:%iKB\n\nwidth:%i blocks(блоков)\n\nheight:%i blocks(блоков)\n\ntotal:%i blocks(блоков)\n\nmemory allocation[walls]...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
		walls=Array(u8, WORLD_SIZE);
		showStatusf("free ram:%iKB\n\nwidth:%i blocks(блоков)\n\nheight:%i blocks(блоков)\n\ntotal:%i blocks(блоков)\n\nmemory allocation[final]...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
		
		generator();
		
		//*getWall(x,y)=*getBlock(x,y);//get & set example
		player.x=WORLD_W*16/2;
		player.y=(WORLD_H*(1.0f-0.82f)-25)*16;
		player.w=32;
		player.h=50;
		player.acc=0.0f;
		player.spd=8.0f;
		player.dir=0;//0,1 (left,right)
		player.mv=0;//1/0
		player.hp=200;
		player.mp=200;
		player.hpMax=200;
		player.mpMax=200;
		
		player.inv=Array(Item*,inv_size);//4 lines * 10 ceils
		for(i=0;i<inv_size;i++)player.inv[i]=item_none();
		player.inv[0]=item_dirt();
		player.inv[1]=item_stone();
		player.inv[2]=item_glass();
	}
	
	showStatusf("free ram:%iKB\n\nwidth:%i blocks\n\nheight:%i blocks\n\ntotal:%i blocks\n\nLoading textures...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
	
	cursor.x=480/2;
	cursor.y=272/2;
	cursor.spd=2.5f;
	cursor.img=loadImage("imgs/GUI/Cursor.png");
	
	player.image=Array(Animation*,2);
	player.image[0]=animLoad("imgs/NPC/hero_0/Left/",0,6,0.1f);
	player.image[1]=animLoad("imgs/NPC/hero_0/Right/",0,6,0.1f);
	//sceKernelExitGame();
	
	for(i=0;i<BLOCKS_COUNT;i++){
		tiles[i]=NULL;
		tiles2[i]=NULL;
	}
	
	tiles[0]=NULL;
	tiles[1]=loadImage("imgs/Tiles/Tiles_0.png");//dirt
	tiles[2]=loadImage("imgs/Tiles/Tiles_2.png");//grass
	tiles[3]=loadImage("imgs/Tiles/Tiles_1.png");//stone
	
	tiles[4]=loadImage("imgs/Tiles/Tiles_5.png");//tree
	tiles[5]=loadImage("imgs/Tiles/Tree_Tops.png");
	tiles[6]=loadImage("imgs/Tiles/Tree_Branches.png");
	setImageZ(tiles[4],-0.05f);
	setImageZ(tiles[5],-0.05f);
	setImageZ(tiles[6],-0.05f);
	
	tiles[7]=loadImage("imgs/Tiles/Tiles_54.png");//glass
	
	tiles2[0]=NULL;
	tiles2[1]=loadImage("imgs/x2/Tiles/Tiles_0.png");
	tiles2[2]=loadImage("imgs/x2/Tiles/Tiles_2.png");
	tiles2[3]=loadImage("imgs/x2/Tiles/Tiles_1.png");
	
	tiles2[4]=loadImage("imgs/x2/Tiles/Tiles_5.png");
	tiles2[5]=loadImage("imgs/x2/Tiles/Tree_Tops.png");
	tiles2[6]=loadImage("imgs/x2/Tiles/Tree_Branches.png");
	setImageZ(tiles2[4],-0.05f);
	setImageZ(tiles2[5],-0.05f);
	setImageZ(tiles2[6],-0.05f);
	
	tiles2[7]=loadImage("imgs/x2/Tiles/Tiles_54.png");

	for(i=0;i<WALLS_COUNT;i++){
		backs[i]=NULL;
		backs2[i]=NULL;
	}
	
	backs[0]=NULL;
	backs[1]=loadImage("imgs/Walls/Wall_1.png");
	backs[2]=loadImage("imgs/Walls/Wall_2.png");
	
	backs2[0]=NULL;
	backs2[1]=loadImage("imgs/x2/Walls/Wall_1.png");
	backs2[2]=loadImage("imgs/x2/Walls/Wall_2.png");
	
	for(i=0;i<ITEMS_COUNT;i++)
		items[i]=NULL;

	items[0]=loadImage("imgs/GUI/Items/Item_0.png");
	items[1]=loadImage("imgs/GUI/Items/Item_2.png");
	items[2]=loadImage("imgs/GUI/Items/Item_61.png");
	items[3]=loadImage("imgs/GUI/Items/Item_593.png");
	
	for(i=0;i<ITEMS_COUNT;i++)
		stretchImage(items[i],inv_scale,inv_scale);
}

u8 r_r(struct Rect rect,struct Rect rect2){
	return !(rect2.x>rect.x+rect.w||rect2.x+rect2.w<rect.x||rect2.y>rect.y+rect.h||rect2.y+rect2.h<rect.y);
}

u8 collisionBXY(int x, int y){
		struct Rect block={x*16,y*16,16,16};
		struct Rect pl={player.x+8,player.y+6,player.w-16,player.h-7};
		if(getBlockCollision(x,y)&&r_r(block,pl)){
			return 1;
		}
		return 0;
}

u8 collisionB(void){
	int x,y;
	for(x=(player.x/16)-1; x<((player.x+player.w)/16)+2; x++)//2+2
		for(y=(player.y/16)-1; y<((player.y+player.h)/16)+2; y++)//4+2
		{//4*6=24 times
			if(collisionBXY(x,y))
				return 1;
		}
	return 0;
}

void move(float dx, float dy, float spd){
	int i=0;
	for(i=0; i<spd; i++){
		player.x+=dx;
		if(collisionB()){
			player.y-=16;
			if(collisionB()){
				player.y+=16;
				player.x-=dx;
				break;
			}
		}
	}
	for(i=0; i<spd; i++){
		player.y+=dy;
		if(collisionB()){
			player.y-=dy;
			break;
		}
	}
	if(player.x<0)player.x=0;
	if(player.x+player.w>WORLD_W*16)player.x=WORLD_W*16-player.w;
	if(player.y+player.h>WORLD_H*16)player.y=WORLD_H*16-player.y;
}

int buildBlock(int x, int y, u16 id){
	if(x<WORLD_W&&x>=0&&y<WORLD_H&&y>=0&&id!=*getBlock(x,y)){
		Block cBlock=getBlockInfo(*getBlock(x,y));
		
		if(cBlock.id!=5&&cBlock.id!=6&&cBlock.id!=4&&(*getBlock(x,y-1)!=4||*getBlock(x-1,y)==4||*getBlock(x+1,y)==4)){
			*getBlock(x,y)=id;
			if(collisionBXY(x,y))
				*getBlock(x,y)=cBlock.id;
			else
				return 1;
		}
	}
	return 0;
}

int buildWall(int x, int y, u8 id){
	if(x<WORLD_W&&x>=0&&y<WORLD_H&&y>=0&&id!=*getWall(x,y)){
		//Wall cWall=getWallInfo(*getWall(x,y));
		
		*getWall(x,y)=id;
		return 1;
	}
	return 0;
}

void game(void){
	intraFontSetStyle(Andy, 0.8f, clrs.yellow, 0, INTRAFONT_ALIGN_CENTER);
	switch(worldSizeType){
		case 0:init(1750,900);break;//5(5) MB
		case 1:init(4200,1200);break;//22(15) MB
		case 2:init(6400,1800);break;//44(33) MB
		case 3:init(8400,2400);break;//77(58) MB
	}
	int x,y;
	
	/*
	if(!pgeFileExists("sounds/BGSounds/day.mp3"))sceKernelExitGame();
	if(!pgeMp3Play("sounds/BGSounds/day.mp3"))sceKernelExitGame();
	pgeMp3Loop(1);*/
	OSL_SOUND* SMP3 = oslLoadSoundFileMP3("sounds/BGSounds/day.mp3",OSL_FMT_STREAM);
	oslPlaySound(SMP3,2);
	oslSetSoundLoop(SMP3,1);
	
	Image* BG = loadImage("imgs/BG.png");
	setImageZ(BG,-0.99f);

	Image* Inv_Ceil = loadImage("imgs/GUI/Inventory_Back/Inventory_Back.png");
	stretchImage(Inv_Ceil,inv_scale,inv_scale);
	Image* Inv_Ceil_Cur = loadImage("imgs/GUI/Inventory_Back/Inventory_Cur.png");
	stretchImage(Inv_Ceil_Cur,inv_scale,inv_scale);
	
	float dg=1.0f;
	float g=3.0f;
	
	int i;
	int bAL = 32;//base analog limit
	
	int cycles=0;
	u32 ram=getFreeMemory();
	
	while(globalState==2){
		cycles++;
		if(cycles>60){
			ram=getFreeMemory();
			cycles=0;
		}
		pgeControlsUpdate();
		if(pgeControlsPressed(PGE_CTRL_CIRCLE)){globalState = 1;break;}
		move(0.0f,dg+player.acc,g);//gravity
		if(player.acc<0)player.acc+=dg;
		if(player.acc>0)player.acc=0;
		
		if(pgeControlsAnalogY()<-bAL || pgeControlsAnalogY()>bAL)cursor.y+=cursor.spd*pgeControlsAnalogY()/100;
		if(pgeControlsAnalogX()<-bAL || pgeControlsAnalogX()>bAL)cursor.x+=cursor.spd*pgeControlsAnalogX()/100;
		if(cursor.x<0)cursor.x = 0;if(cursor.x>SCR_WIDTH)cursor.x = SCR_WIDTH-1;
		if(cursor.y<0)cursor.y = 0;if(cursor.y>SCR_HEIGHT)cursor.y = SCR_HEIGHT-1;
		
		if(pgeControlsHeld(PGE_CTRL_SQUARE)){
			//
		}
		if(pgeControlsHeld(PGE_CTRL_CROSS)){//jump
			player.y+=1;
			if(collisionB()){
				player.acc=-player.spd;
			}
			player.y-=1;
		}
		
		if(pgeControlsPressed(PGE_CTRL_TRIANGLE)){//scale
			if(BLOCK_SIZE==16){
				BLOCK_SIZE=8;
				WALL_SIZE=16;
				animStretch(player.image[0],0.5f,0.5f);
				animStretch(player.image[1],0.5f,0.5f);
			}else{
				BLOCK_SIZE=16;
				WALL_SIZE=32;
				animStretch(player.image[0],2.0f,2.0f);
				animStretch(player.image[1],2.0f,2.0f);
			}
		}
		
		if(pgeControlsHeld(PGE_CTRL_LEFT)){//move
			move(-1.0f,0.0f,player.spd);
			player.mv=1;
			if(player.dir!=0){
				player.image[player.dir]->i=0;
				player.dir=0;
			}
		}else	if(pgeControlsHeld(PGE_CTRL_RIGHT)){//move
			move(1.0f,0.0f,player.spd);
			player.mv=1;
			if(player.dir!=1){
				player.image[player.dir]->i=0;
				player.dir=1;
			}
		}else{//stay
			player.mv=0;
			player.image[0]->i=0;//reset animation
			player.image[1]->i=0;
		}
		if(pgeControlsPressed(PGE_CTRL_UP)){//switch inventory
			inv_cur++;
			if(inv_cur>9)inv_cur=0;
		}else	if(pgeControlsPressed(PGE_CTRL_DOWN)){
			inv_cur--;
			if(inv_cur<0)inv_cur=9;
		}
		
		if(pgeControlsHeld(PGE_CTRL_LTRIGGER)){//break under
			x=(player.x)/16;
			y=(player.y+player.h-5)/16;
			buildBlock(x,y+1,0);
			buildBlock(x+1,y+1,0);
		}
		if(pgeControlsHeld(PGE_CTRL_RTRIGGER)){//set under
			x=(player.x)/16;
			y=(player.y+player.h-5)/16;
			if(player.inv[inv_cur]->id!=0){
				if(player.inv[inv_cur]->type){
					buildBlock(x,y+1,player.inv[inv_cur]->id0);
					buildBlock(x+1,y+1,player.inv[inv_cur]->id0);
				}else{
					buildWall(x,y+1,player.inv[inv_cur]->id0);
					buildWall(x+1,y+1,player.inv[inv_cur]->id0);
				}
			}
		}
		focus0(16, 64, player.x*BLOCK_SIZE/16,(player.y+(2-BLOCK_SIZE/8)*2)*BLOCK_SIZE/16,0,0,WORLD_W*BLOCK_SIZE,WORLD_H*BLOCK_SIZE);
		startDrawing();
		clearScreen(clrs.white);
		
		drawImage(BG, 0, 0, 0, 255);
		if(WALL_SIZE==32){
			for(x=(player.x-240*16/BLOCK_SIZE+player.w/2)/16-1; x<(player.x+256*16/BLOCK_SIZE+player.w/2)/16; x++)
				for(y=(player.y-136*16/BLOCK_SIZE+player.h/2)/16; y<(player.y+158*16/BLOCK_SIZE+player.h/2)/16; y++)
					if(x<WORLD_W&&x>=0&&y<WORLD_H&&y>=0)
						setWall(backs[*getWall(x,y)],x,y);
		}else
			for(x=(player.x-240*16/BLOCK_SIZE+player.w/2)/16; x<(player.x+256*16/BLOCK_SIZE+player.w/2)/16; x++)
				for(y=(player.y-136*16/BLOCK_SIZE+player.h/2)/16; y<(player.y+158*16/BLOCK_SIZE+player.h/2)/16; y++)
					if(x<WORLD_W&&x>=0&&y<WORLD_H&&y>=0)
						setWall(backs2[*getWall(x,y)],x,y);
					
		if(player.mv)
			draw(animFrame(player.image[player.dir]),player.x*BLOCK_SIZE/16,(player.y+(2-BLOCK_SIZE/8)*2)*BLOCK_SIZE/16);
		else
			draw(player.image[player.dir]->collection[0],player.x*BLOCK_SIZE/16,(player.y+(2-BLOCK_SIZE/8)*2)*BLOCK_SIZE/16);
		
		if(BLOCK_SIZE==16){
			for(x=(player.x-240*16/BLOCK_SIZE+player.w/2)/16-1; x<(player.x+256*16/BLOCK_SIZE+player.w/2)/16; x++)
				for(y=(player.y-136*16/BLOCK_SIZE+player.h/2)/16; y<(player.y+158*16/BLOCK_SIZE+player.h/2)/16; y++)
					if(x<WORLD_W&&x>=0&&y<WORLD_H&&y>=0)
						setTile(tiles[*getBlock(x,y)],x,y);
		}else
			for(x=(player.x-240*16/BLOCK_SIZE+player.w/2)/16; x<(player.x+256*16/BLOCK_SIZE+player.w/2)/16; x++)
				for(y=(player.y-136*16/BLOCK_SIZE+player.h/2)/16; y<(player.y+158*16/BLOCK_SIZE+player.h/2)/16; y++)
					if(x<WORLD_W&&x>=0&&y<WORLD_H&&y>=0)
						setTile(tiles2[*getBlock(x,y)],x,y);

		for(i=0;i<10;i++){
			int offsX=i*(Inv_Ceil->width+2);
			if(inv_cur==i)
				drawImage(Inv_Ceil_Cur,10+offsX,15,0,255);
			else 
				drawImage(Inv_Ceil,10+offsX,15,0,255);
			
			int offsIX=Inv_Ceil->width/2-items[player.inv[i]->id]->width/2;
			int offsIY=Inv_Ceil->height/2-items[player.inv[i]->id]->height/2;
			drawImage(items[player.inv[i]->id],
			10+offsX+offsIX,
			15+offsIY,0,255);
		}
		drawImage(cursor.img,cursor.x,cursor.y,0,255);
		
		intraFontSetStyle(Andy, 0.4f, clrs.white, 0, 0);
		intraFontPrintf(Andy, 0, 272-10, "free memory:%i/64MB (%iKB)", ram/1024/1024, ram/1024);
		
		endDrawing();
		oslAudioVSync();
		flipScreen();
	}
	nosol=1;
	oslDeleteSound(SMP3);
	SMP3=NULL;
	intraFontSetStyle(Andy, 0.8f, clrs.yellow, 0, INTRAFONT_ALIGN_CENTER);
	if((int)(getFreeMemory()/1024/1024)>=15){
		showStatusf("free ram:%iKB\n\nwidth:%i blocks\n\nheight:%i blocks\n\ntotal:%i blocks\n\nSaving World...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
		saveGame();
		showStatusf("free ram:%iKB\n\nwidth:%i blocks\n\nheight:%i blocks\n\ntotal:%i blocks\n\nfree ram++;...",getFreeMemory()/1024,WORLD_W,WORLD_H,WORLD_SIZE);
	}
	freeImage(BG);
	animFree(player.image[0]);
	animFree(player.image[1]);
	freeImage(cursor.img);
	freeImage(Inv_Ceil);
	freeImage(Inv_Ceil_Cur);
	FreeImageArray(tiles,BLOCKS_COUNT);
	FreeImageArray(tiles2,BLOCKS_COUNT);
	FreeImageArray(backs,WALLS_COUNT);
	FreeImageArray(backs2,WALLS_COUNT);
	FreeImageArray(items,ITEMS_COUNT);
	for(i=0;i<inv_size;i++)
		free(player.inv[i]);
	free(player.inv);
	free(player.image);
	free(blocks);
	free(walls);
}
