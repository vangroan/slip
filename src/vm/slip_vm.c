
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

// Load bytecode from buffer into vm memory
void _slipLoadBytecodeIntoMemory(SlipVM* vm, SlipBytecode* bytecode) {
    
    int16_t i = SLIP_MEM_START;

    // TODO: Handle case of bytecode not fitting in memory
    for (size_t j = 0; j < bytecode->size; j++ ) {
        printf("Loading %d:0x%02x into ox%04x\n", j, bytecode->buffer[j], i);
        vm->memory[i++] = bytecode->buffer[j];
    }
}

void slipInterpretBytecode(SlipVM* vm, SlipBytecode* bytecode) {
    // TODO: Timers
    vm->config.writeFn("Starting interpreter...\n");
    _slipLoadBytecodeIntoMemory(vm, bytecode);

    vm->PC = SLIP_MEM_START;
    uint16_t opcode;

    printf("Memory [ox%04x]: ox%02x\n", SLIP_MEM_START, vm->memory[SLIP_MEM_START]);
    printf("Memory [ox%04x]: ox%02x\n", SLIP_MEM_START+1, vm->memory[SLIP_MEM_START]+1);

    // TODO: Use for loop
    while (vm->PC < SLIP_MEM-1) {
        opcode = (vm->memory[vm->PC] << 8) | vm->memory[vm->PC+1];
        slipOpcodeDispatch(vm, opcode);

        // TODO: Program counter will be incremented by opcodes. For now, break 
        // to avoid infinite loop.
        break;
    }
    
}

void slipOpcodeDispatch(SlipVM* vm, uint16_t opcode) {
    printf("Opcode: ox%04x\n");
    SlipByte op = (opcode & 0xF000) >> 12;
    
    switch (op) {
        
        // 1NNN
        // Jumps to address NNN
        case 0x1:
            vm->PC = opcode & 0x0FFF;
        break;

    }

}
