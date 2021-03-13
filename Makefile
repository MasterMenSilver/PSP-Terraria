TARGET = Terraria
OBJS = libs/graphics2d.o \
libs/pge/pgeMath.o\
libs//pge/pgeControls.o\
libs/pge/pgeTimer.o\
libs/pge/pgeWav.o\
libs/sound_utils/audio.o\
libs/sound_utils/bgm.o\
libs/sound_utils/VirtualFile.o\
libs/sound_utils/media.o\
libs/sound_utils/mod.o\
libs/sound_utils/vfsFile.o\
project/custom_libs/camera.o\
project/custom_libs/animation.o\
project/walls.o \
project/blocks.o \
project/items.o \
project/tileManager.o\
project/saveManager.o \
project/game.o \
project/menu.o \
main.o

INCDIR =
CFLAGS = -O2 -G0 -Wall -g
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR =
LDFLAGS = 
LIBS = -lmikmod -lmmio -lpsprtc -lpsphprm -lpsppower -lpspaudiocodec -lpspaudiolib -lpspaudio -lpng -lz -lintraFont -lpspgum -lpspgu -lm

PSP_LARGE_MEMORY=1

EXTRA_TARGETS = EBOOT.PBP
PSP_EBOOT_TITLE = Terraria
#PSP_EBOOT_ICON = ICON0.png
PSP_EBOOT_PIC1 = PIC1.png

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak

