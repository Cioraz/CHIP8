#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "structs.h"
#include "raylib.h"

bool pause=true;

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

#ifdef DEBUG
void print_debug_for_instruction(chip8_t *chip8){
    char instruction_desc[1000];
    snprintf(instruction_desc,sizeof instruction_desc,"Addr: 0x%04X, Opcode: 0x%04X, Desc: ",chip8->pc-2,chip8->instruction.opcode);
    switch((chip8->instruction.opcode>>12)& 0x0F){
        case 0x0:
            if(chip8->instruction.NN==0xE0){
                strcat(instruction_desc,"Clear Screen");
            }else if(chip8->instruction.NN==0x0EE){
                strcat(instruction_desc,"Return from Subroutine to address");
            }else strcat(instruction_desc,"Unimplemented Opcode!");
            break;

        case 0x02:
            *chip8->stack_ptr++ = chip8->pc;
            chip8->pc = chip8->instruction.NNN;
            break;
        
        case 0x6:
            strcat(instruction_desc, "Set register V ");
            sprintf(&instruction_desc[strlen(instruction_desc)], "%X", chip8->instruction.NNN);  // Efficient formatting within string
            break; 

       case 0x0A:
            sprintf(instruction_desc + strlen(instruction_desc), "Set I to 0x%04X", chip8->instruction.NNN);
            break; 

        default:
            strcat(instruction_desc,"Unimplemented Opcode!");
            break;
    }
    TraceLog(LOG_INFO,"%s",instruction_desc);
}
#endif

void emulate_instruction(chip8_t *chip8){
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

        case 0x2:
            *chip8->stack_ptr++ = chip8->pc;
            chip8->pc = chip8->instruction.NNN;
            break;

        case 0x6:
            chip8->V[chip8->instruction.X] = chip8->instruction.NNN;
            break;
        
        case 0xA:
            chip8->index_reg = chip8->instruction.NNN;
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
        if(chip8->state==PAUSED){
            BeginDrawing();
            ClearBackground(BLACK);
            EndDrawing();
        }else {
            emulate_instruction(chip8);
            BeginDrawing();
            ClearBackground(BLACK);
            // Draw your CHIP8 display here
            EndDrawing();
        }
    }

    CloseWindow();
    return 0;

}
