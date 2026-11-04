#include <MSL_C/math.h>

int __float_nan[1] = {0x7FFFFFFF};
int __float_huge[1] = {0x7F800000};
__declspec(align(8)) int __double_huge[2] = {0x7FF00000, 0};
