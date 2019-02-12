CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

OBJ = hud.o audio.o audio_search.o wayland.o info.o
OBJ += wl/keyboard.o wl/touch.o wl/draw.o wl/ui.o wl/text.o
OBJ += gui/root.o gui/nav.o gui/music.o

CFLAGS := -Wall -Wextra -Werror $(CFLAGS)
CFLAGS += -Wno-sign-compare

CFLAGS += -Wno-deprecated-declarations

CFLAGS += -std=c11 -O0

# CFLAGS += -isystem/usr/include/freetype2/

CFLAGS += -mcpu=arm7 -mfloat-abi=hard
CFLAGS += --sysroot=/usr/arm-linux-gnueabihf
CFLAGS += -L/home/grayhatter/mazda/libs2/lib/
CFLAGS += -I/usr/arm-linux-gnueabihf/include/freetype2/


MAZDA_C_SO =
MAZDA_C_SO = /home/grayhatter/mazda/libs2/lib/libc.so.6

LIBS   +=  -lfreetype -lz
LIBS   +=  -lwayland-ivi-shell-client -lwayland-ivi-client
LIBS   +=  -lwayland-client
LIBS   +=  -lavcodec -lavutil -lavformat -lswresample
LIBS   +=  -lffi -lgcc_s -ldl -lrt -lpthread -lm -lasound
LIBS   +=  -lvorbis -lvorbisfile -logg

%.o: %.c %.h
	$(CC) -c -o $@ $(INCLUDE) -I$(shell dirname $<) $(CFLAGS) $<

hudtds: $(OBJ)
	# $(LD) --verbose -o hudtds $(OBJ) $(LDFLAGS) $(LIBS)
	$(CC) $(OBJ) $(CFLAGS) $(LIBS) $(MAZDA_C_SO) -o $@

all: hudtds

install: hudtds
	cp run-hud.sh /home/grayhatter/mazda/live/tmp/root/
	cp hudtds /home/grayhatter/mazda/live/tmp/root/
	cp SCP.otf /home/grayhatter/mazda/live/tmp/root/

install-audio:
	cp black.mp3 /home/grayhatter/mazda/live/tmp/root/
	cp dream.mp3 /home/grayhatter/mazda/live/tmp/root/
	cp test.ogg /home/grayhatter/mazda/live/tmp/root/
	cp test2.ogg /home/grayhatter/mazda/live/tmp/root/

install-all: install-audio install

clean:
	rm $(OBJ) || true
	rm hudtds || true
