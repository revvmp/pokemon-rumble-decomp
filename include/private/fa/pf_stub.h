#ifndef PRIVATE_FA_PF_STUB_H
#define PRIVATE_FA_PF_STUB_H

#include <revolution/fa.h>
#include <revolution/os/OSThread.h>
#include <revolution/fa/pf_volume.h>

typedef union PF_STUB_SCALAR {
    s32 count;
    s32 offset;
    s32 mode;
    s32 attributes;
    s32 device_type;
    u32 size;
    u32 flags;
    s32 origin;
    u32 clusters;
} PF_STUB_SCALAR;

typedef union PF_STUB_POINTER {
    void* buffer;
    const char* path;
    const u16* wide_path;
    const char* mode;
    const u16* wide_mode;
    PF_FILE* file;
    PF_DIR* directory;
    struct PF_DRV_TBL** drivers;
    struct PF_DTA* dta;
    struct PF_STAT* stat;
    struct PF_INFO* file_info;
    struct PF_DEV_INF* device_info;
    struct PF_CLUSTER_LINK_CONFIG* cluster_link;
    struct PF_VOLUME_CONFIG* volume_config;
    struct PF_CHARCODE* codeset;
    struct PF_DIRENT* entry;
    struct PF_TAIL_BUF* tail_buffer;
    const struct PF_NAND_FILE* nand_file;
    u32* position;
} PF_STUB_POINTER;

typedef enum PF_STUB_OPERATION {
    PF_STUB_INVALID = -1,
    PF_STUB_STANDARD_END = 100,
    PF_STUB_UNICODE_END = 200
} PF_STUB_OPERATION;

typedef struct PF_STUB_COMMAND {
    s32 context_id;
    PF_STUB_OPERATION command;
    s8 drive;
    PF_STUB_SCALAR first;
    PF_STUB_SCALAR second;
    PF_STUB_POINTER primary;
    PF_STUB_POINTER secondary;
    struct PF_DRV_TBL* driver_table[2];
    OSSemaphore* p_semaphore;
    s32* p_result;
    PDM_DISK* p_disk;
    PDM_PARTITION* p_partition;
    void (*callback)(s32 result, void* p_parameter);
    void* p_parameter;
    s32 result;
} PF_STUB_COMMAND;


void pfstub_init_stub(void);
void* pfstub_entry(void* p_parameter);
s32 pfstub_com_massage(PF_STUB_COMMAND* p_command);
s32 pfstub_store_result(PF_STUB_COMMAND* p_command);
void pfstub_call_function_standard(PF_STUB_COMMAND* p_command);
void pfstub_call_function_unicode(PF_STUB_COMMAND* p_command);
s32 pfstub_attach(FACommandBlock* p_request);
s32 pfstub_buffering(s8 drive, u32 mode);
s32 pfstub_chdir(const char* path);
s32 pfstub_create(FACommandBlock* p_request);
s32 pfstub_detach(s8 drive);
s32 pfstub_errnum(void);
s32 pfstub_fclose(PF_FILE* p_file);
s32 pfstub_fsfirst(FACommandBlock* p_request);
s32 pfstub_fsnext(FACommandBlock* p_request);
s32 pfstub_fwrite(FACommandBlock* p_request);
s32 pfstub_devinf(s8 drive, struct PF_DEV_INF* p_info);
s32 pfstub_init_prfile2(s32 config, void* p_parameter);
s32 pfstub_iswriteprotected(s8 drive);
s32 pfstub_mkdir_async(FACommandBlock* p_request);
s32 pfstub_mount_async(FACommandBlock* p_request);
s32 pfstub_remove(const char* path);
s32 pfstub_remove_async(FACommandBlock* p_request);
s32 pfstub_rmdir_async(FACommandBlock* p_request);
s32 pfstub_setupfsi(s8 drive, u32 flags);
s32 pfstub_unmount(s8 drive, u32 mode);
s32 pfstub_unmount_async(FACommandBlock* p_request);

#endif
