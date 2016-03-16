
#include "slip_vm.h"

SlipVM* slipNewVM(SlipConfig* config) {
    SlipVM* vm = (SlipVM*)malloc(sizeof(SlipVM));

    vm->memory = (SlipByte*)malloc(SLIP_MEM * sizeof(SlipByte));

    return vm;
}

void slipFreeVM(SlipVM* vm) {

    free(vm->memory);
    free(vm);

}

void slipInterpret(SlipVM* vm, const char* bytecode) {
    // TODO
}
