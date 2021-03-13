#include <pspkernel.h>
#include <psppower.h>
#include "main.h"
#include "project/menu.h"
#include "project/game.h"

PSP_MODULE_INFO("Terraria", PSP_MODULE_USER, 1, 1);

int exit_callback(int arg1, int arg2, void* common){
	//sceKernelExitGame();
	globalState=0;
	return 0;
}

int CallbackThread(SceSize args, void* argp){
	//register the exit callback
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	
	// Register the power callback
	cbid = sceKernelCreateCallback("Power Callback", osl_powerCallback, NULL);
	scePowerRegisterCallback(0, cbid);
	
	sceKernelSleepThreadCB();
	return 0;
}
int SetupCallbacks(void){
	int thid = sceKernelCreateThread("CallbackThread", CallbackThread, 0x11, 0xFA0, PSP_THREAD_ATTR_USER, 0);
	if (thid >= 0) sceKernelStartThread(thid, 0, 0);
	return thid;
}

double fixAngle(double angle){
	while(angle<0)angle+=2*PI;
	while(angle>=2*PI)angle-=2*PI;
	return angle;
}

void FreeImageArray(Image** t, u8 n){
	if(t!=NULL){
		u8 i=0;
		for(i = 0; i < n; i++)
			if(t[i]!=NULL)
				freeImage(t[i]);
		free(t);t=NULL;
	}
}

struct clrs_t clrs;
u8 globalState = 2;
u8 worldSizeType = 0;
u8 nosol=0;
intraFont* Andy;
int version = 1;
char* worldName;

#define RAM_BLOCK      (1024 * 1024)
u32 ramAvailableLineareMax(void){
	u32 size, sizeblock;
	u8 *ram;

	// Init variables
	size = 0;
	sizeblock = RAM_BLOCK;

	// Check loop
	while (sizeblock)
	{
		// Increment size
		size += sizeblock;

		// Allocate ram
		ram = (u8*)malloc(size);

		// Check allocate
		if (!(ram))
		{
			// Restore old size
			size -= sizeblock;

			// Size block / 2
			sizeblock >>= 1;
		}
		else
			free(ram);
	}

	return size;
}

u32 getFreeMemory(void){
	u8 **ram, **temp;
	u32 size, count, x;


	// Init variables
	ram = NULL;
	size = 0;
	count = 0;

	// Check loop
	for (;;)
	{
		// Check size entries
		if (!(count % 10))
		{
			// Allocate more entries if needed
			temp = (u8**)realloc(ram,sizeof(u8*) * (count + 10));
			if (!(temp)) break;

			// Update entries and size (size contains also size of entries)
			ram = temp;
			size += (sizeof(u8 *) * 10);
		}

		// Find max lineare size available
		x = ramAvailableLineareMax();
		if (!(x)) break;

		// Allocate ram
		ram[count] = (u8*)malloc(x);
		if (!(ram[count])) break;

		// Update variables
		size += x;
		count++;
	}

	// Free ram
	if (ram)
	{
		for (x=0;x<count;x++) free(ram[x]);
		free(ram);
	}

	return size;
}

void initEngine(void){
  intraFontInit();//libs/intrafont (or maybe in pspsdk, don't sure completely)
	pgeWavInit();//libs/pge/...
	initGraphics();//path: libs/graphics2d
	pgeControlsInit();//libs/pge/...
  VirtualFileInit();
	oslInitAudioME(OSL_FMT_MP3);
	oslInitAudio();
	
	clrs.red = GU_RGBA(0xFF,0,0,0xFF);//my colors, xd
	clrs.blue = GU_RGBA(0,0,0xFF,0xFF);
	clrs.green = GU_RGBA(0,0xFF,0,0xFF);
	clrs.black = GU_RGBA(0,0,0,0xFF);
	clrs.white = GU_RGBA(0xFF,0xFF,0xFF,0xFF);
	clrs.gray = GU_RGBA(0x88,0x88,0x88,0xFF);
	clrs.yellow = GU_RGBA(0xFF,0xFF,0,0xFF);
	clrs.purple = GU_RGBA(0x88,0,0x88,0xFF);
	clrs.cyan = GU_RGBA(0,0xFF,0xFF,0xFF);
	clrs.pink = GU_RGBA(0xFF,0,0xFF,0xFF);
}

void shutdownEngine(void){
	pgeWavShutdown();
	endGraphics();
	oslDeinitAudio();
}

int main(){
	initEngine();
	SetupCallbacks();

	//scePowerSetClockFrequency(333, 333, 166);//еее, разгооон (обычно стоит 222,222,111 без разгона)
	
	SetStr(worldName,"world_0");
	Andy = intraFontLoad("Fonts/ComicSans.pgf",INTRAFONT_STRING_UTF8);
	intraFontSetStyle(Andy, 0.6f, clrs.white, 0, 0);	
	globalState = 1;
	
	while(globalState){
		switch(globalState){
			case 1:
				menu();
			break;
			case 2:
				game();
			break;
			default:
				sceKernelExitGame();
				return 0;
			break;
		}
	}
	sceKernelExitGame();
	return 0;
}