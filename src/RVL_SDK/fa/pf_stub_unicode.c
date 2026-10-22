#include <private/fa/pf_stub.h>
#include <revolution/fa/pf_w_cdelete.h>
#include <revolution/fa/pf_w_chdir.h>
#include <revolution/fa/pf_w_chdmod.h>
#include <revolution/fa/pf_w_chmod.h>
#include <revolution/fa/pf_w_cinsert.h>
#include <revolution/fa/pf_w_combine.h>
#include <revolution/fa/pf_w_create.h>
#include <revolution/fa/pf_w_createdir.h>
#include <revolution/fa/pf_w_cut.h>
#include <revolution/fa/pf_w_divide.h>
#include <revolution/fa/pf_w_fconcat.h>
#include <revolution/fa/pf_w_fopen.h>
#include <revolution/fa/pf_w_fsexec.h>
#include <revolution/fa/pf_w_fsfirst.h>
#include <revolution/fa/pf_w_fsnext.h>
#include <revolution/fa/pf_w_fstat.h>
#include <revolution/fa/pf_w_insert.h>
#include <revolution/fa/pf_w_mkdir.h>
#include <revolution/fa/pf_w_move.h>
#include <revolution/fa/pf_w_opendir.h>
#include <revolution/fa/pf_w_remove.h>
#include <revolution/fa/pf_w_rename.h>
#include <revolution/fa/pf_w_rmdir.h>
#include <revolution/fa/pf_w_xdivide.h>

void pfstub_call_function_unicode(PF_STUB_COMMAND* p) {
    switch (p->command) {
    case 100:
        p->result = (s32)pf2_w_create(p->primary.wide_path, p->first.mode);
        break;
    case 101:
        p->result = (s32)pf2_w_fopen(p->primary.wide_path, p->secondary.wide_mode);
        break;
    case 102:
        p->result = (s32)pf2_w_remove(p->primary.wide_path);
        break;
    case 103:
        p->result = (s32)pf2_w_rename(p->primary.wide_path, p->secondary.wide_path);
        break;
    case 117:
        p->result = (s32)pf2_w_move(p->primary.wide_path, p->secondary.wide_path);
        break;
    case 104:
        p->result = (s32)pf2_w_mkdir(p->primary.wide_path);
        break;
    case 123:
        p->result = (s32)pf2_w_createdir(p->primary.wide_path, p->first.mode, p->secondary.dta);
        break;
    case 105:
        p->result = (s32)pf2_w_rmdir(p->primary.wide_path);
        break;
    case 106:
        p->result = (s32)pf2_w_chdir(p->primary.wide_path);
        break;
    case 107:
        p->result = (s32)pf2_w_fstat(p->primary.wide_path, p->secondary.stat);
        break;
    case 108:
        p->result = (s32)pf2_w_chmod(p->primary.wide_path, p->first.attributes);
        break;
    case 109:
        p->result = (s32)pf2_w_chdmod(p->primary.wide_path, p->first.attributes);
        break;
    case 110:
        p->result = (s32)pf2_w_fconcat(p->primary.wide_path, p->secondary.wide_path);
        break;
    case 111:
        p->result = (s32)pf2_w_fsfirst(p->primary.wide_path, p->first.attributes, p->secondary.dta);
        break;
    case 112:
        p->result = (s32)pf2_w_fsnext(p->primary.dta);
        break;
    case 113:
        p->result = (s32)pf2_w_fsexec(p->primary.dta, p->first.mode, p->drive);
        break;
    case 114:
        p->result = (s32)pf2_w_combine(p->primary.wide_path, p->secondary.wide_path);
        break;
    case 115:
        p->result = (s32)pf2_w_divide(p->primary.wide_path, p->secondary.wide_path, p->first.offset);
        break;
    case 116:
        p->result = (s32)pf2_w_xdivide(p->primary.wide_path, p->secondary.wide_path, p->first.offset);
        break;
    case 118:
        p->result = (s32)pf2_w_cinsert(p->primary.wide_path, p->first.offset, p->second.count);
        break;
    case 119:
        p->result = (s32)pf2_w_insert(p->primary.wide_path, p->first.offset);
        break;
    case 120:
        p->result = (s32)pf2_w_cdelete(p->primary.wide_path, p->first.offset, p->second.count);
        break;
    case 121:
        p->result = (s32)pf2_w_cut(p->primary.wide_path, p->first.offset);
        break;
    case 122:
        p->result = (s32)pf2_w_opendir(p->primary.wide_path);
        break;
    default:
        p->result = -1;
        break;
    }
}
