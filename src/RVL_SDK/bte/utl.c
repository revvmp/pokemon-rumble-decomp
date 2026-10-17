#include <RVL_SDK/bte/gki.h>
#include <RVL_SDK/bte/utl.h>

void utl_freebuf(void** p) {
    if (*p != NULL) {
        GKI_freebuf(*p);
        *p = NULL;
    }
}
