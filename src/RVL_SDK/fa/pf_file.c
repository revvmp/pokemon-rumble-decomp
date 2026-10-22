#include <revolution/fa/pf_file.h>
#include <revolution/fa/pf_cluster.h>
#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_sector.h>
#include <revolution/fa/pf_clib.h>
#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_entry_iterator.h>
#include <revolution/fa/pf_fat.h>
#include <revolution/fa/pf_filelock.h>
#include <stddef.h>

static PF_VOLUME* PFFILE_GetVolume(PF_FILE* p_file) {
    if (p_file == NULL) {
        return NULL;
    }
    return p_file->p_sfd->dir_entry.p_vol;
}

static void PFFILE_Cursor_Recalc(PF_FILE* p_file) {
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);

    p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol->bpb.log2_bytes_per_sector;
    p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol->bpb.bytes_per_sector - 1);
}

static void PFFILE_Cursor_SetPosition(PF_FILE* p_file, u32 position) {
    p_file->cursor.position = position;
    PFFILE_Cursor_Recalc(p_file);
}

static u32 PFFILE_Cursor_AdvanceToWrite(PF_FILE* p_file, u32 n, u32 sector) {
    u32 result = 1;
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);
    u32 wk_sector = sector + ((p_file->cursor.offset_in_sector + n) >> p_vol->bpb.log2_bytes_per_sector);

    if (((p_vol->bpb.bytes_per_sector - 1) & (p_file->cursor.offset_in_sector + n)) != 0) {
        p_file->cursor.sector = sector + ((p_file->cursor.offset_in_sector + n) >> p_vol->bpb.log2_bytes_per_sector);
    } else {
        p_file->cursor.sector = -1;
    }
    p_file->cursor.position += n;
    if (p_file->cursor.position > p_file->p_sfd->dir_entry.file_size) {
        p_file->p_sfd->dir_entry.file_size = p_file->cursor.position;
    }
    PFFILE_Cursor_Recalc(p_file);
    PFCLUSTER_UpdateLastAccessCluster(p_file, wk_sector);
    return result;
}

static void PFFILE_Cursor_Initialize(PF_FILE* p_file) {
    p_file->cursor.sector = -1;
    PFFILE_Cursor_SetPosition(p_file, 0);
}

static u32 PFFILE_Cursor_IsOutOfFile(PF_FILE* p_file) {
    if (p_file->cursor.position >= p_file->p_sfd->dir_entry.file_size) {
        return 1;
    }
    return 0;
}

static void PFFILE_InitSFD(PF_SFD* p_sfd, PF_DIR_ENT* p_dir_entry) {
    p_sfd->stat = 3;
    p_sfd->num_handlers = 0;
    p_sfd->dir_entry = *p_dir_entry;
    p_sfd->lock.mode = 0;
    p_sfd->lock.count = 0;
    p_sfd->lock.wcount = 0;
    p_sfd->lock.owner = NULL;
    p_sfd->lock.resource = 0;
    PFFAT_InitFFD(&p_sfd->ffd, NULL, p_dir_entry->p_vol, &p_sfd->dir_entry.start_cluster);
}

static void PFFILE_FinalizeSFD(PF_SFD* p_sfd) {
    p_sfd->stat = 0;
    PFFAT_FinalizeFFD(&p_sfd->ffd);
}

static void PFFILE_InitUFD(PF_FILE* p_file, s32 open_mode) {
    p_file->stat = 1;
    p_file->open_mode = open_mode;
    p_file->last_error = 0;
    p_file->lock_count = 0;
    PFFAT_InitHint(&p_file->hint);
    PFFILE_Cursor_Initialize(p_file);
}

static void PFFILE_FinalizeUFD(PF_FILE* p_file) {
    p_file->stat &= ~0x01;
}

static PF_FILE* PFFILE_GetFreeUFD(PF_VOLUME* p_vol) {
    s32 i;

    for (i = 0; i < 5; i++) {
        if ((p_vol->ufds[i].stat & 0x01) == 0) {
            return &p_vol->ufds[i];
        }
    }
    return NULL;
}

static s32 PFFILE_ReleaseUFD(PF_FILE* p_file) {
    p_file->stat &= ~0x01;
    return 0;
}

static void PFFILE_EmptyFile(PF_FFD* p_ffd, PF_DIR_ENT* p_ent) {
    if (p_ent->start_cluster >= 2 && p_ent->start_cluster != -1 && p_ent->file_size != 0) {
        PFFAT_FreeChain(p_ffd, p_ent->start_cluster, -1, p_ent->file_size);
    }
    p_ent->start_cluster = 0;
    p_ent->file_size = 0;
    PFENT_getcurrentDateTimeForEnt(&p_ent->modify_date, &p_ent->modify_time);
    p_ent->access_date = p_ent->modify_date;
}

static s32 PFFILE_p_fadjust(PF_FILE* p_file) {
    s32 err;
    if ((p_file->open_mode & 8) == 0 && (p_file->open_mode & 2) != 0) {
        return 10;
    }
    if ((p_file->p_sfd->lock.mode & 3) != 0 &&
        ((p_file->p_sfd->lock.mode & 1) != 0 ||
        ((p_file->p_sfd->lock.mode & 2) != 0 && p_file->lock_count == 0))) {
        return 25;
    }
    err = PFCLUSTER_FreeUnusedCluster(p_file);
    if (err != 0) {
        return err;
    }
    return 0;
}

static u32 PFFILE_Cursor_AdvanceToRead(PF_FILE* p_file, u32 n, u32 sector) {
    u32 result;
    PF_VOLUME* p_vol;
    u32 wk_sector;
    result = 1;
    p_vol = PFFILE_GetVolume(p_file);
    wk_sector = sector + ((p_file->cursor.offset_in_sector + n) >> p_vol->bpb.log2_bytes_per_sector);

    if ((p_file->cursor.position + n) < p_file->p_sfd->dir_entry.file_size) {
        if (((p_vol->bpb.bytes_per_sector - 1) & (p_file->cursor.offset_in_sector + n)) != 0) {
            p_file->cursor.sector = wk_sector;
        } else {
            p_file->cursor.sector = -1;
        }
        p_file->cursor.position += n;
    } else if (p_file->p_sfd->dir_entry.file_size == 0) {
        p_file->cursor.sector = -1;
        p_file->cursor.position = 0;
        if (n != 0) {
            result = 0;
        }
    } else {
        p_file->cursor.sector = -1;
        p_file->cursor.position = p_file->p_sfd->dir_entry.file_size;
        result = 0;
    }
    PFFILE_Cursor_Recalc(p_file);
    PFCLUSTER_UpdateLastAccessCluster(p_file, wk_sector);
    return result;
}

static s32 PFFILE_Cursor_ReadHeadSector(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_read) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 max_readable_size;
    u32 success_size;

    *p_size_read = 0;
    if (p_file->cursor.offset_in_sector == 0) {
        return 0;
    }
    p_vol = PFFILE_GetVolume(p_file);
    max_readable_size = p_vol->bpb.bytes_per_sector - p_file->cursor.offset_in_sector;
    if (size > max_readable_size) {
        size = max_readable_size;
    }
    if ((p_file->cursor.position + size) > p_file->p_sfd->dir_entry.file_size) {
        size = p_file->p_sfd->dir_entry.file_size - p_file->cursor.position;
        if (size < max_readable_size) {
            return 0;
        }
    }
    if (p_file->cursor.sector == -1) {
        err = PFFAT_GetSectorSpecified(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, 0, &p_file->cursor.sector);
        if (err != 0) {
            return err;
        }
        if (p_file->cursor.sector == -1) {
            PFFILE_Cursor_MoveToEnd(p_file);
            return 28;
        }
        if ((p_file->cursor.position + size) > p_file->p_sfd->dir_entry.file_size) {
            PFFILE_Cursor_MoveToEnd(p_file);
            return 27;
        }
    }
    err = PFSEC_ReadData(p_vol, p_buf, p_file->cursor.sector, p_file->cursor.offset_in_sector, size, &success_size, 1);
    if (err != 0 && success_size == 0) {
        return err;
    }
    *p_size_read = success_size;
    PFFILE_Cursor_AdvanceToRead(p_file, success_size, p_file->cursor.sector);
    if (success_size != size) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_ReadBodySectors(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_read) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 num_sector;
    u32 max_readable_size;
    u32 success_size;

    *p_size_read = 0;
    p_vol = PFFILE_GetVolume(p_file);
    num_sector = 0;
    err = PFFAT_GetContinuousSector(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, size, &p_file->cursor.sector, &num_sector);
    if (err != 0) {
        return err;
    }
    if (p_file->cursor.sector == -1) {
        PFFILE_Cursor_MoveToEnd(p_file);
        return 28;
    }
    max_readable_size = num_sector << p_vol->bpb.log2_bytes_per_sector;
    if (size > max_readable_size) {
        size = max_readable_size;
    }
    if ((p_file->cursor.position + size) > p_file->p_sfd->dir_entry.file_size) {
        size = p_file->p_sfd->dir_entry.file_size - p_file->cursor.position;
        size -= size & (p_vol->bpb.bytes_per_sector - 1);
        if (size < p_vol->bpb.bytes_per_sector) {
            return 0;
        }
    }
    err = PFSEC_ReadData(p_vol, p_buf, p_file->cursor.sector, p_file->cursor.offset_in_sector, size, &success_size, 1);
    if (err != 0 && success_size == 0) {
        return err;
    }
    *p_size_read = success_size;
    PFFILE_Cursor_AdvanceToRead(p_file, success_size, p_file->cursor.sector);
    if (success_size != size - (size & (p_vol->bpb.bytes_per_sector - 1))) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_ReadTailSector(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_read) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 success_size;

    *p_size_read = 0;
    p_vol = PFFILE_GetVolume(p_file);
    if (size == 0) {
        return 0;
    }
    if ((p_file->cursor.position + size) > p_file->p_sfd->dir_entry.file_size) {
        size = p_file->p_sfd->dir_entry.file_size - p_file->cursor.position;
        if (size == 0) {
            return 0;
        }
    }
    if (p_file->cursor.sector == -1) {
        err = PFFAT_GetSectorSpecified(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, 0, &p_file->cursor.sector);
        if (err != 0) {
            return err;
        }
        if (p_file->cursor.sector == -1) {
            PFFILE_Cursor_MoveToEnd(p_file);
            return 28;
        }
        if ((p_file->cursor.position + size) > p_file->p_sfd->dir_entry.file_size) {
            PFFILE_Cursor_MoveToEnd(p_file);
            return 27;
        }
    }
    err = PFSEC_ReadData(p_vol, p_buf, p_file->cursor.sector, p_file->cursor.offset_in_sector, size, &success_size, 1);
    if (err != 0 && success_size == 0) {
        return err;
    }
    *p_size_read = success_size;
    PFFILE_Cursor_AdvanceToRead(p_file, success_size, p_file->cursor.sector);
    if (success_size != size) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_Read(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_read) {
    s32 err;
    PF_VOLUME* p_vol;
    u32 size_read;

    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    *p_size_read = 0;
    p_vol = PFFILE_GetVolume(p_file);
    err = PFFILE_Cursor_ReadHeadSector(p_file, p_buf, size, &size_read);
    *p_size_read += size_read;
    if (err != 0) {
        return err;
    }
    if (size_read >= size) {
        return 0;
    }
    size -= size_read;
    p_vol = PFFILE_GetVolume(p_file);
    for (; size >= p_vol->bpb.bytes_per_sector; size -= size_read) {
        err = PFFILE_Cursor_ReadBodySectors(p_file, &p_buf[*p_size_read], size, &size_read);
        *p_size_read += size_read;
        if (err != 0) {
            return err;
        }
        if (size_read == 0) {
            break;
        }
    }
    err = PFFILE_Cursor_ReadTailSector(p_file, &p_buf[*p_size_read], size, &size_read);
    *p_size_read += size_read;
    if (err != 0) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_WriteHeadSector(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_write) {
    PF_VOLUME* p_vol;
    s32 err;
    u32 max_writable_size;
    u32 success_size;

    *p_size_write = 0;
    if (p_file->cursor.offset_in_sector == 0) {
        return 0;
    }
    p_vol = PFFILE_GetVolume(p_file);
    max_writable_size = p_vol->bpb.bytes_per_sector - p_file->cursor.offset_in_sector;
    if (size > max_writable_size) {
        size = max_writable_size;
    }
    if (p_file->cursor.sector == -1) {
        err = PFFAT_GetSectorSpecified(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, 0, &p_file->cursor.sector);
        if (err != 0) {
            return err;
        }
        if (p_file->cursor.sector == -1) {
            PFFILE_Cursor_MoveToEnd(p_file);
            return 28;
        }
    }
    err = PFSEC_WriteData(p_vol, p_buf, p_file->cursor.sector, p_file->cursor.offset_in_sector, size, &success_size, 1);
    if (err != 0 && success_size == 0) {
        return err;
    }
    *p_size_write = success_size;
    PFFILE_Cursor_AdvanceToWrite(p_file, success_size, p_file->cursor.sector);
    if (success_size != size) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_WriteBodySectors(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_write) {
    s32 err;
    PF_VOLUME* p_vol;
    u32 num_sector;
    u32 max_writable_size;
    u32 success_size;
    *p_size_write = 0;
    num_sector = 0;
    err = PFFAT_GetContinuousSector(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, size, &p_file->cursor.sector, &num_sector);
    if (err != 0) {
        return err;
    }
    if (p_file->cursor.sector == 0xFFFFFFFF) {
        PFFILE_Cursor_MoveToEnd(p_file);
        return 28;
    }
    p_vol = PFFILE_GetVolume(p_file);
    max_writable_size = num_sector << p_vol->bpb.log2_bytes_per_sector;
    if (size > max_writable_size) {
        size = max_writable_size;
    }
    err = PFSEC_WriteData(p_vol, p_buf, p_file->cursor.sector, p_file->cursor.offset_in_sector, size, &success_size, 1);
    if (err != 0 && success_size == 0) {
        return err;
    }
    *p_size_write = success_size;
    PFFILE_Cursor_AdvanceToWrite(p_file, success_size, p_file->cursor.sector);
    if (success_size != size - (size & (p_vol->bpb.bytes_per_sector - 1))) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_WriteTailSector(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_write) {
    s32 err;
    PF_VOLUME* p_vol;
    u32 success_size;
    *p_size_write = 0;
    if (size == 0) {
        return 0;
    }
    p_vol = PFFILE_GetVolume(p_file);
    if (size > p_vol->bpb.bytes_per_sector - p_file->cursor.offset_in_sector || size > p_vol->bpb.bytes_per_sector) {
        return 26;
    }
    if (p_file->cursor.sector == 0xFFFFFFFF) {
        err = PFFAT_GetSectorSpecified(&p_file->p_sfd->ffd, p_file->cursor.file_sector_index, 0, &p_file->cursor.sector);
        if (err != 0) {
            return err;
        }
        if (p_file->cursor.sector == 0xFFFFFFFF) {
            PFFILE_Cursor_MoveToEnd(p_file);
            return 28;
        }
    }
    err = PFSEC_WriteData(p_vol, p_buf, p_file->cursor.sector, p_file->cursor.offset_in_sector, size, &success_size, 1);
    if (err != 0 && success_size == 0) {
        return err;
    }
    *p_size_write = success_size;
    PFFILE_Cursor_AdvanceToWrite(p_file, success_size, p_file->cursor.sector);
    if (success_size != size) {
        return err;
    }
    return 0;
}

static s32 PFFILE_Cursor_Write(PF_FILE* p_file, u8* p_buf, u32 size, u32* p_size_write) {
    s32 err;
    PF_VOLUME* p_vol;
    u32 size_write;
    u32 num_cluster;
    u32 append_size;
    u32 size_appended;
    u32 max_size;
    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    *p_size_write = 0;
    max_size = 0xFFFFFFFF - p_file->cursor.position;
    if (max_size < size) {
        size = max_size;
        p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = 37;
    }
    err = PFFILE_Cursor_WriteHeadSector(p_file, p_buf, size, &size_write);
    *p_size_write += size_write;
    if (err != 0) {
        return err;
    }
    if (size_write >= size) {
        return 0;
    }
    size -= size_write;
    PFFAT_CountAllocatedClusters(&p_file->p_sfd->ffd, p_file->cursor.position + size, &num_cluster);
    p_vol = PFFILE_GetVolume(p_file);
    if (p_file->cursor.position + size > (num_cluster << (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster))) {
        append_size = p_file->cursor.position + size - (num_cluster << (p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster));
    } else {
        append_size = 0;
    }
    size_appended = 0;
    while (append_size != 0 || size >= p_vol->bpb.bytes_per_sector) {
        if (append_size != 0) {
            err = PFCLUSTER_AppendCluster(p_file, append_size, &size_appended);
            if (err != 0 && size == append_size) {
                return err;
            }
            if (size_appended == 0) {
                append_size = 0;
            }
            append_size -= size_appended;
        }
        while (size >= append_size + size_appended && size >= p_vol->bpb.bytes_per_sector) {
            err = PFFILE_Cursor_WriteBodySectors(p_file, &p_buf[*p_size_write], size, &size_write);
            *p_size_write += size_write;
            if (err != 0) {
                return err;
            }
            if (size_write == 0) {
                return 0;
            }
            size -= size_write;
            if (size == 0) {
                break;
            }
        }
    }
    err = PFFILE_Cursor_WriteTailSector(p_file, &p_buf[*p_size_write], size, &size_write);
    *p_size_write += size_write;
    if (err != 0) {
        return err;
    }
    return 0;
}

void PFFILE_Cursor_MoveToEnd(PF_FILE* p_file) {
    p_file->cursor.position = p_file->p_sfd->dir_entry.file_size;
    PFFILE_Cursor_Recalc(p_file);
}

void PFFILE_Cursor_MoveToClusterEnd(PF_FILE* p_file, u32 size) {
    u32 cluster;
    PF_VOLUME* p_vol;
    u32 shift;

    p_vol = PFFILE_GetVolume(p_file);
    PFFAT_CountAllocatedClusters(&p_file->p_sfd->ffd, size, &cluster);
    shift = p_vol->bpb.log2_bytes_per_sector + p_vol->bpb.log2_sectors_per_cluster;
    if (cluster > (0xFFFFFFFF >> shift)) {
        p_file->cursor.position = -1;
        PFFILE_Cursor_Recalc(p_file);
        return;
    }
    p_file->cursor.position = cluster << shift;
    PFFILE_Cursor_Recalc(p_file);
}

static PF_SFD* PFFILE_GetSFD(PF_VOLUME* p_vol, PF_DIR_ENT* p_ent) {
    u32 sfd_num = 0;
    PF_SFD* p_first_free = NULL;
    u32 i;

    for (i = 0; i < 5; ++i) {
        if (!(p_vol->sfds[i].stat & 1) || ((p_vol->sfds[i].stat & 1) && !p_vol->sfds[i].stat & 2)) {
            if (p_first_free == NULL) {
                p_first_free = &p_vol->sfds[i];
                sfd_num = i;
            }
        } else {
            if (p_ent->p_vol == p_vol->sfds[i].dir_entry.p_vol && p_ent->entry_sector == p_vol->sfds[i].dir_entry.entry_sector &&
                p_ent->entry_offset == p_vol->sfds[i].dir_entry.entry_offset) {
                return &p_vol->sfds[i];
            }
        }
    }

    if (p_first_free == NULL) {
        return NULL;
    }
    PFFILE_InitSFD(p_first_free, p_ent);
    if ((p_vol->cluster_link.flag & 0x01) != 0) {
        p_first_free->ffd.cluster_link.buffer = &p_vol->cluster_link.buffer[sfd_num * p_vol->cluster_link.link_max];
        pf_memset(p_first_free->ffd.cluster_link.buffer, 0, p_vol->cluster_link.link_max * 4);
        p_first_free->ffd.cluster_link.max_count = p_vol->cluster_link.link_max;
        p_first_free->ffd.cluster_link.interval = p_vol->cluster_link.interval;
        p_first_free->ffd.cluster_link.interval_offset = 0;
        p_first_free->ffd.cluster_link.position = 0;
        p_first_free->ffd.cluster_link.save_index = 0;
        if (p_ent->file_size != 0 && p_first_free->ffd.cluster_link.max_count != 0) {
            *p_first_free->ffd.cluster_link.buffer = p_ent->start_cluster;
            p_first_free->ffd.cluster_link.position += 1;
        }
    }
    return p_first_free;
}

static s32 PFFILE_createEmptyFile(PF_VOLUME* p_vol, PF_DIR_ENT* p_file_ent, PF_STR* p_filename) {
    PF_DIR_ENT ent;
    s32 name_length;
    u8 buf[32];
    u8 num_entry_LFNs = 0;
    u32 i = 0;
    u8 sum;
    u32 current_sector = 0;
    s32 next_chain_index = 0;
    u32 next_chain[2];
    s32 err = 0;
    PF_FFD ffd;
    PF_FAT_HINT hint;
    u32 success_size;
    u32 position;

    ent = *p_file_ent;
    name_length = PFSTR_StrNumChar(p_filename, 1);
    if (name_length + ent.path_len > 0x103) {
        return 1;
    }
    if (name_length > 0xFF) {
        return 1;
    }
    PFFAT_InitFFD(&ffd, &hint, ent.p_vol, &ent.start_cluster);
    if (p_vol->num_opened_files >= 5) {
        return 21;
    }
    err = VFiPFPATH_parseShortName(p_file_ent->short_name, p_filename);
    if (err != 0 && p_file_ent->short_name[0] == 0) {
        return 1;
    }
    if (err != 0) {
        err = PFENT_AdjustSFN(&ent, p_file_ent->short_name);
        if (err != 0) {
            return err;
        }
        if (PFSTR_GetCodeMode(p_filename) == 1) {
            VFiPFPATH_transformInUnicode(p_file_ent->long_name, PFSTR_GetStrPos(p_filename, 1));
        } else {
            pf_w_strcpy(p_file_ent->long_name, (u16*)PFSTR_GetStrPos(p_filename, 1));
        }
    } else {
        p_file_ent->long_name[0] = 0;
    }

    if (p_vol->bpb.fat_type == PDM_FAT_32) {
        p_file_ent->start_cluster = 1;
    } else {
        p_file_ent->start_cluster = 0;
    }
    p_file_ent->file_size = 0;
    p_file_ent->p_vol = p_vol;
    p_file_ent->small_letter_flag = 0;
    p_file_ent->attr = 0x20;
    p_file_ent->create_time_ms = PFENT_getcurrentDateTimeForEnt(&p_file_ent->create_date, &p_file_ent->create_time);
    p_file_ent->access_date = p_file_ent->create_date;
    p_file_ent->modify_time = p_file_ent->create_time;
    p_file_ent->modify_date = p_file_ent->create_date;
    if (p_file_ent->long_name[0] != 0 && (p_file_ent->small_letter_flag & 0x18) == 0) {
        num_entry_LFNs = (name_length / 13) + ((name_length % 13) != 0 ? 1 : 0);
        err = PFENT_allocateEntry(p_file_ent, (num_entry_LFNs + 1), &ffd, (u32*)&next_chain, p_filename, &position);
        if (err != 0) {
            if (err == 8) {
                return err;
            }
            return err;
        }
        if ((pf_vol_set.setting & 0x02) == 0x02) {
            PFPATH_AdjustExtShortName(p_file_ent->short_name, position);
        }
        p_file_ent->num_entry_LFNs = num_entry_LFNs;
        current_sector = p_file_ent->entry_sector;
        sum = PFENT_CalcCheckSum(p_file_ent);

        for (i = num_entry_LFNs; i >= 1; i--) {
            PFENT_storeLFNEntryFieldsToBuf((u8*)&buf, p_file_ent, i, sum, i == num_entry_LFNs);
            err = PFSEC_WriteData(p_vol, (u8*)&buf, current_sector, (u16)p_file_ent->entry_offset, 0x20, &success_size, 0);
            if (err != 0) {
                return err;
            }
            if (success_size != 0x20) {
                return 17;
            }
            p_file_ent->entry_offset += 0x20;
            if (p_file_ent->entry_offset >= p_vol->bpb.bytes_per_sector) {
                p_file_ent->entry_offset = 0;
                current_sector = next_chain[next_chain_index];
                next_chain_index += 1;
            }
        }
        p_file_ent->entry_sector = current_sector;
    } else {
        err = PFENT_AllocateEntry(p_file_ent, 1, &ffd, (u32*)&next_chain, p_filename);
        if (err != 0) {
            if (err == 8) {
                return err;
            }
            return err;
        }
    }

    if (p_file_ent->start_cluster == 1) {
        p_file_ent->start_cluster = 0;
    }
    return PFENT_UpdateSFNEntry(p_file_ent, 1);
}

static s32 PFFILE_p_remove(PF_VOLUME* p_vol, PF_STR* p_path_str) {
    PF_STR dir_path;
    PF_STR filename;
    PF_FAT_HINT save_hint;
    PF_ENT_ITER iter;
    PF_DIR_ENT ent;
    s32 err;
    u32 start_cluster;
    err = PFPATH_SplitPath(p_path_str, &dir_path, &filename);
    if (err != 0) {
        return err;
    }
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path_str, 0);
    if (err != 0) {
        return err;
    }
    save_hint = *iter.ffd.p_hint;
    if ((ent.attr & 0x19) != 0) {
        return 11;
    }
    if (PFFILE_IsOpened(&ent) != 0) {
        return 19;
    }
    iter.ffd.p_hint = &save_hint;
    start_cluster = ent.start_cluster;
    err = PFENT_RemoveEntry(&ent, &iter);
    if (err != 0) {
        return err;
    }
    return PFFAT_FreeChain(&iter.ffd, start_cluster, 0xFFFFFFFF, ent.file_size);
}

static s32 PFFILE_p_fopen(PF_VOLUME* p_vol, PF_STR* p_path_str, s32 mode, PF_FILE** pp_file) {
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_SFD* p_sfd;
    PF_FILE* p_file;
    s32 err;
    u16 access_time;
    PF_STR file_str;
    PF_STR dir_str;
    PF_FFD ffd;
    PF_FAT_HINT hint;
    s8 local_name[512];
    s32 name_length;

    *pp_file = NULL;
    err = PFPATH_SplitPath(p_path_str, &dir_str, &file_str);
    if (err != 0) {
        return 1;
    }

    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path_str, 1);
    if (err != 0) {
        return err;
    }
    if ((ent.attr & 0x10) == 0) {
        return 1;
    }
    if (PFSTR_GetCodeMode(&file_str) == 2) {
        name_length = PFSTR_StrNumChar(&file_str, 1);
        if (name_length > 0xFF) {
            return 1;
        }
        VFiPFPATH_transformFromUnicodeToNormal((s8*)&local_name, (u16*)PFSTR_GetStrPos(&file_str, 1));
    }
    PFSTR_SetMarkPos(&file_str, (s8*)&local_name);
    if ((mode & 0x02) != 0) {
        PFFAT_InitFFD(&ffd, &hint, ent.p_vol, &ent.start_cluster);
        err = PFENT_SearchEntry(&ffd, &ent, 0, &file_str, 0x7FU, 0);
        if (err != 0) {
            return 3;
        }
        if ((mode & 0x08) != 0 && (ent.attr & 0x01) != 0) {
            return 10;
        }
        if ((ent.attr & 0x10) != 0) {
            return 23;
        }
        PFENT_getcurrentDateTimeForEnt(&ent.access_date, &access_time);
    } else {
        err = PFFILE_createEmptyFile(p_vol, &ent, &file_str);
        if (err == 8) {
            if ((mode & 0x10) != 0) {
                return 8;
            }
            if ((mode & 0x01) != 0 && PFFILE_IsOpened(&ent) != 0) {
                return 8;
            }
            if ((ent.attr & 0x01) != 0 && ((mode & 0x01) != 0 || (mode & 0x04) != 0 || (mode & 0x08) != 0)) {
                return 10;
            }
            if ((ent.attr & 0x10) != 0) {
                return 23;
            }
            if ((mode & 0x01) != 0) {
                PFFILE_EmptyFile(&iter.ffd, &ent);
            } else {
                PFENT_getcurrentDateTimeForEnt(&ent.access_date, &access_time);
            }
        } else {
            if (err != 0) {
                return err;
            }
        }
    }
    p_vol = ent.p_vol;
    p_sfd = PFFILE_GetSFD(p_vol, &ent);
    if (p_sfd == NULL) {
        return 21;
    }
    p_file = PFFILE_GetFreeUFD(p_vol);
    if (p_file == NULL) {
        return 22;
    }
    p_file->p_sfd = p_sfd;
    PFFILE_InitUFD(p_file, mode);
    if ((mode & 0x01) != 0) {
        p_file->p_sfd->stat = p_file->p_sfd->stat | 4;
    }
    if ((p_file->open_mode & 0x04) != 0) {
        PFFILE_Cursor_MoveToEnd(p_file);
    }
    p_sfd->num_handlers++;
    *pp_file = p_file;
    return 0;
}

static s32 PFFILE_p_fread(PF_VOLUME* p_vol, u8* p_buf, u32 size, u32 count, PF_FILE* p_file, u32* p_count_read) {
    u32 size_read = 0;
    u32 total_size = 0;
    s32 err;

    (void)p_vol;  

    *p_count_read = 0;
    if ((p_file->open_mode & 0x08) == 0 && ((p_file->open_mode & 0x01) != 0 || (p_file->open_mode & 0x04) != 0)) {
        return 10;
    }
    if ((p_file->p_sfd->lock.mode & 0x3) != 0 && p_file->lock_count == 0) {
        return 25;
    }
    if (PFFILE_Cursor_IsOutOfFile(p_file)) {
        return 28;
    }
    total_size = size * count;
    err = PFFILE_Cursor_Read(p_file, p_buf, total_size, &size_read);
    *p_count_read = size_read / size;
    if (err != 0) {
        return err;
    }
    return 0;
}

static s32 PFFILE_p_fwrite(PF_VOLUME* p_vol, u8* p_buf, u32 size, u32 count, PF_FILE* p_file, u32* p_count_written) {
    u32 append_size;
    PF_CACHE_PAGE* p_page;
    u32 is_hit;
    u32 size_written = 0;
    s32 err;

    *p_count_written = 0;
    if ((p_file->open_mode & 0x08) == 0 && (p_file->open_mode & 0x02) != 0) {
        return 10;
    }
    if ((p_file->p_sfd->lock.mode & 0x3) != 0 &&
        ((p_file->p_sfd->lock.mode & 0x01) != 0 || ((p_file->p_sfd->lock.mode & 0x02) != 0 && p_file->lock_count == 0))) {
        return 25;
    }
    if ((p_file->open_mode & 0x04) != 0) {
        PFFILE_Cursor_MoveToEnd(p_file);
    }
    if (p_file->cursor.position > p_file->p_sfd->dir_entry.file_size) {
        append_size = p_file->cursor.position - p_file->p_sfd->dir_entry.file_size;
        p_file->cursor.position = p_file->p_sfd->dir_entry.file_size;
        p_file->cursor.file_sector_index = p_file->cursor.position >> p_vol->bpb.log2_bytes_per_sector;
        p_file->cursor.offset_in_sector = p_file->cursor.position & (p_vol->bpb.bytes_per_sector - 1);
        err = PFCACHE_AllocateDataPage(p_vol, -1, &p_page, &is_hit);
        if (err != 0) {
            return err;
        }
        pf_memset(p_page->p_buf, 0, p_vol->bpb.bytes_per_sector);

        while (append_size != 0) {
            if (append_size > p_vol->bpb.bytes_per_sector) {
                err = PFFILE_Cursor_Write(p_file, p_page->p_buf, p_vol->bpb.bytes_per_sector, &size_written);
                if (err != 0) {
                    return err;
                }
            } else {
                err = PFFILE_Cursor_Write(p_file, p_page->p_buf, append_size, &size_written);
                if (err != 0) {
                    return err;
                }
                break;
            }
            append_size -= p_vol->bpb.bytes_per_sector;
        }

        PFCACHE_FreeDataPage(p_vol, p_page);
    }
    err = PFFILE_Cursor_Write(p_file, p_buf, size * count, &size_written);
    *p_count_written = size_written / size;
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFFILE_p_fappend(PF_FILE* p_file, u32 count, u32* p_count_appended) {
    u32 count_appended;
    u32 max_count;
    s32 err;
    *p_count_appended = 0;
    if ((p_file->open_mode & 8) == 0 && (p_file->open_mode & 2) != 0) {
        return 10;
    }
    if ((p_file->p_sfd->lock.mode & 3) != 0 &&
        ((p_file->p_sfd->lock.mode & 1) != 0 || ((p_file->p_sfd->lock.mode & 2) != 0 && p_file->lock_count == 0))) {
        return 25;
    }
    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    max_count = 0xFFFFFFFF - p_file->cursor.position;
    if (max_count < count) {
        count = max_count;
        p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = 37;
    }
    err = PFCLUSTER_AppendCluster(p_file, count, &count_appended);
    *p_count_appended = count_appended;
    if (err != 0) {
        return err;
    }
    return 0;
}

static s32 PFFILE_p_finfo(PF_FILE* p_file, PF_INFO* p_info) {
    PF_VOLUME* p_vol;
    u32 cluster_size;
    s32 err;

    p_file->p_sfd->ffd.p_hint = &p_file->hint;
    p_vol = PFFILE_GetVolume(p_file);
    p_info->file_size = p_file->p_sfd->dir_entry.file_size;
    p_info->io_pointer = p_file->cursor.position;
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    if (p_file->p_sfd->dir_entry.start_cluster != 0) {
        err = PFCLUSTER_GetAppendSize(p_file, &p_info->allocated_size);
        if (err != 0) {
            return err;
        }
        p_info->empty_size =
            p_info->allocated_size +
            ((p_file->p_sfd->dir_entry.file_size % cluster_size) != 0 ? (cluster_size - (p_file->p_sfd->dir_entry.file_size % cluster_size)) : 0);
    } else {
        p_info->allocated_size = 0;
        p_info->empty_size = 0;
    }
    p_info->lock_mode = p_file->p_sfd->lock.mode & 0x3;
    p_info->lock_owner = p_file->p_sfd->lock.owner;
    p_info->lock_count = p_file->lock_count;
    p_info->lock_tcount = p_file->p_sfd->lock.count;
    return 0;
}

s32 PFFILE_p_combine(PF_VOLUME* p_vol, PF_STR* p_base, PF_STR* p_add) {
    PF_DIR_ENT base;
    PF_DIR_ENT add;
    PF_ENT_ITER iter_base;
    PF_ENT_ITER iter_add;
    PF_ENT_ITER saved_iter;
    PF_FAT_HINT hint_base;
    PF_FAT_HINT hint_add;
    s32 err;
    err = PFENT_GetEntryOfPath(&iter_base, &base, p_vol, p_base, 0);
    if (err != 0) {
        return err;
    }
    err = PFENT_GetEntryOfPath(&iter_add, &add, p_vol, p_add, 0);
    if (err != 0) {
        return err;
    }
    if ((base.attr & 0x10) != 0 || (add.attr & 0x10) != 0) {
        return 23;
    }
    if ((base.attr & 1) != 0 || (add.attr & 1) != 0) {
        return 24;
    }
    if (PFFILE_IsOpened(&base) != 0 || PFFILE_IsOpened(&add) != 0) {
        return 19;
    }
    if (base.entry_sector == add.entry_sector && base.entry_offset == add.entry_offset) {
        return 10;
    }
    if (0xFFFFFFFF - base.file_size < add.file_size) {
        return 37;
    }
    PFFAT_InitHint(&hint_base);
    PFFAT_InitFFD(&iter_base.ffd, &hint_base, p_vol, &base.start_cluster);
    PFFAT_InitHint(&hint_add);
    PFFAT_InitFFD(&iter_add.ffd, &hint_add, p_vol, &add.start_cluster);
    pf_memcpy(&saved_iter, &iter_add, sizeof(saved_iter));
    err = PFENT_RemoveEntry(&add, &iter_add);
    if (err != 0) {
        return err;
    }
    err = PFCLUSTER_CombineChain(&iter_base, &saved_iter, &base, &add);
    if (err != 0) {
        return err;
    }
    PFENT_getcurrentDateTimeForEnt(&base.modify_date, &base.modify_time);
    base.access_date = base.modify_date;
    err = PFENT_UpdateSFNEntry(&base, 1);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFFILE_p_divide(PF_VOLUME* p_vol, PF_STR* p_path, PF_STR* p_new, u32 offset) {
    PF_DIR_ENT ent;
    PF_DIR_ENT new_ent;
    PF_DIR_ENT saved_ent;
    s8 filename[512];
    PF_ENT_ITER iter;
    PF_ENT_ITER new_iter;
    PF_STR file_str;
    PF_STR dir_str;
    PF_FAT_HINT hint;
    PF_FAT_HINT new_hint;
    u32 saved_size;
    u32 saved_cluster;
    s32 err;
    s32 result;
    s32 length;
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }
    if ((ent.attr & 0x10) != 0) {
        return 23;
    }
    if ((ent.attr & 1) != 0) {
        return 24;
    }
    if (PFFILE_IsOpened(&ent) != 0) {
        return 19;
    }
    if (ent.file_size == 0) {
        return 10;
    }
    if (ent.file_size < offset) {
        return 10;
    }
    if (PFPATH_SplitPath(p_new, &dir_str, &file_str) != 0) {
        return 1;
    }
    if (PFSTR_GetCodeMode(&file_str) == 2) {
        length = PFSTR_StrNumChar(&file_str, 1);
        if (length > 0xFF) {
            return 1;
        }
        VFiPFPATH_transformFromUnicodeToNormal(filename, (u16*)PFSTR_GetStrPos(&file_str, 1));
    }
    PFSTR_SetMarkPos(&file_str, filename);
    err = PFENT_GetEntryOfPath(&new_iter, &new_ent, p_vol, p_new, 1);
    if (err != 0) {
        return err;
    }
    if ((new_ent.attr & 0x10) == 0) {
        return 1;
    }
    err = PFFILE_createEmptyFile(p_vol, &new_ent, &file_str);
    if (err != 0) {
        return err;
    }
    PFFAT_InitHint(&hint);
    PFFAT_InitFFD(&iter.ffd, &hint, p_vol, &ent.start_cluster);
    PFFAT_InitHint(&new_hint);
    PFFAT_InitFFD(&new_iter.ffd, &new_hint, p_vol, &new_ent.start_cluster);
    pf_memcpy(&saved_ent, &ent, sizeof(saved_ent));
    saved_size = ent.file_size;
    saved_cluster = ent.start_cluster;
    PFENT_getcurrentDateTimeForEnt(&ent.modify_date, &ent.modify_time);
    ent.access_date = ent.modify_date;
    if (offset == 0) {
        ent.start_cluster = 0;
    }
    ent.file_size = offset;
    err = PFENT_UpdateSFNEntry(&ent, 1);
    if (err != 0) {
        return err;
    }
    if ((p_vol->cache.mode & 4) != 0) {
        err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
        if (err != 0) {
            return err;
        }
    }
    result = PFCLUSTER_DivideChain(&iter, &new_iter, &saved_ent, &new_ent, offset);
    if (result != 0) {
        PFCACHE_FlushFATCache(p_vol);
        ent.file_size = saved_size;
        ent.start_cluster = saved_cluster;
        PFENT_UpdateSFNEntry(&ent, 1);
        PFENT_GetEntryOfPath(&new_iter, &new_ent, p_vol, p_new, 0);
        PFENT_RemoveEntry(&new_ent, &new_iter);
        return result;
    }
    new_ent.modify_date = ent.modify_date;
    new_ent.modify_time = ent.modify_time;
    new_ent.access_date = ent.access_date;
    new_ent.attr = ent.attr;
    err = PFENT_UpdateSFNEntry(&new_ent, 1);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFFILE_p_cinsert(PF_VOLUME* p_vol, PF_STR* p_path, u32 index, u32 count, u32* p_count) {
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_FAT_HINT hint;
    u32 cluster_size;
    u32 num_clusters;
    s32 err;
    u16 time;
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }
    if ((ent.attr & 0x10) != 0) {
        return 23;
    }
    if ((ent.attr & 1) != 0) {
        return 24;
    }
    if (PFFILE_IsOpened(&ent) != 0) {
        return 19;
    }
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    num_clusters = ent.file_size / cluster_size;
    if ((ent.file_size % cluster_size) != 0) {
        num_clusters++;
    }
    if (num_clusters < index) {
        return 10;
    }
    PFFAT_InitHint(&hint);
    PFFAT_InitFFD(&iter.ffd, &hint, p_vol, &ent.start_cluster);
    err = PFCLUSTER_InsertChain(&iter, &ent, index, count, p_count);
    if (err != 0) {
        return err;
    }
    PFENT_getcurrentDateTimeForEnt(&ent.modify_date, &ent.modify_time);
    ent.access_date = ent.modify_date;
    err = PFENT_UpdateSFNEntry(&ent, 1);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFFILE_p_cdelete(PF_VOLUME* p_vol, PF_STR* p_path, u32 index, u32 count, u32* p_count) {
    PF_DIR_ENT ent;
    PF_ENT_ITER iter;
    PF_FAT_HINT hint;
    u32 cluster_size;
    u32 num_clusters;
    s32 err;
    err = PFENT_GetEntryOfPath(&iter, &ent, p_vol, p_path, 0);
    if (err != 0) {
        return err;
    }
    if ((ent.attr & 0x10) != 0) {
        return 23;
    }
    if ((ent.attr & 1) != 0) {
        return 24;
    }
    if (PFFILE_IsOpened(&ent) != 0) {
        return 19;
    }
    if (ent.file_size == 0) {
        return 10;
    }
    cluster_size = p_vol->bpb.bytes_per_sector << p_vol->bpb.log2_sectors_per_cluster;
    num_clusters = ent.file_size / cluster_size;
    if ((ent.file_size % cluster_size) != 0) {
        num_clusters++;
    }
    if (num_clusters <= index) {
        return 10;
    }
    PFFAT_InitHint(&hint);
    PFFAT_InitFFD(&iter.ffd, &hint, p_vol, &ent.start_cluster);
    err = PFCLUSTER_DeleteChain(&iter, &ent, index, count, p_count);
    if (err != 0) {
        return err;
    }
    return 0;
}

s32 PFFILE_p_flock(PF_FILE* p_file, u32 mode) {
    if ((mode & 3) != 0) {
        if ((p_file->p_sfd->lock.mode & 3) != 0 && (mode & 8) != 0) {
            if (((mode & 1) != 0 && (p_file->p_sfd->lock.mode & 2) != 0) ||
                ((mode & 2) != 0 && (p_file->p_sfd->lock.mode & 1) != 0) ||
                (p_file->p_sfd->lock.owner != NULL && p_file->p_sfd->lock.owner != p_file)) {
                return 39;
            }
        }
        if ((mode & 1) != 0) {
            if (p_file->p_sfd->lock.count == 0 || (p_file->p_sfd->lock.mode & 2) != 0) {
                p_file->p_sfd->lock.wcount++;
                PF_LockFile(p_file, NULL);
                p_file->p_sfd->lock.wcount--;
                p_file->p_sfd->lock.mode = 1;
                p_file->p_sfd->lock.owner = NULL;
            }
        } else if (p_file->p_sfd->lock.owner != p_file) {
            p_file->p_sfd->lock.wcount++;
            PF_LockFile(p_file, p_file);
            p_file->p_sfd->lock.wcount--;
            p_file->p_sfd->lock.mode = 2;
            p_file->p_sfd->lock.owner = p_file;
        }
        p_file->p_sfd->lock.count++;
        p_file->lock_count++;
    } else {
        if (p_file->lock_count == 0 || p_file->p_sfd->lock.count == 0) {
            return 39;
        }
        if ((p_file->p_sfd->lock.mode & 2) != 0 && p_file->p_sfd->lock.owner != p_file) {
            return 39;
        }
        p_file->lock_count--;
        p_file->p_sfd->lock.count--;
        if (p_file->p_sfd->lock.count == 0) {
            PF_UnLockFile(p_file);
            if ((p_file->p_sfd->lock.mode & 2) != 0) {
                p_file->p_sfd->lock.owner = NULL;
            }
            p_file->p_sfd->lock.mode &= ~3;
        }
    }
    return 0;
}

s32 PFFILE_OpenEntry(PF_DIR_ENT* p_ent, PF_ENT_ITER* p_iter, u32 mode, PF_FILE** pp_file) {
    PF_VOLUME* p_vol = p_ent->p_vol;
    PF_SFD* p_sfd;
    PF_FILE* p_file;
    u16 time;
    *pp_file = NULL;
    if ((p_ent->attr & 0x10) != 0) {
        return 23;
    }
    if ((p_ent->attr & 1) != 0 && ((mode & 1) != 0 || (mode & 4) != 0 || (mode & 8) != 0)) {
        return 10;
    }
    if ((mode & 1) != 0 && PFFILE_IsOpened(p_ent) != 0) {
        return 8;
    }
    if ((mode & 1) != 0) {
        if (p_ent->start_cluster >= 2 && p_ent->start_cluster != 0xFFFFFFFF && p_ent->file_size != 0) {
            PFFAT_FreeChain(&p_iter->ffd, p_ent->start_cluster, 0xFFFFFFFF, p_ent->file_size);
        }
        p_ent->start_cluster = 0;
        p_ent->file_size = 0;
        PFENT_getcurrentDateTimeForEnt(&p_ent->modify_date, &p_ent->modify_time);
        p_ent->access_date = p_ent->modify_date;
    } else {
        PFENT_getcurrentDateTimeForEnt(&p_ent->access_date, &time);
    }
    p_sfd = PFFILE_GetSFD(p_vol, p_ent);
    if (p_sfd == NULL) {
        return 21;
    }
    p_file = PFFILE_GetFreeUFD(p_vol);
    if (p_file == NULL) {
        return 22;
    }
    p_file->p_sfd = p_sfd;
    PFFILE_InitUFD(p_file, mode);
    if ((mode & 1) != 0) {
        p_file->p_sfd->stat |= 4;
    }
    if ((p_file->open_mode & 4) != 0) {
        PFFILE_Cursor_MoveToEnd(p_file);
    }
    p_sfd->num_handlers++;
    *pp_file = p_file;
    return 0;
}

u32 PFFILE_IsOpened(PF_DIR_ENT* p_ent) {
    PF_VOLUME* p_vol = p_ent->p_vol;
    s32 i;
    for (i = 0; i < 5; i++) {
        PF_SFD* p_sfd = &p_vol->sfds[i];
        if ((p_sfd->stat & 1) != 0 && (p_sfd->stat & 2) != 0 &&
            p_ent->p_vol == p_sfd->dir_entry.p_vol &&
            p_ent->entry_sector == p_sfd->dir_entry.entry_sector &&
            p_ent->entry_offset == p_sfd->dir_entry.entry_offset) {
            return 1;
        }
    }
    return 0;
}

void PFFILE_FinalizeAllFiles(PF_VOLUME* p_vol) {
    u16 i;

    for (i = 0; i < 5; i++) {
        PFFILE_FinalizeSFD(&p_vol->sfds[i]);
    }

    for (i = 0; i < 5; i++) {
        PFFILE_FinalizeUFD(&p_vol->ufds[i]);
    }

    p_vol->num_opened_files = 0;
}

s32 PFFILE_remove(PF_STR* p_path_str) {
    PF_VOLUME* p_vol = PFPATH_GetVolumeFromPath(p_path_str);
    s32 err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    p_vol->cache.signature = NULL;
    err = PFFILE_p_remove(p_vol, p_path_str);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    }
    return err;
}

s32 PFFILE_ferror(PF_FILE* p_file) {
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);
    s32 err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
        return -1;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        p_vol->last_error = pf_vol_set.last_error = 38;
        return -1;
    }
    return p_file->last_error;
}

s32 PFFILE_fopen(PF_STR* p_path_str, s32 mode, PF_FILE** pp_file) {
    PF_VOLUME* p_vol;
    s32 err;
    *pp_file = NULL;
    p_vol = PFPATH_GetVolumeFromPath(p_path_str);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0 && !(err == 11 && mode == 2)) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    err = PFFILE_p_fopen(p_vol, p_path_str, mode, pp_file);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    } else {
        p_vol->num_opened_files++;
    }
    return err;
}

s32 PFFILE_fclose(PF_FILE* p_file) {
    PF_VOLUME* p_vol;
    s32 err = 0;
    s32 result;

    p_vol = PFFILE_GetVolume(p_file);
    result = PFVOL_CheckForWrite(p_vol);
    if (result != 0 && !(result == 11 && p_file->open_mode == 2)) {
        return p_vol->last_error = pf_vol_set.last_error = result;
    }
    if ((p_file->p_sfd->stat & 0x01) == 0 || (p_file->p_sfd->stat & 0x02) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 10;
    }
    p_vol->cache.signature = p_file;
    if (((p_file->p_sfd->num_handlers - 1) <= 0) && ((p_file->p_sfd->dir_entry.attr & 0x19) == 0) && ((result & 11) == 0) &&
        (((p_file->p_sfd->stat & 0x04) != 0) || ((p_vol->file_config & 0x01) != 1))) {
        err = PFENT_UpdateSFNEntry(&p_file->p_sfd->dir_entry, 1);
    }
    if (p_file->lock_count != 0) {
        if ((p_file->p_sfd->lock.mode & 0x01) != 0) {
            p_file->p_sfd->lock.count -= p_file->lock_count;
            p_file->lock_count = 0;
            if (p_file->p_sfd->lock.count == 0) {
                PF_UnLockFile(p_file);
            }
            p_file->p_sfd->lock.mode &= (u16) ~(0x01 | 0x02);
        } else if (p_file->p_sfd->lock.owner != p_file) {
            err = 0x19;
        } else {
            p_file->p_sfd->lock.count = 0;
            p_file->lock_count = 0;
            p_file->p_sfd->lock.owner = NULL;
            PF_UnLockFile(p_file);
            p_file->p_sfd->lock.mode &= (u16) ~(0x01 | 0x02);
        }
    }
    if (err != 0) {
        p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    } else {
        if ((p_vol->cache.mode & 0x02) != 0) {
            err = PFCACHE_FlushFATCache(p_vol);
            if (err != 0) {
                p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
            } else {
                err = PFCACHE_FlushDataCacheSpecific(p_vol, p_file);
                if (err != 0) {
                    p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
                }
            }
        }
        if (err == 0) {
            if (--p_file->p_sfd->num_handlers == 0) {
                p_file->p_sfd->stat &= ~1;
                p_file->p_sfd->ffd.cluster_link.buffer = NULL;
            }
            PFFILE_ReleaseUFD(p_file);
            p_vol->num_opened_files--;
        }
    }
    p_vol->cache.signature = NULL;
    return err;
}

s32 PFFILE_fread(u8* p_buf, u32 size, u32 count, PF_FILE* p_file, u32* p_count_read) {
    PF_VOLUME* p_vol;
    u32 count_read;
    s32 err;
    *p_count_read = 0;
    p_vol = PFFILE_GetVolume(p_file);
    err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    p_vol->cache.signature = p_file;
    err = PFFILE_p_fread(p_vol, p_buf, size, count, p_file, &count_read);
    *p_count_read = count_read;
    p_vol->cache.signature = NULL;
    if (err != 0) {
        p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    return err;
}

s32 PFFILE_fwrite(u8* p_buf, u32 size, u32 count, PF_FILE* p_file, u32* p_count_written) {
    PF_VOLUME* p_vol;
    u32 count_written;
    s32 err;
    s32 result;

    *p_count_written = 0;
    p_vol = PFFILE_GetVolume(p_file);
    result = PFVOL_CheckForWrite(&*p_vol);
    if (result != 0) {
        return p_vol->last_error = pf_vol_set.last_error = result;
    }
    if ((p_file->p_sfd->stat & 0x01) == 0 || (p_file->p_sfd->stat & 0x02) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    p_vol->cache.signature = p_file;
    err = PFFILE_p_fwrite(p_vol, p_buf, size, count, &*p_file, &count_written);
    *p_count_written = count_written;
    p_vol->cache.signature = NULL;
    if (err != 0) {
        p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    if (count_written != 0) {
        PFENT_getcurrentDateTimeForEnt(&p_file->p_sfd->dir_entry.modify_date, &p_file->p_sfd->dir_entry.modify_time);
        p_file->p_sfd->dir_entry.access_date = p_file->p_sfd->dir_entry.modify_date;
        p_file->p_sfd->stat = p_file->p_sfd->stat | 4;
    }
    return err;
}

s32 PFFILE_feof(PF_FILE* p_file) {
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);
    s32 err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    if (p_file->cursor.position >= p_file->p_sfd->dir_entry.file_size) {
        err = 1;
    }
    return err;
}

s32 PFFILE_fseek(PF_FILE* p_file, s32 offset, int origin) {
    PF_VOLUME* p_vol;
    s32 err = 0;
    u32 file_io;
    u32 wk_offset;

    p_vol = PFFILE_GetVolume(p_file);
    err = PFVOL_CheckForRead(&*p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 0x01) == 0 || (p_file->p_sfd->stat & 0x02) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    switch (origin) {
        case 1: {
            file_io = p_file->cursor.position;
            break;
        }
        case 0: {
            file_io = 0;
            break;
        }
        case 2: {
            file_io = p_file->p_sfd->dir_entry.file_size;
            break;
        }
        default: {
            return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = 10;
        }
    }

    if ((offset & 0x080000000) != 0) {
        wk_offset = ~(offset | offset) & 0x7FFFFFFF;
        wk_offset += 1;
        if (file_io < wk_offset) {
            return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = 10;
        }
        file_io -= wk_offset;
    } else {
        wk_offset = 0xFFFFFFFF - file_io;
        if (offset > wk_offset) {
            return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = 0x25;
        }
        file_io += offset;
    }
    PFFILE_Cursor_Initialize(&*p_file);
    p_file->cursor.position = file_io;
    PFFILE_Cursor_Recalc(p_file);
    return err;
}

s32 PFFILE_fsetclstlink(PF_FILE* p_file, u32 mode, PF_CLUSTER_LINK_CONFIG* p_config) {
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);
    s32 err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    if (mode == 1) {
        p_file->p_sfd->ffd.cluster_link.buffer = p_config->buffer;
        pf_memset(p_config->buffer, 0, 4 * p_config->count);
        p_file->p_sfd->ffd.cluster_link.max_count = p_config->count;
        p_file->p_sfd->ffd.cluster_link.interval = p_config->interval;
        p_file->p_sfd->ffd.cluster_link.interval_offset = 0;
        p_file->p_sfd->ffd.cluster_link.position = 0;
        p_file->p_sfd->ffd.cluster_link.save_index = 0;
        if (p_file->p_sfd->dir_entry.file_size != 0 && p_file->p_sfd->ffd.cluster_link.max_count != 0) {
            p_file->p_sfd->ffd.cluster_link.buffer[0] = *p_file->p_sfd->ffd.p_start_cluster;
            p_file->p_sfd->ffd.cluster_link.position++;
        }
        if (p_file->p_sfd->ffd.p_hint != NULL) {
            p_file->p_sfd->ffd.p_hint->chain_index = 0;
            p_file->p_sfd->ffd.p_hint->cluster = 0;
            p_file->p_sfd->ffd.p_hint->chain_stamp = 0;
        }
        p_file->p_sfd->ffd.last_cluster.num_last_cluster = 0;
        p_file->p_sfd->ffd.last_cluster.max_chain_index = 0;
        p_file->p_sfd->ffd.last_access.chain_index = 0;
        p_file->p_sfd->ffd.last_access.cluster = 0;
    } else {
        p_file->p_sfd->ffd.cluster_link.buffer = NULL;
    }
    return err;
}

s32 PFFILE_fsync(PF_FILE* p_file) {
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);
    u16 time;
    s32 err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    err = PFCACHE_FlushFATCache(p_vol);
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    PFENT_getcurrentDateTimeForEnt(&p_file->p_sfd->dir_entry.access_date, &time);
    err = PFENT_UpdateSFNEntry(&p_file->p_sfd->dir_entry, 1);
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    err = PFCACHE_FlushDataCacheSpecific(p_vol, p_file);
    if (err != 0) {
        p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    return err;
}

s32 PFFILE_fappend(PF_FILE* p_file, u32 count, u32* p_count_appended) {
    PF_VOLUME* p_vol;
    u16 time;
    u32 count_appended;
    s32 err;
    *p_count_appended = 0;
    p_vol = PFFILE_GetVolume(p_file);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    p_vol->cache.signature = p_file;
    err = PFFILE_p_fappend(p_file, count, &count_appended);
    *p_count_appended = count_appended;
    p_vol->cache.signature = NULL;
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    PFENT_getcurrentDateTimeForEnt(&p_file->p_sfd->dir_entry.access_date, &time);
    return 0;
}

s32 PFFILE_fadjust(PF_FILE* p_file) {
    PF_VOLUME* p_vol;
    PF_CURSOR save_cursor;
    PF_FAT_HINT save_hint;
    u16 time;
    s32 err;
    p_vol = PFFILE_GetVolume(p_file);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    p_vol->cache.signature = p_file;
    save_cursor = p_file->cursor;
    save_hint = p_file->hint;
    err = PFFILE_p_fadjust(p_file);
    p_file->cursor = save_cursor;
    p_file->hint = save_hint;
    p_vol->cache.signature = NULL;
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    PFENT_getcurrentDateTimeForEnt(&p_file->p_sfd->dir_entry.access_date, &time);
    return 0;
}

s32 PFFILE_finfo(PF_FILE* p_file, PF_INFO* p_info) {
    PF_VOLUME* p_vol;
    PF_CURSOR save_cursor;
    PF_FAT_HINT save_hint;
    s32 err;

    p_vol = PFFILE_GetVolume(p_file);
    err = PFVOL_CheckForRead(&*p_vol);
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 0x01) == 0 || (p_file->p_sfd->stat & 0x02) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    p_vol->cache.signature = p_file;
    save_cursor = p_file->cursor;
    save_hint = p_file->hint;

    err = PFFILE_p_finfo(p_file, p_info);

    p_file->cursor = save_cursor;
    p_file->hint = save_hint;
    p_vol->cache.signature = NULL;
    if (err != 0) {
        return p_file->last_error = p_file->p_sfd->ffd.p_vol->last_error = pf_vol_set.last_error = err;
    }
    return 0;
}

s32 PFFILE_combine(PF_STR* p_base, PF_STR* p_add) {
    PF_VOLUME* p_vol = PFPATH_GetVolumeFromPath(p_base);
    s32 err;
    if (p_vol != PFPATH_GetVolumeFromPath(p_add)) {
        return pf_vol_set.last_error = 10;
    }
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    p_vol->cache.signature = NULL;
    err = PFFILE_p_combine(p_vol, p_base, p_add);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    } else {
        err = PFCACHE_FlushFATCache(p_vol);
        if (err != 0) {
            p_vol->last_error = pf_vol_set.last_error = err;
        } else {
            err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
            if (err != 0) {
                p_vol->last_error = pf_vol_set.last_error = err;
            }
        }
    }
    p_vol->cache.signature = NULL;
    return err;
}

s32 PFFILE_divide(PF_STR* p_base, PF_STR* p_new, u32 offset) {
    PF_VOLUME* p_vol = PFPATH_GetVolumeFromPath(p_base);
    s32 err;
    if (p_vol != PFPATH_GetVolumeFromPath(p_new)) {
        return pf_vol_set.last_error = 31;
    }
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    p_vol->cache.signature = NULL;
    err = PFFILE_p_divide(p_vol, p_base, p_new, offset);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    } else {
        err = PFCACHE_FlushFATCache(p_vol);
        if (err != 0) {
            p_vol->last_error = pf_vol_set.last_error = err;
        } else {
            err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
            if (err != 0) {
                p_vol->last_error = pf_vol_set.last_error = err;
            }
        }
    }
    p_vol->cache.signature = NULL;
    return err;
}

s32 PFFILE_cinsert(PF_STR* p_path, u32 offset, u32 count, u32* p_count) {
    PF_VOLUME* p_vol;
    s32 err;
    *p_count = 0;
    p_vol = PFPATH_GetVolumeFromPath(p_path);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    p_vol->cache.signature = NULL;
    err = PFFILE_p_cinsert(p_vol, p_path, offset, count, p_count);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    } else {
        err = PFCACHE_FlushFATCache(p_vol);
        if (err != 0) {
            p_vol->last_error = pf_vol_set.last_error = err;
        } else {
            err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
            if (err != 0) {
                p_vol->last_error = pf_vol_set.last_error = err;
            }
        }
    }
    p_vol->cache.signature = NULL;
    return err;
}

s32 PFFILE_cdelete(PF_STR* p_path, u32 offset, u32 count, u32* p_count) {
    PF_VOLUME* p_vol;
    s32 err;
    *p_count = 0;
    p_vol = PFPATH_GetVolumeFromPath(p_path);
    err = PFVOL_CheckForWrite(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    p_vol->cache.signature = NULL;
    err = PFFILE_p_cdelete(p_vol, p_path, offset, count, p_count);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    } else {
        err = PFCACHE_FlushDataCacheSpecific(p_vol, NULL);
        if (err != 0) {
            p_vol->last_error = pf_vol_set.last_error = err;
        } else {
            err = PFCACHE_FlushFATCache(p_vol);
            if (err != 0) {
                p_vol->last_error = pf_vol_set.last_error = err;
            }
        }
    }
    p_vol->cache.signature = NULL;
    return err;
}

s32 PFFILE_flock(PF_FILE* p_file, u32 mode) {
    PF_VOLUME* p_vol = PFFILE_GetVolume(p_file);
    s32 err = PFVOL_CheckForRead(p_vol);
    if (err != 0) {
        return p_vol->last_error = pf_vol_set.last_error = err;
    }
    if ((p_file->p_sfd->stat & 1) == 0 || (p_file->p_sfd->stat & 2) == 0) {
        return p_vol->last_error = pf_vol_set.last_error = 38;
    }
    err = PFFILE_p_flock(p_file, mode);
    if (err != 0) {
        p_vol->last_error = pf_vol_set.last_error = err;
    }
    return err;
}
