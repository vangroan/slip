
#include <stdio.h>
#include "vm/slip_vm.h"


void writeFn(const char* message) {
    printf(message);
}


int main(void) {

    SlipConfig config;
    slipInitConfig(&config);
    config.writeFn = writeFn;

    // TODO: Load bytecode files
    const char bytecode[3] = {0x1F, 0xFF, '\0'};

    SlipVM* vm = slipNewVM(&config);
    slipInterpret(vm, bytecode);

    slipFreeVM(vm);
    return 0;
}
