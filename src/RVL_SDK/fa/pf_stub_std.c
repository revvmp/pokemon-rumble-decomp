#include <private/fa/pf_stub.h>
#include <revolution/fa/pf_attach.h>
#include <revolution/fa/pf_buffering.h>
#include <revolution/fa/pf_cdelete.h>
#include <revolution/fa/pf_chdir.h>
#include <revolution/fa/pf_chdmod.h>
#include <revolution/fa/pf_chmod.h>
#include <revolution/fa/pf_cinsert.h>
#include <revolution/fa/pf_closedir.h>
#include <revolution/fa/pf_combine.h>
#include <revolution/fa/pf_create.h>
#include <revolution/fa/pf_createdir.h>
#include <revolution/fa/pf_cut.h>
#include <revolution/fa/pf_derrnum.h>
#include <revolution/fa/pf_detach.h>
#include <revolution/fa/pf_divide.h>
#include <revolution/fa/pf_errnum.h>
#include <revolution/fa/pf_fadjust.h>
#include <revolution/fa/pf_fappend.h>
#include <revolution/fa/pf_fchdir.h>
#include <revolution/fa/pf_fclose.h>
#include <revolution/fa/pf_fconcat.h>
#include <revolution/fa/pf_feof.h>
#include <revolution/fa/pf_ferror.h>
#include <revolution/fa/pf_finfo.h>
#include <revolution/fa/pf_flock.h>
#include <revolution/fa/pf_fopen.h>
#include <revolution/fa/pf_format.h>
#include <revolution/fa/pf_fread.h>
#include <revolution/fa/pf_fseek.h>
#include <revolution/fa/pf_fsetclstlink.h>
#include <revolution/fa/pf_fsexec.h>
#include <revolution/fa/pf_fsfirst.h>
#include <revolution/fa/pf_fsnext.h>
#include <revolution/fa/pf_fstat.h>
#include <revolution/fa/pf_fsync.h>
#include <revolution/fa/pf_fwrite.h>
#include <revolution/fa/pf_getdev.h>
#include <revolution/fa/pf_getvol.h>
#include <revolution/fa/pf_getvolcfg.h>
#include <revolution/fa/pf_insert.h>
#include <revolution/fa/pf_iswriteprotected.h>
#include <revolution/fa/pf_mkdir.h>
#include <revolution/fa/pf_mount.h>
#include <revolution/fa/pf_move.h>
#include <revolution/fa/pf_opendir.h>
#include <revolution/fa/pf_readdir.h>
#include <revolution/fa/pf_regctx.h>
#include <revolution/fa/pf_remove.h>
#include <revolution/fa/pf_rename.h>
#include <revolution/fa/pf_rewinddir.h>
#include <revolution/fa/pf_rmdir.h>
#include <revolution/fa/pf_rmvvol.h>
#include <revolution/fa/pf_seekdir.h>
#include <revolution/fa/pf_setclstlink.h>
#include <revolution/fa/pf_setcode.h>
#include <revolution/fa/pf_setencode.h>
#include <revolution/fa/pf_settailbuf.h>
#include <revolution/fa/pf_setupfsi.h>
#include <revolution/fa/pf_setvol.h>
#include <revolution/fa/pf_setvolcfg.h>
#include <revolution/fa/pf_sync.h>
#include <revolution/fa/pf_telldir.h>
#include <revolution/fa/pf_unmount.h>
#include <revolution/fa/pf_unregctx.h>
#include <revolution/fa/pf_xdivide.h>

void pfstub_call_function_standard(PF_STUB_COMMAND* p) {
    switch (p->command) {
    case 0:
        p->result = (s32)pf2_create(p->primary.path, p->first.attributes);
        break;
    case 1:
        p->result = (s32)pf2_fopen(p->primary.path, p->secondary.mode);
        break;
    case 2:
        p->result = (s32)pf2_fclose(p->primary.file);
        break;
    case 3:
        p->result = (s32)pf2_fread(p->primary.buffer, p->first.size, p->second.count, p->secondary.file);
        break;
    case 4:
        p->result = (s32)pf2_fwrite(p->primary.buffer, p->first.size, p->second.count, p->secondary.file);
        break;
    case 5:
        p->result = (s32)pf2_fseek(p->primary.file, p->first.offset, p->second.origin);
        break;
    case 7:
        p->result = (s32)pf2_remove(p->primary.path);
        break;
    case 8:
        p->result = (s32)pf2_rename(p->primary.path, p->secondary.path);
        break;
    case 46:
        p->result = (s32)pf2_move(p->primary.path, p->secondary.path);
        break;
    case 9:
        p->result = (s32)pf2_mkdir(p->primary.path);
        break;
    case 65:
        p->result = (s32)pf2_createdir(p->primary.path, p->first.mode, p->secondary.dta);
        break;
    case 10:
        p->result = (s32)pf2_rmdir(p->primary.path);
        break;
    case 11:
        p->result = (s32)pf2_chdir(p->primary.path);
        break;
    case 12:
        p->result = (s32)pf2_fstat(p->primary.path, p->secondary.stat);
        break;
    case 13:
        p->result = (s32)pf2_chmod(p->primary.path, p->first.attributes);
        break;
    case 14:
        p->result = (s32)pf2_mount(p->drive);
        break;
    case 39:
        p->result = (s32)pf2_unmount(p->drive, p->first.mode);
        break;
    case 15:
        p->result = (s32)pf2_format(p->drive, p->primary.buffer);
        break;
    case 16:
        p->result = (s32)pf2_buffering(p->drive, p->first.mode);
        break;
    case 17:
        p->result = (s32)pf2_ferror(p->primary.file);
        break;
    case 18:
        p->result = (s32)pf2_feof(p->primary.file);
        break;
    case 19:
        p->result = (s32)pf2_errnum();
        break;
    case 20:
        p->result = (s32)pf2_devinf(p->drive, p->primary.device_info);
        break;
    case 21:
        p->result = (s32)pf2_setvol(p->drive, p->primary.path);
        break;
    case 22:
        p->result = (s32)pf2_getvol(p->drive, p->primary.buffer);
        break;
    case 23:
        p->result = (s32)pf2_rmvvol(p->drive);
        break;
    case 24:
        p->result = (s32)pf2_chdmod(p->primary.path, p->first.attributes);
        break;
    case 25:
        p->result = (s32)pf2_fconcat(p->primary.path, p->secondary.path);
        break;
    case 27:
        p->result = (s32)pf2_attach(p->primary.drivers, p->secondary.nand_file);
        break;
    case 41:
        p->result = (s32)pf2_detach(p->drive);
        break;
    case 28:
        p->result = (s32)pf2_fsfirst(p->primary.path, p->first.attributes, p->secondary.dta);
        break;
    case 29:
        p->result = (s32)pf2_fsnext(p->primary.dta);
        break;
    case 38:
        p->result = (s32)pf2_fsexec(p->primary.dta, p->first.mode, p->drive);
        break;
    case 30:
        p->result = (s32)pf2_setupfsi(p->drive, (s16)p->first.flags);
        break;
    case 31:
        p->result = (s32)pf2_setclstlink(p->drive, p->first.mode, p->primary.cluster_link);
        break;
    case 32:
        p->result = (s32)pf2_fsetclstlink(p->primary.file, p->first.mode, p->secondary.cluster_link);
        break;
    case 33:
        p->result = (s32)pf2_sync(p->drive, p->first.mode);
        break;
    case 34:
        p->result = (s32)pf2_fsync(p->primary.file);
        break;
    case 35:
        p->result = (s32)pf2_fappend(p->primary.file, p->first.count);
        break;
    case 44:
        p->result = (s32)pf2_fadjust(p->primary.file);
        break;
    case 45:
        p->result = (s32)pf2_finfo(p->primary.file, p->secondary.file_info);
        break;
    case 36:
        p->result = (s32)pf2_settailbuf(p->drive, p->first.size, p->primary.tail_buffer);
        break;
    case 37:
        p->result = (s32)pf2_derrnum(p->drive);
        break;
    case 40:
        p->result = (s32)pf2_combine(p->primary.path, p->secondary.path);
        break;
    case 42:
        p->result = (s32)pf2_divide(p->primary.path, p->secondary.path, p->first.offset);
        break;
    case 43:
        p->result = (s32)pf2_xdivide(p->primary.path, p->secondary.path, p->first.offset);
        break;
    case 47:
        p->result = (s32)pf2_cinsert(p->primary.path, p->first.offset, p->second.clusters);
        break;
    case 48:
        p->result = (s32)pf2_insert(p->primary.path, p->first.offset);
        break;
    case 49:
        p->result = (s32)pf2_cdelete(p->primary.path, p->first.offset, p->second.clusters);
        break;
    case 50:
        p->result = (s32)pf2_cut(p->primary.path, p->first.offset);
        break;
    case 51:
        p->result = (s32)pf2_setvolcfg(p->drive, p->primary.volume_config);
        break;
    case 52:
        p->result = (s32)pf2_getvolcfg(p->drive, p->primary.volume_config);
        break;
    case 53:
        p->result = (s32)pf2_setcode(p->primary.codeset);
        break;
    case 54:
        p->result = (s32)pf2_opendir(p->primary.path);
        break;
    case 55:
        p->result = (s32)pf2_closedir(p->primary.directory);
        break;
    case 56:
        p->result = (s32)pf2_readdir(p->primary.directory, p->secondary.entry);
        break;
    case 57:
        p->result = (s32)pf2_telldir(p->primary.directory, p->secondary.position);
        break;
    case 58:
        p->result = (s32)pf2_seekdir(p->primary.directory, p->first.offset);
        break;
    case 59:
        p->result = (s32)pf2_rewinddir(p->primary.directory);
        break;
    case 60:
        p->result = (s32)pf2_fchdir(p->primary.directory);
        break;
    case 61:
        p->result = (s32)pf2_regctx();
        break;
    case 62:
        p->result = (s32)pf2_unregctx();
        break;
    case 63:
        p->result = (s32)pf2_flock(p->primary.file, p->first.mode);
        break;
    case 64:
        p->result = (s32)pf2_setencode(p->first.mode);
        break;
    case 80:
        p->result = (s32)pf2_iswriteprotected(p->drive);
        break;
    case 6:
        break;
    default:
        p->result = -1;
        break;
    }
}
