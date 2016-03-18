
#include <stdio.h>
#include "vm/slip_vm.h"
#include "util/slip_io.h"


void writeFn(const char* message) {
    printf(message);
}


int main(void) {

    SlipConfig config;
    slipInitConfig(&config);
    config.writeFn = writeFn;

    SlipBytecode bytecode;
    slipLoadBytecode(&bytecode, "sample/hello_world.ch8");

    SlipVM* vm = slipNewVM(&config);
    slipInterpretBytecode(vm, &bytecode);

    slipFreeVM(vm);
    slipFreeBytecode(&bytecode);
    return 0;
}
