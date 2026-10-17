#ifndef METROTRK_PORTABLE_MSGHNDLR_H
#define METROTRK_PORTABLE_MSGHNDLR_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

DSError TRKDoConnect(TRKBuffer* buffer);
DSError TRKDoDisconnect(TRKBuffer* buffer);
DSError TRKDoReset(TRKBuffer* buffer);
DSError TRKDoOverride(TRKBuffer* buffer);
DSError TRKDoReadMemory(TRKBuffer* buffer);
DSError TRKDoWriteMemory(TRKBuffer* buffer);
DSError TRKDoReadRegisters(TRKBuffer* buffer);
DSError TRKDoWriteRegisters(TRKBuffer* buffer);
DSError TRKDoContinue(TRKBuffer* buffer);
DSError TRKDoStep(TRKBuffer* buffer);
DSError TRKDoStop(TRKBuffer* buffer);
DSError TRKDoSetOption(TRKBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
