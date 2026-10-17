#ifndef RVL_SDK_BTE_DATA_TYPES_H
#define RVL_SDK_BTE_DATA_TYPES_H

#include <string.h>

#define BCM_STRNCPY_S(dst, dst_size, src, count) strncpy(dst, src, count)

typedef signed char INT8;
typedef signed short INT16;
typedef signed long INT32;

typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned long UINT32;

typedef unsigned char BOOLEAN;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#endif
