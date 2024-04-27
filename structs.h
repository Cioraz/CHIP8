#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint32_t window_h;
    uint32_t window_w;
    uint32_t scale_factor;
} config_t;

typedef struct{
    uint8_t ram[4096];
    uint16_t stack[12];
    uint16_t pc;
    uint16_t index_reg;
    bool display[64*32];
    uint8_t V[16];
    uint8_t deplay_timer;
    uint8_t sound_timer;
    bool keypad[16];
    char *rom_name;
} chip8_t;
