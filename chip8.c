#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "structs.h"
#include "raylib.h"

bool pause=false;

bool set_config(config_t *config,int argc,char **argv){
    config->window_h=32;
    config->window_w=64;
    config->scale_factor=20;
    for(int i=1;i<argc;i++) (void)argv[i];
    return true;
}

void handle_input(chip8_t *chip8){
    (void)chip8;
    if(IsKeyPressed(KEY_SPACE)) pause=!pause;
}

bool init_chip8(chip8_t *chip8,char *rom_name){
    uint32_t entry = 0x200; 
    uint8_t font[] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
        0x20, 0x60, 0x20, 0x20, 0x70,		// 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
        0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
        0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
        0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
        0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
        0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
        0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
        0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
        0xF0, 0x80, 0xF0, 0x80, 0x80		
    };

    memcpy(&chip8->ram[0],font,sizeof font);
    FILE *rom = fopen(rom_name,"rb");
    if(!rom){
        printf("ROM %s doesnt exist \n",rom_name);
        return false;
    }
    fseek(rom,0,SEEK_END);
    size_t rom_size = ftell(rom);
    size_t max_size = sizeof chip8->ram - entry;
    rewind(rom);

    if(rom_size > max_size){
        printf("Rom file out of size!\n");
        return false;
    }

    if(fread(&chip8->ram[entry],rom_size,1,rom)!=1){
        printf("Unable to read Rom file %s\n",rom_name);
        return false;
    }

    fclose(rom);

    chip8->pc = entry;
    chip8->rom_name = rom_name;
    return true;
}

int main(int argc,char **argv){
    (void)argc;
    (void)argv;
    config_t config;
    chip8_t *chip8 = malloc(sizeof(chip8_t));

    SetTargetFPS(60);
    if(!set_config(&config,argc,argv)) exit(EXIT_FAILURE);
    if(!init_chip8(chip8,argv[1])) exit(EXIT_FAILURE);

    InitWindow(config.window_w*config.scale_factor,config.window_h*config.scale_factor,"CHIP8 Emulator");

    while(!WindowShouldClose()){
        handle_input(chip8);
        BeginDrawing();
        ClearBackground(BLACK);
        EndDrawing();
    }

    CloseWindow();
    return 0;

}
