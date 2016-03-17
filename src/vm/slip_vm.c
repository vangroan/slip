
#include "slip_vm.h"

void slipInitConfig(SlipConfig* config) {
    // TODO: Config defaults
}

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
