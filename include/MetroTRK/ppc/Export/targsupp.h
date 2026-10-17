#ifndef PPC_EXPORT_TARGSUPP_H
#define PPC_EXPORT_TARGSUPP_H

#include <MetroTRK/trk.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

u32 TRKAccessFile(MessageCommandID command, u32 handle, size_t* count, u8* buffer);

#ifdef __cplusplus
}
#endif

#endif
