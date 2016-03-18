
#ifndef SLIP
#define SLIP

typedef struct SlipBytecode {

    unsigned char* buffer;

    int16_t size;

} SlipBytecode;

typedef struct SlipConfig SlipConfig;

typedef struct SlipVM SlipVM;

#endif