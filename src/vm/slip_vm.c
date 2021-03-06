
#include <stdio.h>
#include "slip_vm.h"


void seedRandom() {
    srand(time(NULL));
}


void slipInitConfig(SlipConfig* config) {
    config->inputFn = NULL;
    config->displayFn = NULL;
    config->writeFn = NULL;
}


SlipVM* slipNewVM(SlipConfig* config) {
    SlipVM* vm = (SlipVM*)malloc(sizeof(SlipVM));

    // Copy config
    vm->config = *config;

    // Memory
    vm->memory = (SlipByte*)malloc(SLIP_MEM * sizeof(SlipByte));

    for (int i = 0; i < SLIP_MEM; i++) {
        vm->memory[i] = 0x0;
    }

    // Registers
    for (int i = 0; i < SLIP_REGISTERS; i++) {
        vm->V[i] = 0x0;
    }

    vm->I = 0x0;

    // Stack
    vm->SP = 0x0;

    for (int i = 0; i < SLIP_STACK; i++) {
        vm->stack[i] = 0x0;
    }

    // Timers
    vm->delayTimer = 0x0;
    vm->soundTimer = 0x0;

    // Display
    vm->display = malloc(SLIP_SCREEN_WIDTH * SLIP_SCREEN_HEIGHT * sizeof(SlipByte));
    for (int i = 0; i < (SLIP_SCREEN_WIDTH * SLIP_SCREEN_HEIGHT); i++) {
        vm->display[i] = 0x0;
    }

    // Keys
    for (int i = 0; i < SLIP_NUM_KEYS; i++) {
        vm->keys[i] = 0x0;
    }

    // Program Counter
    vm->PC = 0x0;

    return vm;
}


void slipFreeVM(SlipVM* vm) {
    free(vm->memory);
    free(vm->display);
    free(vm);
}


// Load bytecode from buffer into vm memory
void _slipLoadBytecodeIntoMemory(SlipVM* vm, SlipBytecode* bytecode) {

    int16_t i = SLIP_MEM_START;

    // TODO: Handle case of bytecode not fitting in memory
    for (size_t j = 0; j < bytecode->size; j++ ) {
        vm->memory[i++] = bytecode->buffer[j];
    }
}


void slipInterpretBytecode(SlipVM* vm, SlipBytecode* bytecode) {
    // TODO: Timers
    vm->config.writeFn("Starting interpreter...\n");
    _slipLoadBytecodeIntoMemory(vm, bytecode);

    vm->PC = SLIP_MEM_START;
    uint16_t opcode;

    // Break out of loop after counter reaches 0
    int break_after = 10;

    // TODO: Use for loop
    while (vm->PC < SLIP_MEM-1) {
        opcode = (vm->memory[vm->PC] << 8) | vm->memory[vm->PC+1];
        slipOpcodeDispatch(vm, opcode);

        // TODO: Program counter will be incremented by opcodes. For now, break
        // to avoid infinite loop.
        if (break_after <= 0) {
            break;
        } else {
            break_after--;
        }
    }

}


void slipLoad(SlipVM* vm, SlipBytecode* bytecode) {
    _slipLoadBytecodeIntoMemory(vm, bytecode);
}


void slipReset(SlipVM* vm) {
    vm->PC = SLIP_MEM_START;
}


void slipStep(SlipVM* vm) {
    uint16_t opcode = (vm->memory[vm->PC] << 8) | vm->memory[vm->PC+1];
    slipOpcodeDispatch(vm, opcode);
}


void slipKeyDown(SlipVM* vm, const uint8_t key) {
    vm->keys[key % SLIP_NUM_KEYS] = true;
}


void slipKeyUp(SlipVM* vm, const uint8_t key) {
    vm->keys[key % SLIP_NUM_KEYS] = false;
}

bool slipIsKeyDown(SlipVM* vm, const uint8_t key) {
    return vm->keys[key % SLIP_NUM_KEYS];
}


static void
draw(SlipVM* vm, const SlipByte x, const SlipByte y, const SlipByte rows) {
    printf("Drawing (%d, %d) n=%d\n", x, y, rows);
    bool collision = false;

    for (int n = 0; n < rows; n++) {
        SlipByte spriteRow = vm->memory[vm->I + n];

        for (int b = 7; b >= 0; b--) {
            // Coordinates are wrapped around the edge of the screen. The X
            // coordinate must increase with the column position of the sprite
            // row.
            int i = ((x + (7 - b)) % SLIP_SCREEN_WIDTH)
                    + ((y + n) % SLIP_SCREEN_HEIGHT) * SLIP_SCREEN_WIDTH;

            SlipByte pixel = (spriteRow >> b) & 0x1;

            // Pixels are XORed to the screen. If the flipping clears a pixel,
            // the flag register is set to 1, otherwise 0.
            SlipByte prev = vm->display[i];
            vm->display[i] ^= pixel;
            if (prev == 1 && vm->display[i] == 0)
                collision = true;
            //printf("%d", vm->display[i]);
        }
    }

    vm->V[0xF] = collision ? 1 : 0;
}


void slipOpcodeDispatch(SlipVM* vm, uint16_t opcode) {
    printf("[0x%04x] 0x%04x ", vm->PC, opcode);

    switch (opcode) {

        // 00EE
        // Returns from a subroutine
        case 0x00EE:
            // Move pointer back before getting the address off it
            vm->PC = vm->stack[--vm->SP];

            // Returning to the saved address will jut result in another call,
            // causing an infinite loop.
            vm->PC += 2;
            printf("Returning to 0x%04x", vm->PC);
        break;

        default:
            switch(SLIP_OP_A(opcode)) {
                // 1NNN
                // Jumps to address NNN
                case 0x1:
                    printf("Jump to 0x%04x", SLIP_OP_BCD(opcode));
                    vm->PC = SLIP_OP_BCD(opcode);
                break;

                // 2NNN
                // Call a subroutine at NNN
                case 0x2:
                    printf("Call 0x%04x", SLIP_OP_BCD(opcode));
                    // Store current address on stack
                    vm->stack[vm->SP++] = vm->PC;
                    vm->PC = SLIP_OP_BCD(opcode);
                break;

                // 3XNN
                // Skips the next instruction if VX equals NN
                case 0x3: ; // Empty statement
                    if (vm->V[SLIP_OP_B(opcode)] == SLIP_OP_CD(opcode)) {
                        vm->PC += 4;
                        printf("Skip to 0x%04x", vm->PC);
                    } else {
                        vm->PC += 2;
                        printf("Don't skip");
                    }
                break;

                // 4XNN
                // Skips the next instruction if VX doesn't equal NN
                case 0x4: ; // Empty statement
                    if (vm->V[SLIP_OP_B(opcode)] != SLIP_OP_CD(opcode)) {
                        vm->PC += 4;
                        printf("Skip to 0x%04x", vm->PC);
                    } else {
                        vm->PC += 2;
                        printf("Don't skip");
                    }
                break;

                // 5XY0
                // Skips the next instruction if VX equals VY
                case 0x5:
                    if (vm->V[SLIP_OP_B(opcode)] == vm->V[SLIP_OP_C(opcode)]) {
                        vm->PC += 4;
                        printf("Skip to 0x%04x", vm->PC);
                    } else {
                        vm->PC += 2;
                        printf("Don't skip");
                    }
                break;

                // 6XNN
                // Sets VX to NN
                case 0x6:
                    vm->V[SLIP_OP_B(opcode)] = SLIP_OP_CD(opcode);
                    vm->PC += 2;
                break;

                // 7XNN
                // Adds NN to VX
                case 0x7:
                    vm->V[SLIP_OP_B(opcode)] += SLIP_OP_CD(opcode);
                    vm->PC += 2;
                break;

                // 9XY0
                // Skip next instruction if VX does not equal VY
                case 0x9:
                    if (vm->V[SLIP_OP_B(opcode)] != vm->V[SLIP_OP_C(opcode)])
                        vm->PC += 2;
                    vm->PC += 2;
                break;

                // ANNN
                // Set I to NNN
                case 0xA:
                    vm->I = SLIP_OP_BCD(opcode);
                    vm->PC += 2;
                break;

                // BNNN
                // Set I to NNN plus V0
                case 0xB:
                    vm->I = SLIP_OP_BCD(opcode) + vm->V[0x0];
                    vm->PC += 2;
                break;

                // CXNN
                // Set VX to a random number masked to NN
                case 0xC:
                    vm->V[SLIP_OP_B(opcode)] = (rand() % 0xFF) & SLIP_OP_CD(opcode);
                    vm->PC += 2;
                break;

                // EX9E
                // Skips the next instruction if the key stored in VX is pressed
                case 0xE:
                    if (slipIsKeyDown(vm, vm->V[SLIP_OP_B(opcode)]))
                        vm->PC += 2;
                    vm->PC += 2;
                break;

                // DXYN
                // Draws sprite pointed to by register I.
                case 0xD:
                    draw(vm, vm->V[SLIP_OP_B(opcode)], vm->V[SLIP_OP_C(opcode)],
                        SLIP_OP_D(opcode));
                    vm->PC += 2;
                break;

                // 8***
                // Arithmetic opcodes
                case 0x8:
                    switch(SLIP_OP_D(opcode)) {

                        // 8XY0
                        // Sets VX to the value of VY
                        case 0x0:
                            printf("Set V0x%01x to V0x%01x value V0x%02x",
                                SLIP_OP_B(opcode), SLIP_OP_C(opcode),
                                vm->V[SLIP_OP_C(opcode)]);
                            vm->V[SLIP_OP_B(opcode)] = vm->V[SLIP_OP_C(opcode)];
                            vm->PC += 2;
                        break;

                        // 8XY1
                        // Sets VX to VX OR VY
                        case 0x1:
                            printf("Set V0x%01x to V0x%01x OR V0x%01x",
                                SLIP_OP_B(opcode), SLIP_OP_B(opcode),
                                SLIP_OP_C(opcode));
                            vm->V[SLIP_OP_B(opcode)] |= vm->V[SLIP_OP_C(opcode)];
                            vm->PC += 2;
                        break;

                        // 8XY2
                        // Sets VX to VX AND VY
                        case 0x2:
                            printf("Set V0x%01x to V0x%01x AND V0x%01x",
                                SLIP_OP_B(opcode), SLIP_OP_B(opcode),
                                SLIP_OP_C(opcode));
                            vm->V[SLIP_OP_B(opcode)] &= vm->V[SLIP_OP_C(opcode)];
                            vm->PC += 2;
                        break;

                        // 8XY3
                        // Sets VX to VX XOR VY
                        case 0x3:
                            printf("Set V0x%01x to V0x%01x XOR V0x%01x",
                                SLIP_OP_B(opcode), SLIP_OP_B(opcode),
                                SLIP_OP_C(opcode));
                            vm->V[SLIP_OP_B(opcode)] ^= vm->V[SLIP_OP_C(opcode)];
                            vm->PC += 2;
                        break;

                        // 8XY4
                        // Adds VY to VX. VF is set to 1 when there's a carry,
                        // and to 0 when there isn't.
                        case 0x4:
                            printf("Add V%01x to V%01x", SLIP_OP_C(opcode),
                                SLIP_OP_B(opcode));
                            uint16_t result = vm->V[SLIP_OP_B(opcode)]
                                            + vm->V[SLIP_OP_C(opcode)];
                            vm->V[0xF] = result > 0xFF ? 1 : 0;
                            vm->V[SLIP_OP_B(opcode)] = (SlipByte) (result & 0xFF);
                            vm->PC += 2;
                        break;

                        // 8XY5
                        // VY is subtracted from VX. VF is set to 0 when there's
                        // a borrow, and 1 when there isn't.
                        case 0x5:
                            printf("Subtract V%01x from V%01x",
                                SLIP_OP_C(opcode), SLIP_OP_B(opcode));
                            vm->V[0xF] = vm->V[SLIP_OP_B(opcode)] < vm->V[SLIP_OP_C(opcode)] ? 1 : 0;
                            vm->V[SLIP_OP_B(opcode)] = vm->V[SLIP_OP_B(opcode)] - vm->V[SLIP_OP_C(opcode)];
                            vm->PC += 2;
                        break;

                        // 8XY6
                        // Shifts VX right by one. VF is set to the value of the
                        // least significant bit of VX before the shift. Y is
                        // ignored.
                        case 0x6:
                            printf("Shift V%01x right by 1", SLIP_OP_B(opcode));
                            vm->V[0xF] = vm->V[SLIP_OP_B(opcode)] & 0x0001;
                            vm->V[SLIP_OP_B(opcode)] >>= 1;
                            vm->PC += 2;
                        break;

                        // 8XY7
                        // VX is subtracted from VY. VF is set to 0 when there's
                        // a borrow, and 1 when there isn't.
                        case 0x7:
                            printf("Subtract V%01x from V%01x",
                                SLIP_OP_B(opcode), SLIP_OP_C(opcode));
                            vm->V[0xF] = vm->V[SLIP_OP_B(opcode)] > vm->V[SLIP_OP_C(opcode)] ? 1 : 0;
                            vm->V[SLIP_OP_B(opcode)] = vm->V[SLIP_OP_C(opcode)] - vm->V[SLIP_OP_B(opcode)];
                            vm->PC += 2;
                        break;

                        // 8XYE
                        // Shift VX left by one. VF is set to the value of the
                        // most significant bit of VX before the shift. Y is
                        // ignored
                        case 0xE:
                            printf("Shift V%01x left by 1", SLIP_OP_B(opcode));
                            vm->V[0xF] = (vm->V[SLIP_OP_B(opcode)] & 0x80) >> 7;
                            vm->V[SLIP_OP_B(opcode)] <<= 1;
                            vm->PC += 2;
                        break;
                    }
                break;

                case 0xF:
                    switch(SLIP_OP_CD(opcode)) {

                        // FX55
                        // Stores contents of V0 to VX (including VX) in memory
                        // starting at address stored in I.
                        case 0x55:
                            for (int v = 0; v <= SLIP_OP_B(opcode); v++) {
                                vm->memory[vm->I + v] = vm->V[v];
                            }
                            vm->PC += 2;
                        break;

                        // FX65
                        // Stores contents of memory starting at I in registers
                        // V0 to VX (including VX).
                        case 0x65:
                            for (int v = 0; v <= SLIP_OP_B(opcode); v++) {
                                vm->V[v] = vm->memory[vm->I + v];
                            }
                            vm->PC += 2;
                        break;
                    }
                break;
            }
        break;
    }

    printf("\n");

}


// Prints out the contents of the vm's memory. The `start` argument is
// inclusive and `end` is exclusive.
void slipDumpMemoryRange(SlipVM* vm, uint16_t start, uint16_t end) {
    printf("Memory Dump\n");
    uint16_t newend = end > SLIP_MEM ? SLIP_MEM : end;

    // end before start is invalid
    if (start > newend) {
        uint16_t tmp = newend;
        newend = start;
        start = tmp;
    }

    for (uint16_t i = start; i < newend; i++) {
        printf("[0x%04x] 0x%02x\n", i, vm->memory[i]);
    }
}


void slipDumpKeys(SlipVM* vm) {
    for (int i = 0; i < SLIP_NUM_KEYS; i++) {
        printf("0x%02x %s\n", i, vm->keys[i] ? "down" : "up");
    }
}


void slipDumpDisplay(SlipVM* vm) {
    unsigned char linebuf[SLIP_SCREEN_WIDTH+1];
    SlipByte* display = vm->display;

    linebuf[SLIP_SCREEN_WIDTH+1] = '\0';

    for (int y = 0; y < SLIP_SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SLIP_SCREEN_WIDTH; x++) {
            int i = x + y * SLIP_SCREEN_WIDTH;
            linebuf[x] = display[i] != 0 ? '#' : ' ';
        }
        printf("%s\n", linebuf);
    }
}


SlipByte slipGetMem(SlipVM* vm, const uint16_t address) {
    return vm->memory[address % SLIP_MEM];
}

void slipSetMem(SlipVM* vm, uint16_t address, SlipByte value) {
    vm->memory[address] = value;
}
