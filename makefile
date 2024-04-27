CFLAGS=-std=c17 -Wall -Wextra -Werror

all:
	gcc chip8.c -o chip8 $(CFLAGS) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

debug:
	gcc chip8.c -o chip8 $(CFLAGS) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -DDEBUG


clean:
	rm -rf ./chip8
