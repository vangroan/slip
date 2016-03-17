
#include <stdio.h>
#include "vm/slip_vm.h"



int main(void) {

    SlipConfig config;
    slipInitConfig(&config);

    SlipVM* vm = slipNewVM(&config);

    slipInterpret(vm, "foobar");

    slipFreeVM(vm);
    return 0;
}
