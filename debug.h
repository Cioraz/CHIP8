#ifdef DEBUG
void print_debug_for_instruction(chip8_t *chip8) {
    char instruction_desc[1000];
    int desc_length = 0;

    // Formatting common parts
    desc_length = snprintf(instruction_desc, sizeof(instruction_desc),
                           "Addr: 0x%04X, Opcode: 0x%04X, Desc: ",
                           chip8->pc - 2, chip8->instruction.opcode);

    // Determining instruction based on opcode
    switch ((chip8->instruction.opcode >> 12) & 0x0F) {
        case 0x0:
            if (chip8->instruction.NN == 0xE0)
                desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Clear Screen");
            else if (chip8->instruction.NN == 0x0EE)
                desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Return from Subroutine to address 0x%04X",*(chip8->stack_ptr-1));
            else
                desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Unimplemented Opcode!");
            break;

        case 0x1:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set PC to NNN 0x%04X",chip8->instruction.NNN);
            break;


        case 0x2:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Calling subroutine at 0x%04X",chip8->instruction.NNN);
            break;
        
        case 0x3:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "VX==KK (0x%02X,0x%02X) PC+=2 (if true)",chip8->V[chip8->instruction.X], chip8->instruction.NN);
            break;

        case 0x4:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "VX!=KK (0x%02X,0x%02X) PC+=2 (if true)",chip8->V[chip8->instruction.X], chip8->instruction.NN);
            break;

        case 0x5:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "VX==VY (0x%02X,0x%02X) PC+=2 (if true)",chip8->V[chip8->instruction.X], chip8->V[chip8->instruction.Y]);
            break;



        case 0x6:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set register V%X to 0x%02X",chip8->instruction.X, chip8->instruction.NN);
            break;

        case 0x7:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Initial V%X 0x%04X updated after adding (kk) 0x%02X to V%X Result:0x%04X",chip8->instruction.X,chip8->V[chip8->instruction.X],chip8->instruction.NN,chip8->instruction.X,chip8->V[chip8->instruction.X]+chip8->instruction.NN);
            break;

        case 0x8:
            switch (chip8->instruction.N){
                case 0x0:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Sets V%X=V%X 0x%04X",chip8->instruction.X,chip8->instruction.Y,chip8->V[chip8->instruction.Y]);
                    break;
                
                case 0x1:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Set V%X |= V%X, Result: 0x%04X",chip8->instruction.X,chip8->instruction.Y,chip8->instruction.X|chip8->instruction.Y);
                    break;

                case 0x2:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Set V%X &= V%X, Result: 0x%04X",chip8->instruction.X,chip8->instruction.Y,chip8->instruction.X&chip8->instruction.Y);
                    break; 

                case 0x3:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Set V%X ^= V%X, Result: 0x%04X",chip8->instruction.X,chip8->instruction.Y,chip8->instruction.X^chip8->instruction.Y);
                    break; 

                case 0x4:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Set V%X+=V%x,if carry VF=1 (currently VF=%X), Result: 0x%04X",chip8->instruction.X,chip8->instruction.Y,((uint16_t)(chip8->V[chip8->instruction.X]+chip8->V[chip8->instruction.Y] > 255)),chip8->V[chip8->instruction.X]+chip8->V[chip8->instruction.Y]);
                    break; 

                case 0x5:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Set V%X-=V%x,if no borrow VF=1 (currently VF=%X), Result: 0x%04X",chip8->instruction.X,chip8->instruction.Y,(chip8->V[chip8->instruction.X]>=chip8->V[chip8->instruction.Y]),chip8->V[chip8->instruction.X]+chip8->V[chip8->instruction.Y]);
                    break; 

                case 0x6:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "ALU, Set V%X>>=1,VF = bit shifted (current VF=%X), Result: 0x%04X",chip8->instruction.X,chip8->V[chip8->instruction.X]>>1,chip8->V[chip8->instruction.X]>>1);
                    break; 



                

                





            }
            break;

        case 0xA:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set I to 0x%04X", chip8->instruction.NNN);
            break;

        case 0xB:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Jumping to nnn(0x%04X)+V[0](0x%04X), Result: 0x%04X", chip8->instruction.NNN,chip8->V[0x0],chip8->instruction.NNN+chip8->V[0x0]);
            break;

        case 0xC:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set V%X=rand()%%256 & NN(0x%04X)", chip8->instruction.X,chip8->instruction.NN);
            break;

        case 0xD:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Drawing N %u height sprite at V%X 0x%02X, V%X 0x%02X from I 0x%04X", chip8->instruction.N,chip8->instruction.X,chip8->V[chip8->instruction.X],chip8->instruction.Y,chip8->V[chip8->instruction.Y],chip8->index_reg);
            break;

        case 0xE:
            switch(chip8->instruction.NN){
                case 0x9E:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Skip inst if key in V%X is pressed, Keypad Val: %d", chip8->instruction.X,chip8->keys[chip8->V[chip8->instruction.X]]);
                    break;

                case 0xA1:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Skip inst if key in V%X is not pressed, Keypad Val: %d", chip8->instruction.X,chip8->keys[chip8->V[chip8->instruction.X]]);
                    break;
            }
            break;

        case 0xF:
            switch(chip8->instruction.NN){
                case 0x07:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set V%X to delay timer", chip8->instruction.X);
                    break;

            }
            break;



        default:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Unimplemented Opcode!");
            break;
    }

    // Logging the instruction description
    TraceLog(LOG_INFO, "%s", instruction_desc);
}
#endif
