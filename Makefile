CC = arm-linux-gnueabihf-gcc
LD = arm-linux-gnueabihf-ld

OBJ = hud.o sound.o wl/keyboard.o

CFLAGS := -Wall -Wextra -Werror -std=c11 -fPIC -mcpu=arm7 -O3 $(CFLAGS)
LIBS   := -lasound -lwayland-client -lpthread -lm -ldl -lrt -lffi -lgcc_s -lvorbis -lvorbisfile -logg

%.o: %.c
	$(CC) -c -o $@ $(INCLUDE) -I$(shell dirname $<) $(CFLAGS) $<

hudtds: $(OBJ)
	$(CC) -o hudtds $(OBJ) $(CFLAGS) $(LIBS)

all: hudtds

clean:
	rm *.o
	rm hudtds
