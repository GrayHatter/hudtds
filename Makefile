CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

OBJ = hud.o sound.o wayland.o info.o
OBJ += wl/keyboard.o wl/touch.o wl/draw.o wl/ui.o

CFLAGS := -Wall -Wextra -Werror $(CFLAGS)

CFLAGS += -std=c11 -O0 -mcpu=arm7

CFLAGS += -mfloat-abi=hard
CFLAGS += --sysroot=/usr/arm-linux-gnueabihf
CFLAGS += -L/home/grayhatter/mazda/libs2/lib/

LIBS   := -lwayland-client -lwayland-ivi-shell-client  -lwayland-ivi-client -lffi -lgcc_s -lwayland-server -ldl -lrt -lpthread -lm -lasound -lvorbis -lvorbisfile -logg

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

clean:
	rm $(OBJ) || true
	rm hudtds || true
