#ifndef PRIVATE_ENC_LATIN_H
#define PRIVATE_ENC_LATIN_H

#include <revolution/types.h>

#include <revolution/enc/encutility.h>

#ifdef __cplusplus
extern "C" {
#endif

struct latin_hash_table_entry {
    u16 c16;
    u8 cc;
    u8 next;
};

extern const u16 latin1_unicode_array[];
extern const u16 latin2_unicode_array[];
extern const u16 latin3_unicode_array[];
extern const u16 greek_unicode_array[];
extern const u16 latin6_unicode_array[];
extern const u16 latin9_unicode_array[];
extern const u16 win1250_unicode_array[];
extern const u16 win1253_unicode_array[];
extern const u16 macroman_unicode_array[];
extern const u16 macgreek_unicode_array[];
extern const u16 macce_unicode_array[];
extern const u16 ibm850_unicode_array[];
extern const u16 ibm852_unicode_array[];

extern const struct latin_hash_table_entry unicode_latin1_array[];
extern const struct latin_hash_table_entry unicode_latin2_array[];
extern const struct latin_hash_table_entry unicode_latin3_array[];
extern const struct latin_hash_table_entry unicode_greek_array[];
extern const struct latin_hash_table_entry unicode_latin6_array[];
extern const struct latin_hash_table_entry unicode_latin9_array[];
extern const struct latin_hash_table_entry unicode_win1252_array[];

ENCResult ENCiConvertStringLatinToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType, const u16* table);
ENCResult ENCiConvertStringUnicodeToLatin(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType,
                                          const struct latin_hash_table_entry* table);

ENCResult ENCiConvertStringLatin1ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin1(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringLatin2ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin2(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringLatin3ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin3(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringGreekToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToGreek(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringLatin6ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin6(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringLatin9ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToLatin9(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringUnicodeToWin1252(u8* dst, s32* dstSize, const u16* src, s32* srcSize, ENCBreakType breakType);


ENCResult ENCiConvertStringWin1250ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringWin1253ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringMacromanToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringMacgreekToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringMacceToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringIbm850ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);
ENCResult ENCiConvertStringIbm852ToUnicode(u16* dst, s32* dstSize, const u8* src, s32* srcSize, ENCBreakType breakType);

#ifdef __cplusplus
}
#endif

#endif
