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

void* malloc(size_t size);
void free(void* ptr);
div_t div(int numerator, int denominator);
void abort(void);

#ifdef __cplusplus
}
#endif

#endif
