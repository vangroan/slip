
#ifndef SLIP_VM_H
#define SLIP_VM_H

#include <stdlib.h>
#include <stdint.h>

#include "include/slip.h"
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

// The size of the subroutine call stack
#define SLIP_STACK 16

// Helper macros to get data out of the opcode. The opcode is always 16-bits 
// (2-bytes)
#define SLIP_OP_A(opcode) ((opcode & 0xF000) >> 12)
#define SLIP_OP_B(opcode) ((opcode & 0x0F00) >> 8)
#define SLIP_OP_C(opcode) ((opcode & 0x0F00) >> 8)
#define SLIP_OP_D(opcode) (opcode & 0x000F)

#define SLIP_OP_CD(opcode) (opcode & 0x00FF)
#define SLIP_OP_BCD(opcode) (opcode & 0x0FFF)

// The function type for the input callback. Called before the interpreter loop
// reads an opcode. It is used for setting the key press flags.
typedef void (*SlipInputFn)(SlipVM* vm);


// The function type for the display output callback. Called after the display
// buffer has been updated.
typedef void (*SlipDisplayFn)(SlipVM* vm);


// The interpreter will use the write function callback for textual output. If
// no function is provided, output is discarded.
typedef void (*SlipWriteFn)(const char* message);


struct SlipConfig {
    
    SlipInputFn inputFn;

    SlipDisplayFn displayFn;

    SlipWriteFn writeFn;

    // TODO: realloc callback

};


struct SlipVM {

    // The vm's memory space. Allocated on heap.
    SlipByte* memory;

    // The data registers, named V1 to VF. The VF register is a special status
    // register used for arithmetic carries or borrows.
    SlipByte V[SLIP_REGISTERS];

    // The address register, used in several opcodes for memory operations. It
    // is 16-bits wide
    int16_t I;

    // The stack pointer.
    SlipByte SP;

    // The subroutine call stack. Whenever a subroutine is called, the current
    // address is stored on the stack, and the stack pointer is incremented.
    // Returning from a subrouting pops the address off the stack and positions
    // the program counter to the address.
    int16_t stack[SLIP_STACK];

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

    // The program counter.
    int16_t PC;

    SlipConfig config;

};

void slipInitConfig(SlipConfig* config);

SlipVM* slipNewVM(SlipConfig* config);

void slipFreeVM(SlipVM* vm);

void slipInterpretBytecode(SlipVM* vm, SlipBytecode* bytecode);

void slipOpcodeDispatch(SlipVM* vm, uint16_t opcode);


#endif
