#ifndef FA_PF_CACHE_H
#define FA_PF_CACHE_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PF_CACHE_PAGE_STAT_USED 0x1
#define PF_CACHE_PAGE_STAT_MODIFIED 0x2
#define PF_CACHE_PAGE_STAT_FAT 0x4

#define PF_CACHE_MODE_WRITE_THROUGH 0x1
#define PF_CACHE_MODE_SIGNATURE 0x2
#define PF_CACHE_MODE_FAT_WRITE_THROUGH 0x4

#define PF_SECTOR_INVALID 0xFFFFFFFF

typedef struct PF_CACHE_PAGE {
    u16 stat;
    u16 option;
    u8* buffer;
    u8* p_buf;
    u8* p_mod_sbuf;
    u8* p_mod_ebuf;
    u32 size;
    u32 sector;
    void* signature;
    struct PF_CACHE_PAGE* p_next;
    struct PF_CACHE_PAGE* p_prev;
} PF_CACHE_PAGE;

typedef struct PF_SECTOR_CACHE {
    u32 mode;
    u16 num_fat_pages;
    u16 num_data_pages;
    PF_CACHE_PAGE* pages;
    PF_CACHE_PAGE* p_current_fat;
    PF_CACHE_PAGE* p_current_data;
    u8 (*buffers)[512];
    u32 fat_buff_size;
    u32 data_buff_size;
    void* signature;
} PF_SECTOR_CACHE;

struct PF_VOLUME;

s32 PFCACHE_InitPageList(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* pages, u8 (*buffers)[512], u16 num,
                         u32 size, u32 is_fat);
PF_CACHE_PAGE* PFCACHE_SearchForPage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_head, u32 sector);
u32 PFCACHE_SearchForFreePage(PF_CACHE_PAGE* p_head, PF_CACHE_PAGE** pp_page);
s32 PFCACHE_FlushPageIfNeeded(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);
s32 PFCACHE_DoAllocatePage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector, PF_CACHE_PAGE** pp_page,
                           u32* p_is_hit);
s32 PFCACHE_DoReadPage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig);
s32 PFCACHE_DoReadPageAndFlushIfNeeded(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector,
                                       PF_CACHE_PAGE** pp_page, u32 set_sig);
s32 PFCACHE_DoReadNumSector(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u8* p_buf, u32 sector, u32 num_sector,
                            u32* p_num_success);
s32 PFCACHE_DoWritePage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page, u32 set_sig);
s32 PFCACHE_DoWriteSector(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector);
s32 PFCACHE_DoWriteNumSectorAndFreeIfNeeded(struct PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector,
                                            u32 num_sector, u32* p_num_success);
s32 PFCACHE_DoFlushCache(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_head);
void PFCACHE_SetCache(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_cache_page, u8 (*p_cache_buf)[512], u16 num_fat_pages,
                      u16 num_data_pages);
void PFCACHE_SetFATBufferSize(struct PF_VOLUME* p_vol, u32 size);
void PFCACHE_SetDataBufferSize(struct PF_VOLUME* p_vol, u32 size);
s32 PFCACHE_InitCaches(struct PF_VOLUME* p_vol);
s32 PFCACHE_SetWriteThrough(struct PF_VOLUME* p_vol);
s32 PFCACHE_ClearWriteThrough(struct PF_VOLUME* p_vol);
s32 PFCACHE_AllocateFATPage(struct PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit);
s32 PFCACHE_AllocateDataPage(struct PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit);
void PFCACHE_FreeFATPage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);
void PFCACHE_FreeDataPage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);
s32 PFCACHE_ReadFATPage(struct PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page);
s32 PFCACHE_ReadDataPage(struct PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig);
s32 PFCACHE_ReadDataPageAndFlushIfNeeded(struct PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig);
s32 PFCACHE_ReadDataNumSector(struct PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success);
s32 PFCACHE_WriteFATPage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page);
s32 PFCACHE_WriteDataPage(struct PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 set_sig);
s32 PFCACHE_WriteFATSector(struct PF_VOLUME* p_vol, const u8* p_buf, u32 sector);
s32 PFCACHE_WriteDataNumSectorAndFreeIfNeeded(struct PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u32 num_sector,
                                              u32* p_num_success);
PF_CACHE_PAGE* PFCACHE_SearchDataCache(struct PF_VOLUME* p_vol, u32 sector);
s32 PFCACHE_FlushFATCache(struct PF_VOLUME* p_vol);
s32 PFCACHE_FlushDataCache(struct PF_VOLUME* p_vol);
s32 PFCACHE_FlushDataCacheSpecific(struct PF_VOLUME* p_vol, void* signature);
s32 PFCACHE_FlushAllCaches(struct PF_VOLUME* p_vol);
void PFCACHE_FreeAllCaches(struct PF_VOLUME* p_vol);

#ifdef __cplusplus
}
#endif

#endif
