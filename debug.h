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


        case 0x02:
            *chip8->stack_ptr++ = chip8->pc;
            chip8->pc = chip8->instruction.NNN;
            break;

        case 0x6:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set register V%X to 0x%02X",chip8->instruction.X, chip8->instruction.NN);
            break;

        case 0x0A:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length,
                                    "Set I to 0x%04X", chip8->instruction.NNN);
            break;

        default:
            desc_length += snprintf(instruction_desc + desc_length, sizeof(instruction_desc) - desc_length, "Unimplemented Opcode!");
            break;
    }

    // Logging the instruction description
    TraceLog(LOG_INFO, "%s", instruction_desc);
}
#endif

