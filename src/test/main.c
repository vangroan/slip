
#include <stdio.h>


#include "unittest.h"
#include "vm/slip_vm.h"


void test_1NNN(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Jump to 0x0500
    slipOpcodeDispatch(vm, 0x1500);
    
    assertEqual(vm->PC == 0x0500, "Should jump to address");

    slipFreeVM(vm);
}


void test_2NNN(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Jump to 0x0500
    slipOpcodeDispatch(vm, 0x1500);

    // Call subroutine at 0x0502
    slipOpcodeDispatch(vm, 0x2502);
    assertEqual(vm->PC == 0x0502, "Call subroutine should set program counter");
    assertEqual(vm->stack[0] == 0x0500, "Call subroutine should store previous address on stack");
    assertEqual(vm->SP == 1, "Call subroutine should increase stack pointer");

    slipFreeVM(vm);
}


void test_6XNN(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V2 to 0x45
    slipOpcodeDispatch(vm, 0x6245);
    assertEqual(vm->V[0x2] == 0x45, "Should set register V2 to 0x45");

    slipFreeVM(vm);
}


void test_8XY0(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V4 to 0x23
    slipOpcodeDispatch(vm, 0x6423);

    // Set V3 to V4
    slipOpcodeDispatch(vm, 0x8340);
    assertEqual(vm->V[0x3] == vm->V[0x4], "Should set register V3 to V4");

    slipFreeVM(vm);
}

void test_8XY1(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V3 to 0x0C (bx1100)
    slipOpcodeDispatch(vm, 0x630C);

    // Set V4 to 0x0A (bx1010)
    slipOpcodeDispatch(vm, 0x640A);

    // Set V3 to V3 OR V4 (0x0E / bx1110)
    slipOpcodeDispatch(vm, 0x8341);
    assertEqual(vm->V[0x3] == 0x0E, "Should set V3 to V3 OR V4");
    assertEqual(vm->V[0x4] == 0x0A, "OR should not affect right hand register");

    slipFreeVM(vm);
}


void test_8XY2(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V3 to 0x0C (bx1100)
    slipOpcodeDispatch(vm, 0x630C);

    // Set V4 to 0x07 (bx0111)
    slipOpcodeDispatch(vm, 0x6407);

    // Set V3 to V3 OR V4 (0x04 / bx0100)
    slipOpcodeDispatch(vm, 0x8342);
    assertEqual(vm->V[0x3] == 0x04, "Should set V3 to V3 AND V4");
    assertEqual(vm->V[0x4] == 0x07, "AND should not affect right hand register");

    slipFreeVM(vm);
}


int main() {
    printf("Running tests\n");

    SlipConfig config;
    slipInitConfig(&config);

    test_1NNN(&config);
    test_2NNN(&config);
    test_6XNN(&config);
    test_8XY0(&config);
    test_8XY1(&config);
    test_8XY2(&config);

    printf("Tests done\n");
    return 0;
}