
#ifndef SLIP_IO_H
#define SLIP_IO_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "include/slip.h"

// Load contents of a ch8 file into the buffer. If a bytecode struct is provided
// that already has data loaded into it, the pointer to the data will be lost 
// and cause a memory leak.
void slipLoadBytecode(SlipBytecode* bytecode, const char* filename);

void slipFreeBytecode(SlipBytecode* bytecode);

#endif