#include <signal.h>
#include <stdlib.h>

#define SIGNAL_COUNT 7

static __signal_func_ptr signal_funcs[SIGNAL_COUNT];

int raise(int sig) {
    __signal_func_ptr signal_func;

    if (sig < 1 || sig > SIGNAL_COUNT) {
        return -1;
    }

    signal_func = signal_funcs[sig - 1];

    if (signal_func != SIG_IGN) {
        signal_funcs[sig - 1] = SIG_DFL;
    }

    if (signal_func == SIG_IGN || (signal_func == SIG_DFL && sig == SIGABRT)) {
        return 0;
    }

    if (signal_func == SIG_DFL) {
        exit(0);
    }

    (*signal_func)(sig);
    return 0;
}
