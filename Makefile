CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

OBJ = hud.o sound.o wayland.o info.o
OBJ += wl/keyboard.o wl/touch.o wl/draw.o wl/ui.o wl/text.o
OBJ += gui/root.o gui/nav.o

CFLAGS := -Wall -Wextra -Werror $(CFLAGS)
CFLAGS += -Wno-sign-compare

CFLAGS += -std=c11 -O0 -mcpu=arm7

CFLAGS += -mfloat-abi=hard
CFLAGS += --sysroot=/usr/arm-linux-gnueabihf
CFLAGS += -L/home/grayhatter/mazda/libs2/lib/
CFLAGS += -I/usr/arm-linux-gnueabihf/include/freetype2/

LIBS   :=  -lfreetype -lz -lwayland-client -lwayland-ivi-shell-client -lwayland-ivi-client
LIBS   +=  -lffi -lgcc_s -lwayland-server -ldl -lrt -lpthread -lm -lasound
LIBS   +=  -lvorbis -lvorbisfile -logg

%.o: %.c %.h
	$(CC) -c -o $@ $(INCLUDE) -I$(shell dirname $<) $(CFLAGS) $<

hudtds: $(OBJ)
	# $(LD) --verbose -o hudtds $(OBJ) $(LDFLAGS) $(LIBS)
	$(CC) $(OBJ) $(CFLAGS) $(LIBS) /home/grayhatter/mazda/libs2/lib/libc.so.6 -o $@

all: hudtds

install: hudtds
	cp hudtds /home/grayhatter/mazda/live/tmp/root/
	cp test.ogg /home/grayhatter/mazda/live/tmp/root/
	cp test2.ogg /home/grayhatter/mazda/live/tmp/root/
	cp run-hud.sh /home/grayhatter/mazda/live/tmp/root/
	cp SCP.otf /home/grayhatter/mazda/live/tmp/root/

clean:
	rm $(OBJ) || true
	rm hudtds || true
