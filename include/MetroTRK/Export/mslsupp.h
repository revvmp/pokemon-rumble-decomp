#ifndef METROTRK_EXPORT_MSLSUPP_H
#define METROTRK_EXPORT_MSLSUPP_H

#include <MetroTRK/trk.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

DSIOResult __read_console(u32 handle, u8* buffer, size_t* count, void* refCon);
DSIOResult __TRK_write_console(u32 handle, u8* buffer, size_t* count, void* refCon);

#ifdef __cplusplus
}
#endif

#endif
