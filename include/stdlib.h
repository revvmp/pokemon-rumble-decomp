#ifndef MSL_STDLIB_H
#define MSL_STDLIB_H

#include <stddef.h>
#include <wchar_t.h>

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
int mbtowc(wchar_t* pwc, const char* s, size_t n);
size_t mbstowcs(wchar_t* pwcs, const char* s, size_t n);
size_t wcstombs(char* s, const wchar_t* pwcs, size_t n);
long strtol(const char* str, char** end, int base);
unsigned long strtoul(const char* str, char** end, int base);
float strtof(const char* str, char** end);
int atoi(const char* str);
int rand(void);
void srand(unsigned int seed);
void abort(void);
void exit(int status);

#ifdef __cplusplus
}
#endif

#endif
