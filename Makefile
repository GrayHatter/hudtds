CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld
# CC = arm-linux-gnueabi-gcc
# LD = arm-linux-gnueabi-ld
# CC = arm-linux-musleabi-gcc
# LD = arm-linux-mulleabi-ld
# CC = clang
# LD = clang

OBJ = hud.o
OBJ += wayland.o wl/info.o wl/keyboard.o wl/touch.o wl/draw.o wl/ui.o wl/text.o wl/ivi.o wl/seat.o
OBJ += gps.o
OBJ += audio.o audio_search.o


OBJ += gui/root.o gui/nav.o gui/notifier.o gui/onscreenkeys.o
OBJ += gui/gps_gui.o
OBJ += gui/music.o gui/music_tracks.o gui/music_buttons.o



CFLAGS := -Wall -Wextra -Werror $(CFLAGS)
CFLAGS += -std=c11
CFLAGS += -O0
CFLAGS += -Wno-sign-compare
CFLAGS += -Wno-deprecated-declarations  # Needed for legacy version of ffmpeg
LDFLAGS =


ifeq (${CC}, clang)
	CFLAGS += --target=arm-linux-gnueabi
	CFLAGS += --sysroot=/usr/arm-linux-gnueabi
else
	CFLAGS += -mcpu=arm710
endif

CFLAGS += -mtune=arm710
CFLAGS += -mfpu=vfpv3
CFLAGS += -mfloat-abi=soft

CFLAGS += -isystem/usr/arm-linux-gnueabi/include/freetype2

LDFLAGS += -Wl,-rpath-link=/home/grayhatter/mazda/libs/lib
LDFLAGS += -Wl,-rpath-link=/home/grayhatter/mazda/libs/usr/lib

LIBS   +=  -lfreetype -lz
LIBS   +=  -lwayland-ivi-shell-client -lwayland-ivi-client
LIBS   +=  -lwayland-client
LIBS   +=  -lavcodec -lavutil -lavformat -lswresample -lasound
LIBS   +=  -lgps
LIBS   +=  -lffi -ldl -lrt -lpthread -lm

%.o: %.c %.h
	@# $(CC) -v -g -c $(CFLAGS) $< -o $@
	@$(CC) -c  $(CFLAGS) $< -o $@


hudtds: $(OBJ)
	@# $(CC) -v $(OBJ) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@
	@$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $@

all: hudtds

install: hudtds
	cp run-hud.sh /home/grayhatter/mazda/live/tmp/root/
	cp hudtds /home/grayhatter/mazda/live/tmp/root/
	cp SCP.otf /home/grayhatter/mazda/live/tmp/root/


install-all: install

install-root: hudtds
	cp hudtds /home/grayhatter/mazda/live/jci/scripts/

clean:
	rm $(OBJ) || true
	rm hudtds || true
