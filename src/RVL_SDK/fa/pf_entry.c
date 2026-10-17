#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_cache.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_code.h>
#include <revolution/fa/pf_entry_iterator.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_sector.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_system.h>

#define GET_U16_LE(p) ((u16)(((*(u16*)(p) >> 8) & 0xFF) | ((*(u16*)(p) & 0xFF) << 8)))
#define GET_U32_LE(p) ((((*(u32*)(p)) << 24) | (((*(u32*)(p)) << 8) & 0xFF0000)) | (((*(u32*)(p)) >> 24) | (((*(u32*)(p)) >> 8) & 0xFF00)))
#define SET_U16_LE(p, val) (*(u16*)(p) = (u16)((((val) >> 8) & 0xFF) | (((val) & 0xFF) << 8)))
#define SET_U32_LE(p, val) (*(u32*)(p) = ((((u32)(val)) << 24) | ((((u32)(val)) << 8) & 0xFF0000)) | ((((u32)(val)) >> 24) | ((((u32)(val)) >> 8) & 0xFF00)))

s32 PFENT_IsNotMatchedEntry(PF_DIR_ENT* p_ent, const u8* buf, PF_STR* p_pattern, u8 attr, u8 attr_required, u8 attr_excluded,
                            u32* p_num_skipped)
{
    u32 is_volume;
    s32 result;
    u32 is_attr_matched;

    result = 1;
    is_attr_matched = 1;
    if ((attr & PF_ATTR_LFN) == PF_ATTR_LFN) {
        result = 1;
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
                is_attr_matched = 0;
            }
        } else if (attr_required != 0x7F && attr != attr_required && (!(attr & attr_required) || (attr & attr_excluded))) {
            is_attr_matched = 0;
        }

        if (!is_attr_matched) {
            result = 1;
        } else {
            is_volume = attr & PF_ATTR_VOLUME;
            if (is_volume) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;
            }
            PFENT_LoadShortNameFromBuf(p_ent, buf);

            if (p_ent->num_entry_LFNs != 0 && p_ent->ordinal == 1 && p_ent->check_sum == PFENT_CalcCheckSum(p_ent)) {
                result = PFPATH_CmpLongName(p_ent->long_name, p_pattern) != 0;
            }

            if (result == 1) {
                if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE && !is_volume) {
                    if (PFPATH_CmpShortName(p_ent->short_name, p_pattern, 0) == 0) {
                        result = 0;
                    }
                } else if (PFPATH_CmpShortName(p_ent->short_name, p_pattern, 1) == 0) {
                    result = 0;
                }
            }

            if (result == 1) {
                (*p_num_skipped)++;
            }
        }
    }
    return result;
}

s32 PFENT_LoadMatchedEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded,
                           u32* p_num_skipped)
{
    s32 len;
    u8 attr;
    s8 long_name[13];

    attr = p_iter->buf[11];
    if (PFENT_IsNotMatchedEntry(p_ent, p_iter->buf, p_pattern, attr, attr_required, attr_excluded, p_num_skipped) == 1) {
        if ((attr & PF_ATTR_LFN) == PF_ATTR_LFN) {
            if (PFENT_LoadLFNEntryFieldsFromBuf(p_ent, p_iter->buf) != 0) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;
            }
        } else {
            p_ent->num_entry_LFNs = 0;
            p_ent->long_name[0] = 0;
        }
        return -1;
    }

    if (p_ent->num_entry_LFNs == 0 && (p_iter->buf[12] & 0x18)) {
        VFiPFPATH_getLongNameformShortName(p_ent->short_name, long_name, p_iter->buf[12]);
        len = VFiPFPATH_transformInUnicode(p_ent->long_name, long_name);
        p_ent->num_entry_LFNs = len / 13 + (len % 13 != 0 ? 1 : 0);
        p_ent->check_sum = PFENT_CalcCheckSum(p_ent);
        p_ent->ordinal = 1;
    }

    PFENT_loadEntryNumericFieldsFromBuf(p_ent, p_iter->buf);
    p_ent->entry_sector = p_iter->sector;
    p_ent->entry_offset = p_iter->offset;
    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 0) {
        p_ent->start_cluster = 1;
    }
    return 0;
}

s32 PFENT_searchEmptyTailSFN(PF_FFD* p_ffd, u32 tail_index, const s8* pattern, u32* p_tail_bit)
{
    s8 sfnbuf[13];
    s8 patbuf[13];
    s32 err;
    u32 sfn_taillen;
    u32 sfn_baselen;
    u32 i;
    u32 bit_pos;
    PF_ENT_ITER iter;
    PF_VOLUME* p_vol;

    p_vol = p_ffd->p_vol;
    pf_memset(p_tail_bit, 0, p_vol->tail_entry.tracker_size * 4);
    iter.ffd = *p_ffd;

    for (err = PFENT_ITER_IteratorInitialize(&iter, 0); !PFENT_ITER_IsAtLogicalEnd(&iter); err = PFENT_ITER_Advance(&iter, 0)) {
        if (err != 0) {
            return err;
        }
        if (iter.buf[0] == PF_ENT_END) {
            break;
        }
        if (iter.buf[0] != PF_ENT_DELETED && (iter.buf[11] & PF_ATTR_LFN) != PF_ATTR_LFN && !(iter.buf[11] & PF_ATTR_VOLUME)) {
            VFiPFPATH_getShortName(sfnbuf, iter.buf, 0);
            for (sfn_taillen = 1; sfnbuf[sfn_taillen] != '~' && sfnbuf[sfn_taillen] != '\0' && sfn_taillen < 7; sfn_taillen++) {
            }
            if (sfn_taillen < 7 && sfnbuf[sfn_taillen] == '~') {
                for (sfn_baselen = sfn_taillen + 1; sfnbuf[sfn_baselen] >= '0' && sfnbuf[sfn_baselen] <= '9'; sfn_baselen++) {
                }
                if (sfnbuf[sfn_baselen] == '.' || sfnbuf[sfn_baselen] == '\0') {
                    i = sfn_baselen - sfn_taillen - 1;
                    bit_pos = 0;
                    while (i != 0) {
                        bit_pos *= 10;
                        bit_pos += sfnbuf[sfn_baselen - i] - '0';
                        i--;
                    }
                    pf_strcpy(patbuf, pattern);
                    VFiPFPATH_parseShortNameNumeric(patbuf, bit_pos);
                    if (VFiPFPATH_cmpTailSFN(sfnbuf, patbuf) == 0 && bit_pos >= tail_index &&
                        bit_pos < tail_index + p_vol->tail_entry.tracker_size * 32)
                    {
                        bit_pos -= tail_index;
                        p_tail_bit[bit_pos >> 5] |= 1 << bit_pos;
                    }
                }
            }
        }
    }
    return 0;
}

s32 PFENT_findEmptyTailSFN(PF_DIR_ENT* p_ent_containing_dir, const s8* name, u32* p_tails)
{
    PF_FAT_HINT hint;
    PF_FFD ffd;
    PF_VOLUME* p_vol;
    u32 num;
    s32 err;
    u32 track_num;

    p_vol = p_ent_containing_dir->p_vol;
    *p_tails = 1;
    PFFAT_InitFFD(&ffd, &hint, p_ent_containing_dir->p_vol, &p_ent_containing_dir->start_cluster);
    for (num = 1; num <= 999999; num += p_vol->tail_entry.tracker_size * 32) {
        err = PFENT_searchEmptyTailSFN(&ffd, num, name, p_vol->tail_entry.tracker_bits);
        if (err != 0) {
            return err;
        }
        for (track_num = 0; track_num < p_vol->tail_entry.tracker_size; track_num++) {
            if (p_vol->tail_entry.tracker_bits[track_num] != 0xFFFFFFFF) {
                while (p_vol->tail_entry.tracker_bits[track_num] & 1) {
                    p_vol->tail_entry.tracker_bits[track_num] >>= 1;
                    (*p_tails)++;
                }
                num = 1000000;
                break;
            }
            *p_tails += 32;
        }
    }
    return 0;
}

u8 PFENT_CalcCheckSum(PF_DIR_ENT* p_ent)
{
    u8 name[13];
    u8 sum;
    u16 i;

    VFiPFPATH_putShortName(name, p_ent->short_name, 0);
    sum = 0;
    for (i = 0; i < 11; i++) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + name[i];
    }
    return sum;
}

void PFENT_LoadShortNameFromBuf(PF_DIR_ENT* p_ent, const u8* buf)
{
    VFiPFPATH_getShortName(p_ent->short_name, buf, buf[11]);
    if (p_ent->short_name[0] == 0x05) {
        p_ent->short_name[0] = PF_ENT_DELETED;
    }
}

void PFENT_loadEntryNumericFieldsFromBuf(PF_DIR_ENT* p_ent, const u8* buf)
{
    p_ent->attr = buf[11];
    p_ent->small_letter_flag = buf[12];
    p_ent->create_time_ms = buf[13];
    p_ent->create_time = GET_U16_LE(&buf[14]);
    p_ent->create_date = GET_U16_LE(&buf[16]);
    p_ent->access_date = GET_U16_LE(&buf[18]);
    p_ent->modify_time = GET_U16_LE(&buf[22]);
    p_ent->modify_date = GET_U16_LE(&buf[24]);
    p_ent->file_size = GET_U32_LE(&buf[28]);
    p_ent->start_cluster = (GET_U16_LE(&buf[20]) << 16) | GET_U16_LE(&buf[26]);
}

void PFENT_StoreEntryNumericFieldsToBuf(u8* buf, const PF_DIR_ENT* p_ent)
{
    buf[11] = p_ent->attr;
    buf[12] = p_ent->small_letter_flag;
    buf[13] = p_ent->create_time_ms;
    SET_U16_LE(&buf[14], p_ent->create_time);
    SET_U16_LE(&buf[16], p_ent->create_date);
    SET_U16_LE(&buf[18], p_ent->access_date);
    SET_U16_LE(&buf[22], p_ent->modify_time);
    SET_U16_LE(&buf[24], p_ent->modify_date);
    SET_U16_LE(&buf[20], (u16)(p_ent->start_cluster >> 16));
    SET_U16_LE(&buf[26], (u16)p_ent->start_cluster);
    SET_U32_LE(&buf[28], p_ent->file_size);
}

s32 PFENT_LoadLFNEntryFieldsFromBuf(PF_DIR_ENT* p_ent, const u8* buf)
{
    u8 ordinal;
    u8 check_sum;
    u32 is_last_LFN_ent;
    u8* p;
    u16* q;
    u16* q_after;

    check_sum = buf[13];
    ordinal = buf[0];

    if ((u8)(ordinal & ~0x40) > 20) {
        p_ent->ordinal = 0;
        p_ent->check_sum = 0;
        p_ent->num_entry_LFNs = 0;
        return 33;
    }

    if (ordinal & 0x40) {
        p_ent->num_entry_LFNs = 0;
        ordinal &= ~0x40;
        is_last_LFN_ent = 1;
    } else {
        is_last_LFN_ent = 0;
        if (p_ent->num_entry_LFNs == 0) {
            return 33;
        }
        if (ordinal != p_ent->ordinal - 1 || check_sum != p_ent->check_sum) {
            p_ent->ordinal = 0;
            p_ent->check_sum = 0;
            p_ent->num_entry_LFNs = 0;
            return 33;
        }
    }

    p_ent->ordinal = ordinal;
    p_ent->check_sum = check_sum;

    p = (u8*)p_ent->long_name + (ordinal - 1) * 26;

    pf_memcpy(p, buf + 1, 10);
    pf_memcpy(p + 10, buf + 14, 12);
    pf_memcpy(p + 22, buf + 28, 4);

    PF_LE16_TO_U16_STR(p, 10);
    PF_LE16_TO_U16_STR(p + 10, 12);
    PF_LE16_TO_U16_STR(p + 22, 4);

    if (is_last_LFN_ent) {
        ((u16*)p)[13] = 0;
        q = (u16*)p;
        q_after = (u16*)p + 13;

        while (q < q_after) {
            if (*q == 0x0000) {
                q++;
                break;
            }
            q++;
        }

        while (q < q_after) {
            if (*q != 0xFFFF) {
                p_ent->num_entry_LFNs = 0;
                return 33;
            }
            q++;
        }
    }

    p_ent->num_entry_LFNs++;
    return 0;
}

void PFENT_storeLFNEntryFieldsToBuf(u8* buf, PF_DIR_ENT* p_ent, u8 ord, u8 sum, u32 is_last)
{
    u8* p_seg;
    u16* p;
    u16* p_after;

    if (is_last) {
        buf[0] = ord | 0x40;
    } else {
        buf[0] = ord;
    }

    buf[11] = PF_ATTR_LFN;
    buf[13] = sum;
    buf[12] = 0;
    *(u16*)&buf[26] = 0;

    p_seg = (u8*)p_ent->long_name + (u32)(ord - 1) * 26;

    if (is_last) {
        p = (u16*)p_seg;
        p_after = (u16*)(p_seg + 26);

        while (p < p_after) {
            if (*p == 0) {
                p++;
                break;
            }
            p++;
        }

        while (p < p_after) {
            *p++ = 0xFFFF;
        }
    }

    pf_memcpy(buf + 1, p_seg, 10);
    pf_memcpy(buf + 14, p_seg + 10, 12);
    pf_memcpy(buf + 28, p_seg + 22, 4);

    PF_LE16_TO_U16_STR(buf + 1, 10);
    PF_LE16_TO_U16_STR(buf + 14, 12);
    PF_LE16_TO_U16_STR(buf + 28, 4);
}

s32 PFENT_findEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded,
                    u32* p_lpos, u32* p_ppos)
{
    s32 err;
    u32 is_index_search;
    u32 num_skipped;
    PF_ENT_ITER iter;

    is_index_search = 0;
    *p_lpos = 0;
    *p_ppos = 0;
    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;
    p_ent->long_name[0] = 0;
    num_skipped = 0;

    if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
        is_index_search = PFPATH_ParseEntryIndex(p_pattern, &index_search_from);
    }

    iter.ffd = *p_ffd;

    if (is_index_search == 1) {
        err = PFENT_ITER_IteratorInitialize(&iter, index_search_from - 1);
        if (err != 0) {
            *p_lpos = 999999;
            *p_ppos = 999999;
            return err;
        }
        if (PFENT_LoadMatchedEntry(p_ent, &iter, p_pattern, attr_required, attr_excluded, &num_skipped) == 0) {
            *p_lpos = 999999;
            *p_ppos = 999999;
            return 3;
        }
        if (iter.buf[0] & 0x40) {
            err = PFENT_ITER_IteratorInitialize(&iter, index_search_from);
            if (err != 0) {
                *p_lpos = 999999;
                *p_ppos = 999999;
                return err;
            }
            if (PFENT_LoadMatchedEntry(p_ent, &iter, p_pattern, attr_required, attr_excluded, &num_skipped) == 0) {
                p_ent->p_vol = p_ffd->p_vol;
                *p_lpos = num_skipped;
                *p_ppos = iter.index;
                return 0;
            }
        }
    } else {
        for (err = PFENT_ITER_IteratorInitialize(&iter, index_search_from); !PFENT_ITER_IsAtLogicalEnd(&iter);
             err = PFENT_ITER_Advance(&iter, 0))
        {
            if (err != 0) {
                *p_lpos = 999999;
                *p_ppos = 999999;
                return err;
            }
            if (iter.buf[0] == PF_ENT_END) {
                break;
            }
            if (iter.buf[0] == PF_ENT_DELETED) {
                p_ent->num_entry_LFNs = 0;
                p_ent->long_name[0] = 0;
            } else if (PFENT_LoadMatchedEntry(p_ent, &iter, p_pattern, attr_required, attr_excluded, &num_skipped) == 0) {
                p_ent->p_vol = p_ffd->p_vol;
                *p_lpos = num_skipped;
                *p_ppos = iter.index;
                return 0;
            }
        }
    }

    *p_lpos = 999999;
    *p_ppos = 999999;
    return 3;
}

s32 PFENT_SearchEntry(PF_FFD* p_ffd, PF_DIR_ENT* p_ent, u32 index_search_from, PF_STR* p_pattern, u8 attr_required, u8 attr_excluded)
{
    u32 lpos;
    u32 ppos;

    return PFENT_findEntry(p_ffd, p_ent, index_search_from, p_pattern, attr_required, attr_excluded, &lpos, &ppos);
}

s32 PFENT_allocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_prev_chain, PF_STR* p_filename, u32* p_pos)
{
    PF_VOLUME* p_vol;
    u32 run_sector;
    u16 run_offset;
    u32 sector_c;
    u32 sector_b;
    u32 sector_a;
    u32 is_found;
    u32 run_count;
    u32 pos;
    s32 err;
    u32 sector;
    u32 num_skipped;
    PF_ENT_ITER iter;
    PF_DIR_ENT wk_ent;

    if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE && PFPATH_CheckExtShortNameSignature(p_filename) == 1) {
        return 1;
    }

    p_vol = p_ffd->p_vol;
    run_sector = 0xFFFFFFFF;
    run_count = 0;
    pos = 0;
    run_offset = 0;
    sector_c = 0xFFFFFFFF;
    sector_a = 0xFFFFFFFF;
    sector_b = 0xFFFFFFFF;
    is_found = 0;
    p_prev_chain[1] = 0xFFFFFFFF;
    p_prev_chain[0] = 0xFFFFFFFF;
    iter.sector = 0;
    iter.index = 0;

    iter.ffd = *p_ffd;
    err = PFENT_ITER_IteratorInitialize(&iter, 0);

    for (;;) {
        if (err != 0) {
            if (err == 16) {
                break;
            }
            return err;
        }
        if (iter.sector == 0xFFFFFFFF) {
            break;
        }
        if (!is_found && run_count == 0) {
            run_offset = iter.offset;
            run_sector = iter.sector;
            sector_c = iter.sector;
        }
        if (!is_found) {
            if (iter.buf[0] == PF_ENT_END || iter.buf[0] == PF_ENT_DELETED) {
                if (sector_c != iter.sector) {
                    if (sector_b == 0xFFFFFFFF) {
                        sector_b = iter.sector;
                    } else {
                        sector_a = iter.sector;
                    }
                    sector_c = iter.sector;
                }
                run_count++;
            } else {
                run_count = 0;
                sector_b = 0xFFFFFFFF;
                sector_a = 0xFFFFFFFF;
            }
        }
        if (run_count >= num_entries) {
            if (!is_found) {
                pos = iter.index;
                is_found = 1;
            }
            if (iter.buf[0] == PF_ENT_END) {
                break;
            }
        }
        if (iter.buf[0] != PF_ENT_END && iter.buf[0] != PF_ENT_DELETED &&
            PFENT_LoadMatchedEntry(&wk_ent, &iter, p_filename, 0x7F, 0, &num_skipped) == 0)
        {
            *p_ent = wk_ent;
            p_ent->p_vol = p_ffd->p_vol;
            return 8;
        }
        if (iter.offset + 32 == p_vol->bpb.bytes_per_sector) {
            err = PFFAT_GetSectorSpecified(p_ffd, iter.file_sector_index + 1, 0, &sector);
            if (err != 0) {
                return err;
            }
            if (sector == 0xFFFFFFFF && is_found == 1) {
                break;
            }
        }
        err = PFENT_ITER_Advance(&iter, 1);
    }

    if (!is_found) {
        return 5;
    }
    p_prev_chain[0] = sector_b;
    p_prev_chain[1] = sector_a;
    p_ent->entry_sector = run_sector;
    p_ent->entry_offset = run_offset;
    *p_pos = pos;
    return 0;
}

s32 PFENT_AllocateEntry(PF_DIR_ENT* p_ent, u8 num_entries, PF_FFD* p_ffd, u32* p_prev_chain, PF_STR* p_filename)
{
    u32 pos;

    return PFENT_allocateEntry(p_ent, num_entries, p_ffd, p_prev_chain, p_filename, &pos);
}

s32 PFENT_GetRootDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent)
{
    p_ent->long_name[0] = '\\';
    p_ent->long_name[1] = 0;

    p_ent->num_entry_LFNs = 0;
    p_ent->ordinal = 0;
    p_ent->check_sum = 0;

    p_ent->short_name[0] = '\\';
    p_ent->short_name[1] = 0;
    p_ent->small_letter_flag = 0;

    p_ent->attr = PF_ATTR_DIR;

    p_ent->create_time_ms = 0;
    p_ent->create_time = 0;
    p_ent->create_date = 0;
    p_ent->access_date = 0;
    p_ent->modify_time = 0;
    p_ent->modify_date = 0;
    p_ent->file_size = 0;

    p_ent->p_vol = p_vol;
    p_ent->path_len = 3;
    p_ent->start_cluster = 1;
    p_ent->entry_sector = 0xFFFFFFFF;
    p_ent->entry_offset = 0;

    return 0;
}

s32 PFENT_MakeRootDir(PF_VOLUME* p_vol)
{
    s32 err;
    u32 sector;
    PF_CACHE_PAGE* p_page;
    u32 success_size;
    u32 is_hit;

    switch (p_vol->bpb.fat_type) {
    case PDM_FAT_32:
        PFFAT_MakeRootDir(p_vol);
        break;
    case PDM_FAT_12:
    case PDM_FAT_16:
        err = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &is_hit);
        if (err != 0) {
            return err;
        }
        pf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
        for (sector = p_vol->bpb.first_root_dir_sector; sector < p_vol->bpb.first_data_sector; sector++) {
            err = PFSEC_WriteData(p_vol, p_page->p_buf, sector, 0, p_vol->bpb.bytes_per_sector, &success_size, 0);
            if (err != 0) {
                PFCACHE_FreeDataPage(p_vol, p_page);
                return err;
            }
            if (success_size != p_vol->bpb.bytes_per_sector) {
                PFCACHE_FreeDataPage(p_vol, p_page);
                return 17;
            }
        }
        PFCACHE_FreeDataPage(p_vol, p_page);
        break;
    default:
        return 7;
    }

    PFVOL_LoadVolumeLabelFromBuf(p_vol, (const u8*)"NO NAME    ");
    return 0;
}

static void PFENT_storeShortNameToBuf(u8* buf, const PF_DIR_ENT* p_ent)
{
    VFiPFPATH_putShortName(buf, p_ent->short_name, p_ent->attr);
    if (buf[0] == PF_ENT_DELETED) {
        buf[0] = 0x05;
    }
}

s32 PFENT_UpdateSFNEntry(PF_DIR_ENT* p_ent, u32 flag)
{
    PF_VOLUME* p_vol;
    s32 err;
    u32 success_size;
    u8 buf[32];

    p_vol = p_ent->p_vol;

    if (p_ent->start_cluster == 1) {
        return 14;
    }

    if (flag == 1) {
        p_ent->attr |= PF_ATTR_ARCHIVE;
    }

    PFENT_storeShortNameToBuf(buf, p_ent);
    PFENT_StoreEntryNumericFieldsToBuf(buf, p_ent);

    err = PFSEC_WriteData(p_vol, buf, p_ent->entry_sector, p_ent->entry_offset, 32, &success_size, 0);
    if (err != 0) {
        return err;
    }

    if (success_size != 32) {
        return 17;
    }
    return 0;
}

s32 PFENT_AdjustSFN(PF_DIR_ENT* p_ent, s8* p_short_name)
{
    u32 i;
    u32 tail_num;
    s32 err;

    for (i = 1; p_short_name[i] != '~' && p_short_name[i] != '\0' && i < 7; i++) {
    }

    if (i < 7 && p_short_name[i] == '~') {
        for (i = i + 1; p_short_name[i] >= '0' && p_short_name[i] <= '9'; i++) {
        }

        if (p_short_name[i] == '.' || p_short_name[i] == '\0') {
            err = PFENT_findEmptyTailSFN(p_ent, p_short_name, &tail_num);
            if (err != 0) {
                return err;
            }

            if (tail_num != 1) {
                VFiPFPATH_parseShortNameNumeric(p_short_name, tail_num);
            }
        }
    }
    return 0;
}

u8 PFENT_getcurrentDateTimeForEnt(u16* p_date, u16* p_time)
{
    PF_SYS_DATE sys_date;
    PF_SYS_TIME sys_time;

    PFSYS_TimeStamp(&sys_date, &sys_time);

    *p_date = (sys_date.sys_day & 0x1F) | ((sys_date.sys_month & 0x0F) << 5) | (((sys_date.sys_year - 1980) & 0x7F) << 9);
    *p_time = ((sys_time.sys_sec >> 1) & 0x1F) | ((sys_time.sys_min & 0x3F) << 5) | ((sys_time.sys_hour & 0x1F) << 11);

    return sys_time.sys_ms;
}

s32 PFENT_RemoveEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter)
{
    u32 success_size;
    s32 err;
    u32 i;
    PF_VOLUME* p_vol;
    u8 dir_fb_free[1] = {PF_ENT_DELETED};

    p_vol = p_ent->p_vol;

    if (p_ent->num_entry_LFNs != 0 && !(p_ent->small_letter_flag & 0x18)) {
        for (i = 1; i <= p_ent->num_entry_LFNs; i++) {
            err = PFENT_ITER_Retreat(p_iter, 0);
            if (err != 0) {
                return err;
            }

            err = PFSEC_WriteData(p_vol, dir_fb_free, p_iter->sector, p_iter->offset, 1, &success_size, 0);
            if (err != 0) {
                return err;
            }

            if (success_size != 1) {
                return 17;
            }
        }
    }

    err = PFSEC_WriteData(p_vol, dir_fb_free, p_ent->entry_sector, p_ent->entry_offset, 1, &success_size, 0);
    if (err != 0) {
        return err;
    }

    if (success_size != 1) {
        return 17;
    }

    err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
    if (err != 0) {
        return err;
    }
    return 0;
}
