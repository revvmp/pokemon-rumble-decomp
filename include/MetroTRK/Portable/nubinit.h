#ifndef METROTRK_PORTABLE_NUBINIT_H
#define METROTRK_PORTABLE_NUBINIT_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

DSError TRKInitializeNub(void);
DSError TRKTerminateNub(void);
void TRKNubWelcome(void);

#ifdef __cplusplus
}
#endif

#endif
