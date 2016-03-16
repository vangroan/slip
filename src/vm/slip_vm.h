
#ifndef SLIP_VM_H
#define SLIP_VM_H

#include <stdlib.h>
#include "slip_common.h"

// Size of the vm's memory
#define SLIP_MEM 4096

// The memory address where programs start. Historically the first 512 bytes
// were reserved for the interpreter itself, now it's used for font data.
#define SLIP_MEM_START 512


typedef struct {
    // TODO: Interpreter callbacks
} SlipConfig;


typedef struct {

    // The vm's memory space. Allocated on heap.
    SlipByte* memory;

} SlipVM;


SlipVM* slipNewVM(SlipConfig* config);

void slipFreeVM(SlipVM* vm);

void slipInterpret(SlipVM* vm, const char* bytecode);


#endif