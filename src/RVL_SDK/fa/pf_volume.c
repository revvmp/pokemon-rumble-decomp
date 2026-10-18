#include <revolution/fa/pf_volume.h>
#include <revolution/fa/pf_str.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_sector.h>
#include <revolution/fa/pf_cp932.h>
#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_dir.h>
#include <revolution/fa/pf_system.h>
#include <revolution/fa/pf_filelock.h>
#include <revolution/fa/nand_drv.h>
#include <stddef.h>

s32 PFVOL_DoMountVolume(PF_VOLUME* p_vol);
s32 PFVOL_p_unmount(PF_VOLUME* p_vol, u32 mode);
s32 PFVOL_p_format(PF_VOLUME* p_vol, const u8* param);
s32 PFVOL_p_setvol(PF_VOLUME* p_vol, const s8* label);
s32 PFVOL_p_getvol(PF_VOLUME* p_vol, PF_VOL_INF* p_info);
s32 PFVOL_p_rmvvol(PF_VOLUME* p_vol);

PF_VOLUME_SET pf_vol_set;

static inline u32 PFVOL_CheckContextRegistered(s32 context_id) {
    u32 i;
    for (i = 1; i < 4; i++) {
        pf_vol_set.context[i].stat &= 1;
        if (pf_vol_set.context[i].stat != 0 && pf_vol_set.context[i].context_id == context_id) {
            return 1;
        }
    }
    return 0;
}

static inline void PFVOL_SetUpVolumeForMount(PF_VOLUME* p_vol) {
    p_vol->last_error = 0;
    p_vol->last_driver_error = 0;
}

static inline s32 PFVOL_InitCurrentDir(PF_VOLUME* p_vol) {
    s32 err;
    u32 i;
    s32 context_id;
    if ((p_vol->flags & 8) == 0) {
        return 9;
    }
    PFSYS_GetCurrentContextID(&context_id);
    p_vol->current_dir[0].stat |= 1;
    if (PFVOL_CheckContextRegistered(context_id)) {
        p_vol->current_dir[1].stat |= 1;
        p_vol->current_dir[1].context_id = context_id;
    }
    for (i = 0; i < 4; i++) {
        err = PFENT_GetRootDir(p_vol, &p_vol->current_dir[i].dir_entry);
        if (err) {
            return err;
        }
    }
    return 0;
}

static inline s32 PFVOL_FinalizeCurrentDir(PF_VOLUME* p_vol) {
    u32 i;
    if ((p_vol->flags & 8) == 0) {
        return 9;
    }
    for (i = 0; i < 4; i++) {
        p_vol->current_dir[i].stat = 0;
    }
    return 0;
}

static inline s32 PFVOL_DoUnmountVolume(PF_VOLUME* p_vol, u32 mode) {
    s32 err;
    err = PFDRV_unmount(p_vol, mode);
    if (err) {
        return err;
    }
    PFVOL_FinalizeCurrentDir(p_vol);
    p_vol->flags &= ~8;
    return 0;
}

static inline void PFVOL_UnmountVolumeByEject(PF_VOLUME* p_vol) {
    PFFILE_FinalizeAllFiles(p_vol);
    PFDIR_FinalizeAllDirs(p_vol);
    PFVOL_DoUnmountVolume(p_vol, 1);
    PFDRV_ClearUnmountRequested(p_vol);
    pf_vol_set.num_mounted_volumes--;
}

static inline s32 PFVOL_p_mount(PF_VOLUME* p_vol) {
    s32 err;
    if ((p_vol->flags & 8) == 0) {
        err = PFVOL_DoMountVolume(p_vol);
        if (err) {
            return err;
        }
        p_vol->fsi_flag &= ~7;
        PFDRV_ClearUnmountRequested(p_vol);
        pf_vol_set.num_mounted_volumes++;
    }
    PFDRV_ClearMountRequested(p_vol);
    return 0;
}

static inline s32 PFVOL_CheckMediaInsert(PF_VOLUME* p_vol) {
    s32 err;
    if (PFDRV_IsInserted(p_vol)) {
        if (PFDRV_IsUnmountRequested(p_vol) && (p_vol->flags & 8) == 0) {
            PFVOL_UnmountVolumeByEject(p_vol);
        }
        if (PFDRV_IsMountRequested(p_vol)) {
            err = PFVOL_p_mount(p_vol);
            if (err != 0) {
                return err;
            }
        }
    } else {
        if (PFDRV_IsUnmountRequested(p_vol) && (p_vol->flags & 8) == 0) {
            PFVOL_UnmountVolumeByEject(p_vol);
        }
        if (PFDRV_IsMountRequested(p_vol)) {
            PFDRV_ClearMountRequested(p_vol);
        }
    }
    return 0;
}

static inline s32 PFVOL_p_attach(PF_VOLUME* p_vol, PF_DRV_TBL* p_drv, s16 vol_idx) {
    s32 err;
    p_vol->num_free_clusters = -1;
    p_vol->last_free_cluster = -1;
    pf_memset(p_vol, 0, sizeof(PF_VOLUME));
    p_vol->p_part = p_drv->p_part;
    p_vol->drv_char = vol_idx + 'A';
    p_vol->tail_entry.tracker_size = 1;
    p_vol->tail_entry.tracker_bits = p_vol->tail_entry.tracker_buff;
    err = PFDRV_init(p_vol);
    if (err) {
        return err;
    }
    PFCACHE_SetCache(p_vol, p_drv->cache->pages, p_drv->cache->buffers,
        p_drv->cache->num_fat_pages, p_drv->cache->num_data_pages);
    PFCACHE_SetFATBufferSize(p_vol, p_drv->cache->num_fat_buf_size);
    PFCACHE_SetDataBufferSize(p_vol, p_drv->cache->num_data_buf_size);
    return 0;
}

static inline s32 PFVOL_p_detach(PF_VOLUME* p_vol) {
    s32 err = PFDRV_finalize(p_vol);
    if (err) {
        return err;
    }
    return 0;
}

s32 PFVOL_DoMountVolume(PF_VOLUME* p_vol) {
    s32 result;
    s32 err;
    PFVOL_SetUpVolumeForMount(p_vol);
    result = PFDRV_mount(p_vol);
    if (result) {
        return result;
    }
    if (!p_vol->bpb.bytes_per_sector || (p_vol->bpb.bytes_per_sector & 0x1FF) != 0) {
        return 15;
    }
    if ((p_vol->bpb.bytes_per_sector >> 9) > p_vol->cache.num_fat_pages ||
        2 * (p_vol->bpb.bytes_per_sector >> 9) > p_vol->cache.num_data_pages) {
        return 30;
    }
    result = PFCACHE_InitCaches(p_vol);
    if (result) {
        return result;
    }
    p_vol->flags |= 8;
    err = PFVOL_InitCurrentDir(p_vol);
    if (!err && (p_vol->flags & 0x40) != 0) {
        err = PFDRV_format(p_vol, p_vol->format_param);
        if (!err && (p_vol->flags & 0x80) == 0) {
            err = PFFAT_InitFATRegion(p_vol);
            if (!err) {
                err = PFENT_MakeRootDir(p_vol);
            }
        }
    }
    if (err) {
        p_vol->flags &= ~8;
    }
    return err;
}

s32 PFVOL_p_unmount(PF_VOLUME* p_vol, u32 mode) {
    s32 err;
    s32 next_err;
    err = 0;
    if ((p_vol->flags & 8) == 0) {
        return 9;
    }
    PFFILE_FinalizeAllFiles(p_vol);
    PFDIR_FinalizeAllDirs(p_vol);
    next_err = PFCACHE_FlushAllCaches(p_vol);
    if (next_err) {
        err = next_err;
    }
    if (next_err == 0 || (mode & 1) != 0) {
        PFCACHE_FreeAllCaches(p_vol);
        next_err = PFVOL_DoUnmountVolume(p_vol, mode);
        if (next_err != 0 && err == 0) {
            err = next_err;
        }
    }
    if (err == 0 || (mode & 1) != 0) {
        PFDRV_ClearMountRequested(p_vol);
        PFDRV_ClearUnmountRequested(p_vol);
        pf_vol_set.num_mounted_volumes--;
    }
    return err;
}

s32 PFVOL_p_format(PF_VOLUME* p_vol, const u8* param) {
    s32 err;
    u32 free_clusters;
    if (!p_vol) {
        return 10;
    }
    if ((p_vol->flags & 2) != 0) {
        return 11;
    }
    err = PFDRV_format(p_vol, param);
    if (err) {
        return err;
    }
    if ((p_vol->flags & 8) != 0) {
        PFCACHE_FreeAllCaches(p_vol);
        err = PFVOL_p_unmount(p_vol, 0);
        if (err) {
            return err;
        }
    }
    err = PFVOL_p_mount(p_vol);
    if (err) {
        return err;
    }
    if ((p_vol->flags & 2) != 0) {
        return 11;
    }
    if ((p_vol->flags & 0x80) != 0) {
        if (p_vol->bpb.fat_type == PDM_FAT_32 && (p_vol->fsi_flag & 2) != 0) {
            p_vol->fsi_flag |= PF_FSI_FLAG_FREE_COUNT_VALID;
            PFFAT_RefreshFSINFO(p_vol);
        } else {
            p_vol->fsi_flag &= ~PF_FSI_FLAG_FREE_COUNT_VALID;
            if (p_vol->bpb.fat_type != PDM_FAT_32) {
                p_vol->fsi_flag &= ~3;
            }
            err = PFFAT_CountFreeClusters(p_vol, &free_clusters);
            if (err) {
                return err;
            }
        }
        return 0;
    } else {
        err = PFFAT_InitFATRegion(p_vol);
        if (err) {
            return err;
        }
        err = PFENT_MakeRootDir(p_vol);
        if (err) {
            return err;
        }
        if (p_vol->bpb.fat_type != PDM_FAT_32) {
            p_vol->fsi_flag &= ~3;
        }
        if (p_vol->bpb.fat_type == PDM_FAT_32 && (p_vol->fsi_flag & 2) != 0) {
            err = PFFAT_RefreshFSINFO(p_vol);
            if (err) {
                return err;
            }
        }
    }
    return 0;
}

s32 PFVOL_p_setvol(PF_VOLUME* p_vol, const s8* label) {
    PF_FFD ffd;
    PF_FAT_HINT hint;
    PF_DIR_ENT root_dir;
    PF_DIR_ENT ent;
    PF_STR pattern;
    u32 logical_pos;
    u32 physical_pos;
    u32 prev_chain[2];
    s32 err;
    err = PFENT_GetRootDir(p_vol, &root_dir);
    if (err) {
        return err;
    }
    PFFAT_InitFFD(&ffd, &hint, p_vol, &root_dir.start_cluster);
    PFSTR_InitStr(&pattern, (const s8*)"*", PF_CODE_MODE_OEM);
    PFSTR_SetMarkPos(&pattern, NULL);
    err = PFENT_findEntry(&ffd, &ent, 0, &pattern, PF_ATTR_VOLUME, 0, &logical_pos, &physical_pos);
    if (err != 0 && err != 3) {
        return err;
    } else {
        if (physical_pos != 999999) {
            if (ent.start_cluster == 1) {
                ent.start_cluster = 0;
            }
        } else {
            PFSTR_InitStr(&pattern, (const s8*)"", PF_CODE_MODE_OEM);
            PFSTR_SetMarkPos(&pattern, NULL);
            err = PFENT_AllocateEntry(&ent, 1, &ffd, prev_chain, &pattern);
            if (err) {
                return err;
            }
            ent.long_name[0] = 0;
            ent.start_cluster = 0;
            ent.file_size = 0;
            ent.p_vol = p_vol;
            ent.small_letter_flag = 0;
            ent.attr = PF_ATTR_VOLUME;
            ent.path_len = 3;
            ent.create_date = 0;
            ent.create_time = 0;
            ent.access_date = 0;
            ent.create_time_ms = 0;
        }
        pf_memcpy(ent.short_name, label, 12);
        PFENT_getcurrentDateTimeForEnt(&ent.modify_date, &ent.modify_time);
        return PFENT_UpdateSFNEntry(&ent, 0);
    }
    return err;
}

s32 PFVOL_p_getvol(PF_VOLUME* p_vol, PF_VOL_INF* p_info) {
    PF_FFD ffd;
    PF_FAT_HINT hint;
    PF_DIR_ENT root_dir;
    PF_DIR_ENT ent;
    PF_STR pattern;
    u32 logical_pos;
    u32 physical_pos;
    s32 err;
    err = PFENT_GetRootDir(p_vol, &root_dir);
    if (err) {
        return err;
    }
    PFFAT_InitFFD(&ffd, &hint, p_vol, &root_dir.start_cluster);
    PFSTR_InitStr(&pattern, (const s8*)"*", PF_CODE_MODE_OEM);
    PFSTR_SetMarkPos(&pattern, (const s8*)"*");
    err = PFENT_findEntry(&ffd, &ent, 0, &pattern, PF_ATTR_VOLUME, 0, &logical_pos, &physical_pos);
    if (err != 0 && err != 3) {
        return err;
    } else {
        if (physical_pos != 999999) {
            p_info->modify_date = ent.modify_date;
            p_info->modify_time = ent.modify_time;
            p_info->attr = ent.attr;
            pf_strcpy(p_info->label, ent.short_name);
        } else {
            p_info->modify_date = 0;
            p_info->modify_time = 0;
            p_info->attr = PF_ATTR_VOLUME;
            PFDRV_LoadVolumeLabelFromBuf((u8*)p_info->label, p_vol);
        }
        return 0;
    }
    return err;
}

s32 PFVOL_p_rmvvol(PF_VOLUME* p_vol) {
    PF_FFD ffd;
    PF_FAT_HINT hint;
    PF_DIR_ENT root_dir;
    PF_DIR_ENT ent;
    PF_STR pattern;
    u32 logical_pos;
    u32 physical_pos;
    u32 success_size;
    u8 deleted[1] = {PF_ENT_DELETED};
    s32 err;
    err = PFENT_GetRootDir(p_vol, &root_dir);
    if (err) {
        return err;
    }
    PFFAT_InitFFD(&ffd, &hint, p_vol, &root_dir.start_cluster);
    PFSTR_InitStr(&pattern, (const s8*)"*", PF_CODE_MODE_OEM);
    PFSTR_SetMarkPos(&pattern, (const s8*)"*");
    err = PFENT_findEntry(&ffd, &ent, 0, &pattern, PF_ATTR_VOLUME, 0, &logical_pos, &physical_pos);
    if (err != 0 && err != 3) {
        return err;
    } else {
        if (physical_pos != 999999) {
            err = PFSEC_WriteData(p_vol, deleted, ent.entry_sector, ent.entry_offset, 1, &success_size, 0);
            if (err) {
                return err;
            }
            if (success_size != 1) {
                return 17;
            }
        }
        return 0;
    }
    return err;
}

s32 PFVOL_InitModule(u32 config, void* param) {
    s32 vol_idx;
    u32 i;
    if ((config & 0x10000) != 0) {
        pf_vol_set.config |= 0x10000;
    } else {
        pf_vol_set.config &= ~0x10000;
    }
    for (i = 0; i < 4; i++) {
        pf_vol_set.current_vol[i].p_vol = &pf_vol_set.volumes[0];
    }
    pf_vol_set.current_vol[0].stat |= 1;
    pf_vol_set.num_attached_drives = 0;
    pf_vol_set.num_mounted_volumes = 0;
    if ((config & 0x10000) != 0) {
        pf_vol_set.config |= 0x10000;
    } else {
        pf_vol_set.config &= ~0x10000;
    }
    pf_vol_set.param = param;
    pf_vol_set.last_error = 0;
    pf_vol_set.last_driver_error = 0;
    pf_vol_set.setting = 1;
    pf_vol_set.codeset.oem2unicode = &PFCODE_CP932_OEM2Unicode;
    pf_vol_set.codeset.unicode2oem = &PFCODE_CP932_Unicode2OEM;
    pf_vol_set.codeset.oem_char_width = &PFCODE_CP932_OEMCharWidth;
    pf_vol_set.codeset.is_oem_mb_char = &PFCODE_CP932_isOEMMBchar;
    pf_vol_set.codeset.unicode_char_width = &PFCODE_CP932_UnicodeCharWidth;
    pf_vol_set.codeset.is_unicode_mb_char = &PFCODE_CP932_isUnicodeMBchar;
    for (vol_idx = 0; vol_idx < 26; vol_idx++) {
        pf_memset(&pf_vol_set.volumes[vol_idx], 0, sizeof(PF_VOLUME));
    }
    PF_InitLockFile();
    return 0;
}

s32 PFVOL_CheckForRead(PF_VOLUME* p_vol) {
    s32 err;
    err = PFVOL_CheckMediaInsert(p_vol);
    if (err) {
        return err;
    }
    if ((p_vol->flags & 8) != 0) {
        return 0;
    } else {
        return 9;
    }
}

s32 PFVOL_CheckForWrite(PF_VOLUME* p_vol) {
    s32 err;
    err = PFVOL_CheckMediaInsert(p_vol);
    if (err) {
        return err;
    }
    if ((p_vol->flags & 8) == 0) {
        return 9;
    } else if ((p_vol->flags & 2) != 0) {
        return 11;
    } else {
        return 0;
    }
}

s32 PFVOL_IsCurrentDirCluster(PF_VOLUME* p_vol, u32 cluster) {
    s32 err = 0;
    s32 context_id;
    u32 i;
    if ((p_vol->flags & 8) == 0) {
        return 9;
    }
    PFSYS_GetCurrentContextID(&context_id);
    for (i = 1; i < 4; i++) {
        if ((p_vol->current_dir[i].stat & 1) != 0 && p_vol->current_dir[i].context_id == context_id) {
            break;
        }
    }
    if (i == 4) {
        i = 0;
    }
    if (cluster == p_vol->current_dir[i].dir_entry.start_cluster) {
        err = -1;
    }
    return err;
}

s32 PFVOL_SetCurrentDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent) {
    s32 context_id;
    u32 i;
    if ((p_vol->flags & 8) == 0) {
        return 9;
    }
    PFSYS_GetCurrentContextID(&context_id);
    for (i = 1; i < 4; i++) {
        if ((p_vol->current_dir[i].stat & 1) != 0 && p_vol->current_dir[i].context_id == context_id) {
            p_vol->current_dir[i].dir_entry = *p_ent;
            break;
        }
    }
    if (i == 4) {
        if (PFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 4; i++) {
                if ((p_vol->current_dir[i].stat & 1) == 0) {
                    p_vol->current_dir[i].stat |= 1;
                    p_vol->current_dir[i].context_id = context_id;
                    p_vol->current_dir[i].dir_entry = *p_ent;
                    break;
                }
            }
        }
    }
    p_vol->current_dir[0].dir_entry = *p_ent;
    return 0;
}

s32 PFVOL_GetCurrentDir(PF_VOLUME* p_vol, PF_DIR_ENT* p_current_dir) {
    s32 context_id;
    u32 i;
    if ((p_vol->flags & 8) == 0) {
        return 9;
    }
    PFSYS_GetCurrentContextID(&context_id);
    for (i = 1; i < 4; i++) {
        if ((p_vol->current_dir[i].stat & 1) != 0 && p_vol->current_dir[i].context_id == context_id) {
            *p_current_dir = p_vol->current_dir[i].dir_entry;
            break;
        }
    }
    if (i == 4) {
        if (PFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 4; i++) {
                if ((p_vol->current_dir[i].stat & 1) == 0) {
                    p_vol->current_dir[i].stat |= 1;
                    p_vol->current_dir[i].context_id = context_id;
                    *p_current_dir = p_vol->current_dir[i].dir_entry;
                    break;
                }
            }
        } else {
            *p_current_dir = p_vol->current_dir[0].dir_entry;
        }
    }
    return 0;
}

void PFVOL_SetCurrentVolume(PF_VOLUME* p_vol) {
    s32 context_id;
    u32 i;
    PFSYS_GetCurrentContextID(&context_id);
    for (i = 1; i < 4; i++) {
        if ((pf_vol_set.current_vol[i].stat & 1) != 0 && pf_vol_set.current_vol[i].context_id == context_id) {
            pf_vol_set.current_vol[i].p_vol = p_vol;
            break;
        }
    }
    if (i == 4) {
        if (PFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 4; i++) {
                if ((pf_vol_set.current_vol[i].stat & 1) == 0) {
                    pf_vol_set.current_vol[i].stat |= 1;
                    pf_vol_set.current_vol[i].context_id = context_id;
                    pf_vol_set.current_vol[i].p_vol = p_vol;
                    break;
                }
            }
        }
    }
    pf_vol_set.current_vol[0].p_vol = p_vol;
}

PF_VOLUME* PFVOL_GetCurrentVolume(void) {
    PF_VOLUME* p_vol = NULL;
    s32 context_id;
    u32 i;
    PFSYS_GetCurrentContextID(&context_id);
    for (i = 1; i < 4; i++) {
        if ((pf_vol_set.current_vol[i].stat & 1) != 0 && pf_vol_set.current_vol[i].context_id == context_id) {
            p_vol = pf_vol_set.current_vol[i].p_vol;
            break;
        }
    }
    if (i == 4) {
        if (PFVOL_CheckContextRegistered(context_id)) {
            for (i = 1; i < 4; i++) {
                if ((pf_vol_set.current_vol[i].stat & 1) == 0) {
                    pf_vol_set.current_vol[i].stat |= 1;
                    pf_vol_set.current_vol[i].context_id = context_id;
                    p_vol = pf_vol_set.current_vol[i].p_vol;
                    break;
                }
            }
        } else {
            p_vol = pf_vol_set.current_vol[0].p_vol;
        }
    }
    return p_vol;
}

PF_VOLUME* PFVOL_GetVolumeFromDrvChar(s8 drv_char) {
    s16 vol_idx;
    vol_idx = pf_toupper(drv_char) - 'A';
    if (vol_idx < 0 || 26 <= vol_idx) {
        return NULL;
    } else {
        return &pf_vol_set.volumes[vol_idx];
    }
}

s32 PFVOL_derrnum(s8 drv_char) {
    return PFVOL_GetVolumeFromDrvChar(drv_char)->last_driver_error;
}

s32 PFVOL_errnum(void) {
    return pf_vol_set.last_error;
}

s32 PFVOL_setvol(s8 drv_char, PF_STR* p_name) {
    PF_VOLUME* p_vol;
    u32 name_length;
    s32 err;
    s8 label[12];
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    name_length = PFSTR_StrLen(p_name);
    err = PFVOL_CheckForWrite(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    PFSTR_ToUpperNStr(p_name, 11, label);
    if (name_length < 11) {
        label[name_length] = 0;
    }
    err = PFVOL_p_setvol(p_vol, label);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
    } else {
        err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
        if (err) {
            pf_vol_set.last_error = err;
            p_vol->last_error = err;
        }
    }
    if (!err) {
        err = PFDRV_StoreVolumeLabelToBPB(p_vol, label);
        if (err) {
            pf_vol_set.last_error = err;
            p_vol->last_error = err;
            return err;
        }
    }
    return err;
}

s32 PFVOL_getvol(s8 drv_char, PF_VOL_INF* p_info) {
    PF_VOLUME* p_vol;
    s32 err;
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    if (pf_memcmp(p_vol->label, (const s8*)"NO NAME    ", 11) == 0) {
        pf_vol_set.last_error = 3;
        p_vol->last_error = 3;
        return 3;
    }
    err = PFVOL_CheckForRead(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFVOL_p_getvol(p_vol, p_info);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    return 0;
}

s32 PFVOL_rmvvol(s8 drv_char) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    s32 err;
    if (pf_memcmp(p_vol->label, (const s8*)"NO NAME    ", 11) == 0) {
        pf_vol_set.last_error = 3;
        p_vol->last_error = 3;
        return 3;
    }
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFDRV_StoreVolumeLabelToBPB(p_vol, (const s8*)"NO NAME    ");
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFVOL_p_rmvvol(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
    }
    return err;
}

s32 PFVOL_getdev(s8 drv_char, PF_DEV_INF* p_info) {
    PF_VOLUME* p_vol;
    s32 err;
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    err = PFVOL_CheckForRead(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_info->bps = p_vol->bpb.bytes_per_sector;
    p_info->spc = p_vol->bpb.sectors_per_cluster;
    p_info->cls = p_vol->bpb.num_clusters;
    err = PFFAT_CountFreeClusters(p_vol, &p_info->ecl);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    return 0;
}

s32 PFVOL_buffering(s8 drv_char, u8 mode) {
    PF_VOLUME* p_vol;
    s32 err;
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    err = PFVOL_CheckForWrite(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if ((mode & 4) && !(p_vol->cache.mode & 1)) {
        err = PFCACHE_FlushFATCache(p_vol);
        if (err) {
            pf_vol_set.last_error = err;
            p_vol->last_error = err;
            return err;
        }
    }
    if (mode & 1) {
        err = PFCACHE_SetWriteThrough(p_vol);
        if (err) {
            pf_vol_set.last_error = err;
            p_vol->last_error = err;
            return err;
        }
    } else {
        err = PFCACHE_ClearWriteThrough(p_vol);
        if (err) {
            pf_vol_set.last_error = err;
            p_vol->last_error = err;
            return err;
        }
    }
    if (mode & 2) {
        p_vol->cache.mode |= 2;
    } else {
        p_vol->cache.mode &= ~2;
    }
    if (mode & 4) {
        p_vol->cache.mode |= 4;
    } else {
        p_vol->cache.mode &= ~4;
    }
    return 0;
}

s32 PFVOL_attach(PF_DRV_TBL* p_drv, const char* path, u32 size) {
    s32 vol_idx;
    PF_VOLUME* p_vol;
    s32 err;
    if (!p_drv->cache->num_fat_buf_size) {
        p_drv->cache->num_fat_buf_size = 1;
    }
    if (!p_drv->cache->num_data_buf_size) {
        p_drv->cache->num_data_buf_size = 1;
    }
    p_drv->stat = 0;
    p_drv->drive = 0;
    if (path && pfd_nanddrv_register_file((const s8*)path, size)) {
        pf_vol_set.last_error = 17;
        return 17;
    }
    for (vol_idx = 0; vol_idx < 26; vol_idx++) {
        p_vol = &pf_vol_set.volumes[vol_idx];
        if ((p_vol->flags & 1) == 0) {
            break;
        }
    }
    if (vol_idx < 0 || 26 <= vol_idx || pf_vol_set.num_attached_drives < 0 || 26 <= pf_vol_set.num_attached_drives) {
        pf_vol_set.last_error = 4;
        return 4;
    } else {
    err = PFVOL_p_attach(p_vol, p_drv, vol_idx);
    if (err) {
        pf_vol_set.last_error = err;
        return err;
    }
    p_vol->flags |= 1;
    p_drv->stat |= 1;
    p_vol->drv_char = vol_idx + 'A';
    p_drv->drive = vol_idx + 'A';
    pf_vol_set.num_attached_drives++;
    if (PFDRV_IsInserted(p_vol)) {
        p_drv->stat |= 0x10;
        err = PFVOL_p_mount(p_vol);
        if (err) {
            pf_vol_set.last_error = err;
            p_vol->last_error = err;
            return 0;
        }
        p_drv->stat |= 2;
    }
    return 0;
    }
}

s32 PFVOL_detach(s8 drv_char) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    s32 err;
    if ((p_vol->flags & 8) != 0) {
        pf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    if ((p_vol->flags & 1) == 0) {
        pf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    err = PFVOL_p_detach(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->flags &= ~1;
    pf_vol_set.num_attached_drives--;
    return 0;
}

s32 PFVOL_format(s8 drv_char, u8* param) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 mount_requested;
    mount_requested = 0;
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    if ((p_vol->flags & 1) == 0) {
        pf_vol_set.last_error = 10;
        p_vol->last_error = 10;
        return 10;
    }
    if (!PFDRV_IsInserted(p_vol)) {
        pf_vol_set.last_error = 9;
        p_vol->last_error = 9;
        return 9;
    }
    if (PFDRV_IsMountRequested(p_vol)) {
        PFDRV_ClearMountRequested(p_vol);
        mount_requested = 1;
    }
    err = PFVOL_CheckMediaInsert(p_vol);
    if (err) {
        if (mount_requested) {
            p_vol->flags |= 0x10;
        }
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if (p_vol->num_opened_files != 0) {
        PFFILE_FinalizeAllFiles(p_vol);
        PFCACHE_FreeAllCaches(p_vol);
    }
    if (p_vol->num_opened_directories != 0) {
        PFDIR_FinalizeAllDirs(p_vol);
    }
    err = PFVOL_p_format(p_vol, param);
    if (err) {
        if (mount_requested) {
            p_vol->flags |= 0x10;
        }
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->fsi_flag |= PF_FSI_FLAG_FREE_COUNT_VALID;
    if (p_vol->bpb.fat_type == PDM_FAT_32) {
        p_vol->num_free_clusters = p_vol->bpb.num_clusters - 1;
    } else {
        p_vol->num_free_clusters = p_vol->bpb.num_clusters;
    }
    return 0;
}

s32 PFVOL_mount(s8 drv_char) {
    PF_VOLUME* p_vol;
    s32 err;
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    err = PFVOL_CheckMediaInsert(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if (!PFDRV_IsInserted(p_vol)) {
        pf_vol_set.last_error = 9;
        p_vol->last_error = 9;
        return 9;
    }
    if ((p_vol->flags & 8) != 0) {
        return 0;
    }
    err = PFVOL_p_mount(p_vol);
    if (err) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    return 0;
}

s32 PFVOL_unmount(s8 drv_char, u32 mode) {
    PF_VOLUME* p_vol;
    s32 err;
    s32 err2;
    err2 = 0;
    p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    if ((p_vol->flags & 1) == 0) {
        pf_vol_set.last_error = 10;
        return 10;
    }
    PFDRV_ClearMountRequested(p_vol);
    err = PFVOL_CheckMediaInsert(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if ((p_vol->flags & 8) == 0) {
        return 0;
    }
    if (p_vol->num_opened_files != 0 || p_vol->num_opened_directories != 0) {
        err2 = 1;
        pf_vol_set.last_error = 19;
        p_vol->last_error = 19;
        if ((mode & 1) == 0) {
            return 19;
        }
    }
    err = PFVOL_p_unmount(p_vol, mode);
    if (err != 0 && err2 == 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        err2 = 1;
        if ((mode & 1) == 0) {
            err2 = err;
        }
    }
    return err2;
}

s32 PFVOL_setupfsi(s8 drv_char, u16 flags) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    s32 err;
    u32 free_clusters;
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if (p_vol->bpb.fat_type != PDM_FAT_32) {
        pf_vol_set.last_error = 12;
        p_vol->last_error = 12;
        return 12;
    }
    if ((flags & 1) == 1) {
        p_vol->fsi_flag |= 5;
    } else if ((flags & 2) == 2) {
        p_vol->fsi_flag &= ~1;
    }
    if ((flags & 4) == 4) {
        p_vol->fsi_flag |= 2;
        if ((p_vol->fsi_flag & 4) == 0) {
            PFFAT_CountFreeClusters(p_vol, &free_clusters);
        }
    } else if ((flags & 8) == 8) {
        p_vol->fsi_flag &= ~2;
    }
    return 0;
}

s32 PFVOL_setclstlink(s8 drv_char, u32 mode, PF_CLUSTER_LINK_CONFIG* p_info) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    s32 err;
    if (p_vol->num_opened_files != 0) {
        pf_vol_set.last_error = 19;
        p_vol->last_error = 19;
        return 19;
    }
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if (mode == 1) {
        if (p_info->buffer == NULL) {
            pf_vol_set.last_error = 10;
            p_vol->last_error = 10;
            return 10;
        }
        p_vol->cluster_link.flag |= 1;
        p_vol->cluster_link.buffer = p_info->buffer;
        pf_memset(p_info->buffer, 0, p_info->count * sizeof(PF_CLUSTER_LINK));
        p_vol->cluster_link.interval = p_info->interval;
        p_vol->cluster_link.link_max = p_info->count;
    } else {
        p_vol->cluster_link.flag &= ~1;
        p_vol->cluster_link.buffer = NULL;
        p_vol->cluster_link.interval = 0;
        p_vol->cluster_link.link_max = 0;
    }
    return err;
}

s32 PFVOL_sync(s8 drv_char, u32 mode) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    s32 err;
    u16 file_idx;
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    for (file_idx = 0; file_idx < PF_MAX_FILES; file_idx++) {
        if ((p_vol->ufds[file_idx].stat & 1) != 0) {
            err = PFENT_UpdateSFNEntry(&p_vol->ufds[file_idx].p_sfd->dir_entry, 1);
            if (err != 0) {
                pf_vol_set.last_error = err;
                p_vol->last_error = err;
                return err;
            }
        }
    }
    err = PFCACHE_FlushDataCache(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    if (mode == 1) {
        PFCACHE_FreeAllCaches(p_vol);
    }
    return err;
}

s32 PFVOL_settailbuf(s8 drv_char, u32 size, PF_TAIL_BUF* p_buffer) {
    PF_VOLUME* p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
    s32 err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        pf_vol_set.last_error = err;
        p_vol->last_error = err;
        return err;
    }
    p_vol->tail_entry.tracker_size = size;
    p_vol->tail_entry.tracker_bits = (u32*)p_buffer;
    return err;
}

s32 PFVOL_setvolcfg(s8 drv_char, PF_VOLUME_CONFIG* p_config) {
    PF_VOLUME* p_vol;
    if (drv_char == -1) {
        if ((p_config->flags & 0x10000) != 0) {
            pf_vol_set.config |= 0x10000;
        } else if ((p_config->flags & 0x20000) != 0) {
            pf_vol_set.config &= ~0x10000;
        }
    } else {
        p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
        if (p_config->fat_buffer_size == 0) {
            p_config->fat_buffer_size = 1;
        }
        if (p_config->data_buffer_size == 0) {
            p_config->data_buffer_size = 1;
        }
        PFVOL_CheckMediaInsert(p_vol);
        if (p_vol->num_opened_files != 0 || p_vol->num_opened_directories != 0) {
            pf_vol_set.last_error = 19;
            p_vol->last_error = 19;
            return 19;
        }
        if ((p_config->flags & 1) != 0 || (p_config->flags & 2) != 0) {
            PFCACHE_FlushAllCaches(p_vol);
            PFCACHE_FreeAllCaches(p_vol);
            if ((p_config->flags & 1) != 0 && p_config->fat_buffer_size != p_vol->cache.fat_buff_size) {
                PFCACHE_SetFATBufferSize(p_vol, p_config->fat_buffer_size);
                p_config->flags &= ~1;
            }
            if ((p_config->flags & 2) != 0 && p_config->data_buffer_size != p_vol->cache.data_buff_size) {
                PFCACHE_SetDataBufferSize(p_vol, p_config->data_buffer_size);
                p_config->flags &= ~2;
            }
            PFCACHE_InitCaches(p_vol);
        }
        if ((p_config->file_flags & 1) != 0) {
            p_vol->file_config |= 1;
        } else if ((p_config->file_flags & 2) != 0) {
            p_vol->file_config &= ~1;
        }
    }
    return 0;
}

s32 PFVOL_getvolcfg(s8 drv_char, PF_VOLUME_CONFIG* p_config) {
    PF_VOLUME* p_vol;
    if (drv_char == -1) {
        p_config->flags = pf_vol_set.config;
        if ((pf_vol_set.config & 0x10000) == 0) {
            p_config->flags |= 0x20000;
        }
        p_config->file_flags = 0;
        p_config->fat_buffer_size = 0;
        p_config->data_buffer_size = 0;
    } else {
        p_vol = PFVOL_GetVolumeFromDrvChar(drv_char);
        PFVOL_CheckMediaInsert(p_vol);
        p_config->flags = 0;
        p_config->file_flags = p_vol->file_config;
        if ((p_vol->file_config & 1) != 1) {
            p_config->file_flags |= 2;
        }
        p_config->fat_buffer_size = p_vol->cache.fat_buff_size;
        p_config->data_buffer_size = p_vol->cache.data_buff_size;
    }
    return 0;
}

s32 PFVOL_setcode(PF_CHARCODE* p_codeset) {
    if (pf_vol_set.num_mounted_volumes > 0) {
        pf_vol_set.last_error = 36;
        return 36;
    }
    pf_vol_set.codeset.oem2unicode = p_codeset->oem2unicode;
    pf_vol_set.codeset.unicode2oem = p_codeset->unicode2oem;
    pf_vol_set.codeset.oem_char_width = p_codeset->oem_char_width;
    pf_vol_set.codeset.is_oem_mb_char = p_codeset->is_oem_mb_char;
    pf_vol_set.codeset.unicode_char_width = p_codeset->unicode_char_width;
    pf_vol_set.codeset.is_unicode_mb_char = p_codeset->is_unicode_mb_char;
    return 0;
}

s32 PFVOL_regctx(void) {
    s32 context_id;
    s32 err = PFSYS_GetCurrentContextID(&context_id);
    u32 i;
    s32 free_index;
    if (err != 0) {
        pf_vol_set.last_error = 26;
        return 26;
    }
    free_index = 0;
    for (i = 1; i < 4; i++) {
        if ((pf_vol_set.context[i].stat & 1) != 0 && pf_vol_set.context[i].context_id == context_id) {
            break;
        }
        if ((pf_vol_set.context[i].stat & 1) == 0 && free_index == 0) {
            free_index = i;
        }
    }
    if (i == 4) {
        if (free_index != 0) {
            pf_vol_set.context[free_index].stat = 1;
            pf_vol_set.context[free_index].context_id = context_id;
        } else {
            pf_vol_set.last_error = 26;
            err = 26;
        }
    }
    return err;
}

s32 PFVOL_unregctx(void) {
    s32 context_id;
    s32 err = PFSYS_GetCurrentContextID(&context_id);
    u32 i;
    u32 j;
    u32 volume;
    if (err != 0) {
        pf_vol_set.last_error = 26;
        return 26;
    }
    for (i = 1; i < 4; i++) {
        if ((pf_vol_set.context[i].stat & 1) != 0 && pf_vol_set.context[i].context_id == context_id) {
            pf_vol_set.context[i].stat &= ~1;
            for (j = 1; j < 4; j++) {
                if ((pf_vol_set.current_vol[j].stat & 1) != 0 && pf_vol_set.current_vol[j].context_id == context_id) {
                    pf_vol_set.current_vol[j].stat &= ~1;
                    break;
                }
            }
            for (volume = 0; volume < 26; volume++) {
                for (j = 1; j < 4; j++) {
                    if ((pf_vol_set.volumes[volume].current_dir[j].stat & 1) != 0 && pf_vol_set.volumes[volume].current_dir[j].context_id == context_id) {
                        pf_vol_set.volumes[volume].current_dir[j].stat &= ~1;
                        break;
                    }
                }
            }
            break;
        }
    }
    if (i == 4) {
        pf_vol_set.last_error = 26;
        err = 26;
    }
    return err;
}

s32 PFVOL_setencode(u32 mode) {
    pf_vol_set.setting = (pf_vol_set.setting & ~3) | mode;
    return 0;
}
