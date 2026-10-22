#ifndef MSL_SIGNAL_H
#define MSL_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define SIGABRT 1

typedef void (*__signal_func_ptr)(int);

#define SIG_DFL ((__signal_func_ptr)0)
#define SIG_IGN ((__signal_func_ptr)1)

int raise(int sig);

#ifdef __cplusplus
}
#endif

#endif
