#ifndef MSL_STDLIB_H
#define MSL_STDLIB_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct div_t {
    int quot;
    int rem;
} div_t;

void* malloc(size_t size) __attribute__((nothrow));
void free(void* ptr) __attribute__((nothrow));
div_t div(int numerator, int denominator);
unsigned long strtoul(const char* str, char** end, int base);
void abort(void);
void exit(int status);

#ifdef __cplusplus
}
#endif

#endif
