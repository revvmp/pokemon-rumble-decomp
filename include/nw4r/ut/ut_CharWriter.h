#ifndef NW4R_UT_CHAR_WRITER_H
#define NW4R_UT_CHAR_WRITER_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_Color.h>

#include <nw4r/math/math_types.h>
#include <nw4r/ut/ut_Font.h>

#include <revolution/GX.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/gx/GXVert.h>

namespace nw4r {
namespace ut {

class Font;
struct Glyph;

class CharWriter {
public:
    enum GradationMode {
        GRADMODE_NONE,
        GRADMODE_H,
        GRADMODE_V,

        GRADMODE_MAX
    };

public:
    CharWriter();
    ~CharWriter();

    void SetupGX();
    void SetupGXForDisplayList();
    void EnableLinearFilter(bool atSmall, bool atLarge);
    f32 Print(u16 ch);

    void SetColorMapping(Color min, Color max) {
        mColorMapping.min = min;
        mColorMapping.max = max;
    }

    void ResetColorMapping() {
        SetColorMapping(DEFAULT_COLOR_MAPPING_MIN, DEFAULT_COLOR_MAPPING_MAX);
    }

    void SetTextColor(Color start) {
        mTextColor.start = start;
        UpdateVertexColor();
    }

    void SetTextColor(Color start, Color end) {
        mTextColor.start = start;
        mTextColor.end = end;
        UpdateVertexColor();
    }

    void SetGradationMode(GradationMode mode) {
        mTextColor.gradationMode = mode;
        UpdateVertexColor();
    }

    Color GetTextColorStart() const {
        return mTextColor.start;
    }

    Color GetTextColorEnd() const {
        return mTextColor.end;
    }

    GradationMode GetGradationMode() const {
        return mTextColor.gradationMode;
    }

    f32 GetScaleH() const {
        return mScale.x;
    }
    f32 GetScaleV() const {
        return mScale.y;
    }

    void SetScale(f32 x, f32 y) {
        mScale.x = x;
        mScale.y = y;
    }

    f32 GetCursorX() const {
        return mCursorPos.x;
    }
    void SetCursorX(f32 x) {
        mCursorPos.x = x;
    }

    f32 GetCursorY() const {
        return mCursorPos.y;
    }
    void SetCursorY(f32 y) {
        mCursorPos.y = y;
    }

    void SetCursor(f32 x, f32 y) {
        mCursorPos.x = x;
        mCursorPos.y = y;
    }
    void SetCursor(f32 x, f32 y, f32 z) {
        mCursorPos.x = x;
        mCursorPos.y = y;
        mCursorPos.z = z;
    }

    void MoveCursorX(f32 dx) {
        mCursorPos.x += dx;
    }
    void MoveCursorY(f32 dy) {
        mCursorPos.y += dy;
    }

    void SetAlpha(u8 alpha) {
        mAlpha = alpha;
        UpdateVertexColor();
    }
    u8 GetAlpha() const {
        return mAlpha;
    }

    void EnableFixedWidth(bool enable) {
        mIsWidthFixed = enable;
    }
    bool IsWidthFixed() const {
        return mIsWidthFixed;
    }

    void SetFixedWidth(f32 width) {
        mFixedWidth = width;
    }
    f32 GetFixedWidth() const {
        return mFixedWidth;
    }

    void SetFont(const Font& rFont) {
        mFont = &rFont;
    }
    const Font* GetFont() const {
        return mFont;
    }

    void SetFontSize(f32 width, f32 height);
    void SetFontSize(f32 height);

    f32 GetFontWidth() const;
    f32 GetFontHeight() const;
    f32 GetFontAscent() const;
    f32 GetFontDescent() const;

private:
    struct ColorMapping {
        Color min;
        Color max;
    };

    struct VertexColor {
        Color lu;
        Color ru;
        Color ld;
        Color rd;
    };

    struct TextColor {
        Color start;
        Color end;
        GradationMode gradationMode;
    };

    struct TextureFilter {
        GXTexFilter atSmall;
        GXTexFilter atLarge;

        bool operator!=(const TextureFilter& rOther) const {
            return atSmall != rOther.atSmall || atLarge != rOther.atLarge;
        }
    };

    struct LoadingTexture {
        GXTexMapID slot;
        void* texture;
        TextureFilter filter;

        bool operator!=(const LoadingTexture& rOther) const {
            return slot != rOther.slot || texture != rOther.texture ||
                   filter != rOther.filter;
        }

        void Reset() {
            slot = GX_TEXMAP_NULL;
            texture = NULL;
        }
    };

    static const u32 DEFAULT_COLOR_MAPPING_MIN = 0x00000000;
    static const u32 DEFAULT_COLOR_MAPPING_MAX = 0xFFFFFFFF;

private:
    static void SetupVertexFormat();
    static void SetupGXDefault();
    static void SetupGXWithColorMapping(Color min, Color max);
    static void SetupGXForI();
    static void SetupGXForRGBA();
    static void SetupGXForDisplayListDefault();
    static void SetupGXForDisplayListWithColorMapping();
    static void SetupGXForDisplayListForI();
    static void SetupGXForDisplayListForRGBA();

    void UpdateVertexColor();
    void PrintGlyph(f32 x, f32 y, f32 z, const Glyph& rGlyph);

    void LoadTexture(const Glyph& rGlyph, GXTexMapID slot);
    void ResetTextureCache() {
        mLoadingTexture.Reset();
    }

private:
    ColorMapping mColorMapping;
    VertexColor mVertexColor;
    TextColor mTextColor;
    math::VEC2 mScale;
    math::VEC3 mCursorPos;
    TextureFilter mFilter;
    u8 PADDING_0x40[0x42 - 0x40];
    u8 mAlpha;
    bool mIsWidthFixed;
    f32 mFixedWidth;
    const Font* mFont;

    static LoadingTexture mLoadingTexture;
};


inline f32 CharWriter::Print(u16 code) {
    f32 width;
    f32 left;

    Glyph glyph;
    mFont->GetGlyph(&glyph, code);

    if (mIsWidthFixed) {
        width = mFixedWidth;
        left = (width - glyph.widths.charWidth * mScale.x) / 2 + (glyph.widths.left * mScale.x);
    } else {
        width = glyph.widths.charWidth * mScale.x;
        left = glyph.widths.left * mScale.x;
    }

    f32 posX = mCursorPos.x + left;
    f32 posY = mCursorPos.y - mFont->GetBaselinePos(code) * mScale.y;

    PrintGlyph(posX, posY, mCursorPos.z, glyph);
    mCursorPos.x += width;

    return width;
}

inline void CharWriter::PrintGlyph(f32 x, f32 y, f32 z, const Glyph& rGlyph) {
    f32 posLeft = x;
    f32 posTop = y;
    f32 posRight = posLeft + rGlyph.widths.glyphWidth * mScale.x;
    f32 posBottom = posTop + rGlyph.height * mScale.y;
    f32 posZ = z;

    u16 texLeft = static_cast<u32>(rGlyph.cellX << 15) / rGlyph.texWidth;
    u16 texTop = static_cast<u32>(rGlyph.cellY << 15) / rGlyph.texHeight;
    u16 texRight = static_cast<u32>((rGlyph.cellX + rGlyph.widths.glyphWidth) << 15) / rGlyph.texWidth;
    u16 texBottom = static_cast<u32>((rGlyph.cellY + rGlyph.height) << 15) / rGlyph.texHeight;

    LoadTexture(rGlyph, GX_TEXMAP0);

    GXBegin(GX_QUADS, GX_VTXFMT0, 4);

    GXPosition3f32(posLeft, posTop, posZ);
    GXColor1u32(mVertexColor.lu);
    GXTexCoord2u16(texLeft, texTop);

    GXPosition3f32(posRight, posTop, posZ);
    GXColor1u32(mVertexColor.ru);
    GXTexCoord2u16(texRight, texTop);

    GXPosition3f32(posRight, posBottom, posZ);
    GXColor1u32(mVertexColor.rd);
    GXTexCoord2u16(texRight, texBottom);

    GXPosition3f32(posLeft, posBottom, posZ);
    GXColor1u32(mVertexColor.ld);
    GXTexCoord2u16(texLeft, texBottom);

    GXEnd();
}

inline void CharWriter::LoadTexture(const Glyph& rGlyph, GXTexMapID slot) {
    if (rGlyph.pTexture != mLoadingTexture.texture) {
        GXTexObj texObj;

        GXInitTexObj(&texObj, rGlyph.pTexture, rGlyph.texWidth, rGlyph.texHeight, rGlyph.texFormat, GX_CLAMP, GX_CLAMP,
                     GX_FALSE);
        GXInitTexObjLOD(&texObj, mFilter.atSmall, mFilter.atLarge, 0.0f, 0.0f, 0.0f, GX_FALSE, GX_FALSE, GX_ANISO_1);
        GXLoadTexObj(&texObj, slot);

        mLoadingTexture.texture = rGlyph.pTexture;
    }
}
}
}

#endif
