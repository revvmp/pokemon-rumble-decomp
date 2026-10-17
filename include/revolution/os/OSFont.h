#ifndef REVOLUTION_OS_OSFONT_H
#define REVOLUTION_OS_OSFONT_H

#include "revolution/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OS_FONT_ENCODE_ANSI 0
#define OS_FONT_ENCODE_SJIS 1
#define OS_FONT_ENCODE_UTF8 3
#define OS_FONT_ENCODE_UTF16 4
#define OS_FONT_ENCODE_UTF32 5
#define OS_FONT_ENCODE_MAX 5
#define OS_FONT_ENCODE_VOID 0xFFFF

#define OS_FONT_ROM_SIZE_ANSI 0x3000
#define OS_FONT_ROM_SIZE_SJIS 0x4D000
#define OS_FONT_DATA_SIZE_ANSI 0x20120
#define OS_FONT_DATA_SIZE_SJIS 0x120F00

typedef struct OSFontHeader {
    u16 fontType;
    u16 firstChar;
    u16 lastChar;
    u16 invalChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
    u16 cellWidth;
    u16 cellHeight;
    u32 sheetSize;
    u16 sheetFormat;
    u16 sheetColumn;
    u16 sheetRow;
    u16 sheetWidth;
    u16 sheetHeight;
    u16 widthTable;
    u32 sheetImage;
    u32 sheetFullSize;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;
} OSFontHeader;

u16 OSGetFontEncode(void);
u16 OSSetFontEncode(u16 encode);
u32 OSLoadFont(OSFontHeader* fontData, void* temp);
char* OSGetFontTexel(const char* string, void* image, s32 pos, s32 stride, s32* width);
BOOL OSInitFont(OSFontHeader* fontData);
char* OSGetFontTexture(const char* string, void** image, s32* x, s32* y, s32* width);
char* OSGetFontWidth(const char* string, s32* width);

#ifdef __cplusplus
}
#endif

#endif
