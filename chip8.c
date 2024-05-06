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

// Input handler
void handle_input(chip8_t *chip8){
    if(IsKeyPressed(KEY_SPACE)){
        if(chip8->state==RUNNING){
            chip8->state=PAUSED;
            TraceLog(LOG_INFO,"CHIP8 Paused!");
        }else chip8->state=RUNNING;
    }
    if(IsKeyPressed(KEY_ESCAPE)) chip8->state=QUIT;
}

void emulate_instruction(chip8_t *chip8, config_t config){
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

    // Finding opcode category
    uint8_t category=chip8->instruction.opcode>>12&0xF;

    switch(category){
        case 0x0:
            // 00E0: Clear Screen
            if(chip8->instruction.NN==0xE0){
                memset(&chip8->display[0],false,sizeof chip8->display);
            // 00EE: Return from subroutine
            }else if(chip8->instruction.NN==0x0EE){
                chip8->pc = *--chip8->stack_ptr;
            }
            break;

        case 0x1:
            // 1nnn: Jump to location nnn
            chip8->pc=chip8->instruction.NNN;
            break;

        case 0x2:
            // 2nnn: Call subroutine at nnn
            *chip8->stack_ptr++ = chip8->pc;
            chip8->pc = chip8->instruction.NNN;
            break;

        case 0x3:
            // 3xkk: Skip next inst if vx==kk
            if(chip8->V[chip8->instruction.X] == chip8->instruction.NN) chip8->pc+=2;
            break;

        case 0x4:
            // 4xkk: Skip next inst if vx!=kk
            if(chip8->V[chip8->instruction.X] != chip8->instruction.NN) chip8->pc+=2;
            break;

        case 0x5:
            // 5xy0: Skip next inst if vx==vy
            if(chip8->V[chip8->instruction.X] == chip8->V[chip8->instruction.Y]) chip8->pc+=2;
            break;

        case 0x6:
            // 6xkk: Set vx==kk
            chip8->V[chip8->instruction.X] = chip8->instruction.NNN;
            break;
        
        case 0x7:
            // 7xkk: Set vx=vx+kk
            chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]+chip8->instruction.NN;
            break;

        case 0x8:
            switch(chip8->instruction.N){
                case 0x0:
                    // 8xy0: Set vx=vy
                    chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.Y];
                    break;
                
                case 0x1:
                    // 8xy1: Set vx=vx|vy
                    chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]|chip8->V[chip8->instruction.Y];
                    break;

                case 0x2:
                    // 8xy2: Set vx=vx&vy
                    chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]&chip8->V[chip8->instruction.Y];
                    break;

                case 0x3:
                    // 8xy3: Set vx=vx^vy
                    chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.X]^chip8->V[chip8->instruction.Y];
                    break;

                case 0x4:
                    // 8xy4: Set vx=vx+vy, if result more than 8 bit vf=1
                    uint16_t result = chip8->V[chip8->instruction.X]+chip8->V[chip8->instruction.Y];
                    if(result>255) chip8->V[0xF]=1;
                    chip8->V[chip8->instruction.X]+=chip8->V[chip8->instruction.Y];
                    break;
                
                case 0x5:
                    // 8xy5: Set vx=vx-vy, set vf= NOT borrow
                    if(chip8->V[chip8->instruction.X]>=chip8->V[chip8->instruction.Y]) chip8->V[0xF]=1;
                    chip8->V[chip8->instruction.X]-=chip8->V[chip8->instruction.Y];
                    break;
                
                case 0x6:  
                    // 8xy6: If least sig of vx=1, vf=1 and divide vx by 2
                    chip8->V[0xF]=chip8->V[chip8->instruction.X]&1;
                    chip8->V[chip8->instruction.X]>>=1;
                    break;

                case 0x7:
                    // 8xy7: If vy>vx set vf=1 and vx is sub from vy
                    if(chip8->V[chip8->instruction.Y]>=chip8->V[chip8->instruction.X]) chip8->V[0xF]=1;
                    chip8->V[chip8->instruction.X]=chip8->V[chip8->instruction.Y]-chip8->V[chip8->instruction.X];
                    break;

                case 0xE:
                    // 8xye: Skip inst if vx!=vy
                    chip8->V[0xF]=(chip8->V[chip8->instruction.X]&0x80)>>7;
                    chip8->V[chip8->instruction.X]<<=1;
                    break;
            }
            break;

        case 0x9:
            // 9xy0: Skip next inst if vx!=vy
            if(chip8->V[chip8->instruction.X]!=chip8->V[chip8->instruction.Y]) chip8->pc+=2;
            break;

        case 0xA:
            // annn: Set index_reg to nnn
            chip8->index_reg = chip8->instruction.NNN;
            break;

        case 0xB:
            // bnnn: Jump to location nnn+v[0]
            chip8->pc=chip8->V[0]+chip8->instruction.NNN;
            break;
        
        case 0xD:
            // dxyn: Draw n byte sprite at mem location I at vx,vy and set vf=collision

            // Wrap and find X and Y coords
            uint8_t X_coor=chip8->V[chip8->instruction.X]%config.window_w;
            uint8_t Y_coor=chip8->V[chip8->instruction.Y]%config.window_h;

            uint8_t mover = X_coor;
            chip8->V[0xF]=0;

            // Loop over height of sprite
            for(uint8_t i=0;i<chip8->instruction.N;i++){
                // Fetch sprite data
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

        case 0xF:
            chip8->V[chip8->instruction.X]=chip8->deplay_timer;
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
    // Setting rectangle dimensions
    int rect_x,rect_y;
    for(uint32_t i=0;i<sizeof chip8->display;i++){
        rect_x=(i%config.window_w)*config.scale_factor; 
        rect_y=(i/config.window_w)*config.scale_factor;

        // If pixel ON draw  white color
        if(chip8->display[i]) {
            // Rect + outline
            DrawRectangle(rect_x,rect_y,config.scale_factor,config.scale_factor,WHITE);
            DrawRectangleLines(rect_x,rect_y,config.scale_factor,config.scale_factor,BLACK);
        }
        // If pixel OFF draw black color
        else DrawRectangle(rect_x,rect_y,config.scale_factor,config.scale_factor,BLACK);
    } 
}

// Main Function
int main(int argc,char **argv){
    // Will use later
    (void)argc;
    (void)argv;

    // Invalid inputs for args
    if(argc<2){
        TraceLog(LOG_ERROR,"Usage: ./chip8 <ROM_NAME>");
        exit(EXIT_FAILURE);
    }

    // Logging info
    SetTraceLogLevel(LOG_ALL);

    // Setting up configs
    config_t config;
    chip8_t *chip8 = malloc(sizeof(chip8_t));

    SetTargetFPS(60);

    // Main configs
    if(!set_config(&config,argc,argv)) exit(EXIT_FAILURE);
    if(!init_chip8(chip8,argv[1])) exit(EXIT_FAILURE);
    InitWindow(config.window_w*config.scale_factor,config.window_h*config.scale_factor,"CHIP8 Emulator");

    while(chip8->state!=QUIT){
        handle_input(chip8);
        if(chip8->state!=PAUSED) emulate_instruction(chip8,config);
        BeginDrawing();
        ClearBackground(BLACK);
        updateScreen(chip8,config);
        EndDrawing();

    }

    CloseWindow();
    return 0;
}
