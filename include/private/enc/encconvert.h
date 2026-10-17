#ifndef PRIVATE_ENC_CONVERT_H
#define PRIVATE_ENC_CONVERT_H

#include <revolution/types.h>

#include <private/enc/encutility.h>
#include <revolution/enc.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ENCODING_NAME_LENGTH 16
#define ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH 24

struct encoding {
    u32 width;
    BOOL is7bit;
    const char name[ENCODING_NAME_LENGTH];
};

struct encoding_hash_table_entry {
    const char name[ENCODING_HASH_TABLE_ENTRY_NAME_LENGTH];
    ENCEncoding encoding;
    int nextIndex;
};

extern const struct encoding encoding_array[];
extern const struct encoding_hash_table_entry encoding_table[];

extern BOOL enc_tbl_jp_loaded;
extern BOOL enc_tbl_kr_loaded;
extern BOOL enc_tbl_uhc_loaded;
extern BOOL enc_tbl_cn_loaded;

#ifdef __cplusplus
}
#endif

#endif
