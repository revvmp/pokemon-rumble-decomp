#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_volume.h>

s32 PFCACHE_InitPageList(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* pages, u8 (*buffers)[512], u16 num,
                         u32 size, u32 is_fat)
{
    u32 i;
    u32 bps_per_buf;

    *pp_head = pages;
    bps_per_buf = (p_vol->bpb.bytes_per_sector >> 9) * size;
    if (num / bps_per_buf == 0) {
        return 30;
    }

    if (num / bps_per_buf == 1) {
        pages->p_prev = pages;
        pages->p_next = pages;
        pages->buffer = *buffers;
        pages->p_buf = *buffers;
        pages->p_mod_sbuf = NULL;
        pages->p_mod_ebuf = NULL;
        pages->size = size;
        pages->sector = PF_SECTOR_INVALID;
        pages->option = 0;
        pages->signature = NULL;
        pages->stat &= ~(PF_CACHE_PAGE_STAT_USED | PF_CACHE_PAGE_STAT_MODIFIED);
        if (is_fat) {
            pages->stat |= PF_CACHE_PAGE_STAT_FAT;
        } else {
            pages->stat &= ~PF_CACHE_PAGE_STAT_FAT;
        }
        return 0;
    }

    pages->p_prev = &pages[num - num % bps_per_buf - bps_per_buf];
    pages->p_next = &pages[bps_per_buf];
    pages->buffer = *buffers;
    pages->p_buf = *buffers;
    pages->p_mod_sbuf = NULL;
    pages->p_mod_ebuf = NULL;
    pages->size = size;
    pages->sector = PF_SECTOR_INVALID;
    pages->option = 0;
    pages->signature = NULL;
    pages->stat &= ~(PF_CACHE_PAGE_STAT_USED | PF_CACHE_PAGE_STAT_MODIFIED);
    if (is_fat) {
        pages->stat |= PF_CACHE_PAGE_STAT_FAT;
    } else {
        pages->stat &= ~PF_CACHE_PAGE_STAT_FAT;
    }

    for (i = bps_per_buf; i < num - num % bps_per_buf - bps_per_buf; i += bps_per_buf) {
        pages[i].p_prev = &pages[i - bps_per_buf];
        pages[i].p_next = &pages[i + bps_per_buf];
        pages[i].buffer = buffers[i];
        pages[i].p_buf = buffers[i];
        pages[i].p_mod_sbuf = NULL;
        pages[i].p_mod_ebuf = NULL;
        pages[i].size = size;
        pages[i].sector = PF_SECTOR_INVALID;
        pages[i].option = 0;
        pages[i].signature = NULL;
        pages[i].stat &= ~(PF_CACHE_PAGE_STAT_USED | PF_CACHE_PAGE_STAT_MODIFIED);
        if (is_fat) {
            pages[i].stat |= PF_CACHE_PAGE_STAT_FAT;
        } else {
            pages[i].stat &= ~PF_CACHE_PAGE_STAT_FAT;
        }
    }

    pages[num - num % bps_per_buf - bps_per_buf].p_prev = &pages[num - num % bps_per_buf - bps_per_buf - bps_per_buf];
    pages[num - num % bps_per_buf - bps_per_buf].p_next = pages;
    pages[num - num % bps_per_buf - bps_per_buf].buffer = buffers[num - num % bps_per_buf - bps_per_buf];
    pages[num - num % bps_per_buf - bps_per_buf].p_buf = buffers[num - num % bps_per_buf - bps_per_buf];
    pages[num - num % bps_per_buf - bps_per_buf].p_mod_sbuf = NULL;
    pages[num - num % bps_per_buf - bps_per_buf].p_mod_ebuf = NULL;
    pages[num - num % bps_per_buf - bps_per_buf].size = size;
    pages[num - num % bps_per_buf - bps_per_buf].sector = PF_SECTOR_INVALID;
    pages[num - num % bps_per_buf - bps_per_buf].option = 0;
    pages[num - num % bps_per_buf - bps_per_buf].signature = NULL;
    pages[num - num % bps_per_buf - bps_per_buf].stat &= ~(PF_CACHE_PAGE_STAT_USED | PF_CACHE_PAGE_STAT_MODIFIED);
    if (is_fat) {
        pages[num - num % bps_per_buf - bps_per_buf].stat |= PF_CACHE_PAGE_STAT_FAT;
    } else {
        pages[num - num % bps_per_buf - bps_per_buf].stat &= ~PF_CACHE_PAGE_STAT_FAT;
    }
    return 0;
}

static inline void PFCACHE_ClearPageModified(PF_CACHE_PAGE* p_page)
{
    p_page->p_mod_sbuf = NULL;
    p_page->p_mod_ebuf = NULL;
    p_page->stat &= ~PF_CACHE_PAGE_STAT_MODIFIED;
}

PF_CACHE_PAGE* PFCACHE_SearchForPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_head, u32 sector)
{
    PF_CACHE_PAGE* p_page;

    if (sector == PF_SECTOR_INVALID) {
        return NULL;
    }

    if (!(p_head->stat & PF_CACHE_PAGE_STAT_USED)) {
        return NULL;
    }
    if (p_head->sector <= sector && p_head->sector + p_head->size - 1 >= sector) {
        p_head->p_buf = &p_head->buffer[(sector - p_head->sector) << p_vol->bpb.log2_bytes_per_sector];
        return p_head;
    }

    for (p_page = p_head->p_next; p_page != p_head; p_page = p_page->p_next) {
        if (!(p_page->stat & PF_CACHE_PAGE_STAT_USED)) {
            return NULL;
        }
        if (p_page->sector <= sector && p_page->sector + p_page->size - 1 >= sector) {
            p_page->p_buf = &p_page->buffer[(sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector];
            return p_page;
        }
    }
    return NULL;
}

u32 PFCACHE_SearchForFreePage(PF_CACHE_PAGE* p_head, PF_CACHE_PAGE** pp_page)
{
    PF_CACHE_PAGE* p_page;

    for (p_page = p_head->p_prev; p_page != p_head; p_page = p_page->p_prev) {
        if (!(p_page->stat & PF_CACHE_PAGE_STAT_USED)) {
            *pp_page = p_page;
            return 1;
        }
        if (p_page->sector != PF_SECTOR_INVALID) {
            *pp_page = p_page;
            return 0;
        }
    }

    if (!(p_page->stat & PF_CACHE_PAGE_STAT_USED)) {
        *pp_page = p_page;
        return 1;
    }
    if (p_page->sector != PF_SECTOR_INVALID) {
        *pp_page = p_page;
        return 0;
    }
    *pp_page = NULL;
    return 0;
}

s32 PFCACHE_FlushPageIfNeeded(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page)
{
    s32 err;
    u32 num_success;
    u8* p_buf;
    u32 size;
    u32 sector;

    if (p_page != NULL && (p_page->stat & PF_CACHE_PAGE_STAT_MODIFIED) && p_page->sector != PF_SECTOR_INVALID) {
        p_buf = p_page->p_mod_sbuf;
        if (p_buf == NULL) {
            size = p_page->size;
            err = PFDRV_lwrite(p_vol, p_page->buffer, p_page->sector, size, &num_success);
        } else {
            sector = p_page->sector + ((u32)(p_buf - p_page->buffer) >> p_vol->bpb.log2_bytes_per_sector);
            size = ((u32)p_page->p_mod_ebuf >> p_vol->bpb.log2_bytes_per_sector) -
                   ((u32)p_buf >> p_vol->bpb.log2_bytes_per_sector) + 1;
            err = PFDRV_lwrite(p_vol, p_buf, sector, size, &num_success);
        }
        if (err != 0) {
            return err;
        }
        if (num_success != size) {
            return 17;
        }
        PFCACHE_ClearPageModified(p_page);
    }
    return 0;
}

static inline void PFCACHE_MovePageToHead(PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page)
{
    if (p_page != *pp_head) {
        if (p_page == (*pp_head)->p_prev) {
            *pp_head = p_page;
            return;
        }
        p_page->p_prev->p_next = p_page->p_next;
        p_page->p_next->p_prev = p_page->p_prev;
        p_page->p_next = *pp_head;
        p_page->p_prev = (*pp_head)->p_prev;
        p_page->p_next->p_prev = p_page;
        p_page->p_prev->p_next = p_page;
        *pp_head = p_page;
    }
}

static inline void PFCACHE_MovePageToTail(PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page)
{
    if (p_page == *pp_head) {
        *pp_head = p_page->p_next;
    } else if (p_page != (*pp_head)->p_prev) {
        p_page->p_prev->p_next = p_page->p_next;
        p_page->p_next->p_prev = p_page->p_prev;
        p_page->p_next = *pp_head;
        p_page->p_prev = (*pp_head)->p_prev;
        p_page->p_next->p_prev = p_page;
        p_page->p_prev->p_next = p_page;
    }
}

s32 PFCACHE_DoAllocatePage(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit)
{
    s32 err;

    *pp_page = PFCACHE_SearchForPage(p_vol, *pp_head, sector);
    if (*pp_page != NULL) {
        *p_is_hit = 1;
    } else {
        *p_is_hit = 0;
        if (!PFCACHE_SearchForFreePage(*pp_head, pp_page)) {
            if (*pp_page == NULL) {
                *pp_page = NULL;
                return 30;
            }
            err = PFCACHE_FlushPageIfNeeded(p_vol, *pp_page);
            if (err != 0) {
                *pp_page = NULL;
                return err;
            }
            (*pp_page)->p_mod_sbuf = NULL;
            (*pp_page)->p_mod_ebuf = NULL;
        }

        if (sector != PF_SECTOR_INVALID) {
            if ((*pp_page)->stat & PF_CACHE_PAGE_STAT_FAT) {
                (*pp_page)->sector = sector - sector % (*pp_page)->size;
                (*pp_page)->p_buf =
                    &(*pp_page)->buffer[(sector % (*pp_page)->size) << p_vol->bpb.log2_bytes_per_sector];
            } else {
                if (p_vol->bpb.fat_type == PDM_FAT_32 &&
                    (sector < p_vol->bpb.first_data_sector || sector >= p_vol->bpb.total_sectors) &&
                    sector == p_vol->bpb.fs_info_sector) {
                    (*pp_page)->size = 1;
                }
                (*pp_page)->sector = sector - sector % (*pp_page)->size;
                (*pp_page)->p_buf =
                    &(*pp_page)->buffer[(sector % (*pp_page)->size) << p_vol->bpb.log2_bytes_per_sector];
            }
        } else {
            (*pp_page)->sector = sector;
            (*pp_page)->p_buf = (*pp_page)->buffer;
        }
        (*pp_page)->stat |= PF_CACHE_PAGE_STAT_USED;
    }
    PFCACHE_MovePageToHead(pp_head, *pp_page);
    return 0;
}

static inline void PFCACHE_DoFreeCachePage(PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page)
{
    p_page->stat &= ~PF_CACHE_PAGE_STAT_MODIFIED;
    p_page->stat &= ~PF_CACHE_PAGE_STAT_USED;
    p_page->p_mod_sbuf = NULL;
    p_page->p_mod_ebuf = NULL;
    p_page->sector = PF_SECTOR_INVALID;
    p_page->signature = NULL;
    PFCACHE_MovePageToTail(pp_head, p_page);
}

s32 PFCACHE_DoReadPage(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig)
{
    s32 err;
    s32 read_err;
    u32 num_success;
    u32 is_hit;

    err = PFCACHE_DoAllocatePage(p_vol, pp_head, sector, pp_page, &is_hit);
    if (err != 0) {
        return err;
    }

    if (!is_hit) {
        read_err = PFDRV_lread(p_vol, (*pp_page)->buffer, (*pp_page)->sector, (*pp_page)->size, &num_success);
        if (read_err != 0) {
            PFCACHE_DoFreeCachePage(pp_head, *pp_page);
            return read_err;
        }
        if (num_success != (*pp_page)->size && p_vol->bpb.total_sectors != (*pp_page)->sector + num_success) {
            PFCACHE_DoFreeCachePage(pp_head, *pp_page);
            return 17;
        }

        (*pp_page)->stat &= ~PF_CACHE_PAGE_STAT_MODIFIED;
        (*pp_page)->p_mod_sbuf = NULL;
        (*pp_page)->p_mod_ebuf = NULL;
        if (set_sig) {
            (*pp_page)->signature = p_vol->cache.signature;
        } else {
            (*pp_page)->signature = NULL;
        }
    }
    return 0;
}

s32 PFCACHE_DoReadPageAndFlushIfNeeded(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u32 sector, PF_CACHE_PAGE** pp_page,
                                       u32 set_sig)
{
    s32 err;
    s32 read_err;
    u32 num_success;
    u32 is_hit;

    err = PFCACHE_DoAllocatePage(p_vol, pp_head, sector, pp_page, &is_hit);
    if (err != 0) {
        return err;
    }

    if (!is_hit) {
        read_err = PFDRV_lread(p_vol, (*pp_page)->buffer, (*pp_page)->sector, (*pp_page)->size, &num_success);
        if (read_err != 0) {
            PFCACHE_DoFreeCachePage(pp_head, *pp_page);
            return read_err;
        }
        if (num_success != (*pp_page)->size && p_vol->bpb.total_sectors != (*pp_page)->sector + num_success) {
            PFCACHE_DoFreeCachePage(pp_head, *pp_page);
            return 17;
        }

        (*pp_page)->stat &= ~PF_CACHE_PAGE_STAT_MODIFIED;
        (*pp_page)->p_mod_sbuf = NULL;
        (*pp_page)->p_mod_ebuf = NULL;
        if (set_sig) {
            (*pp_page)->signature = p_vol->cache.signature;
        } else {
            (*pp_page)->signature = NULL;
        }
    } else if (set_sig && ((*pp_page)->stat & PF_CACHE_PAGE_STAT_MODIFIED) &&
               (p_vol->cache.mode & PF_CACHE_MODE_SIGNATURE) && (*pp_page)->signature != p_vol->cache.signature) {
        err = PFCACHE_FlushPageIfNeeded(p_vol, *pp_page);
        if (err != 0) {
            *pp_page = NULL;
            return err;
        }
    }
    return 0;
}

static inline PF_CACHE_PAGE* PFCACHE_SearchForUsedPage(PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page)
{
    PF_CACHE_PAGE* p_next;

    if (p_page == NULL) {
        p_next = *pp_head;
    } else {
        p_next = p_page->p_next;
        if (p_next == *pp_head) {
            return NULL;
        }
    }

    while (p_next->stat & PF_CACHE_PAGE_STAT_USED) {
        if (p_next->sector != PF_SECTOR_INVALID) {
            p_next->p_buf = p_next->buffer;
            return p_next;
        }
        p_next = p_next->p_next;
    }
    return NULL;
}

s32 PFCACHE_DoReadNumSector(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, u8* p_buf, u32 sector, u32 num_sector,
                            u32* p_num_success)
{
    PF_CACHE_PAGE* p_page;
    u32 num_rest_sector;
    u32 num_success_sector;
    s32 err;

    *p_num_success = 0;
    err = PFDRV_lread(p_vol, p_buf, sector, num_sector, p_num_success);
    if (err != 0) {
        return err;
    }

    p_page = NULL;
    num_rest_sector = *p_num_success;
    num_success_sector = *p_num_success;
    do {
        p_page = PFCACHE_SearchForUsedPage(pp_head, p_page);
        if (p_page != NULL && p_page->sector != PF_SECTOR_INVALID) {
            if (p_page->sector <= sector && p_page->sector + p_page->size >= sector + num_success_sector) {
                pf_memcpy(p_buf, &p_page->buffer[(sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector],
                          num_success_sector << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= num_success_sector;
            } else if (p_page->sector >= sector && p_page->sector + p_page->size <= sector + num_success_sector) {
                pf_memcpy(&p_buf[(p_page->sector - sector) << p_vol->bpb.log2_bytes_per_sector], p_page->buffer,
                          p_page->size << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= p_page->size;
            } else if (p_page->sector > sector && p_page->sector < sector + num_success_sector &&
                       p_page->sector + p_page->size >= sector + num_success_sector) {
                pf_memcpy(&p_buf[(p_page->sector - sector) << p_vol->bpb.log2_bytes_per_sector], p_page->buffer,
                          (p_page->size - (p_page->sector + p_page->size - (sector + num_success_sector)))
                              << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= p_page->size - (p_page->sector + p_page->size - (sector + num_success_sector));
            } else if (p_page->sector < sector && p_page->sector + p_page->size > sector &&
                       p_page->sector + p_page->size <= sector + num_success_sector) {
                pf_memcpy(p_buf, &p_page->buffer[(sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector],
                          (p_page->size - (sector - p_page->sector)) << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= p_page->size - (sector - p_page->sector);
            }
        }
    } while (p_page != NULL && num_rest_sector != 0);

    if (*p_num_success != num_sector && p_vol->bpb.total_sectors != sector + *p_num_success) {
        return 17;
    }
    return 0;
}

s32 PFCACHE_DoWritePage(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, PF_CACHE_PAGE* p_page, u32 set_sig)
{
    s32 err;

    PFCACHE_MovePageToHead(pp_head, p_page);

    p_page->stat |= PF_CACHE_PAGE_STAT_MODIFIED;
    if (p_page->p_mod_sbuf == NULL) {
        p_page->p_mod_sbuf = p_page->p_buf;
        p_page->p_mod_ebuf = p_page->p_buf;
    } else if (p_page->p_buf < p_page->p_mod_sbuf) {
        p_page->p_mod_sbuf = p_page->p_buf;
    } else if (p_page->p_buf > p_page->p_mod_ebuf) {
        p_page->p_mod_ebuf = p_page->p_buf;
    }

    if (set_sig) {
        p_page->signature = p_vol->cache.signature;
    } else {
        p_page->signature = NULL;
    }

    if ((p_vol->cache.mode & PF_CACHE_MODE_WRITE_THROUGH) ||
        ((p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) && (p_page->stat & PF_CACHE_PAGE_STAT_FAT))) {
        err = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 PFCACHE_DoWriteSector(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector)
{
    s32 err;
    u32 num_success;
    PF_CACHE_PAGE* p_page;

    p_page = PFCACHE_SearchForPage(p_vol, *pp_head, sector);
    if (p_page != NULL) {
        pf_memcpy(p_page->p_buf, p_buf, p_vol->bpb.bytes_per_sector);
        p_page->stat |= PF_CACHE_PAGE_STAT_MODIFIED;
        if (p_page->p_mod_sbuf == NULL) {
            p_page->p_mod_sbuf = p_page->p_buf;
            p_page->p_mod_ebuf = p_page->p_buf;
        } else if (p_page->p_buf < p_page->p_mod_sbuf) {
            p_page->p_mod_sbuf = p_page->p_buf;
        } else if (p_page->p_buf > p_page->p_mod_ebuf) {
            p_page->p_mod_ebuf = p_page->p_buf;
        }
        err = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0) {
            return err;
        }
    } else {
        err = PFDRV_lwrite(p_vol, p_buf, sector, 1, &num_success);
        if (err != 0) {
            return err;
        }
        if (num_success != 1) {
            return 17;
        }
    }
    return 0;
}

s32 PFCACHE_DoWriteNumSectorAndFreeIfNeeded(PF_VOLUME* p_vol, PF_CACHE_PAGE** pp_head, const u8* p_buf, u32 sector,
                                            u32 num_sector, u32* p_num_success)
{
    PF_CACHE_PAGE* p_page;
    u8* p_sbuf;
    u8* p_ebuf;
    u32 num_rest_sector;
    s32 err;

    p_page = NULL;
    *p_num_success = 0;
    num_rest_sector = num_sector;
    do {
        p_page = PFCACHE_SearchForUsedPage(pp_head, p_page);
        if (p_page != NULL && p_page->sector != PF_SECTOR_INVALID) {
            if (p_page->sector <= sector && p_page->sector + p_page->size >= sector + num_sector) {
                pf_memcpy(&p_page->buffer[(sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector], p_buf,
                          num_sector << p_vol->bpb.log2_bytes_per_sector);
                *p_num_success += num_rest_sector;
                num_rest_sector = 0;
                p_page->stat |= PF_CACHE_PAGE_STAT_MODIFIED;
                p_sbuf = &p_page->buffer[(sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector];
                p_ebuf = &p_sbuf[(num_sector - 1) << p_vol->bpb.log2_bytes_per_sector];
                if (p_page->p_mod_sbuf == NULL) {
                    p_page->p_mod_sbuf = p_sbuf;
                    p_page->p_mod_ebuf = p_ebuf;
                } else if (p_sbuf < p_page->p_mod_sbuf) {
                    p_page->p_mod_sbuf = p_sbuf;
                } else if (p_page->p_mod_ebuf < p_ebuf) {
                    p_page->p_mod_ebuf = p_ebuf;
                }
            } else if (p_page->sector >= sector && p_page->sector + p_page->size <= sector + num_sector) {
                pf_memcpy(p_page->buffer, &p_buf[(p_page->sector - sector) << p_vol->bpb.log2_bytes_per_sector],
                          p_page->size << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= p_page->size;
                *p_num_success += p_page->size;
                p_page->stat |= PF_CACHE_PAGE_STAT_MODIFIED;
                p_page->p_mod_sbuf = p_page->buffer;
                p_page->p_mod_ebuf = &p_page->buffer[(p_page->size - 1) << p_vol->bpb.log2_bytes_per_sector];
            } else if (p_page->sector > sector && p_page->sector < sector + num_sector &&
                       p_page->sector + p_page->size >= sector + num_sector) {
                pf_memcpy(p_page->buffer, &p_buf[(p_page->sector - sector) << p_vol->bpb.log2_bytes_per_sector],
                          (sector + num_sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= p_page->size - (p_page->sector + p_page->size - (sector + num_sector));
                *p_num_success += p_page->size - (p_page->sector + p_page->size - (sector + num_sector));
                p_page->stat |= PF_CACHE_PAGE_STAT_MODIFIED;
                p_ebuf = &p_page->buffer[(p_page->size - (p_page->sector + p_page->size - (sector + num_sector)) - 1)
                                         << p_vol->bpb.log2_bytes_per_sector];
                p_page->p_mod_sbuf = p_page->buffer;
                if (p_page->p_mod_ebuf == NULL || p_page->p_mod_ebuf < p_ebuf) {
                    p_page->p_mod_ebuf = p_ebuf;
                }
            } else if (p_page->sector < sector && p_page->sector + p_page->size > sector &&
                       p_page->sector + p_page->size <= sector + num_sector) {
                pf_memcpy(&p_page->buffer[(sector - p_page->sector) << p_vol->bpb.log2_bytes_per_sector], p_buf,
                          (p_page->size - (sector - p_page->sector)) << p_vol->bpb.log2_bytes_per_sector);
                num_rest_sector -= p_page->size - (sector - p_page->sector);
                *p_num_success += p_page->size - (sector - p_page->sector);
                p_page->stat |= PF_CACHE_PAGE_STAT_MODIFIED;
                p_sbuf = &p_page->buffer[(p_page->size - (sector - p_page->sector) - 1) << p_vol->bpb.log2_bytes_per_sector];
                if (p_page->p_mod_sbuf == NULL || p_sbuf < p_page->p_mod_sbuf) {
                    p_page->p_mod_sbuf = p_sbuf;
                }
                p_page->p_mod_ebuf = &p_page->buffer[(p_page->size - 1) << p_vol->bpb.log2_bytes_per_sector];
            }
        }
    } while (p_page != NULL && num_rest_sector != 0);

    if (num_rest_sector != 0) {
        err = PFDRV_lwrite(p_vol, p_buf, sector, num_sector, p_num_success);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 PFCACHE_DoFlushCache(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_head)
{
    PF_CACHE_PAGE* p_page;
    s32 err;
    s32 first_err;

    first_err = 0;
    p_page = p_head;
    if (!(p_head->stat & PF_CACHE_PAGE_STAT_USED)) {
        return 0;
    }
    if ((p_vol->cache.mode & PF_CACHE_MODE_WRITE_THROUGH) ||
        ((p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) && (p_head->stat & PF_CACHE_PAGE_STAT_FAT))) {
        return 0;
    }

    do {
        if (!(p_page->stat & PF_CACHE_PAGE_STAT_USED)) {
            break;
        }
        err = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
        if (err != 0 && first_err == 0) {
            first_err = err;
        }
        p_page = p_page->p_next;
    } while (p_page != p_head);
    return first_err;
}

void PFCACHE_SetCache(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_cache_page, u8 (*p_cache_buf)[512], u16 num_fat_pages,
                      u16 num_data_pages)
{
    p_vol->cache.pages = p_cache_page;
    p_vol->cache.buffers = p_cache_buf;
    p_vol->cache.num_fat_pages = num_fat_pages;
    p_vol->cache.num_data_pages = num_data_pages;
}

void PFCACHE_SetFATBufferSize(PF_VOLUME* p_vol, u32 size)
{
    if (size != 0) {
        p_vol->cache.fat_buff_size = size;
    }
}

void PFCACHE_SetDataBufferSize(PF_VOLUME* p_vol, u32 size)
{
    if (size != 0) {
        p_vol->cache.data_buff_size = size;
    }
}

s32 PFCACHE_InitCaches(PF_VOLUME* p_vol)
{
    s32 err;

    p_vol->cache.mode &= ~PF_CACHE_MODE_WRITE_THROUGH;
    p_vol->cache.mode |= PF_CACHE_MODE_SIGNATURE;
    p_vol->cache.mode &= ~PF_CACHE_MODE_FAT_WRITE_THROUGH;
    pf_memset(p_vol->cache.buffers, 0, (p_vol->cache.num_fat_pages + p_vol->cache.num_data_pages) << 9);

    err = PFCACHE_InitPageList(p_vol, &p_vol->cache.p_current_fat, p_vol->cache.pages, p_vol->cache.buffers,
                               p_vol->cache.num_fat_pages, p_vol->cache.fat_buff_size, 1);
    if (err != 0) {
        return err;
    }

    err = PFCACHE_InitPageList(p_vol, &p_vol->cache.p_current_data, &p_vol->cache.pages[p_vol->cache.num_fat_pages],
                               &p_vol->cache.buffers[p_vol->cache.num_fat_pages], p_vol->cache.num_data_pages,
                               p_vol->cache.data_buff_size, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_SetWriteThrough(PF_VOLUME* p_vol)
{
    s32 err;

    err = PFCACHE_FlushAllCaches(p_vol);
    if (err != 0) {
        return err;
    }
    p_vol->cache.mode |= PF_CACHE_MODE_WRITE_THROUGH;
    return 0;
}

s32 PFCACHE_ClearWriteThrough(PF_VOLUME* p_vol)
{
    p_vol->cache.mode &= ~PF_CACHE_MODE_WRITE_THROUGH;
    return 0;
}

s32 PFCACHE_AllocateFATPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit)
{
    s32 err;

    if (p_vol->cache.p_current_fat == NULL) {
        p_vol->cache.pages->buffer = *p_vol->cache.buffers;
        p_vol->cache.pages->p_buf = *p_vol->cache.buffers;
        *pp_page = p_vol->cache.pages;
        return 0;
    }

    err = PFCACHE_DoAllocatePage(p_vol, &p_vol->cache.p_current_fat, sector, pp_page, p_is_hit);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_AllocateDataPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32* p_is_hit)
{
    s32 err;

    if (p_vol->cache.p_current_data == NULL) {
        p_vol->cache.pages->buffer = *p_vol->cache.buffers;
        p_vol->cache.pages->p_buf = *p_vol->cache.buffers;
        *pp_page = p_vol->cache.pages;
        return 0;
    }

    err = PFCACHE_DoAllocatePage(p_vol, &p_vol->cache.p_current_data, sector, pp_page, p_is_hit);
    if (err != 0) {
        return err;
    }
    return 0;
}

void PFCACHE_FreeFATPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page)
{
    if (p_vol->cache.p_current_fat != NULL) {
        p_page->p_mod_sbuf = NULL;
        p_page->stat &= ~(PF_CACHE_PAGE_STAT_USED | PF_CACHE_PAGE_STAT_MODIFIED);
        p_page->p_mod_ebuf = NULL;
        p_page->sector = PF_SECTOR_INVALID;
        p_page->signature = NULL;
        PFCACHE_MovePageToTail(&p_vol->cache.p_current_fat, p_page);
    }
}

void PFCACHE_FreeDataPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page)
{
    if (p_vol->cache.p_current_data != NULL) {
        p_page->p_mod_sbuf = NULL;
        p_page->stat &= ~(PF_CACHE_PAGE_STAT_USED | PF_CACHE_PAGE_STAT_MODIFIED);
        p_page->p_mod_ebuf = NULL;
        p_page->sector = PF_SECTOR_INVALID;
        p_page->signature = NULL;
        PFCACHE_MovePageToTail(&p_vol->cache.p_current_data, p_page);
    }
}

s32 PFCACHE_ReadFATPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page)
{
    s32 err;

    err = PFCACHE_DoReadPage(p_vol, &p_vol->cache.p_current_fat, sector, pp_page, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadDataPage(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig)
{
    s32 err;

    err = PFCACHE_DoReadPage(p_vol, &p_vol->cache.p_current_data, sector, pp_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadDataPageAndFlushIfNeeded(PF_VOLUME* p_vol, u32 sector, PF_CACHE_PAGE** pp_page, u32 set_sig)
{
    s32 err;

    err = PFCACHE_DoReadPageAndFlushIfNeeded(p_vol, &p_vol->cache.p_current_data, sector, pp_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_ReadDataNumSector(PF_VOLUME* p_vol, u8* p_buf, u32 sector, u32 num_sector, u32* p_num_success)
{
    s32 err;

    if (sector >= p_vol->bpb.total_sectors) {
        return 16;
    }
    err = PFCACHE_DoReadNumSector(p_vol, &p_vol->cache.p_current_data, p_buf, sector, num_sector, p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_WriteFATPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page)
{
    s32 err;

    err = PFCACHE_DoWritePage(p_vol, &p_vol->cache.p_current_fat, p_page, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_WriteDataPage(PF_VOLUME* p_vol, PF_CACHE_PAGE* p_page, u32 set_sig)
{
    s32 err;

    err = PFCACHE_DoWritePage(p_vol, &p_vol->cache.p_current_data, p_page, set_sig);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_WriteFATSector(PF_VOLUME* p_vol, const u8* p_buf, u32 sector)
{
    s32 err;

    if (sector >= p_vol->bpb.total_sectors) {
        return 16;
    }
    err = PFCACHE_DoWriteSector(p_vol, &p_vol->cache.p_current_fat, p_buf, sector);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_WriteDataNumSectorAndFreeIfNeeded(PF_VOLUME* p_vol, const u8* p_buf, u32 sector, u32 num_sector,
                                              u32* p_num_success)
{
    s32 err;

    if (sector >= p_vol->bpb.total_sectors) {
        return 16;
    }
    err = PFCACHE_DoWriteNumSectorAndFreeIfNeeded(p_vol, &p_vol->cache.p_current_data, p_buf, sector, num_sector,
                                                  p_num_success);
    if (err != 0) {
        return err;
    }
    return 0;
}

PF_CACHE_PAGE* PFCACHE_SearchDataCache(PF_VOLUME* p_vol, u32 sector)
{
    return PFCACHE_SearchForPage(p_vol, p_vol->cache.p_current_data, sector);
}

s32 PFCACHE_FlushFATCache(PF_VOLUME* p_vol)
{
    s32 err;

    err = PFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_fat);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_FlushDataCache(PF_VOLUME* p_vol)
{
    s32 err;

    err = PFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_data);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFCACHE_FlushDataCacheSpecific(PF_VOLUME* p_vol, void* signature)
{
    s32 err;
    PF_CACHE_PAGE* p_head;
    PF_CACHE_PAGE* p_page;

    err = 0;
    if (p_vol->cache.mode & (PF_CACHE_MODE_WRITE_THROUGH | PF_CACHE_MODE_SIGNATURE | PF_CACHE_MODE_FAT_WRITE_THROUGH)) {
        p_head = p_vol->cache.p_current_data;
        if (p_head->signature == NULL || p_head->signature == signature) {
            err = PFCACHE_FlushPageIfNeeded(p_vol, p_head);
            if (err != 0) {
                return err;
            }
        }

        for (p_page = p_head->p_next; p_page != p_head; p_page = p_page->p_next) {
            if (p_page->signature == NULL || p_page->signature == signature) {
                err = PFCACHE_FlushPageIfNeeded(p_vol, p_page);
                if (err != 0) {
                    return err;
                }
            }
        }
    }
    return err;
}

s32 PFCACHE_FlushAllCaches(PF_VOLUME* p_vol)
{
    s32 err;

    err = PFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_fat);
    if (err != 0) {
        return err;
    }
    err = PFCACHE_DoFlushCache(p_vol, p_vol->cache.p_current_data);
    if (err != 0) {
        return err;
    }
    return 0;
}

void PFCACHE_FreeAllCaches(PF_VOLUME* p_vol)
{
    while (p_vol->cache.p_current_fat->stat & PF_CACHE_PAGE_STAT_USED) {
        if (p_vol->cache.p_current_fat != NULL) {
            PFCACHE_DoFreeCachePage(&p_vol->cache.p_current_fat, p_vol->cache.p_current_fat);
        }
    }
    while (p_vol->cache.p_current_data->stat & PF_CACHE_PAGE_STAT_USED) {
        if (p_vol->cache.p_current_data != NULL) {
            PFCACHE_DoFreeCachePage(&p_vol->cache.p_current_data, p_vol->cache.p_current_data);
        }
    }
}
