#ifndef NW4HBM_UT_CHAR_WRITER_H
#define NW4HBM_UT_CHAR_WRITER_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/math/math_types.h"
#include "nw4hbm/ut/ut_Color.h"

#include "revolution/gx.h"

namespace nw4hbm {
namespace ut {

class Font;
struct Glyph;

class CharWriter {
public:
    enum GradationMode {
        GRADMODE_NONE,
        GRADMODE_H,
        GRADMODE_V,

        NUM_OF_GRADMODE
    };

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

    struct TextureFilter {
        bool operator!=(const TextureFilter& rOther) const {
            return atSmall != rOther.atSmall || atLarge != rOther.atLarge;
        }

        GXTexFilter atSmall;
        GXTexFilter atLarge;
    };

    struct TextColor {
        Color start;
        Color end;
        GradationMode gradationMode;
    };

    struct LoadingTexture {
        bool operator!=(const LoadingTexture& rOther) const {
            return slot != rOther.slot || texture != rOther.texture || filter != rOther.filter;
        }

        void Reset() {
            slot = GX_TEXMAP_NULL;
            texture = nullptr;
        }

        GXTexMapID slot;
        void* texture;
        TextureFilter filter;
    };

public:
    CharWriter();
    ~CharWriter();

    void SetFont(const Font& rFont);
    const Font* GetFont() const;

    void SetupGX();

    void SetColorMapping(Color min, Color max);
    void ResetColorMapping();
    void ResetTextureCache();

    void SetGradationMode(GradationMode mode);
    void SetTextColor(Color color);
    void SetTextColor(Color start, Color end);

    void SetScale(f32 hScale, f32 vScale);
    f32 GetScaleH() const;
    f32 GetScaleV() const;

    void SetFontSize(f32 width, f32 height);
    f32 GetFontWidth() const;
    f32 GetFontHeight() const;
    f32 GetFontAscent() const;

    void EnableLinearFilter(bool atSmall, bool atLarge);

    bool IsWidthFixed() const;
    f32 GetFixedWidth() const;

    f32 Print(u16 code);

    void SetCursor(f32 x, f32 y);
    void SetCursor(f32 x, f32 y, f32 z);
    void SetCursorX(f32 x);
    void SetCursorY(f32 y);
    void MoveCursorX(f32 dx);
    void MoveCursorY(f32 dy);
    f32 GetCursorX() const;
    f32 GetCursorY() const;

    void PrintGlyph(f32 x, f32 y, f32 z, const Glyph& rGlyph);

private:
    void LoadTexture(const Glyph& rGlyph, GXTexMapID slot);
    void UpdateVertexColor();

    static void SetupVertexFormat();
    static void SetupGXDefault();
    static void SetupGXWithColorMapping(Color min, Color max);
    static void SetupGXForI();
    static void SetupGXForRGBA();

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

}
}

#endif
