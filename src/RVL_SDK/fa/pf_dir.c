#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_entry_iterator.h>
#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_clib.h>

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

    i = 0;
    lpos = 0;
    ppos = 0;
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
