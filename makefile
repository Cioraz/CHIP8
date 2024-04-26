all:
	gcc chip8.c -o chip8 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

clean:
	rm -rf ./chip8
