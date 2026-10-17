#ifndef BTE_BTA_DM_CO_H
#define BTE_BTA_DM_CO_H

#include <revolution/bte/data_types.h>

#include <revolution/bte/bta_sys.h>

#ifdef __cplusplus
	extern "C" {
#endif

BOOLEAN bta_dm_co_get_compress_memory(tBTA_SYS_ID id, UINT8 **memory_p,
                                      UINT32 *memory_size);

#ifdef __cplusplus
	}
#endif

#endif
