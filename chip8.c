#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "structs.h"
#include "raylib.h"
#include "debug.h"

bool set_config(config_t *config,int argc,char **argv){
    config->window_h=32;
    config->window_w=64;
    config->scale_factor=20;
    for(int i=1;i<argc;i++) (void)argv[i];
    return true;
}

void handle_input(chip8_t *chip8){
    if(IsKeyPressed(KEY_SPACE)){
        if(chip8->state==RUNNING){
            chip8->state=PAUSED;
            TraceLog(LOG_INFO,"Paused Currently!");
        }else chip8->state=RUNNING;
    }
    if(IsKeyPressed(KEY_ESCAPE)) chip8->state=QUIT;
}

void emulate_instruction(chip8_t *chip8, config_t config){
    (void) config;
    chip8->instruction.opcode = chip8->ram[chip8->pc]<<8|chip8->ram[chip8->pc+1];
    chip8->pc+=2;

    chip8->instruction.NNN = chip8->instruction.opcode & 0x0FFF;
    chip8->instruction.NN = chip8->instruction.opcode & 0x0FF;
    chip8->instruction.N = chip8->instruction.opcode & 0x0F;
    chip8->instruction.X = (chip8->instruction.opcode>>8) & 0x0F;
    chip8->instruction.Y = (chip8->instruction.opcode>>4) & 0x0F;

#ifdef DEBUG
    print_debug_for_instruction(chip8);
#endif

    switch((chip8->instruction.opcode>>12)& 0x0F){
        case 0x0:
            if(chip8->instruction.NN==0xE0){
                memset(&chip8->display[0],false,sizeof chip8->display);
            }else if(chip8->instruction.NN==0x0EE){
                chip8->pc = *--chip8->stack_ptr;
            }
            break;

        case 0x1:
            chip8->pc=chip8->instruction.NNN;
            break;

        case 0x2:
            *chip8->stack_ptr++ = chip8->pc;
            chip8->pc = chip8->instruction.NNN;
            break;

        // case 0x3:
        //     if(chip8->V[chip8->instruction.X] == chip8->instruction.NN) chip8->pc+=2;
        //     break;

        // case 0x4:
        //     if(chip8->V[chip8->instruction.X] != chip8->instruction.NN) chip8->pc+=2;
        //     break;

        // case 0x5:
        //     if(chip8->V[chip8->instruction.X] == chip8->V[chip8->instruction.Y]) chip8->pc+=2;
        //     break;

        case 0x6:
            chip8->V[chip8->instruction.X] = chip8->instruction.NNN;
            break;
        
        case 0x7:
            chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]+chip8->instruction.NN;
            break;

        // case 0x8:
        //     switch(chip8->instruction.N){
        //         case 0x0:
        //             chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.Y];
        //             break;
                
        //         case 0x1:
        //             chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]|chip8->V[chip8->instruction.Y];
        //             break;

        //         case 0x2:
        //             chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]&chip8->V[chip8->instruction.Y];
        //             break;

        //         case 0x3:
        //             chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]^chip8->V[chip8->instruction.Y];
        //             break;

        //         case 0x4:
        //             break;
                
        //         case 0x5:
        //             break;
                
        //         case 0x6:
        //             break;

        //         case 0x7:
        //             break;

        //         case 0xE:
        //             break;
        //     }
        //     break;
        
        case 0xA:
            chip8->index_reg = chip8->instruction.NNN;
            break;
        
        case 0xD:
            uint8_t X_coor=chip8->V[chip8->instruction.X]%config.window_w;
            uint8_t Y_coor=chip8->V[chip8->instruction.Y]%config.window_h;
            uint8_t mover = X_coor;
            chip8->V[0xF]=0;

            // Loop over N rows 
            for(uint8_t i=0;i<chip8->instruction.N;i++){
                // Fetch next byte
                uint8_t sprite=chip8->ram[chip8->index_reg+i];
                X_coor=mover; // Restting X_coor for new row to draw

                for(int8_t j=7;j>=0;j--){
                    bool *pixel = &chip8->display[Y_coor*config.window_w+X_coor];
                    bool sprite_bit=sprite&(1<<j);
                    if(sprite_bit&&(*pixel)) chip8->V[0xF]=1;
                    *pixel^=sprite_bit;

                    // Stops drawing sprite if row hits right edge of screen
                    if(++X_coor>=config.window_w)break;
                }
                // Stop entire sprite drawing if hits bottom edge
                if(++Y_coor>=config.window_h) break;

            }
            break;

        default:
            break;
    }
}

bool init_chip8(chip8_t *chip8,char *rom_name){
    chip8->state=RUNNING;
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
        TraceLog(LOG_ERROR,"ROM %s doesnt exist \n",rom_name);
        return false;
    }
    fseek(rom,0,SEEK_END);
    size_t rom_size = ftell(rom);
    size_t max_size = sizeof chip8->ram - entry;
    rewind(rom);

    if(rom_size > max_size){
        TraceLog(LOG_ERROR,"Rom file out of size!\n");
        return false;
    }

    if(fread(&chip8->ram[entry],rom_size,1,rom)!=1){
        TraceLog(LOG_ERROR,"Unable to read Rom file %s\n",rom_name);
        return false;
    }

    fclose(rom);

    chip8->pc = entry;
    chip8->rom_name = rom_name;
    chip8->stack_ptr = &chip8->stack[0];
    return true;
}

void updateScreen(chip8_t *chip8,config_t config){
    int rect_x,rect_y;
    for(uint32_t i=0;i<sizeof chip8->display;i++){
        rect_x=(i%config.window_w)*config.scale_factor; 
        rect_y=(i/config.window_w)*config.scale_factor;

        // If pixel ON draw  white color
        if(chip8->display[i]) DrawRectangle(rect_x,rect_y,config.scale_factor,config.scale_factor,WHITE);
        // If pixel OFF draw black color
        else DrawRectangle(rect_x,rect_y,config.scale_factor,config.scale_factor,BLACK);
    } 
}

int main(int argc,char **argv){
    (void)argc;
    (void)argv;

    if(argc<2){
        TraceLog(LOG_ERROR,"Usage: ./chip8 <ROM_NAME>");
        exit(EXIT_FAILURE);
    }

    SetTraceLogLevel(LOG_ALL);
    config_t config;
    chip8_t *chip8 = malloc(sizeof(chip8_t));

    SetTargetFPS(60);
    if(!set_config(&config,argc,argv)) exit(EXIT_FAILURE);
    if(!init_chip8(chip8,argv[1])) exit(EXIT_FAILURE);
    InitWindow(config.window_w*config.scale_factor,config.window_h*config.scale_factor,"CHIP8 Emulator");

    while(chip8->state!=QUIT){
        handle_input(chip8);
        if(chip8->state!=PAUSED){
            emulate_instruction(chip8,config);
        }
        BeginDrawing();
        ClearBackground(BLACK);
        updateScreen(chip8,config);
        EndDrawing();

    }

    CloseWindow();
    return 0;
}
