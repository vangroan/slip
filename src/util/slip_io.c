
#include "util/slip_io.h"

void slipLoadBytecode(SlipBytecode* bytecode, const char* filename) {

    bytecode->buffer = NULL;
    bytecode->size = 0;

    FILE* fp = fopen(filename, "r");

    if (fp != NULL) {

        // Go to end of file
        if (fseek(fp, 0L, SEEK_END) == 0) {
            long bufsize = ftell(fp);
            slipAssert(bufsize != -1, "Failed to get size of file.");

            bytecode->buffer = (unsigned char*)malloc(bufsize * sizeof(char));

            // Back to start of file
            slipAssert(fseek(fp, 0L, SEEK_SET) == 0, "Failed to seek start of file.");

            size_t newsize = fread(bytecode->buffer, sizeof(char), bufsize, fp);

            slipAssert(newsize != 0, "failed to read file.");
            bytecode->size = newsize;

        } else {
            slipAbort("Failed to get size of file.");
        }
    } else {
        slipAbort("Failed to open file.");
    }

    fclose(fp);
}

void slipFreeBytecode(SlipBytecode* bytecode) {
    free(bytecode->buffer);
    bytecode->size = 0;
}

void slipAssert(bool condition, const char* message) {
    if (!condition) {
        fprintf(stderr, message);
        exit(1);
    }
}

void slipAbort(const char* message) {
    fprintf(stderr, message);
    exit(1);
}
