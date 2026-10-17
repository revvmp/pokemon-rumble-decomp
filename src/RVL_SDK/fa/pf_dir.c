#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_entry_iterator.h>
#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_sector.h>

s32 PFDIR_IsEmptyDirectory(PF_ENT_ITER* p_iter, u32* p_is_empty)
{
    s32 err;
    u32 index;

    if (*p_iter->ffd.p_start_cluster == 1 ||
        (p_iter->ffd.p_vol->bpb.fat_type == PDM_FAT_32 &&
         *p_iter->ffd.p_start_cluster == p_iter->ffd.p_vol->bpb.root_dir_cluster))
    {
        index = 0;
    } else {
        index = 2;
    }

    *p_is_empty = 1;
    for (err = PFENT_ITER_MoveTo(p_iter, index, 0); !PFENT_ITER_IsAtLogicalEnd(p_iter); err = PFENT_ITER_Advance(p_iter, 0)) {
        if (err != 0) {
            return err == 13 ? 0 : err;
        }
        if (p_iter->buf[0] != PF_ENT_DELETED && (p_iter->buf[11] & PF_ATTR_LFN) != PF_ATTR_LFN &&
            !(p_iter->buf[11] & PF_ATTR_VOLUME))
        {
            if (p_iter->buf[0] == PF_ENT_END) {
                *p_is_empty = 1;
            } else {
                *p_is_empty = 0;
            }
            break;
        }
    }
    return 0;
}

PF_SDD* PFDIR_GetSDD(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent)
{
    PF_SDD* p_free;
    u32 i;

    p_free = NULL;
    for (i = 0; i < PF_MAX_DIRS; i++) {
        if (!(p_vol->sdds[i].stat & PF_SDD_STAT_ALLOCATED) || !p_vol->sdds[i].stat & PF_SDD_STAT_OPENED) {
            if (p_free == NULL) {
                p_free = &p_vol->sdds[i];
            }
        } else if (p_ent->p_vol == p_vol->sdds[i].dir_entry.p_vol &&
                   p_ent->entry_sector == p_vol->sdds[i].dir_entry.entry_sector &&
                   p_ent->entry_offset == p_vol->sdds[i].dir_entry.entry_offset)
        {
            return &p_vol->sdds[i];
        }
    }

    if (p_free == NULL) {
        return NULL;
    }
    p_free->stat = PF_SDD_STAT_ALLOCATED | PF_SDD_STAT_OPENED;
    p_free->num_handlers = 0;
    p_free->dir_entry = *p_ent;
    PFFAT_InitFFD(&p_free->ffd, NULL, p_ent->p_vol, &p_free->dir_entry.start_cluster);
    return p_free;
}

static PF_DIR* PFDIR_GetFreeUDD(PF_VOLUME* p_vol)
{
    u32 i;

    for (i = 0; i < PF_MAX_DIRS; i++) {
        if (!(p_vol->udds[i].stat & PF_DIR_STAT_ALLOCATED)) {
            return &p_vol->udds[i];
        }
    }
    return NULL;
}

s32 PFDIR_OpenDirEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, PF_DIR** pp_dir)
{
    PF_VOLUME* p_vol;
    PF_SDD* p_sdd;
    PF_DIR* p_dir;

    p_vol = p_ent->p_vol;
    *pp_dir = NULL;
    if (!(p_ent->attr & PF_ATTR_DIR)) {
        return 20;
    }
    p_sdd = PFDIR_GetSDD(p_vol, p_ent);
    if (p_sdd == NULL) {
        return 21;
    }

    p_dir = PFDIR_GetFreeUDD(p_vol);
    if (p_dir == NULL) {
        return 22;
    }

    p_dir->p_sdd = p_sdd;
    p_dir->stat = PF_DIR_STAT_ALLOCATED;
    PFFAT_InitHint(&p_dir->hint);
    p_dir->cursor.physical_entry_index = 0;
    p_dir->cursor.logical_entry_index = 0;
    p_dir->cursor.logical_seek_index = 0;
    p_sdd->num_handlers++;
    *pp_dir = p_dir;
    return 0;
}

s32 PFDIR_p_opendir(PF_VOLUME* p_vol, PF_STR* p_path, PF_DIR** pp_dir)
{
    s32 err;
    PF_SDD* p_sdd;
    PF_DIR* p_dir;
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;

    if ((PFSTR_StrNCmp(p_path, (const s8*)"\\", PF_STR_POS_HEAD, 0, 1) == 0 || PFSTR_StrNCmp(p_path, (const s8*)"/", PF_STR_POS_HEAD, 0, 1) == 0) &&
        PFSTR_StrNCmp(p_path, (const s8*)"", PF_STR_POS_HEAD, 1, 1) == 0)
    {
        err = PFENT_GetRootDir(p_vol, &ent);
        if (err != 0) {
            return err;
        }
    } else if (PFSTR_StrNumChar(p_path, PF_STR_POS_HEAD) == 2 && PFSTR_StrNCmp(p_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0) {
        PFVOL_GetCurrentDir(p_vol, &ent);
    } else if (PFSTR_StrNumChar(p_path, PF_STR_POS_HEAD) == 3 && PFSTR_StrNCmp(p_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0 &&
               (PFSTR_StrNCmp(p_path, (const s8*)"\\", PF_STR_POS_HEAD, 2, 1) == 0 || PFSTR_StrNCmp(p_path, (const s8*)"/", PF_STR_POS_HEAD, 2, 1) == 0))
    {
        err = PFENT_GetRootDir(p_vol, &ent);
        if (err != 0) {
            return err;
        }
    } else {
        if (PFSTR_StrNCmp(p_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0 &&
            (PFSTR_StrNCmp(p_path, (const s8*)"\\", PF_STR_POS_HEAD, 2, 1) == 0 || PFSTR_StrNCmp(p_path, (const s8*)"/", PF_STR_POS_HEAD, 2, 1) == 0))
        {
            PFSTR_MoveStrPos(p_path, 2);
        }
        err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
        if (err != 0) {
            return err;
        }
        if (!(ent.attr & PF_ATTR_DIR)) {
            return 1;
        }
    }

    p_sdd = PFDIR_GetSDD(p_vol, &ent);
    if (p_sdd == NULL) {
        return 21;
    }
    p_dir = PFDIR_GetFreeUDD(p_vol);
    if (p_dir == NULL) {
        return 22;
    }

    p_dir->p_sdd = p_sdd;
    p_dir->stat = PF_DIR_STAT_ALLOCATED;
    PFFAT_InitHint(&p_dir->hint);
    p_dir->cursor.physical_entry_index = 0;
    p_dir->cursor.logical_entry_index = 0;
    p_dir->cursor.logical_seek_index = 0;
    p_sdd->num_handlers++;
    *pp_dir = p_dir;
    return 0;
}

s32 PFDIR_p_readdir(PF_DIR* p_dir, PF_DIRENT* p_dirent)
{
    s32 err;
    u32 num_skip;
    PF_DIR_ENT ent;
    PF_FFD ffd;
    PF_FAT_HINT hint;
    PF_STR pattern;
    u32 lpos;
    u32 ppos;

    PFFAT_InitFFD(&ffd, &hint, p_dir->p_sdd->ffd.p_vol, &p_dir->p_sdd->dir_entry.start_cluster);
    num_skip = p_dir->cursor.logical_seek_index;
    if (p_dir->cursor.logical_entry_index <= num_skip) {
        num_skip -= p_dir->cursor.logical_entry_index;
    } else if (p_dir->cursor.logical_entry_index > num_skip) {
        p_dir->cursor.physical_entry_index = 0;
        p_dir->cursor.logical_entry_index = 0;
    }

    do {
        PFSTR_InitStr(&pattern, (const s8*)"*", PF_CODE_MODE_OEM);
        PFSTR_SetMarkPos(&pattern, NULL);
        err = PFENT_findEntry(&ffd, &ent, p_dir->cursor.physical_entry_index, &pattern, 0x7F, 0, &lpos, &ppos);
        if (err != 0) {
            return err;
        }
        p_dir->cursor.physical_entry_index = ppos + 1;
        p_dir->cursor.logical_entry_index++;
    } while (num_skip-- != 0);
    p_dir->cursor.logical_seek_index = p_dir->cursor.logical_entry_index;

    pf_strcpy(p_dirent->short_name, ent.short_name);
    if (ent.long_name[0] != 0) {
        VFiPFPATH_transformFromUnicodeToNormal(p_dirent->long_name, ent.long_name);
    } else {
        p_dirent->long_name[0] = 0;
    }
    return 0;
}

static void PFENT_SetDotEntry(u8* entry)
{
    s32 i;

    *entry = '.';
    for (i = 1; i < 11; i++) {
        entry[i] = ' ';
    }
}

static void PFENT_SetDotDotEntry(u8* entry)
{
    s32 i;

    entry[0] = '.';
    entry[1] = '.';
    for (i = 2; i < 11; i++) {
        entry[i] = ' ';
    }
}

s32 PFDIR_p_mkdir(PF_VOLUME* p_vol, PF_STR* p_path, u32 mode, PF_DTA* p_dta)
{
    u8 sum;
    u32 num_sector;
    u32* p_chain;
    u32 ord;
    u16 len;
    u8 num_lfn;
    u32 cluster;
    u32 write_sector;
    u16 entry_offset;
    s32 err;
    PF_DIR_ENT parent_ent;
    PF_DIR_ENT ent;
    s8 name_buf[512];
    PF_ENT_ITER iter;
    PF_FFD ffd;
    u8 lfn_buf[32];
    PF_STR dir_path;
    PF_STR file_name;
    PF_FAT_HINT hint;
    u32 prev_chain[2];
    u32 is_found;
    u32 is_hit;
    PF_CACHE_PAGE* p_page;
    u32 sector;
    u32 success_size;
    u32 pos;

    num_lfn = 0;
    sector = 0;
    err = PFPATH_SplitPath(p_path, &dir_path, &file_name);
    if (err != 0) {
        return err;
    }
    err = PFENT_GetEntryOfPath(&iter, &parent_ent, p_vol, &dir_path, 1);
    if (err != 0) {
        return err;
    }
    if (!(parent_ent.attr & PF_ATTR_DIR)) {
        return 20;
    }

    len = PFSTR_StrNumChar(&file_name, PF_STR_POS_HEAD);
    if (len + parent_ent.path_len > 259) {
        return 2;
    }
    if (len > 255) {
        return 1;
    }

    if (PFSTR_GetCodeMode(&file_name) == PF_CODE_MODE_UNICODE) {
        VFiPFPATH_transformFromUnicodeToNormal(name_buf, (const u16*)PFSTR_GetStrPos(&file_name, PF_STR_POS_HEAD));
    }
    PFSTR_SetMarkPos(&file_name, name_buf);

    if (p_dta != NULL) {
        p_dta->reserved[0] = 0;
        p_dta->reserved[1] = 0;
        p_dta->start_cluster = parent_ent.start_cluster;
        p_dta->p_vol = parent_ent.p_vol;
        p_dta->attr_required = PF_ATTR_DIR;
        if (PFSTR_GetCodeMode(&file_name) == PF_CODE_MODE_OEM) {
            PFPATH_CopyPattern(p_dta->pattern, NULL, &file_name);
            p_dta->status |= PF_DTA_STAT_OEM_PATTERN;
        } else {
            PFPATH_CopyPattern(p_dta->pattern, p_dta->w_pattern, &file_name);
            p_dta->status |= PF_DTA_STAT_UNICODE_PATTERN;
        }
    }

    PFFAT_InitFFD(&ffd, &hint, p_vol, &parent_ent.start_cluster);
    iter.ffd = ffd;
    PFFAT_ResetFFD(&iter.ffd, &parent_ent.start_cluster);
    err = PFENT_ITER_IteratorInitialize(&iter, 0);
    if (err != 0) {
        return err;
    }
    err = PFENT_ITER_FindEntryOfPath(&iter, &ent, &file_name, PF_ATTR_DIR, 0, &is_found);
    if (err != 0) {
        return err;
    }
    if (is_found) {
        return 8;
    }

    err = VFiPFPATH_parseShortName(ent.short_name, &file_name);
    if (err != 0 && ent.short_name[0] == 0) {
        return 1;
    }
    if (err != 0) {
        err = PFENT_AdjustSFN(&parent_ent, ent.short_name);
        if (err != 0) {
            return err;
        }
        if (PFSTR_GetCodeMode(&file_name) == PF_CODE_MODE_OEM) {
            len = VFiPFPATH_transformInUnicode(ent.long_name, PFSTR_GetStrPos(&file_name, PF_STR_POS_HEAD));
        } else {
            pf_w_strcpy(ent.long_name, (const u16*)PFSTR_GetStrPos(&file_name, PF_STR_POS_HEAD));
        }
    } else {
        ent.long_name[0] = 0;
    }

    ent.start_cluster = 0;
    ent.file_size = 0;
    ent.p_vol = p_vol;
    ent.small_letter_flag = 0;
    ent.attr = PF_ATTR_DIR;
    ent.create_time_ms = PFENT_getcurrentDateTimeForEnt(&ent.create_date, &ent.create_time);
    ent.access_date = ent.create_date;
    ent.modify_time = ent.create_time;
    ent.modify_date = ent.create_date;
    entry_offset = ent.entry_offset;

    if (ent.long_name[0] != 0 && !(ent.small_letter_flag & 0x18)) {
        num_lfn = (len % 13u != 0 ? 1 : 0) + len / 13u;
        err = PFENT_allocateEntry(&ent, num_lfn + 1, &ffd, prev_chain, &file_name, &pos);
        if (err != 0) {
            return err;
        }
        if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
            PFPATH_AdjustExtShortName(ent.short_name, pos);
        }
    } else {
        err = PFENT_allocateEntry(&ent, 1, &ffd, prev_chain, &file_name, &pos);
        if (err != 0) {
            return err;
        }
    }

    PFFAT_ResetFFD(&ffd, &ent.start_cluster);
    err = PFFAT_GetSectorSpecified(&ffd, 0, 1, &sector);
    if (err != 0) {
        return err;
    }
    if (sector == 0xFFFFFFFF) {
        return 6;
    }

    cluster = ffd.p_hint->cluster;
    if (p_vol->bpb.fat_type == PDM_FAT_32 && *iter.ffd.p_start_cluster == p_vol->bpb.root_dir_cluster && entry_offset == 0 &&
        cluster == p_vol->bpb.root_dir_cluster)
    {
        iter.ffd = ffd;
        err = PFENT_ITER_IteratorInitialize(&iter, num_lfn);
        if (err != 0) {
            return err;
        }
        if (iter.buf[0] == PF_ENT_END) {
            PFFAT_ResetFFD(&ffd, &ent.start_cluster);
            *ffd.p_start_cluster = 0;
            ffd.p_hint->cluster = 0;
            cluster = p_vol->bpb.root_dir_cluster + 1;
            err = PFFAT_GetSectorSpecified(&ffd, 0, 1, &sector);
            if (err != 0) {
                return err;
            }
        }
    }

    err = PFCACHE_AllocateDataPage(p_vol, 0xFFFFFFFF, &p_page, &is_hit);
    if (err != 0) {
        return err;
    }
    pf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);
    for (num_sector = p_vol->bpb.sectors_per_cluster; num_sector != 0; num_sector--) {
        if (num_sector == 1) {
            PFENT_SetDotEntry(p_page->p_buf);
            PFENT_StoreEntryNumericFieldsToBuf(p_page->p_buf, &ent);
            PFENT_SetDotDotEntry(p_page->p_buf + 32);
            if (parent_ent.entry_sector == 0xFFFFFFFF) {
                ent.start_cluster = 0;
            } else {
                ent.start_cluster = parent_ent.start_cluster;
            }
            PFENT_StoreEntryNumericFieldsToBuf(p_page->p_buf + 32, &ent);
        }
        err = PFSEC_WriteData(p_vol, p_page->p_buf, sector + num_sector - 1, 0, p_vol->bpb.bytes_per_sector, &success_size, 0);
        if (err != 0) {
            break;
        }
        if (success_size != p_vol->bpb.bytes_per_sector) {
            err = 17;
            break;
        }
    }
    PFCACHE_FreeDataPage(p_vol, p_page);
    if (err != 0) {
        return err;
    }

    ent.num_entry_LFNs = num_lfn;
    write_sector = ent.entry_sector;
    sum = PFENT_CalcCheckSum(&ent);
    if (!(parent_ent.small_letter_flag & 0x18)) {
        p_chain = prev_chain;
        for (ord = num_lfn; ord >= 1; ord--) {
            PFENT_storeLFNEntryFieldsToBuf(lfn_buf, &ent, ord, sum, ord == num_lfn);
            err = PFSEC_WriteData(p_vol, lfn_buf, write_sector, ent.entry_offset, 32, &success_size, 0);
            if (err != 0) {
                return err;
            }
            if (success_size != 32) {
                return 17;
            }
            ent.entry_offset += 32;
            if (ent.entry_offset >= p_vol->bpb.bytes_per_sector) {
                ent.entry_offset = 0;
                write_sector = *p_chain++;
            }
        }
        ent.entry_sector = write_sector;
    }

    ent.start_cluster = cluster;
    err = PFENT_UpdateSFNEntry(&ent, 0);
    if (err != 0) {
        return err;
    }

    if (p_dta != NULL) {
        p_dta->index = pos + 1;
        pf_strcpy(p_dta->short_name, ent.short_name);
        if ((p_dta->status & PF_DTA_STAT_UNICODE_PATTERN) == PF_DTA_STAT_UNICODE_PATTERN) {
            VFiPFPATH_transformInUnicode(p_dta->w_short_name, p_dta->short_name);
        }
        if (ent.long_name[0] != 0) {
            if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
                pf_vol_set.setting = (pf_vol_set.setting & ~PF_SETTING_CODE_MODE_MASK) | PF_CODE_MODE_OEM;
                VFiPFPATH_transformFromUnicodeToNormal(p_dta->long_name, ent.long_name);
                pf_vol_set.setting = (pf_vol_set.setting & ~PF_SETTING_CODE_MODE_MASK) | PF_CODE_MODE_UNICODE;
            } else {
                VFiPFPATH_transformFromUnicodeToNormal(p_dta->long_name, ent.long_name);
            }
            if ((p_dta->status & PF_DTA_STAT_UNICODE_PATTERN) == PF_DTA_STAT_UNICODE_PATTERN) {
                pf_w_strcpy(p_dta->w_long_name, ent.long_name);
            }
            p_dta->ordinal = 1;
            p_dta->check_sum = sum;
        } else {
            p_dta->long_name[0] = 0;
            if ((p_dta->status & PF_DTA_STAT_UNICODE_PATTERN) == PF_DTA_STAT_UNICODE_PATTERN) {
                p_dta->w_long_name[0] = 0;
            }
            p_dta->ordinal = 0;
            p_dta->check_sum = 0;
        }
        p_dta->file_size = ent.file_size;
        p_dta->modify_time = ent.modify_time;
        p_dta->modify_date = ent.modify_date;
        p_dta->attr = ent.attr;
        p_dta->num_entry_LFNs = ent.num_entry_LFNs;
    }
    return 0;
}

s32 PFDIR_p_chdir(PF_VOLUME* p_vol, PF_STR* p_path)
{
    s32 err;
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;

    if ((PFSTR_StrNCmp(p_path, (const s8*)"\\", PF_STR_POS_HEAD, 0, 1) == 0 ||
         PFSTR_StrNCmp(p_path, (const s8*)"/", PF_STR_POS_HEAD, 0, 1) == 0) &&
        PFSTR_StrNCmp(p_path, (const s8*)"", PF_STR_POS_HEAD, 1, 1) == 0)
    {
        err = PFENT_GetRootDir(p_vol, &ent);
        if (err != 0) {
            return err;
        }
        err = PFVOL_SetCurrentDir(p_vol, &ent);
        if (err != 0) {
            return err;
        }
        return 0;
    }

    if (PFSTR_StrNumChar(p_path, PF_STR_POS_HEAD) == 2 && PFSTR_StrNCmp(p_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0) {
        return 0;
    }

    if (PFSTR_StrNumChar(p_path, PF_STR_POS_HEAD) == 3 && PFSTR_StrNCmp(p_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0 &&
        (PFSTR_StrNCmp(p_path, (const s8*)"\\", PF_STR_POS_HEAD, 2, 1) == 0 ||
         PFSTR_StrNCmp(p_path, (const s8*)"/", PF_STR_POS_HEAD, 2, 1) == 0))
    {
        err = PFENT_GetRootDir(p_vol, &ent);
        if (err != 0) {
            return err;
        }
        err = PFVOL_SetCurrentDir(p_vol, &ent);
        if (err != 0) {
            return err;
        }
        return 0;
    }

    if (PFSTR_StrNCmp(p_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0 &&
        (PFSTR_StrNCmp(p_path, (const s8*)"\\", PF_STR_POS_HEAD, 2, 1) == 0 ||
         PFSTR_StrNCmp(p_path, (const s8*)"/", PF_STR_POS_HEAD, 2, 1) == 0))
    {
        PFSTR_MoveStrPos(p_path, 2);
    }
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }
    if (!(ent.attr & PF_ATTR_DIR)) {
        return 20;
    }
    err = PFVOL_SetCurrentDir(p_vol, &ent);
    if (err != 0) {
        return err;
    }
    return 0;
}

static u32 PFDIR_IsOpened(PF_DIR_ENT* p_ent)
{
    PF_SDD* p_sdd;
    u32 i;

    p_sdd = p_ent->p_vol->sdds;
    for (i = 0; i < PF_MAX_DIRS; i++, p_sdd++) {
        if ((p_sdd->stat & PF_SDD_STAT_ALLOCATED) && (p_sdd->stat & PF_SDD_STAT_OPENED) &&
            p_ent->p_vol == p_sdd->dir_entry.p_vol && p_ent->entry_sector == p_sdd->dir_entry.entry_sector &&
            p_ent->entry_offset == p_sdd->dir_entry.entry_offset)
        {
            return 1;
        }
    }
    return 0;
}

s32 PFDIR_p_chmod(PF_VOLUME* p_vol, PF_STR* p_path, u8 attr)
{
    s32 err;
    u32 is_dir;
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_STR dir_path;
    PF_STR file_name;
    u16 time;

    err = PFPATH_SplitPath(p_path, &dir_path, &file_name);
    if (err != 0) {
        return err;
    }
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }

    is_dir = ent.attr & PF_ATTR_DIR;
    if ((is_dir && !(attr & PF_ATTR_DIR)) || (!is_dir && (attr & PF_ATTR_DIR))) {
        return 10;
    }
    if (attr & 0x40) {
        attr &= ~0x40;
    }

    if (is_dir) {
        if (PFDIR_IsOpened(&ent)) {
            return 19;
        }
        if ((attr & PF_ATTR_SYSTEM) && (attr & PF_ATTR_LFN) != PF_ATTR_LFN) {
            return 10;
        }
        ent.attr = attr | PF_ATTR_DIR;
    } else {
        if (PFFILE_IsOpened(&ent)) {
            return 19;
        }
        ent.attr = attr;
    }

    PFENT_getcurrentDateTimeForEnt(&ent.access_date, &time);
    if (ent.file_size == 0 && !(ent.attr & PF_ATTR_DIR)) {
        ent.start_cluster = 0;
    }
    err = PFENT_UpdateSFNEntry(&ent, 0);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFDIR_p_rmdir(PF_VOLUME* p_vol, PF_STR* p_path)
{
    s32 err;
    u32 start_cluster;
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_ENT_ITER save_iter;
    PF_FFD ffd;
    PF_STR dir_path;
    PF_STR file_name;
    PF_FAT_HINT hint;
    PF_FAT_HINT save_hint;
    u32 is_empty;

    err = PFPATH_SplitPath(p_path, &dir_path, &file_name);
    if (err != 0) {
        return err;
    }
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }
    save_iter = iter;
    save_hint = *iter.ffd.p_hint;

    if (!(ent.attr & PF_ATTR_DIR) || (ent.attr & PF_ATTR_READONLY)) {
        return 20;
    }
    if (PFDIR_IsOpened(&ent)) {
        return 19;
    }
    if (PFVOL_IsCurrentDirCluster(p_vol, ent.start_cluster) || ent.start_cluster == 1 ||
        (p_vol->bpb.fat_type == PDM_FAT_32 && ent.start_cluster == p_vol->bpb.root_dir_cluster))
    {
        return 28;
    }

    PFFAT_InitFFD(&ffd, &hint, p_vol, &ent.start_cluster);
    iter.ffd = ffd;
    err = PFENT_ITER_IteratorInitialize(&iter, 0);
    if (err != 0) {
        return err;
    }
    err = PFDIR_IsEmptyDirectory(&iter, &is_empty);
    if (err != 0) {
        return err;
    }
    if (!is_empty) {
        return 29;
    }

    start_cluster = ent.start_cluster;
    iter = save_iter;
    iter.ffd.p_hint = &save_hint;
    err = PFENT_RemoveEntry(&ent, &iter);
    if (err != 0) {
        return err;
    }
    err = PFFAT_FreeChain(&iter.ffd, start_cluster, 0xFFFFFFFF, 0xFFFFFFFF);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFDIR_p_fstat(PF_VOLUME* p_vol, PF_STR* p_path, PF_STAT* p_stat)
{
    s32 err;
    s32 i;
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;

    i = 0;
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }

    if (p_vol->num_opened_files != 0) {
        for (i = 0; i < PF_MAX_FILES; i++) {
            if ((p_vol->sfds[i].stat & PF_SFD_STAT_ALLOCATED) && (p_vol->sfds[i].stat & PF_SFD_STAT_OPENED) &&
                ent.p_vol == p_vol->sfds[i].dir_entry.p_vol && ent.entry_sector == p_vol->sfds[i].dir_entry.entry_sector &&
                ent.entry_offset == p_vol->sfds[i].dir_entry.entry_offset)
            {
                p_stat->file_size = p_vol->sfds[i].dir_entry.file_size;
                p_stat->attr = p_vol->sfds[i].dir_entry.attr;
                p_stat->modify_time = p_vol->sfds[i].dir_entry.modify_time;
                p_stat->modify_date = p_vol->sfds[i].dir_entry.modify_date;
                p_stat->access_date = p_vol->sfds[i].dir_entry.access_date;
                p_stat->create_time = p_vol->sfds[i].dir_entry.create_time;
                p_stat->create_date = p_vol->sfds[i].dir_entry.create_date;
                p_stat->create_time_ms = p_vol->sfds[i].dir_entry.create_time_ms;
                break;
            }
        }
    }

    if (p_vol->num_opened_files == 0 || i == PF_MAX_FILES) {
        if (ent.attr & PF_ATTR_DIR) {
            p_stat->file_size = 0;
        } else {
            p_stat->file_size = ent.file_size;
        }
        p_stat->attr = ent.attr;
        p_stat->modify_time = ent.modify_time;
        p_stat->modify_date = ent.modify_date;
        p_stat->access_date = ent.access_date;
        p_stat->create_time = ent.create_time;
        p_stat->create_date = ent.create_date;
        p_stat->create_time_ms = ent.create_time_ms;
    }
    return 0;
}

static u32 PFDIR_IsCurrentDir(PF_DIR_ENT* p_ent)
{
    PF_CUR_DIR* p_cur;
    u32 i;
    u32 j;

    for (i = 0; i < PF_MAX_CUR_VOLUMES; i++) {
        if (p_ent->p_vol == pf_vol_set.current_vol[i].p_vol) {
            p_cur = pf_vol_set.current_vol[i].p_vol->current_dir;
            for (j = 0; j < PF_MAX_CUR_DIRS; j++, p_cur++) {
                if ((p_cur->stat & PF_CUR_DIR_STAT_ALLOCATED) && p_cur->dir_entry.start_cluster == p_ent->start_cluster) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

s32 PFDIR_p_rename(PF_VOLUME* p_vol, PF_STR* p_old_path, PF_STR* p_new_path)
{
    PF_SFD* p_sfd;
    s32 err;
    u32 i;
    u32 j;
    u32 ord;
    s32 result;
    u32* p_chain;
    u32 sector;
    u32 is_lfn;
    PF_DIR_ENT* p_ent;
    u16 first_char;
    s32 len;
    u32 dir_sector;
    PF_DIR_ENT ent;
    PF_DIR_ENT new_ent;
    PF_DIR_ENT dir_ent;
    s8 name_buf[512];
    PF_ENT_ITER iter;
    PF_ENT_ITER new_iter;
    u8 lfn_buf[32];
    PF_STR dir_path;
    PF_STR old_name;
    PF_STR new_name;
    PF_FAT_HINT hint;
    PF_FAT_HINT new_hint;
    u32 prev_chain[2];
    u32 success_size;
    u32 pos;
    u32 is_found;
    u16 time;
    u8 deleted[1] = { PF_ENT_DELETED };

    result = 0;
    first_char = 0;
    err = PFPATH_SplitPath(p_old_path, &dir_path, &old_name);
    if (err != 0) {
        return err;
    }
    if (PFSTR_GetCodeMode(&old_name) == PF_CODE_MODE_UNICODE) {
        len = PFSTR_StrNumChar(&old_name, PF_STR_POS_HEAD);
        if (len > 255) {
            return 1;
        }
        VFiPFPATH_transformFromUnicodeToNormal(name_buf, (const u16*)PFSTR_GetStrPos(&old_name, PF_STR_POS_HEAD));
    }
    PFSTR_SetMarkPos(&old_name, name_buf);

    err = PFENT_GetEntryOfPath(&iter, &dir_ent, p_vol, p_old_path, 1);
    if (err != 0) {
        return err;
    }
    PFFAT_InitFFD(&iter.ffd, &hint, p_vol, &iter.ffd.start_cluster);
    iter.ffd.start_cluster = dir_ent.start_cluster;
    err = PFENT_ITER_IteratorInitialize(&iter, 0);
    if (err != 0) {
        return err;
    }
    err = PFENT_ITER_FindEntryOfPath(&iter, &ent, &old_name, 0x7F, 0, &is_found);
    if (err != 0) {
        return err;
    }
    if (!is_found) {
        return 3;
    }
    if (ent.attr & PF_ATTR_VOLUME) {
        return 3;
    }
    if (ent.attr & PF_ATTR_READONLY) {
        return 24;
    }

    if (PFSTR_StrNCmp(p_new_path, (const s8*)":", PF_STR_POS_HEAD, 1, 1) == 0 && p_vol != PFPATH_GetVolumeFromPath(p_new_path)) {
        return 31;
    }
    err = PFPATH_SplitPath(p_new_path, &dir_path, &new_name);
    if (err != 0) {
        return err;
    }
    if (PFSTR_GetCodeMode(&new_name) == PF_CODE_MODE_UNICODE) {
        len = PFSTR_StrNumChar(&new_name, PF_STR_POS_HEAD);
        if (len > 255) {
            return 1;
        }
        VFiPFPATH_transformFromUnicodeToNormal(name_buf, (const u16*)PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
    }
    PFSTR_SetMarkPos(&new_name, name_buf);

    if (PFSTR_GetCodeMode(&new_name) == PF_CODE_MODE_OEM) {
        err = pf_strcmp(PFSTR_GetStrPos(&dir_path, PF_STR_POS_HEAD), PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
    } else {
        err = pf_w_strcmp((const u16*)PFSTR_GetStrPos(&dir_path, PF_STR_POS_HEAD), (const u16*)PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
    }
    if (err != 0) {
        err = PFENT_GetEntryOfPath(&new_iter, &new_ent, p_vol, p_new_path, 1);
        if (err != 0) {
            return err;
        }
        if (new_ent.start_cluster != dir_ent.start_cluster) {
            return 32;
        }
    }

    new_ent.start_cluster = dir_ent.start_cluster;
    PFFAT_InitFFD(&new_iter.ffd, &new_hint, p_vol, &new_ent.start_cluster);
    err = PFENT_SearchEntry(&new_iter.ffd, &new_ent, 0, &new_name, 0x7F, 0);
    switch (err) {
    case 0:
        return 8;
    case 3:
        break;
    default:
        return err;
    }

    if (!(ent.attr & PF_ATTR_DIR)) {
        if (PFFILE_IsOpened(&ent)) {
            return 19;
        }
    } else {
        if (PFDIR_IsOpened(&ent)) {
            return 19;
        }
        if (PFDIR_IsCurrentDir(&ent)) {
            return 28;
        }
        if (PFSTR_StrNCmp(&old_name, (const s8*)"..", PF_STR_POS_HEAD, 0, 2) == 0) {
            return 28;
        }
        if (PFSTR_StrNCmp(&old_name, (const s8*)".", PF_STR_POS_HEAD, 0, 1) == 0) {
            return 28;
        }
        dir_sector = p_vol->bpb.first_data_sector + ((ent.start_cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
        p_sfd = p_vol->sfds;
        for (i = 0; i < PF_MAX_FILES; i++, p_sfd++) {
            if ((p_sfd->stat & PF_SFD_STAT_ALLOCATED) && (p_sfd->stat & PF_SFD_STAT_OPENED)) {
                if (p_vol == p_sfd->dir_entry.p_vol && dir_sector == p_sfd->dir_entry.entry_sector) {
                    return 19;
                }
                is_found = 0;
                err = PFENT_ITER_IsInDirectory(&p_sfd->dir_entry, ent.start_cluster, &is_found);
                if (err != 0) {
                    return err;
                }
                if (is_found) {
                    return 19;
                }
            }
        }
    }

    len = PFSTR_StrNumChar(&new_name, PF_STR_POS_HEAD);
    if (len + dir_ent.path_len > 259) {
        return 1;
    }
    if (!(ent.attr & PF_ATTR_DIR) && len > 255) {
        return 1;
    }

    pf_memcpy(&new_ent, &ent, sizeof(PF_DIR_ENT));
    is_lfn = VFiPFPATH_parseShortName(new_ent.short_name, &new_name);
    if (new_ent.short_name[0] == 0) {
        return 2;
    }
    if (is_lfn == 0 && ent.num_entry_LFNs == 0) {
        new_ent.long_name[0] = 0;
        new_ent.num_entry_LFNs = 0;
        PFENT_getcurrentDateTimeForEnt(&new_ent.access_date, &time);
        err = PFENT_UpdateSFNEntry(&new_ent, 1);
        if (err != 0) {
            return err;
        }
    } else {
        if (ent.num_entry_LFNs != 0 && !(ent.small_letter_flag & 0x18)) {
            first_char = ent.long_name[0];
            ent.long_name[0] = 0;
            for (j = 1; j <= ent.num_entry_LFNs; j++) {
                err = PFENT_ITER_Retreat(&iter, 0);
                if (err != 0) {
                    return err;
                }
                err = PFSEC_WriteData(p_vol, deleted, iter.sector, iter.offset, 1, &success_size, 0);
                if (err != 0) {
                    return err;
                }
                if (success_size != 1) {
                    return 17;
                }
            }
        }
        err = PFSEC_WriteData(p_vol, deleted, ent.entry_sector, ent.entry_offset, 1, &success_size, 0);
        if (err != 0) {
            return err;
        }
        if (success_size != 1) {
            return 17;
        }
        if (p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) {
            err = PFCACHE_FlushDataCache(p_vol);
            if (err != 0) {
                return err;
            }
        }

        if (is_lfn != 0) {
            err = PFENT_AdjustSFN(&dir_ent, new_ent.short_name);
            if (err != 0) {
                return err;
            }
            if (PFSTR_GetCodeMode(&new_name) == PF_CODE_MODE_OEM) {
                len = VFiPFPATH_transformInUnicode(new_ent.long_name, PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
            } else {
                pf_w_strcpy(new_ent.long_name, (const u16*)PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
            }
            new_ent.num_entry_LFNs = (len % 13 != 0 ? 1 : 0) + len / 13;
        } else {
            new_ent.num_entry_LFNs = 0;
        }
        new_ent.small_letter_flag = 0;

        PFFAT_InitFFD(&iter.ffd, &new_hint, p_vol, &dir_ent.start_cluster);
        result = PFENT_allocateEntry(&new_ent, new_ent.num_entry_LFNs + 1, &iter.ffd, prev_chain, &new_name, &pos);
        if (result != 0) {
            if (ent.num_entry_LFNs != 0 && !(ent.small_letter_flag & 0x18)) {
                ent.long_name[0] = first_char;
                ent.entry_offset -= (ent.num_entry_LFNs * 32) & 0x1FFF;
            }
            p_ent = &ent;
        } else {
            if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
                PFPATH_AdjustExtShortName(new_ent.short_name, pos);
            }
            new_ent.check_sum = PFENT_CalcCheckSum(&new_ent);
            PFENT_getcurrentDateTimeForEnt(&new_ent.access_date, &time);
            p_ent = &new_ent;
        }

        if (p_ent->num_entry_LFNs != 0 && !(p_ent->small_letter_flag & 0x18)) {
            sector = p_ent->entry_sector;
            p_chain = prev_chain;
            for (ord = p_ent->num_entry_LFNs; ord >= 1; ord--) {
                PFENT_storeLFNEntryFieldsToBuf(lfn_buf, p_ent, ord, p_ent->check_sum, ord == p_ent->num_entry_LFNs);
                err = PFSEC_WriteData(p_vol, lfn_buf, sector, p_ent->entry_offset, 32, &success_size, 0);
                if (err != 0) {
                    return err;
                }
                if (success_size != 32) {
                    return 17;
                }
                p_ent->entry_offset += 32;
                if (p_ent->entry_offset >= p_vol->bpb.bytes_per_sector) {
                    p_ent->entry_offset = 0;
                    sector = *p_chain++;
                }
            }
            p_ent->entry_sector = sector;
        }
        err = PFENT_UpdateSFNEntry(p_ent, 1);
        if (err != 0) {
            return err;
        }
    }
    return result;
}

s32 PFDIR_p_move(PF_VOLUME* p_vol, PF_STR* p_src_path, PF_STR* p_dst_path)
{
    u32* p_chain;
    u16 first_char;
    u32 cluster;
    s32 err;
    u32 sector;
    u32 is_lfn;
    s32 len;
    u32 dir_sector;
    s32 result;
    PF_DIR_ENT* p_ent;
    u32 ord;
    u32 j;
    u32 i;
    PF_SFD* p_sfd;
    PF_DIR_ENT ent;
    PF_DIR_ENT new_ent;
    PF_DIR_ENT dir_ent;
    s8 name_buf[512];
    PF_ENT_ITER iter;
    PF_ENT_ITER new_iter;
    u8 lfn_buf[32];
    PF_STR dir_path;
    PF_STR old_name;
    PF_STR new_name;
    PF_STR pattern;
    PF_FAT_HINT hint;
    PF_FAT_HINT new_hint;
    u32 prev_chain[2];
    u32 success_size;
    u32 pos;
    u32 is_found;
    u16 time;
    u8 deleted[1] = { PF_ENT_DELETED };

    first_char = 0;
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_src_path, 0);
    if (err != 0) {
        return err;
    }
    if (p_vol != PFPATH_GetVolumeFromPath(p_dst_path)) {
        return 31;
    }
    err = PFPATH_SplitPath(p_dst_path, &dir_path, &new_name);
    if (err != 0) {
        return err;
    }
    if (PFSTR_GetCodeMode(&new_name) == PF_CODE_MODE_UNICODE) {
        len = PFSTR_StrNumChar(&new_name, PF_STR_POS_HEAD);
        if (len > 255) {
            return 1;
        }
        VFiPFPATH_transformFromUnicodeToNormal(name_buf, (const u16*)PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
    }
    PFSTR_SetMarkPos(&new_name, name_buf);

    err = PFENT_GetEntryOfPath(&new_iter, &dir_ent, p_vol, p_dst_path, 1);
    if (err != 0) {
        return err;
    }
    new_ent.start_cluster = dir_ent.start_cluster;
    PFFAT_InitFFD(&new_iter.ffd, &new_hint, p_vol, &new_ent.start_cluster);
    err = PFENT_SearchEntry(&new_iter.ffd, &new_ent, 0, &new_name, 0x7F, 0);
    switch (err) {
    case 0:
        if (new_ent.entry_sector == ent.entry_sector && new_ent.entry_offset == ent.entry_offset) {
            return 0;
        }
        return 8;
    case 3:
        break;
    default:
        return err;
    }

    if (!(ent.attr & PF_ATTR_DIR)) {
        if (PFFILE_IsOpened(&ent)) {
            return 19;
        }
    } else {
        if (PFDIR_IsOpened(&ent)) {
            return 19;
        }
        if (PFDIR_IsCurrentDir(&ent)) {
            return 28;
        }
        err = PFPATH_SplitPath(p_src_path, &dir_path, &old_name);
        if (err != 0) {
            return err;
        }
        if (PFSTR_StrNCmp(&old_name, (const s8*)"..", PF_STR_POS_HEAD, 0, 2) == 0) {
            return 10;
        }
        if (PFSTR_StrNCmp(&old_name, (const s8*)".", PF_STR_POS_HEAD, 0, 1) == 0) {
            return 10;
        }
        dir_sector = p_vol->bpb.first_data_sector + ((ent.start_cluster - 2) << p_vol->bpb.log2_sectors_per_cluster);
        p_sfd = p_vol->sfds;
        for (i = 0; i < PF_MAX_FILES; i++, p_sfd++) {
            if ((p_sfd->stat & PF_SFD_STAT_ALLOCATED) && (p_sfd->stat & PF_SFD_STAT_OPENED)) {
                if (p_vol == p_sfd->dir_entry.p_vol && dir_sector == p_sfd->dir_entry.entry_sector) {
                    return 19;
                }
                is_found = 0;
                err = PFENT_ITER_IsInDirectory(&p_sfd->dir_entry, ent.start_cluster, &is_found);
                if (err != 0) {
                    return err;
                }
                if (is_found) {
                    return 19;
                }
            }
        }

        new_ent.start_cluster = dir_ent.start_cluster;
        for (;;) {
            if (new_ent.start_cluster <= 1) {
                break;
            }
            PFFAT_InitFFD(&new_iter.ffd, &new_hint, p_vol, &new_ent.start_cluster);
            PFSTR_InitStr(&pattern, (const s8*)"..", PF_CODE_MODE_OEM);
            PFSTR_SetMarkPos(&pattern, NULL);
            err = PFENT_SearchEntry(&new_iter.ffd, &new_ent, 0, &pattern, 0x7F, 0);
            if (err != 0) {
                return err;
            }
            cluster = new_ent.start_cluster;
            if (cluster <= 1) {
                break;
            }
            PFFAT_ResetFFD(&new_iter.ffd, &new_ent.start_cluster);
            err = PFENT_ITER_IteratorInitialize(&new_iter, 0);
            if (err != 0) {
                return err;
            }
            err = PFENT_ITER_FindCluster(&new_iter, &new_ent, cluster, &is_found);
            if (err == 0) {
                if (new_ent.start_cluster == ent.start_cluster) {
                    return 10;
                }
            } else {
                return err;
            }
        }
    }

    len = PFSTR_StrNumChar(&new_name, PF_STR_POS_HEAD);
    if (len + dir_ent.path_len > 259) {
        return 1;
    }
    if (!(ent.attr & PF_ATTR_DIR) && len > 255) {
        return 1;
    }

    pf_memcpy(&new_ent, &ent, sizeof(PF_DIR_ENT));
    is_lfn = VFiPFPATH_parseShortName(new_ent.short_name, &new_name);
    if (new_ent.short_name[0] == 0) {
        return 2;
    }

    if (ent.num_entry_LFNs != 0 && !(ent.small_letter_flag & 0x18)) {
        first_char = ent.long_name[0];
        ent.long_name[0] = 0;
        for (j = 1; j <= ent.num_entry_LFNs; j++) {
            err = PFENT_ITER_Retreat(&iter, 0);
            if (err != 0) {
                return err;
            }
            err = PFSEC_WriteData(p_vol, deleted, iter.sector, iter.offset, 1, &success_size, 0);
            if (err != 0) {
                return err;
            }
            if (success_size != 1) {
                return 17;
            }
        }
    }
    err = PFSEC_WriteData(p_vol, deleted, ent.entry_sector, ent.entry_offset, 1, &success_size, 0);
    if (err != 0) {
        return err;
    }
    if (success_size != 1) {
        return 17;
    }
    if (p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) {
        err = PFCACHE_FlushDataCache(p_vol);
        if (err != 0) {
            return err;
        }
    }

    if (is_lfn != 0) {
        err = PFENT_AdjustSFN(&dir_ent, new_ent.short_name);
        if (err != 0) {
            return err;
        }
        if (PFSTR_GetCodeMode(&new_name) == PF_CODE_MODE_OEM) {
            len = VFiPFPATH_transformInUnicode(new_ent.long_name, PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
        } else {
            pf_w_strcpy(new_ent.long_name, (const u16*)PFSTR_GetStrPos(&new_name, PF_STR_POS_HEAD));
        }
        new_ent.num_entry_LFNs = (len % 13 != 0 ? 1 : 0) + len / 13;
    } else {
        new_ent.num_entry_LFNs = 0;
    }
    new_ent.small_letter_flag = 0;

    PFFAT_InitFFD(&new_iter.ffd, &hint, p_vol, &dir_ent.start_cluster);
    result = PFENT_allocateEntry(&new_ent, new_ent.num_entry_LFNs + 1, &new_iter.ffd, prev_chain, &new_name, &pos);
    if (result != 0) {
        if (ent.num_entry_LFNs != 0 && !(ent.small_letter_flag & 0x18)) {
            ent.long_name[0] = first_char;
            ent.entry_offset -= (ent.num_entry_LFNs * 32) & 0x1FFF;
        }
        p_ent = &ent;
    } else {
        if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
            PFPATH_AdjustExtShortName(new_ent.short_name, pos);
        }
        new_ent.check_sum = PFENT_CalcCheckSum(&new_ent);
        PFENT_getcurrentDateTimeForEnt(&new_ent.access_date, &time);
        p_ent = &new_ent;
        if (ent.attr & PF_ATTR_DIR) {
            PFFAT_InitFFD(&iter.ffd, &hint, p_vol, &ent.start_cluster);
            PFSTR_InitStr(&pattern, (const s8*)"..", PF_CODE_MODE_OEM);
            PFSTR_SetMarkPos(&pattern, NULL);
            err = PFENT_SearchEntry(&iter.ffd, &ent, 0, &pattern, 0x7F, 0);
            if (err != 0) {
                return err;
            }
            if (dir_ent.start_cluster <= 1 || (dir_ent.start_cluster == 2 && p_vol->bpb.fat_type == PDM_FAT_32)) {
                ent.start_cluster = 0;
            } else {
                ent.start_cluster = dir_ent.start_cluster;
            }
            err = PFENT_UpdateSFNEntry(&ent, 1);
            if (err != 0) {
                return err;
            }
            if (p_vol->cache.mode & PF_CACHE_MODE_FAT_WRITE_THROUGH) {
                err = PFCACHE_FlushDataCache(p_vol);
                if (err != 0) {
                    return err;
                }
            }
        }
    }

    if (p_ent->num_entry_LFNs != 0 && !(p_ent->small_letter_flag & 0x18)) {
        sector = p_ent->entry_sector;
        p_chain = prev_chain;
        for (ord = p_ent->num_entry_LFNs; ord >= 1; ord--) {
            PFENT_storeLFNEntryFieldsToBuf(lfn_buf, p_ent, ord, p_ent->check_sum, ord == p_ent->num_entry_LFNs);
            err = PFSEC_WriteData(p_vol, lfn_buf, sector, p_ent->entry_offset, 32, &success_size, 0);
            if (err != 0) {
                return err;
            }
            if (success_size != 32) {
                return 17;
            }
            p_ent->entry_offset += 32;
            if (p_ent->entry_offset >= p_vol->bpb.bytes_per_sector) {
                p_ent->entry_offset = 0;
                sector = *p_chain++;
            }
        }
        p_ent->entry_sector = sector;
    }
    err = PFENT_UpdateSFNEntry(p_ent, 1);
    if (err != 0) {
        result = err;
    }
    return result;
}

s32 PFDIR_p_fsnext(PF_DTA* p_dta)
{
    s32 err;
    s32 i;
    PF_DIR_ENT ent;
    PF_FFD ffd;
    PF_FAT_HINT hint;
    PF_STR pattern;
    u32 lpos;
    u32 ppos;

    lpos = 0;
    ppos = 0;
    i = 0;
    if ((p_dta->status & PF_DTA_STAT_OEM_PATTERN) == PF_DTA_STAT_OEM_PATTERN) {
        err = PFSTR_InitStr(&pattern, p_dta->pattern, PF_CODE_MODE_OEM);
    } else {
        err = PFSTR_InitStr(&pattern, (const s8*)p_dta->w_pattern, PF_CODE_MODE_UNICODE);
    }
    if (err != 0) {
        return err;
    }
    PFSTR_SetMarkPos(&pattern, p_dta->pattern);

    while (ppos < 999999) {
        if (p_dta->index >= 999999) {
            return 10;
        }
        ent.start_cluster = p_dta->start_cluster;
        PFFAT_InitFFD(&ffd, &hint, p_dta->p_vol, &ent.start_cluster);
        err = PFENT_findEntry(&ffd, &ent, p_dta->index, &pattern, p_dta->attr_required, 0, &lpos, &ppos);
        if (err != 0) {
            return err;
        }
        p_dta->index = ppos + 1;
        if (ppos == 999999) {
            return 3;
        }
        break;
    }

    pf_strcpy(p_dta->short_name, ent.short_name);
    if ((p_dta->status & PF_DTA_STAT_UNICODE_PATTERN) == PF_DTA_STAT_UNICODE_PATTERN) {
        VFiPFPATH_transformInUnicode(p_dta->w_short_name, p_dta->short_name);
    }

    if (ent.long_name[0] != 0) {
        if ((pf_vol_set.setting & PF_CODE_MODE_UNICODE) == PF_CODE_MODE_UNICODE) {
            pf_vol_set.setting = (pf_vol_set.setting & ~PF_SETTING_CODE_MODE_MASK) | PF_CODE_MODE_OEM;
            VFiPFPATH_transformFromUnicodeToNormal(p_dta->long_name, ent.long_name);
            pf_vol_set.setting = (pf_vol_set.setting & ~PF_SETTING_CODE_MODE_MASK) | PF_CODE_MODE_UNICODE;
        } else {
            VFiPFPATH_transformFromUnicodeToNormal(p_dta->long_name, ent.long_name);
        }
        if ((p_dta->status & PF_DTA_STAT_UNICODE_PATTERN) == PF_DTA_STAT_UNICODE_PATTERN) {
            pf_w_strcpy(p_dta->w_long_name, ent.long_name);
        }
    } else {
        p_dta->long_name[0] = 0;
        if ((p_dta->status & PF_DTA_STAT_UNICODE_PATTERN) == PF_DTA_STAT_UNICODE_PATTERN) {
            p_dta->w_long_name[0] = 0;
        }
    }

    if (p_dta->p_vol->num_opened_files != 0) {
        for (i = 0; i < PF_MAX_FILES; i++) {
            if ((p_dta->p_vol->sfds[i].stat & PF_SFD_STAT_ALLOCATED) && (p_dta->p_vol->sfds[i].stat & PF_SFD_STAT_OPENED) &&
                ent.p_vol == p_dta->p_vol->sfds[i].dir_entry.p_vol &&
                ent.entry_sector == p_dta->p_vol->sfds[i].dir_entry.entry_sector &&
                ent.entry_offset == p_dta->p_vol->sfds[i].dir_entry.entry_offset)
            {
                p_dta->file_size = p_dta->p_vol->sfds[i].dir_entry.file_size;
                p_dta->modify_time = p_dta->p_vol->sfds[i].dir_entry.modify_time;
                p_dta->modify_date = p_dta->p_vol->sfds[i].dir_entry.modify_date;
                p_dta->attr = p_dta->p_vol->sfds[i].dir_entry.attr;
                p_dta->num_entry_LFNs = p_dta->p_vol->sfds[i].dir_entry.num_entry_LFNs;
                p_dta->ordinal = p_dta->p_vol->sfds[i].dir_entry.ordinal;
                p_dta->check_sum = p_dta->p_vol->sfds[i].dir_entry.check_sum;
                break;
            }
        }
    }

    if (p_dta->p_vol->num_opened_files == 0 || i == PF_MAX_FILES) {
        p_dta->file_size = ent.file_size;
        p_dta->modify_time = ent.modify_time;
        p_dta->modify_date = ent.modify_date;
        p_dta->attr = ent.attr;
        p_dta->num_entry_LFNs = ent.num_entry_LFNs;
        p_dta->ordinal = ent.ordinal;
        p_dta->check_sum = ent.check_sum;
    }
    return 0;
}

s32 PFDIR_p_fsfirst(PF_VOLUME* p_vol, PF_STR* p_path, u8 attr, PF_DTA* p_dta)
{
    s32 err;
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_STR dir_path;
    PF_STR file_name;

    err = PFPATH_SplitPathWildcard(p_path, &dir_path, &file_name);
    if (err != 0) {
        return err;
    }
    err = PFENT_GetParentEntryOfPath(&iter, &ent, p_vol, &dir_path);
    if (err != 0) {
        return err;
    }
    if (!(ent.attr & PF_ATTR_DIR)) {
        return 20;
    }

    p_dta->start_cluster = ent.start_cluster;
    p_dta->index = 0;
    p_dta->p_vol = ent.p_vol;
    p_dta->attr_required = attr;
    p_dta->status = 0;
    if (PFSTR_GetCodeMode(&file_name) == PF_CODE_MODE_OEM) {
        PFPATH_CopyPattern(p_dta->pattern, NULL, &file_name);
        p_dta->status |= PF_DTA_STAT_OEM_PATTERN;
    } else {
        PFPATH_CopyPattern(p_dta->pattern, p_dta->w_pattern, &file_name);
        p_dta->status |= PF_DTA_STAT_UNICODE_PATTERN;
    }
    return PFDIR_p_fsnext(p_dta);
}

s32 PFDIR_ChmodEntry(PF_DTA* p_dta, PF_DIR_ENT* p_ent, u32 mode, u8 attr)
{
    s32 err;
    u16 time;

    if (attr & 0x40) {
        attr &= ~0x40;
    }
    if (p_ent->attr & PF_ATTR_DIR) {
        if (attr & PF_ATTR_SYSTEM) {
            return 10;
        }
        if (PFDIR_IsOpened(p_ent)) {
            return 19;
        }
        p_ent->attr = attr | PF_ATTR_DIR;
    } else {
        if (attr & PF_ATTR_DIR) {
            return 10;
        }
        if (PFFILE_IsOpened(p_ent)) {
            return 19;
        }
        p_ent->attr = attr;
    }

    PFENT_getcurrentDateTimeForEnt(&p_ent->access_date, &time);
    err = PFENT_UpdateSFNEntry(p_ent, 0);
    if (err != 0) {
        return err;
    }
    if (!(mode & 0x80000000)) {
        p_dta->attr = p_ent->attr;
    }
    err = PFCACHE_FlushDataCacheSpecific(p_ent->p_vol, NULL);
    if (err != 0) {
        return err;
    }
    return 0;
}
