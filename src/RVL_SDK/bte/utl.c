#include <revolution/bte/gki.h>
#include <revolution/bte/utl.h>

void utl_freebuf(void** p) {
    if (*p != NULL) {
        GKI_freebuf(*p);
        *p = NULL;
    }
}
