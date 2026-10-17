#ifndef DATA_TYPES_H
#define DATA_TYPES_H

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;
typedef signed long INT32;
typedef signed char INT8;
typedef signed short INT16;
typedef unsigned char BOOLEAN;

typedef unsigned char UBYTE;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define BCM_STRNCPY_S(dst, dst_size, src, count) strncpy(dst, src, count)

#ifdef __cplusplus
}
#endif

#endif
