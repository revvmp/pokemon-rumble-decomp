#include <revolution/fa/pf_sector.h>
#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_clib.h>

s32 PFSEC_ReadFAT(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u16 offset, u16 size) {
    s32 err;
    PF_CACHE_PAGE* p_page;
    if ((p_vol->flags & 1) == 0 || !PFDRV_IsInserted(p_vol)) {
        return 9;
    }
    if (sector >= p_vol->bpb.total_sectors || offset >= p_vol->bpb.bytes_per_sector ||
        offset + size > p_vol->bpb.bytes_per_sector) {
        return 10;
    }
    err = PFCACHE_ReadFATPage(p_vol, sector, &p_page);
    if (err) {
        return err;
    }
    pf_memcpy(p_buf, &p_page->p_buf[offset], size);
    return 0;
}

s32 PFSEC_ReadData(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 offset, u32 size, u32* p_success_size, u32 set_sig) {
    s32 err;
    u32 cached_sectors;
    u32 num_sector;
    u32 adjust_sector;
    PF_CACHE_PAGE* p_page;
    u32 num_success;
    u32 rest_sector;
    *p_success_size = 0;
    if ((p_vol->flags & 1) == 0 || !PFDRV_IsInserted(p_vol)) {
        return 9;
    }
    if (sector >= p_vol->bpb.total_sectors || offset >= p_vol->bpb.bytes_per_sector ||
        offset + size > p_vol->bpb.bytes_per_sector) {
        if (offset) {
            return 10;
        }
    }
    if (offset || size < p_vol->bpb.bytes_per_sector) {
        err = PFCACHE_ReadDataPage(p_vol, sector, &p_page, set_sig);
        if (err) {
            return err;
        }
        pf_memcpy(p_buf, &p_page->p_buf[offset], size);
        *p_success_size = size;
    } else if (!offset && (size & (p_vol->bpb.bytes_per_sector - 1)) == 0) {
        err = PFCACHE_ReadDataNumSector(p_vol, p_buf, sector, size >> p_vol->bpb.log2_bytes_per_sector, &num_success);
        if (err) {
            return err;
        }
        *p_success_size += num_success << p_vol->bpb.log2_bytes_per_sector;
        if (num_success != size >> p_vol->bpb.log2_bytes_per_sector) {
            return 17;
        }
    } else if (!offset) {
        p_page = PFCACHE_SearchDataCache(p_vol, sector);
        if (p_page) {
            cached_sectors = p_page->sector + p_page->size - sector;
            if (cached_sectors <= size >> p_vol->bpb.log2_bytes_per_sector) {
                pf_memcpy(p_buf, p_page->p_buf, cached_sectors << p_vol->bpb.log2_bytes_per_sector);
                *p_success_size += cached_sectors << p_vol->bpb.log2_bytes_per_sector;
                size -= cached_sectors << p_vol->bpb.log2_bytes_per_sector;
                sector += cached_sectors;
            } else if (cached_sectors > size >> p_vol->bpb.log2_bytes_per_sector) {
                pf_memcpy(p_buf, p_page->p_buf, size);
                *p_success_size += size;
                size = 0;
            }
        }
        if (size) {
            num_sector = size >> p_vol->bpb.log2_bytes_per_sector;
            rest_sector = (sector + num_sector) % p_vol->cache.data_buff_size;
            if (num_sector > rest_sector) {
                adjust_sector = num_sector - rest_sector;
                err = PFCACHE_ReadDataNumSector(p_vol, &p_buf[*p_success_size], sector, num_sector - rest_sector, &num_success);
                if (err) {
                    return err;
                }
                *p_success_size += num_success << p_vol->bpb.log2_bytes_per_sector;
                if (num_success != adjust_sector) {
                    return 17;
                }
                sector += adjust_sector;
                num_sector -= adjust_sector;
            }
            if (num_sector) {
                err = PFCACHE_ReadDataPage(p_vol, sector, &p_page, set_sig);
                if (err) {
                    return err;
                }
                pf_memcpy(&p_buf[*p_success_size], p_page->p_buf, num_sector << p_vol->bpb.log2_bytes_per_sector);
                *p_success_size += num_sector << p_vol->bpb.log2_bytes_per_sector;
            }
        }
    } else {
        return 10;
    }
    return 0;
}

s32 PFSEC_WriteFAT(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u16 offset, u16 size) {
    s32 err;
    PF_CACHE_PAGE* p_page;
    if ((p_vol->flags & 1) == 0 || !PFDRV_IsInserted(p_vol) || (p_vol->flags & 2) != 0) {
        return 9;
    }
    if (sector >= p_vol->bpb.total_sectors || offset >= p_vol->bpb.bytes_per_sector ||
        offset + size > p_vol->bpb.bytes_per_sector) {
        return 10;
    }
    err = PFCACHE_ReadFATPage(p_vol, sector, &p_page);
    if (err) {
        return err;
    }
    pf_memcpy(&p_page->p_buf[offset], p_buf, size);
    err = PFCACHE_WriteFATPage(p_vol, p_page);
    if (err) {
        return err;
    }
    return 0;
}

s32 PFSEC_WriteData(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u32 offset, u32 size, u32* p_success_size, u32 set_sig) {
    s32 err;
    u32 num_sector;
    u32 adjust_sector;
    PF_CACHE_PAGE* p_page;
    u32 num_success;
    u32 rest_sector;
    *p_success_size = 0;
    if ((p_vol->flags & 1) == 0 || !PFDRV_IsInserted(p_vol) || (p_vol->flags & 2) != 0) {
        return 9;
    }
    if (sector >= p_vol->bpb.total_sectors || offset >= p_vol->bpb.bytes_per_sector ||
        offset + size > p_vol->bpb.bytes_per_sector) {
        if (offset) {
            return 10;
        }
    }
    if (offset || size < p_vol->bpb.bytes_per_sector) {
        err = PFCACHE_ReadDataPageAndFlushIfNeeded(p_vol, sector, &p_page, set_sig);
        if (err) {
            return err;
        }
        pf_memcpy(&p_page->p_buf[offset], p_buf, size);
        err = PFCACHE_WriteDataPage(p_vol, p_page, set_sig);
        if (err) {
            return err;
        }
        *p_success_size = size;
    } else if (!offset && (size & (p_vol->bpb.bytes_per_sector - 1)) == 0) {
        err = PFCACHE_WriteDataNumSectorAndFreeIfNeeded(p_vol, p_buf, sector, size >> p_vol->bpb.log2_bytes_per_sector, &num_success);
        if (err) {
            return err;
        }
        *p_success_size = num_success << p_vol->bpb.log2_bytes_per_sector;
        if (num_success != size >> p_vol->bpb.log2_bytes_per_sector) {
            return 17;
        }
    } else if (!offset) {
        num_sector = size >> p_vol->bpb.log2_bytes_per_sector;
        rest_sector = (sector + num_sector) % p_vol->cache.data_buff_size;
        if (num_sector > rest_sector) {
            adjust_sector = num_sector - rest_sector;
            err = PFCACHE_WriteDataNumSectorAndFreeIfNeeded(p_vol, p_buf, sector, num_sector - rest_sector, &num_success);
            if (err) {
                return err;
            }
            *p_success_size = num_success << p_vol->bpb.log2_bytes_per_sector;
            if (num_success != adjust_sector) {
                return 17;
            }
            sector += adjust_sector;
            num_sector -= adjust_sector;
        }
        if (num_sector) {
            err = PFCACHE_ReadDataPageAndFlushIfNeeded(p_vol, sector, &p_page, set_sig);
            if (err) {
                return err;
            }
            pf_memcpy(p_page->p_buf, &p_buf[*p_success_size], num_sector << p_vol->bpb.log2_bytes_per_sector);
            err = PFCACHE_WriteDataPage(p_vol, p_page, set_sig);
            if (err) {
                return err;
            }
            *p_success_size += num_sector << p_vol->bpb.log2_bytes_per_sector;
        }
    } else {
        return 10;
    }
    return 0;
}
