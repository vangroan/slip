
#include <stdio.h>
#include "slip_vm.h"


// Prints out the contents of the vm's memory. The `start` argument is 
// inclusive and `end` is exclusive.
void _dumpMemoryRange(SlipVM* vm, uint16_t start, uint16_t end) {
    uint16_t newstart = start < 0 ? 0 : start;
    uint16_t newend = end > SLIP_MEM ? SLIP_MEM : end;

    // end before start is invalid
    if (newstart > newend) {
        uint16_t tmp = newend;
        newend = newstart;
        newstart = tmp;
    }

    for (uint16_t i = newstart; i < newend; i++) {
        printf("[0x%04x] 0x%02x\n", i, vm->memory[i]);
    }
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
    vm->display = (SlipByte*)malloc(SLIP_SCREEN_WIDTH * SLIP_SCREEN_HEIGHT * sizeof(SlipByte));

    // Keys
    for (int i = 0; i < SLIP_KEYS; i++) {
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

    _dumpMemoryRange(vm, 0x200, 0x200 + bytecode->size);

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
                    }
                break;
            }
        break;
    }

    printf("\n");

}
