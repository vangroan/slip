
#include <stdio.h>
#include "vm/slip_vm.h"


void writeFn(const char* message) {
    printf(message);
}


int main(void) {

    SlipConfig config;
    slipInitConfig(&config);
    config.writeFn = writeFn;

    SlipVM* vm = slipNewVM(&config);
    slipInterpret(vm, "foobar");

    slipFreeVM(vm);
    return 0;
}
