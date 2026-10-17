#ifndef METROTRK_PORTABLE_MSG_H
#define METROTRK_PORTABLE_MSG_H

#include <MetroTRK/trk.h>

#ifdef __cplusplus
extern "C" {
#endif

DSError TRKMessageSend(TRKBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
