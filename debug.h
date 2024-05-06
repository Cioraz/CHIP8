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
                desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Return from Subroutine to address");
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
                                    "VX==KK (0x%04X,0x%04X) PC+=2 0x%04X",chip8->V[chip8->instruction.X], chip8->instruction.NN,chip8->pc);
            break;

        case 0x4:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "VX!=KK (0x%04X,0x%04X) PC+=2 0x%04X",chip8->V[chip8->instruction.X], chip8->instruction.NN,chip8->pc);
            break;

        case 0x5:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "VX==VY (0x%04X,0x%04X) PC+=2 0x%04X",chip8->V[chip8->instruction.X], chip8->V[chip8->instruction.Y],chip8->pc);
            break;



        case 0x6:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set register V%X to 0x%02X",chip8->instruction.X, chip8->instruction.NN);
            break;

        case 0x7:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Updates VK by adding KK");
            break;

        case 0x8:
            switch (chip8->instruction.N){
                case 0x0:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set VX=VY");
                    break;
                
                case 0x1:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set VX=VX|VY");
                    break;

                case 0x2:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set VX=VX&VY");
                    break; 

                case 0x3:
                    desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set VX=VX^VY");
                    break;  





            }
            break;

        case 0x0A:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set I to 0x%04X", chip8->instruction.NNN);
            break;

        case 0x0D:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Drawing N %u height sprite at V%X 0x%02X, V%X 0x%02X from I 0x%04X", chip8->instruction.N,chip8->instruction.X,chip8->V[chip8->instruction.X],chip8->instruction.Y,chip8->V[chip8->instruction.Y],chip8->index_reg);
            break;

        default:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Unimplemented Opcode!");
            break;
    }

    // Logging the instruction description
    TraceLog(LOG_INFO, "%s", instruction_desc);
}
#endif
