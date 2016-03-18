
#include "util/slip_io.h"

void slipLoadBytecode(SlipBytecode* bytecode, const char* filename) {

    bytecode->buffer = NULL;
    bytecode->size = 0;

    FILE* fp = fopen(filename, "r");

    if (fp != NULL) {

        // Go to end of file
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp);
            if (bufsize == -1) {
                // TODO: Handle errors
            }

            bytecode->buffer = (unsigned char*)malloc(bufsize * sizeof(char));

        // Back to start of file
        if (fseek(fp, 0L, SEEK_SET) != 0) { 
            // TODO: Handle error 
        }

        size_t newsize = fread(bytecode->buffer, sizeof(char), bufsize, fp);

        if (newsize == 0) {
            // TODO: Handle Error
        } else {
            bytecode->size = newsize;
        }

        } else {
            // TODO: Handle errors
        }
    } else {
        // TODO: Handle errors
    }

    fclose(fp);
}

void slipFreeBytecode(SlipBytecode* bytecode) {
    free(bytecode->buffer);
    bytecode->size = 0;
}
