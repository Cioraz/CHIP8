#include <stdint.h>
#include <stdbool.h>

typedef struct{
    uint32_t window_h;
    uint32_t window_w;
    uint32_t scale_factor;
    uint32_t emulation_speed;
} config_t;

typedef enum{
    QUIT,
    RUNNING,
    PAUSED,
} emulator_state_t;

typedef struct{
    uint16_t opcode;
    uint16_t NNN;
    uint8_t NN;
    uint8_t N;
    uint8_t X;
    uint8_t Y;
} inst_t;

typedef struct{
    emulator_state_t state;
    uint8_t ram[4096];
    uint16_t stack[12];
    uint16_t *stack_ptr;
    uint16_t pc;
    uint16_t index_reg;
    bool display[64*32];
    uint8_t V[16];
    uint8_t deplay_timer;
    uint8_t sound_timer;
    bool keys[16];
    char *rom_name;
    inst_t instruction;
} chip8_t;


