
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


void test_8XY3(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);
    
    // Set V3 to 0x0C (bx1100)
    slipOpcodeDispatch(vm, 0x630C);

    // Set V4 to 0x07 (bx0111)
    slipOpcodeDispatch(vm, 0x6407);

    // Set V3 to V3 AND V4 (0x0B / bx1011)
    slipOpcodeDispatch(vm, 0x8343);
    assertEqual(vm->V[0x3] == 0x0B, "Should set V3 to V3 XOR V4");
    assertEqual(vm->V[0x4] == 0x07, "AND should not affect right hand register");

    slipFreeVM(vm);
}


void test_8XY4(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V3 to 0xFF (bx11111111)
    slipOpcodeDispatch(vm, 0x63FF);

    // Set V4 to 0xFF (bx11111111)
    slipOpcodeDispatch(vm, 0x64FF);

    // Add V4 to V3. 0x1FE masked to 0xFE. Set carry flag to 1
    slipOpcodeDispatch(vm, 0x8344);
    assertEqual(vm->V[0x3] == 0xFE, "Should add V4 to V3");
    assertEqual(vm->V[0xF] == 0x1, "Should set carry to 1");
    assertEqual(vm->V[0x4] == 0xFF, "XOR should not affect right hand register");

    // Set V2 to 0x01
    slipOpcodeDispatch(vm, 0x6201);

    // Add V2 to V3
    slipOpcodeDispatch(vm, 0x8324);
    assertEqual(vm->V[0x3] == 0xFF, "Should add V2 to V3");
    assertEqual(vm->V[0xF] == 0x0, "Should set carry to 0");

    slipFreeVM(vm);
}


void test_8XY5(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    skip("TODO: should subtract VY from VX\n");

    slipFreeVM(vm);
}


void test_8XY6(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V3 to 0xFF
    slipOpcodeDispatch(vm, 0x63FF);

    // Shift V3 right, resulting in 0x7F
    slipOpcodeDispatch(vm, 0x8306);
    assertEqual(vm->V[0x3] == 0x7F, "Should shift V3 right");
    assertEqual(vm->V[0xF] == 0x1, "Should set flag to 1");

    slipFreeVM(vm);
}


void test_8XY7(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V3 to 0x1C
    slipOpcodeDispatch(vm, 0x631C);

    // Set V4 to 0x09
    slipOpcodeDispatch(vm, 0x6409);

    // Subtract V3 from V4. Expect V3 == 0xED
    slipOpcodeDispatch(vm, 0x8347);
    assertEqual(vm->V[0x3] == 0xED, "Should subtract V3 from V4");
    assertEqual(vm->V[0xF] == 0x1, "Should set borrow to 1");

    slipFreeVM(vm);
}


int main() {
    printf("Running tests\n");
    printf("-------------\n\n");

    SlipConfig config;
    slipInitConfig(&config);

    test_1NNN(&config);
    test_2NNN(&config);
    test_6XNN(&config);
    test_8XY0(&config);
    test_8XY1(&config);
    test_8XY2(&config);
    test_8XY3(&config);
    test_8XY4(&config);
    test_8XY5(&config);
    test_8XY6(&config);
    test_8XY7(&config);

    printf("\n");
    printf("Tests done.\n");
    return 0;
}
