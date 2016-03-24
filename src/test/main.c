
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


int main() {
    printf("Running tests\n");

    SlipConfig config;
    slipInitConfig(&config);

    test_1NNN(&config);
    test_2NNN(&config);

    printf("Tests done\n");
    return 0;
}