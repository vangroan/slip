
#include <stdio.h>
#include "slip_vm.h"

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

void _slipLoadBytecode(SlipVM* vm, const char* bytecode) {
    
    int16_t b = 0;
    int16_t i = SLIP_MEM_START;

    char c = bytecode[b++];

    while (c != '\0') {
        vm->memory[i++] = c;
        c = bytecode[b++];
    }
}

void slipInterpret(SlipVM* vm, const char* bytecode) {
    // TODO: Timers
    vm->config.writeFn("Starting interpreter...\n");
    vm->PC = SLIP_MEM_START;
    _slipLoadBytecode(vm, bytecode);

    int16_t opcode;

    // TODO: Use for loop
    while (vm->PC < SLIP_MEM-1) {
        opcode = (vm->memory[vm->PC] << 8) | vm->memory[vm->PC+1];
        slipOpcodeDispatch(vm, opcode);

        // TODO: Program counter will be incremented by opcodes. For now, break 
        // to avoid infinite loop.
        break;
    }
    
}

void slipOpcodeDispatch(SlipVM* vm, int16_t opcode) {

    SlipByte op = (opcode & 0xF000) >> 12;
    
    switch (op) {
        
        // 1NNN
        // Jumps to address NNN
        case 0x1:
            vm->PC = opcode & 0x0FFF;
        break;

    }

}
