#ifndef PPC_GENERIC_TARGIMPL_H
#define PPC_GENERIC_TARGIMPL_H

#include <MetroTRK/Portable/nubevent.h>

#ifdef __cplusplus
extern "C" {
#endif

DSError TRKTargetInterrupt(TRKEvent* event);
DSError TRKTargetSupportRequest(void);
BOOL TRKTargetStopped(void);

#ifdef __cplusplus
}
#endif

#endif
