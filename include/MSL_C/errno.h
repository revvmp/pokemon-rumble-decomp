#ifndef MSL_C_ERRNO_H
#define MSL_C_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

#define EDOM 33
#define ERANGE 34
#define ESPIPE 40

extern int errno;

#ifdef __cplusplus
}
#endif

#endif
