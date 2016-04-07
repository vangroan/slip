
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


void test_8XYE(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V3 to 0xF0
    slipOpcodeDispatch(vm, 0x63F0);

    // Shift V3 left, set VF to 1. Expect 0xE0
    slipOpcodeDispatch(vm, 0x830E);
    assertEqual(vm->V[0x3] == 0xE0, "Should shift V3 left by 1");
    assertEqual(vm->V[0xF] == 0x1, "Should set flag to 1");

    slipFreeVM(vm);
}


void test_9XY0(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V1 to 0x28
    slipOpcodeDispatch(vm, 0x6128);

    // Set V2 to 0x82
    slipOpcodeDispatch(vm, 0x6282);

    // Skip next instruction
    slipOpcodeDispatch(vm, 0x9120);
    assertEqual(vm->PC == 0x8, "Should skip instruction if V1 and V2 are unequal");

    slipFreeVM(vm);
}


void test_ANNN(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    slipOpcodeDispatch(vm, 0xA208);
    assertEqual(vm->I == 0x208, "Should set register I to 0x208");

    slipFreeVM(vm);
}


void test_BNNN(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V0 to 0x08
    slipOpcodeDispatch(vm, 0x6008);

    // Set I to 0x500 + 0x08
    slipOpcodeDispatch(vm, 0xB500);
    assertEqual(vm->I == 0x508, "Should set register I to 0x500 + 0x08");

    slipFreeVM(vm);
}


void test_EX9E(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);



    slipFreeVM(vm);
}


void test_FX55(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Set V0 to 0x1
    slipOpcodeDispatch(vm, 0x6001);

    // Set V1 to 0x2
    slipOpcodeDispatch(vm, 0x6102);

    // Set V2 to 0x3
    slipOpcodeDispatch(vm, 0x6203);

    // Set V3 to 0x4
    slipOpcodeDispatch(vm, 0x6304);

    // Set I to 0x20A
    slipOpcodeDispatch(vm, 0xA20A);

    // Store V0-V2 in 0x20A-0x20C
    slipOpcodeDispatch(vm, 0xF255);
    assertEqual(slipGetMem(vm, 0x20A) == 0x1, "Should set 0x20A to 0x1");
    assertEqual(slipGetMem(vm, 0x20B) == 0x2, "Should set 0x20B to 0x2");
    assertEqual(slipGetMem(vm, 0x20C) == 0x3, "Should set 0x20C to 0x3");
    assertEqual(slipGetMem(vm, 0x20D) == 0x0, "Should not set 0x20D");

    slipFreeVM(vm);
}


void test_FX65(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    slipSetMem(vm, 0x20A, 0x1);
    slipSetMem(vm, 0x20B, 0x2);
    slipSetMem(vm, 0x20C, 0x3);
    slipSetMem(vm, 0x20D, 0x4);
    slipOpcodeDispatch(vm, 0xA20A);

    slipOpcodeDispatch(vm, 0xF265);
    assertEqual(vm->V[0x0] == 0x1, "Should set V0 to 0x1");
    assertEqual(vm->V[0x1] == 0x2, "Should set V1 to 0x2");
    assertEqual(vm->V[0x2] == 0x3, "Should set V2 to 0x3");
    assertEqual(vm->V[0x3] == 0, "Should not set V3");

    slipFreeVM(vm);
}


void test_keys(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    // Press key A and B down
    slipKeyDown(vm, 0xA);
    slipKeyDown(vm, 0xB);
    assertEqual(slipIsKeyDown(vm, 0xA), "Should press key A");
    assertEqual(!slipIsKeyDown(vm, 0xC), "Should not press other keys");

    // Release key A but not key B
    slipKeyUp(vm, 0xA);
    assertEqual(!slipIsKeyDown(vm, 0xA), "Should release key A");
    assertEqual(slipIsKeyDown(vm, 0xB), "Should not release key B");

    slipFreeVM(vm);
}


void test_display(SlipConfig* config) {
    SlipVM* vm = slipNewVM(config);

    printf("Draw test\n");

    // Store sprite at 0x200
    //     0 1 2 3 4 5 6 7
    // 0 | 0 0 0 1 1 0 0 0 | 0x18
    // 1 | 0 0 1 0 0 1 0 0 | 0x24
    // 2 | 0 1 0 0 0 0 1 0 | 0x42
    // 3 | 1 0 0 1 1 0 0 1 | 0x99
    // 4 | 1 0 0 1 1 0 0 1 | 0x99
    // 5 | 0 1 0 0 0 0 1 0 | 0x42
    // 6 | 0 0 1 0 0 1 0 0 | 0x24
    // 7 | 0 0 0 1 1 0 0 0 | 0x18

    slipOpcodeDispatch(vm, 0x6018);
    slipOpcodeDispatch(vm, 0x6124);
    slipOpcodeDispatch(vm, 0x6242);
    slipOpcodeDispatch(vm, 0x6399);
    slipOpcodeDispatch(vm, 0x6499);
    slipOpcodeDispatch(vm, 0x6542);
    slipOpcodeDispatch(vm, 0x6624);
    slipOpcodeDispatch(vm, 0x6718);

    slipOpcodeDispatch(vm, 0xA200);
    slipOpcodeDispatch(vm, 0xF755);

    slipDumpMemoryRange(vm, 0x200, 0x208);

    // Set V1 to 28 for X coordinate
    slipOpcodeDispatch(vm, 0x611C);

    // Set V2 to 12 for Y coordinate
    slipOpcodeDispatch(vm, 0x620C);

    // Draw sprite to (28, 12)
    slipOpcodeDispatch(vm, 0xD128);

    slipDumpDisplay(vm);

    slipFreeVM(vm);
}


int main() {
    printf("Running tests\n");
    printf("-------------\n\n");

    seedRandom();

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
    test_8XYE(&config);
    test_9XY0(&config);
    test_ANNN(&config);
    test_BNNN(&config);
    test_EX9E(&config);
    test_FX55(&config);
    test_FX65(&config);

    test_keys(&config);

    test_display(&config);

    printf("\n");
    printf("Tests done.\n");
    return 0;
}
