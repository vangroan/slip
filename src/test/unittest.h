
#ifndef SLIP_TEST_UTIL_H
#define SLIP_TEST_UTIL_H

#include <stdbool.h>


void assertEqual(bool condition, const char* message) {
    if (!condition) {
        printf("[FAIL] %s\n", message);
    } else {
        printf("[SUCCESS] %s\n", message);
    }
}


void skip(const char* message) {
    printf("[SKIP] %s\n", message);
}


#endif
