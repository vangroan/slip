
#ifndef SLIP_VM_H
#define SLIP_VM_H

#include <stdlib.h>
#include "slip_common.h"

// Size of the vm's memory
#define SLIP_MEM 4096

// The memory address where programs start. Historically the first 512 bytes
// were reserved for the interpreter itself, now it's used for font data.
#define SLIP_MEM_START 512

// The number of data registers.
#define SLIP_REGISTERS 16

// The dimensions of the display screen.
#define SLIP_SCREEN_WIDTH 64
#define SLIP_SCREEN_HEIGHT 32

// The number of keybaord keys that can be entered.
#define SLIP_KEYS 16


typedef struct {
    // TODO: Interpreter callbacks
} SlipConfig;


typedef struct {

    // The vm's memory space. Allocated on heap.
    SlipByte* memory;

    // The data registers, named V1 to VF. The VF register is a special status
    // register used for arithmetic carries or borrows.
    SlipByte V[SLIP_REGISTERS];

    // The address register, used in several opcodes for memory operations.
    SlipByte I;

    // The delay timer. It is used for timing events in games, and counts down 
    // at 60 hertz.
    SlipByte delayTimer;

    // The sound timer. It plays a beeping sound when it reaches 0, and counts
    // down at 60 hertz.
    SlipByte soundTimer;

    // The display buffer where graphics are drawn to. Graphics are monochrome,
    // each pixel will be either 0 or 1. It is allocated on the heap to make it
    // easier to implement the SCHIP screen size in the future.
    // TODO: Consider using different typedef for boolean flags
    SlipByte* display;

    // The keyboard keys that are pressed, stored as flags. The keyboard is 
    // labeled with hex numbers from 0 to F.
    SlipByte keys[SLIP_KEYS];

} SlipVM;

void slipInitConfig(SlipConfig* config);

SlipVM* slipNewVM(SlipConfig* config);

void slipFreeVM(SlipVM* vm);

void slipInterpret(SlipVM* vm, const char* bytecode);


#endif