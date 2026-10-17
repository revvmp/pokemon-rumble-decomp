#include <revolution/fa/pf_entry.h>
#include <revolution/fa/pf_path.h>
#include <revolution/fa/pf_str.h>

#define GET_U16_LE(p) ((u16)(((*(u16*)(p) >> 8) & 0xFF) | ((*(u16*)(p) & 0xFF) << 8)))
#define GET_U32_LE(p) ((((*(u32*)(p)) << 24) | (((*(u32*)(p)) << 8) & 0xFF0000)) | (((*(u32*)(p)) >> 24) | (((*(u32*)(p)) >> 8) & 0xFF00)))

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
            VFiPFPATH_getShortName(p_ent->short_name, buf, buf[11]);
            if (p_ent->short_name[0] == 0x05) {
                p_ent->short_name[0] = PF_ENT_DELETED;
            }

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

    p_ent->attr = p_iter->buf[11];
    p_ent->small_letter_flag = p_iter->buf[12];
    p_ent->create_time_ms = p_iter->buf[13];
    p_ent->create_time = GET_U16_LE(&p_iter->buf[14]);
    p_ent->create_date = GET_U16_LE(&p_iter->buf[16]);
    p_ent->access_date = GET_U16_LE(&p_iter->buf[18]);
    p_ent->modify_time = GET_U16_LE(&p_iter->buf[22]);
    p_ent->modify_date = GET_U16_LE(&p_iter->buf[24]);
    p_ent->file_size = GET_U32_LE(&p_iter->buf[28]);
    p_ent->start_cluster = (GET_U16_LE(&p_iter->buf[20]) << 16) | GET_U16_LE(&p_iter->buf[26]);
    p_ent->entry_sector = p_iter->sector;
    p_ent->entry_offset = p_iter->offset;
    if ((p_ent->attr & PF_ATTR_DIR) && p_ent->start_cluster == 0) {
        p_ent->start_cluster = 1;
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
