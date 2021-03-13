#include "main.h"
#include "menu.h"

static int BUT_COUNT=4;

void menu(void){
	int i = 0;
	int cur = 0;
	int state = 0;
	BUT_COUNT = 4;
	char** textButtons = Array(char*, 4);
			SetStr(textButtons[0],"New Game");
			SetStr(textButtons[1],"Load Game");
			SetStr(textButtons[2],"Settings");
			SetStr(textButtons[3],"Exit");
			
	//6400//4200//1750
	//1800//1200//900
	char** textWorldSize = Array(char*, 3);
			SetStr(textWorldSize[0],"Small");
			SetStr(textWorldSize[1],"Expanded");
			SetStr(textWorldSize[2],"Medium");
	
	OSL_SOUND* SMP3=oslLoadSoundFileMP3("sounds/BGSounds/day.mp3",OSL_FMT_STREAM);
	oslPlaySound(SMP3, 0);
	oslSetSoundLoop(SMP3,1);
	
	pgeWav* choose=pgeWavLoad("sounds/Act/Choose.wav");
	Image* BG = loadImage("imgs/BG.png");
	
	int cycles=0;
	u32 ram=getFreeMemory();
	
	intraFontSetStyle(Andy, 0.8f, clrs.white, 0, INTRAFONT_ALIGN_CENTER);
	while(globalState == 1){
		cycles++;
		if(cycles>60){
			ram=getFreeMemory();
			cycles=0;
		}
		pgeControlsUpdate();
		if(pgeControlsPressed(PGE_CTRL_UP)){
			cur--;
			if(cur<0)
				cur=BUT_COUNT-1;
			pgeWavPlay(choose);
		}else	if(pgeControlsPressed(PGE_CTRL_DOWN)){
			cur++;
			if(cur>=BUT_COUNT)
				cur=0;
			pgeWavPlay(choose);
		}
		
		if(pgeControlsPressed(PGE_CTRL_CIRCLE)){
			if(state!=0){
				pgeWavPlay(choose);
				state=0;
				cur=0;
				BUT_COUNT=4;
			}
		}
		
		if(pgeControlsPressed(PGE_CTRL_CROSS)){
			pgeWavPlay(choose);
			switch(state){
				case 0:
					switch(cur){
						case 0:
							state=1;
							cur=0;
							BUT_COUNT=3;
						break;
						case 1:
							globalState=2;
							nosol=2;//loading
						break;
						case 3:
							globalState=0;
						break;
						default:break;
					}
				break;
				case 1:
					if(!((int)(getFreeMemory()/1024/1024)<35&&cur>=2)){
						worldSizeType=cur;
						nosol=0;//creating
						globalState=2;
					}
				break;
			}
		}
		
		startDrawing();
		clearScreen(clrs.black);
		drawImage(BG,0,0,0,255);
		
		intraFontSetStyle(Andy, 0.4f, clrs.white, 0, INTRAFONT_ALIGN_LEFT);
		intraFontPrintf(Andy, 0, 20, "free ram:%i/64MB (%iKB)",ram/1024/1024,ram/1024);
		intraFontSetStyle(Andy, 0.8f, clrs.white, 0, INTRAFONT_ALIGN_CENTER);
		
		for(i=0;i<BUT_COUNT;i++){
			switch(state){
				case 0:
					if(cur==i)
						intraFontSetStyle(Andy, 1.0f, clrs.yellow, 0, INTRAFONT_ALIGN_CENTER);
					intraFontPrint(Andy, 480/2, 272/2-25+i*25, textButtons[i]);
					if(cur==i)
						intraFontSetStyle(Andy, 0.8f, clrs.white, 0, INTRAFONT_ALIGN_CENTER);
				break;
				case 1:
					if(cur==i)
						intraFontSetStyle(Andy, 1.0f, clrs.yellow, 0, INTRAFONT_ALIGN_CENTER);
					intraFontPrint(Andy, 480/2, 272/2-25+i*25, textWorldSize[i]);
					if(cur==i)
						intraFontSetStyle(Andy, 0.8f, clrs.white, 0, INTRAFONT_ALIGN_CENTER);
				break;
			}
		}
		endDrawing();
		oslAudioVSync();
		flipScreen();
	}
	oslDeleteSound(SMP3);
	SMP3=NULL;
	freeImage(BG);
	pgeWavUnload(choose);
	free(textButtons);
	free(textWorldSize);
}










