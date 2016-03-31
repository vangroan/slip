
#ifndef SLIP
#define SLIP

typedef struct SlipBytecode {

    unsigned char* buffer;

    uint16_t size;

} SlipBytecode;

typedef struct SlipConfig SlipConfig;

typedef struct SlipVM SlipVM;

#endif