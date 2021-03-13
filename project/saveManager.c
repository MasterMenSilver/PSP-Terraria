#include "main.h"
#include "game.h"
#include <zlib.h>

int saveGame(void){
    //first save version and name on normal file
		char* path;
		asprintf(&path,"Data/Worlds/%s.wld",worldName);
		
    gzFile * pFile;
    pFile = gzopen(path,"wb");

    if(pFile == NULL)
        return 0;
		
		showStatusf("Saving world options...","");
    //version
    gzwrite(pFile,&version,sizeof(int));
		
    //name
		int WNL=strlen(worldName);//world name length
    gzwrite(pFile,&WNL,sizeof(int));
    gzwrite(pFile,worldName,sizeof(char)*(WNL));//worldName->char
		//world size
    gzwrite(pFile,&WORLD_W,sizeof(int));
    gzwrite(pFile,&WORLD_H,sizeof(int));
		//world
		showStatusf("Saving blocks...","");
    gzwrite(pFile,blocks,sizeof(u16)*(WORLD_SIZE));//blocks->number
		showStatusf("Saving Walls...","");
    gzwrite(pFile,walls,sizeof(u8)*(WORLD_SIZE));//walls->number
    //player
		showStatusf("Saving player...","");
    gzwrite(pFile,&player.x,sizeof(float));
    gzwrite(pFile,&player.y,sizeof(float));
    gzwrite(pFile,&player.spd,sizeof(int));
    gzwrite(pFile,&player.dir,sizeof(int));
    gzwrite(pFile,&player.hp,sizeof(int));
    gzwrite(pFile,&player.mp,sizeof(int));
    gzwrite(pFile,&player.hpMax,sizeof(int));
    gzwrite(pFile,&player.mpMax,sizeof(int));
    gzwrite(pFile,&inv_size,sizeof(int));
		showStatusf("Saving inventory...","");
		int i;
		for(i=0;i<inv_size;i++)
			gzwrite(pFile,player.inv[i],sizeof(Item));//struct player.inv->[i]->struct item
		
    gzclose(pFile);
    pFile=0;
		return 1;
}
int loadGame(void){
    //first save version and name on normal file
		char* path;
		asprintf(&path,"Data/Worlds/%s.wld",worldName);
		
    gzFile * pFile;
    pFile = gzopen(path,"rb");

    if(pFile == NULL)
        return 0;
		
		showStatusf("Loading world options...","");
    //version
    gzread(pFile,&version,sizeof(int));
		
    //name
		int WNL;
    gzread(pFile,&WNL,sizeof(int));
    gzread(pFile,worldName,sizeof(char)*(WNL));
		//world size
    gzread(pFile,&WORLD_W,sizeof(int));
    gzread(pFile,&WORLD_H,sizeof(int));
		WORLD_SIZE=WORLD_W*WORLD_H;
		//world
		showStatusf("Loading blocks...","");
		blocks=Array(u16,WORLD_SIZE);
    gzread(pFile,blocks,sizeof(u16)*(WORLD_SIZE));
		showStatusf("Loading walls...","");
		walls=Array(u8,WORLD_SIZE);
    gzread(pFile,walls,sizeof(u8)*(WORLD_SIZE));
    //player
		showStatusf("Loading player...","");
		player.w=32;
		player.h=50;
		player.mv=0;
    gzread(pFile,&player.x,sizeof(float));
    gzread(pFile,&player.y,sizeof(float));
    gzread(pFile,&player.spd,sizeof(float));
		
    gzread(pFile,&player.dir,sizeof(int));
    gzread(pFile,&player.hp,sizeof(int));
    gzread(pFile,&player.mp,sizeof(int));
    gzread(pFile,&player.hpMax,sizeof(int));
    gzread(pFile,&player.mpMax,sizeof(int));
    gzread(pFile,&inv_size,sizeof(int));
		showStatusf("Loading inventory...","");
		player.inv=Array(Item*,inv_size);
		int i;
		for(i=0;i<inv_size;i++){
			player.inv[i]=item_none();
			gzread(pFile,player.inv[i],sizeof(Item));
		}
		
    gzclose(pFile);
    pFile=0;
		return 1;
}





