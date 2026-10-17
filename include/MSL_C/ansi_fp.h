#ifndef MSL_C_ANSI_FP_H
#define MSL_C_ANSI_FP_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SIGDIGLEN 36

typedef struct decimal {
    char sign;
    char unused;
    s16 exp;
    struct {
        u8 length;
        u8 text[SIGDIGLEN];
        u8 unused;
    } sig;
} decimal;

typedef struct decform {
    char style;
    char unused;
    short digits;
} decform;

void __ull2dec(decimal* result, u64 value);
void __timesdec(decimal* result, const decimal* x, const decimal* y);
void __str2dec(decimal* result, const char* text, short exp);
void __two_exp(decimal* result, long exp);
BOOL __equals_dec(const decimal* x, const decimal* y);
BOOL __less_dec(const decimal* x, const decimal* y);
void __minus_dec(decimal* result, const decimal* x, const decimal* y);
void __num2dec_internal(decimal* result, double value);
void __num2dec(const decform* form, double value, decimal* result);
double __dec2num(const decimal* value);

#ifdef __cplusplus
}
#endif

#endif
