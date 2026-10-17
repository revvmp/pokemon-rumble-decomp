#include <revolution/fa/pf_entry_iterator.h>
#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_sector.h>
#include <revolution/fa/pf_str.h>

#define GET_U16_LE(p) ((u16)(((*(u16*)(p) >> 8) & 0xFF) | ((*(u16*)(p) & 0xFF) << 8)))

s32 PFENT_ITER_RecalcEntryIterator(PF_ENT_ITER* p_iter, u32 may_allocate)
{
    u32 prev_file_sector_index;
    u32 file_sector_index;
    PF_CACHE_PAGE* p_page;
    u32 success_size;
    u32 is_hit;
    s32 err;
    u32 i;

    file_sector_index = p_iter->index >> p_iter->log2_entries_per_sector;
    prev_file_sector_index = p_iter->file_sector_index;
    if (file_sector_index != prev_file_sector_index) {
        p_iter->file_sector_index = file_sector_index;
        err = PFFAT_GetSectorSpecified(&p_iter->ffd, file_sector_index, 0, &p_iter->sector);
        if (err != 0) {
            return err;
        }
        if (p_iter->sector == 0xFFFFFFFF && may_allocate != 0) {
            err = PFFAT_GetSectorSpecified(&p_iter->ffd, file_sector_index, 1, &p_iter->sector);
            if (err != 0) {
                return err;
            }
            if (p_iter->sector != 0xFFFFFFFF && prev_file_sector_index != 0xFFFFFFFF) {
                err = PFCACHE_AllocateDataPage(p_iter->p_vol, 0xFFFFFFFF, &p_page, &is_hit);
                if (err != 0) {
                    return err;
                }
                pf_memset(p_page->p_buf, 0, p_iter->p_vol->bpb.bytes_per_sector);
                for (i = 0; i < p_iter->p_vol->bpb.sectors_per_cluster; i++) {
                    err = PFSEC_WriteData(p_iter->p_vol, p_page->p_buf, p_iter->sector + i, 0, p_iter->p_vol->bpb.bytes_per_sector,
                                          &success_size, 0);
                    if (err != 0) {
                        PFCACHE_FreeDataPage(p_iter->p_vol, p_page);
                        return err;
                    }
                    if (success_size != p_iter->p_vol->bpb.bytes_per_sector) {
                        PFCACHE_FreeDataPage(p_iter->p_vol, p_page);
                        return 17;
                    }
                }
                PFCACHE_FreeDataPage(p_iter->p_vol, p_page);
            }
        }
    }
    p_iter->offset = (p_iter->index & p_iter->offset_mask) << 5;
    return 0;
}

static inline s32 PFENT_ITER_LoadEntry(PF_ENT_ITER* p_iter)
{
    u32 success_size;
    s32 err;

    if (p_iter->sector == 0xFFFFFFFF) {
        return 16;
    }
    err = PFSEC_ReadData(p_iter->p_vol, p_iter->buf, p_iter->sector, p_iter->offset, 32, &success_size, 0);
    if (success_size != 32) {
        return 17;
    }
    return err;
}

static inline s32 PFENT_ITER_DoMoveTo(PF_ENT_ITER* p_iter, u32 index, u32 may_allocate)
{
    u32 prev_index;
    s32 err;

    prev_index = p_iter->index;
    p_iter->index = index;
    err = PFENT_ITER_RecalcEntryIterator(p_iter, may_allocate);
    if (err != 0) {
        p_iter->index = prev_index;
        PFENT_ITER_RecalcEntryIterator(p_iter, 0);
    } else {
        err = PFENT_ITER_LoadEntry(p_iter);
        return err != 0 ? err : 0;
    }
    return err;
}

static inline void PFENT_ITER_LoadFoundEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent)
{
    PFENT_loadEntryNumericFieldsFromBuf(p_ent, p_iter->buf);
    p_ent->entry_sector = p_iter->sector;
    p_ent->entry_offset = p_iter->offset;
    p_ent->p_vol = p_iter->ffd.p_vol;
    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 0) {
        p_ent->start_cluster = 1;
    }
}

s32 PFENT_ITER_DoFindEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded, u32* p_is_found)
{
    s32 err;
    u32 is_extsfn;
    u32 index_search_from;
    u8 attr;
    s8 filename[13];
    s32 len;

    err = 0;
    *p_is_found = 0;
    is_extsfn = 0;

    if (PFSTR_StrCmp(p_pattern, (const s8*)"..") == 0 && (p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 1) {
        return 2;
    }
    if (PFSTR_StrCmp(p_pattern, (const s8*)".") == 0) {
        *p_is_found = 1;
        return 0;
    }

    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;
    p_ent->long_name[0] = 0;

    if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
        is_extsfn = PFPATH_ParseEntryIndex(p_pattern, &index_search_from);
    }

    if (is_extsfn == 1) {
        for (err = PFENT_ITER_IteratorInitialize(p_iter, index_search_from - 1);; err = PFENT_ITER_Retreat(p_iter, 0)) {
            if (err != 0) {
                return err;
            }
            if ((p_iter->buf[11] & PF_ATTR_LFN) == PF_ATTR_LFN) {
                if (PFENT_LoadLFNEntryFieldsFromBuf(p_ent, p_iter->buf) != 0) {
                    return 3;
                }
                if (!(p_iter->buf[0] & 0x40)) {
                    continue;
                }
            } else {
                return 0;
            }
            err = PFENT_ITER_IteratorInitialize(p_iter, index_search_from);
            if (err != 0) {
                return err;
            }
            attr = p_iter->buf[11];
            if (attr == 0) {
                attr = 0x40;
            }
            if (attr_required & 0x80) {
                attr_required &= 0x7F;
                attr_excluded &= 0x7F;
                if ((attr_required != 0 && attr_required != (attr_required & attr)) ||
                    (attr_excluded != 0 && attr_excluded == (attr_excluded & attr)))
                {
                    err = -1;
                }
            } else if (attr_required != 0x7F && attr != attr_required && (!(attr & attr_required) || (attr & attr_excluded))) {
                err = -1;
            }
            if (err == -1) {
                return 0;
            }
            if (attr & PF_ATTR_VOLUME) {
                return 3;
            }
            PFENT_LoadShortNameFromBuf(p_ent, p_iter->buf);
            if (PFPATH_MatchFileNameWithPattern(p_ent->short_name, p_pattern, 0) == 0) {
                break;
            }
            PFENT_ITER_LoadFoundEntry(p_iter, p_ent);
            *p_is_found = 1;
            return 0;
        }
    } else {
        while (!PFENT_ITER_IsAtLogicalEnd(p_iter)) {
            if (err != 0) {
                return err;
            }
            if (p_iter->buf[0] == PF_ENT_END) {
                break;
            }
            if (p_iter->buf[0] == PF_ENT_DELETED) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;
            } else {
                attr = p_iter->buf[11];
                if ((attr & PF_ATTR_LFN) == PF_ATTR_LFN) {
                    if (PFENT_LoadLFNEntryFieldsFromBuf(p_ent, p_iter->buf) != 0) {
                        p_ent->num_entry_LFNs = 0;
                        p_ent->long_name[0] = 0;
                    }
                } else {
                    if (attr == 0) {
                        attr = 0x40;
                    }
                    if (attr_required & 0x80) {
                        attr_required &= 0x7F;
                        attr_excluded &= 0x7F;
                        if ((attr_required != 0 && attr_required != (attr_required & attr)) ||
                            (attr_excluded != 0 && attr_excluded == (attr_excluded & attr)))
                        {
                            err = -1;
                        }
                    } else if (attr_required != 0x7F && attr != attr_required && (!(attr & attr_required) || (attr & attr_excluded))) {
                        err = -1;
                    }
                    if (err == -1) {
                        p_ent->num_entry_LFNs = 0;
                        p_ent->long_name[0] = 0;
                    } else {
                        if (attr & PF_ATTR_VOLUME) {
                            p_ent->num_entry_LFNs = 0;
                            p_ent->long_name[0] = 0;
                        }
                        PFENT_LoadShortNameFromBuf(p_ent, p_iter->buf);
                        if (p_ent->num_entry_LFNs != 0 && p_ent->ordinal == 1 && p_ent->check_sum == PFENT_CalcCheckSum(p_ent) &&
                            PFPATH_MatchFileNameWithPattern((const s8*)p_ent->long_name, p_pattern, 1) != 0)
                        {
                            PFENT_ITER_LoadFoundEntry(p_iter, p_ent);
                            *p_is_found = 1;
                            return 0;
                        }
                        if (PFPATH_MatchFileNameWithPattern(p_ent->short_name, p_pattern, 0) != 0) {
                            if (p_iter->buf[12] & 0x18) {
                                VFiPFPATH_getLongNameformShortName(p_ent->short_name, filename, p_iter->buf[12]);
                                len = VFiPFPATH_transformInUnicode(p_ent->long_name, filename);
                                p_ent->num_entry_LFNs = len / 13 + (len % 13 != 0);
                                p_ent->check_sum = PFENT_CalcCheckSum(p_ent);
                                p_ent->ordinal = 1;
                            }
                            PFENT_ITER_LoadFoundEntry(p_iter, p_ent);
                            *p_is_found = 1;
                            return 0;
                        }
                        p_ent->num_entry_LFNs = 0;
                        p_ent->long_name[0] = 0;
                    }
                }
            }
            err = PFENT_ITER_Advance(p_iter, 0);
        }
    }
    return 0;
}

s32 PFENT_LookupPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 wildcard, u32 is_parent)
{
    const s8* p;
    PF_STR token;
    u32 is_found;
    PF_FAT_HINT hint;
    u32 code_mode;
    u32 start_cluster;
    PF_DIR_ENT current_ent;
    s32 err;

    if (PFSTR_StrNCmp(p_path, (const s8*)":", 1, 1, 1) == 0) {
        PFSTR_MoveStrPos(p_path, 2);
    }

    if (PFSTR_StrNCmp(p_path, (const s8*)"\\", 1, 0, 1) == 0 || PFSTR_StrNCmp(p_path, (const s8*)"/", 1, 0, 1) == 0) {
        err = PFENT_GetRootDir(p_vol, p_ent);
        if (err != 0) {
            return err;
        }
        if (PFSTR_StrNumChar(p_path, 1) == 1 && PFSTR_StrNCmp(p_path, (const s8*)"\0", 2, 0, 1) == 0) {
            return 0;
        }
    } else {
        err = PFVOL_GetCurrentDir(p_vol, p_ent);
        if (err != 0) {
            return err;
        }
    }

    p = PFSTR_GetStrPos(p_path, 1);
    PFFAT_InitFFD(&p_iter->ffd, &hint, p_vol, &p_iter->ffd.start_cluster);
    p_iter->ffd.start_cluster = p_ent->start_cluster;
    code_mode = PFSTR_GetCodeMode(p_path);
    VFiPFPATH_InitTokenOfPath(&token, p, code_mode);
    err = VFiPFPATH_GetNextTokenOfPath(&token, wildcard);

    while (token.p_head != NULL) {
        if (err != 0) {
            return err;
        }
        if (is_parent != 0 && PFSTR_StrNCmp(&token, (const s8*)"\0", 2, 0, 1) == 0) {
            break;
        }
        if (!(p_ent->attr & PF_ATTR_DIR)) {
            return 2;
        }
        if (PFSTR_StrLen(&token) == 0) {
            return 2;
        }

        p_iter->ffd.start_cluster = p_ent->start_cluster;
        PFFAT_ResetFFD(&p_iter->ffd, &p_iter->ffd.start_cluster);
        err = PFENT_ITER_IteratorInitialize(p_iter, 0);
        if (err != 0) {
            return err;
        }

        start_cluster = *p_iter->ffd.p_start_cluster;
        err = PFENT_ITER_DoFindEntry(p_iter, p_ent, &token, 0x7F, 0, &is_found);
        if (err != 0) {
            return err;
        }

        if (!is_found) {
            err = VFiPFPATH_GetNextTokenOfPath(&token, wildcard);
            if (err != 0) {
                return err;
            }
            return !token.p_head + 2;
        }

        if (wildcard == 0) {
            if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 1) {
                p_ent->path_len = 3;
            } else if ((p_ent->attr & PF_ATTR_DIR) && p_ent->short_name[0] == '.' && p_ent->short_name[1] == '.') {
                PFFAT_ResetFFD(&p_iter->ffd, &p_ent->start_cluster);
                err = PFENT_ITER_IteratorInitialize(p_iter, 0);
                if (err != 0) {
                    return err;
                }
                current_ent.attr = p_ent->attr;
                current_ent.start_cluster = p_ent->start_cluster;
                err = PFENT_ITER_FindCluster(p_iter, &current_ent, start_cluster, &is_found);
                if (err != 0) {
                    return err;
                }
                if (!is_found) {
                    return 2;
                }
                if (current_ent.num_entry_LFNs != 0) {
                    p_ent->path_len -= VFiPFPATH_GetLengthFromUnicode(current_ent.long_name);
                } else {
                    p_ent->path_len -= VFiPFPATH_GetLengthFromShortname(current_ent.short_name);
                }
                p_ent->path_len--;
            } else if (!(p_ent->attr & PF_ATTR_DIR) || p_ent->short_name[0] != '.') {
                p_ent->path_len += PFSTR_StrNumChar(&token, 1) + 1;
                if (PFSTR_StrNCmp(&token, (const s8*)"\0", 2, 0, 1) != 0) {
                    p_ent->path_len -= PFSTR_StrNumChar(&token, 2);
                }
            }
        }

        err = VFiPFPATH_GetNextTokenOfPath(&token, wildcard);
    }

    return 0;
}

s32 PFENT_ITER_IteratorInitialize(PF_ENT_ITER* p_iter, u32 index_start_from)
{
    p_iter->p_vol = p_iter->ffd.p_vol;
    p_iter->log2_entries_per_sector = p_iter->ffd.p_vol->bpb.log2_bytes_per_sector - 5;
    p_iter->offset_mask = (1 << p_iter->log2_entries_per_sector) - 1;
    p_iter->file_sector_index = 0xFFFFFFFF;
    return PFENT_ITER_DoMoveTo(p_iter, index_start_from, 0);
}

u32 PFENT_ITER_IsAtLogicalEnd(PF_ENT_ITER* p_iter)
{
    s32 err;
    u32 save_index;
    u32 save_fsindex;

    if (p_iter->sector == 0xFFFFFFFF || p_iter->buf[0] == 0 || p_iter->index >= 999999) {
        save_index = p_iter->index;
        save_fsindex = p_iter->file_sector_index;
        p_iter->index = (p_iter->file_sector_index + 1) << p_iter->log2_entries_per_sector;
        err = PFENT_ITER_RecalcEntryIterator(p_iter, 0);
        if (err != 0) {
            p_iter->index = save_index;
            p_iter->file_sector_index = save_fsindex;
            return 1;
        }
        if (p_iter->sector != 0xFFFFFFFF) {
            err = PFENT_ITER_LoadEntry(p_iter);
            if (err != 0) {
                return 1;
            }
            return 0;
        }
        return 1;
    }
    return 0;
}

s32 PFENT_ITER_MoveTo(PF_ENT_ITER* p_iter, u32 index, u32 may_allocate)
{
    s32 err;

    if (p_iter->index != index || p_iter->sector != 0xFFFFFFFF || may_allocate == 0) {
        err = PFENT_ITER_DoMoveTo(p_iter, index, may_allocate);
        if (err != 0) {
            return err;
        }
    }
    return 0;
}

s32 PFENT_ITER_Advance(PF_ENT_ITER* p_iter, u32 may_allocate)
{
    return PFENT_ITER_MoveTo(p_iter, p_iter->index + 1, may_allocate);
}

s32 PFENT_ITER_IsInDirectory(PF_DIR_ENT* p_ent, u32 cluster, u32* p_is_found)
{
    u32 start_cluster;
    u32 sector_in_cluster;
    u32 entries_per_sector;
    PF_FAT_HINT hint;
    PF_ENT_ITER iter;
    s32 err;

    *p_is_found = 0;
    if (p_ent->entry_sector < p_ent->p_vol->bpb.first_data_sector) {
        return 0;
    }
    start_cluster = (p_ent->entry_sector - p_ent->p_vol->bpb.first_data_sector) >> p_ent->p_vol->bpb.log2_sectors_per_cluster;
    sector_in_cluster = (p_ent->entry_sector - p_ent->p_vol->bpb.first_data_sector) & (p_ent->p_vol->bpb.sectors_per_cluster - 1);
    if (start_cluster == cluster) {
        *p_is_found = 1;
        return 0;
    }

    iter.ffd.p_start_cluster = &start_cluster;
    iter.ffd.p_hint = &hint;
    iter.ffd.p_vol = p_ent->p_vol;
    iter.ffd.chain_stamp = 1;
    iter.ffd.cluster_link.buffer = NULL;
    iter.log2_entries_per_sector = p_ent->p_vol->bpb.log2_bytes_per_sector - 5;
    iter.file_sector_index = sector_in_cluster;
    entries_per_sector = 1 << iter.log2_entries_per_sector;
    iter.offset_mask = entries_per_sector - 1;
    iter.p_vol = p_ent->p_vol;
    iter.sector = p_ent->entry_sector;
    iter.offset = p_ent->entry_offset;
    iter.index = ((u32)iter.offset >> 5) + sector_in_cluster * entries_per_sector;
    hint.chain_index = 0;
    hint.cluster = start_cluster;
    hint.chain_stamp = 1;
    if (iter.index < 2) {
        hint.chain_index = 1;
        iter.file_sector_index += p_ent->p_vol->bpb.sectors_per_cluster;
        iter.index += entries_per_sector << p_ent->p_vol->bpb.log2_sectors_per_cluster;
    }

    while (iter.index != 0) {
        err = PFENT_ITER_LoadEntry(&iter);
        if (err != 0) {
            return err;
        }
        if (pf_strncmp((const s8*)iter.buf, (const s8*)"..", 2) == 0) {
            break;
        }
        err = PFENT_ITER_Retreat(&iter, 0);
        if (err != 0) {
            return err;
        }
        if (start_cluster == cluster) {
            *p_is_found = 1;
            return 0;
        }
        if (start_cluster <= 2) {
            return 0;
        }
        if (iter.index < 1) {
            hint.chain_index++;
            iter.file_sector_index += p_ent->p_vol->bpb.sectors_per_cluster;
            iter.index += (1 << iter.log2_entries_per_sector) << p_ent->p_vol->bpb.log2_sectors_per_cluster;
        }
    }

    start_cluster = (GET_U16_LE(&iter.buf[20]) << 16) | GET_U16_LE(&iter.buf[26]);
    hint.chain_index = 0;
    iter.file_sector_index = 0;
    hint.cluster = start_cluster;
    iter.sector = p_ent->p_vol->bpb.first_data_sector + ((start_cluster - 2) << p_ent->p_vol->bpb.log2_sectors_per_cluster);
    iter.offset = 32;
    iter.index = 1;
    while (start_cluster > 2) {
        err = PFENT_ITER_LoadEntry(&iter);
        if (err != 0) {
            return err;
        }
        if (pf_strncmp((const s8*)iter.buf, (const s8*)"..", 2) == 0) {
            if (start_cluster == cluster) {
                *p_is_found = 1;
                return 0;
            }
            start_cluster = (GET_U16_LE(&iter.buf[20]) << 16) | GET_U16_LE(&iter.buf[26]);
            hint.cluster = start_cluster;
            iter.sector = p_ent->p_vol->bpb.first_data_sector + ((start_cluster - 2) << p_ent->p_vol->bpb.log2_sectors_per_cluster);
        } else {
            return 0;
        }
    }
    return 0;
}

s32 PFENT_ITER_Retreat(PF_ENT_ITER* p_iter, u32 may_allocate)
{
    u32 before_cluster;
    u32 cur_cluster;
    s32 err;

    if (p_iter->index == 0) {
        return 0;
    }
    if (p_iter->file_sector_index != (p_iter->index - 1) >> p_iter->log2_entries_per_sector) {
        cur_cluster = ((p_iter->sector - p_iter->p_vol->bpb.first_data_sector) >> p_iter->p_vol->bpb.log2_sectors_per_cluster) + 2;
        if (p_iter->file_sector_index % p_iter->p_vol->bpb.sectors_per_cluster == 0 || p_iter->p_vol->bpb.sectors_per_cluster == 1) {
            if (p_iter->p_vol->bpb.fat_type == PDM_FAT_32 ||
                (*p_iter->ffd.p_start_cluster > 1 && p_iter->p_vol->bpb.first_data_sector <= p_iter->sector))
            {
                err = PFFAT_GetBeforeChain(p_iter->p_vol, cur_cluster, cur_cluster, &before_cluster);
                if (err != 0) {
                    return err;
                }
                if (before_cluster == 0xFFFFFFFF) {
                    return 14;
                }
            } else {
                before_cluster = *p_iter->ffd.p_start_cluster;
            }
        } else {
            before_cluster = cur_cluster;
        }

        p_iter->index--;
        p_iter->file_sector_index--;

        if (p_iter->p_vol->bpb.fat_type == PDM_FAT_32 ||
            (*p_iter->ffd.p_start_cluster > 1 && p_iter->p_vol->bpb.first_data_sector <= p_iter->sector))
        {
            p_iter->sector = p_iter->p_vol->bpb.first_data_sector + ((before_cluster - 2) << p_iter->p_vol->bpb.log2_sectors_per_cluster) +
                             (p_iter->file_sector_index & (p_iter->p_vol->bpb.sectors_per_cluster - 1));
        } else {
            p_iter->sector--;
        }

        p_iter->offset = (p_iter->index & p_iter->offset_mask) << 5;
        return 0;
    }
    return PFENT_ITER_MoveTo(p_iter, p_iter->index - 1, may_allocate);
}

s32 PFENT_ITER_FindCluster(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, u32 cluster, u32* p_is_found)
{
    u8 attr;
    s32 err;
    s32 len;
    s8 filename[13];

    err = 0;
    *p_is_found = 0;
    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;
    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 1) {
        return 2;
    }

    while (!PFENT_ITER_IsAtLogicalEnd(p_iter)) {
        if (err != 0) {
            return err;
        }
        if (p_iter->buf[0] == PF_ENT_DELETED) {
            p_ent->num_entry_LFNs = 0;
            p_ent->long_name[0] = 0;
        } else {
            attr = p_iter->buf[11];
            if ((attr & PF_ATTR_LFN) == PF_ATTR_LFN) {
                if (PFENT_LoadLFNEntryFieldsFromBuf(p_ent, p_iter->buf) != 0) {
                    p_ent->num_entry_LFNs = 0;
                    p_ent->long_name[0] = 0;
                }
            } else if (attr & PF_ATTR_VOLUME) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;
            } else if (!(attr & PF_ATTR_DIR)) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;
            } else {
                PFENT_LoadShortNameFromBuf(p_ent, p_iter->buf);
                if (p_ent->num_entry_LFNs != 0 && p_ent->ordinal == 1 && p_ent->check_sum == PFENT_CalcCheckSum(p_ent)) {
                    PFENT_loadEntryNumericFieldsFromBuf(p_ent, p_iter->buf);
                    p_ent->entry_sector = p_iter->sector;
                    p_ent->entry_offset = p_iter->offset;
                    p_ent->p_vol = p_iter->ffd.p_vol;
                    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster != cluster) {
                        p_ent->num_entry_LFNs = 0;
                        p_ent->long_name[0] = 0;
                        goto next_entry;
                    }
                    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 0) {
                        p_ent->start_cluster = 1;
                    }
                    *p_is_found = 1;
                }
                if (p_iter->buf[12] & 0x18) {
                    VFiPFPATH_getLongNameformShortName(p_ent->short_name, filename, p_iter->buf[12]);
                    len = VFiPFPATH_transformInUnicode(p_ent->long_name, filename);
                    p_ent->num_entry_LFNs = len / 13 + (len % 13 != 0);
                    p_ent->check_sum = PFENT_CalcCheckSum(p_ent);
                    p_ent->ordinal = 1;
                }
                PFENT_loadEntryNumericFieldsFromBuf(p_ent, p_iter->buf);
                p_ent->entry_sector = p_iter->sector;
                p_ent->entry_offset = p_iter->offset;
                p_ent->p_vol = p_iter->ffd.p_vol;
                if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster != cluster) {
                    p_ent->num_entry_LFNs = 0;
                    p_ent->long_name[0] = 0;
                    *p_is_found = 0;
                } else {
                    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 0) {
                        p_ent->start_cluster = 1;
                    }
                    *p_is_found = 1;
                    return 0;
                }
            }
        }
    next_entry:
        err = PFENT_ITER_Advance(p_iter, 0);
    }
    return 0;
}

s32 PFENT_ITER_FindEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded, u32* p_is_found)
{
    s32 err;

    *p_is_found = 0;
    if (attr_required & attr_excluded) {
        return 10;
    }

    if (PFSTR_StrNCmp(p_pattern, (const s8*)":", 1, 1, 1) == 0) {
        PFSTR_MoveStrPos(p_pattern, 2);
    }

    if (PFSTR_StrNCmp(p_pattern, (const s8*)"\\", 1, 0, 1) == 0 || PFSTR_StrNCmp(p_pattern, (const s8*)"/", 1, 0, 1) == 0) {
        err = PFENT_GetRootDir(p_iter->p_vol, p_ent);
        if (err != 0) {
            return err;
        }
        if (PFSTR_StrNumChar(p_pattern, 1) == 1 && PFSTR_StrNCmp(p_pattern, (const s8*)"\0", 2, 0, 1) == 0) {
            return 0;
        }
    } else {
        err = PFVOL_GetCurrentDir(p_iter->p_vol, p_ent);
        if (err != 0) {
            return err;
        }
    }

    err = PFENT_ITER_DoFindEntry(p_iter, p_ent, p_pattern, attr_required, attr_excluded, p_is_found);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFENT_ITER_FindEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded, u32* p_is_found)
{
    s32 err;

    *p_is_found = 0;
    if (attr_required & attr_excluded) {
        return 10;
    }
    err = PFENT_ITER_DoFindEntry(p_iter, p_ent, p_pattern, attr_required, attr_excluded, p_is_found);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFENT_GetEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path, u32 is_parent)
{
    return PFENT_LookupPath(p_iter, p_ent, p_vol, p_path, 0, is_parent);
}

s32 PFENT_GetParentEntryOfPath(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent, PF_VOLUME* p_vol, PF_STR* p_path)
{
    return PFENT_LookupPath(p_iter, p_ent, p_vol, p_path, 1, 1);
}

s32 PFENT_ITER_GetEntry(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent)
{
    PFENT_LoadShortNameFromBuf(p_ent, p_iter->buf);
    PFENT_loadEntryNumericFieldsFromBuf(p_ent, p_iter->buf);
    p_ent->entry_sector = p_iter->sector;
    p_ent->entry_offset = p_iter->offset;
    p_ent->p_vol = p_iter->p_vol;
    return 0;
}

s32 PFENT_ITER_LoadLongName(PF_ENT_ITER* p_iter, PF_DIR_ENT* p_ent)
{
    u32 i;
    u32 pos;
    s32 err;

    if (p_ent->num_entry_LFNs != 0 && !(p_ent->small_letter_flag & 0x18)) {
        for (i = 0; i < p_ent->num_entry_LFNs; i++) {
            err = PFENT_ITER_Retreat(p_iter, 0);
            if (err != 0) {
                return err;
            }
            err = PFENT_ITER_LoadEntry(p_iter);
            if (err != 0) {
                return err;
            }
            pos = i * 26;
            pf_memcpy((u8*)p_ent->long_name + pos, &p_iter->buf[1], 10);
            pf_memcpy((u8*)p_ent->long_name + pos + 10, &p_iter->buf[14], 12);
            pf_memcpy((u8*)p_ent->long_name + pos + 22, &p_iter->buf[28], 4);
        }
        p_ent->long_name[p_ent->num_entry_LFNs * 26 / sizeof(u16)] = 0;
    }
    return 0;
}
