#ifndef _STDDEF_H
#define _STDDEF_H

typedef unsigned long size_t;
typedef long ptrdiff_t;

#ifndef __cplusplus
typedef unsigned short wchar_t;
#endif

#define offsetof(type, member) ((size_t) & (((type*)0)->member))

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

#endif
