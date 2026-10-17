#ifndef MSL_SIGNAL_H
#define MSL_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define SIGABRT 1

int raise(int sig);

#ifdef __cplusplus
}
#endif

#endif
