CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

OBJ = hud.o
OBJ += wayland.o wl/info.o wl/keyboard.o wl/touch.o wl/draw.o wl/ui.o wl/text.o wl/ivi.o wl/seat.o
OBJ += gps.o
OBJ += audio.o audio_search.o


OBJ += gui/root.o gui/nav.o gui/notifier.o gui/onscreenkeys.o
OBJ += gui/gps.o
OBJ += gui/music.o gui/music_tracks.o gui/music_buttons.o


CFLAGS := -Wall -Wextra -Werror $(CFLAGS)
CFLAGS += -Wno-sign-compare

CFLAGS += -Wno-deprecated-declarations

CFLAGS += -std=c11 -O0

# CFLAGS += -isystem/usr/include/freetype2/

CFLAGS += -mcpu=arm7 -mfloat-abi=hard
CFLAGS += --sysroot=/usr/arm-linux-gnueabihf
CFLAGS += -L/home/grayhatter/mazda/libs2/lib/
CFLAGS += -isystem/usr/arm-linux-gnueabihf/include/freetype2/

LDFLAGS = -Wl,-rpath-link=/home/grayhatter/mazda/libs2/lib/

MAZDA_C_SO =
MAZDA_C_SO = /home/grayhatter/mazda/libs2/lib/libc.so.6

LIBS   +=  -lfreetype -lz
LIBS   +=  -lwayland-ivi-shell-client -lwayland-ivi-client
LIBS   +=  -lwayland-client
LIBS   +=  -lavcodec -lavutil -lavformat -lswresample -lasound
LIBS   +=  -lgps
LIBS   +=  -lffi -ldl -lrt -lpthread -lm

%.o: %.c %.h
	$(CC) -c -o $@ $(INCLUDE) $(CFLAGS) $<

hudtds: $(OBJ)
	# $(LD) --verbose -o hudtds $(OBJ) $(LDFLAGS) $(LIBS)
	$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) $(LIBS) $(MAZDA_C_SO) -o $@

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
