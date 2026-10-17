#ifndef NW4HBM_LYT_TEXT_BOX_H
#define NW4HBM_LYT_TEXT_BOX_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/lyt/lyt_common.h"
#include "nw4hbm/lyt/lyt_pane.h"

#include "nw4hbm/ut.h"

namespace nw4hbm {
namespace ut {

class Font;

template <typename T>
class TagProcessorBase;

template <typename T>
class TextWriterBase;

}

namespace lyt {

struct ResBlockSet;

namespace res {

struct TextBox : Pane {
    static const u32 SIGNATURE = FOURCC('t', 'x', 't', '1');

    u16 textBufBytes;
    u16 textStrBytes;
    u16 materialIdx;
    u16 fontIdx;
    u8 textPosition;
    u8 PADDING_0x55[0x58 - 0x55];
    u32 textStrOffset;
    u32 textCols[2];
    Size fontSize;
    f32 charSpace;
    f32 lineSpace;
};

}

class TextBox : public Pane {
public:
    NW4R_UT_RTTI_DECL(TextBox);

public:
    TextBox(const res::TextBox* pRes, const ResBlockSet& rBlockSet);
    virtual ~TextBox();

    virtual void DrawSelf(const DrawInfo& rInfo);

    virtual ut::Color GetVtxColor(u32 idx) const;
    virtual void SetVtxColor(u32 idx, ut::Color color);

    virtual u8 GetVtxColorElement(u32 idx) const;
    virtual void SetVtxColorElement(u32 idx, u8 value);

    virtual void AllocStringBuffer(u16 minLen);
    virtual void FreeStringBuffer();

    virtual u16 SetString(const wchar_t* pStr, u16 dstIdx);
    virtual u16 SetString(const wchar_t* pStr, u16 dstIdx, u16 strLen);

    ut::Color GetTextColor(u32 type) const {
        return mTextColors[type];
    }
    void SetTextColor(u32 type, ut::Color value);

    const Size& GetFontSize() const {
        return mFontSize;
    }
    void SetFontSize(const Size& rFontSize) {
        mFontSize = rFontSize;
    }

    void SetFont(const ut::Font* pFont);

    void SetTagProcessor(ut::TagProcessorBase<wchar_t>* pProcessor) {
        mpTagProcessor = pProcessor;
    }

    u16 GetStringBufferLength() const;
    ut::Rect GetTextDrawRect(ut::TextWriterBase<wchar_t>* pWriter) const;

    f32 GetTextMagH() const;
    f32 GetTextMagV() const;

    u8 GetTextPositionH() const {
        return detail::GetHorizontalPosition(mTextPosition);
    }
    void SetTextPositionH(u8 pos) {
        detail::SetHorizontalPosition(&mTextPosition, pos);
    }

    u8 GetTextPositionV() const {
        return detail::GetVerticalPosition(mTextPosition);
    }
    void SetTextPositionV(u8 pos) {
        detail::SetVerticalPosition(&mTextPosition, pos);
    }

private:
    void Init(u16 allocStrLen);

private:
    wchar_t* mTextBuf;
    ut::Color mTextColors[2];
    const ut::Font* mpFont;
    Size mFontSize;
    f32 mLineSpace;
    f32 mCharSpace;
    ut::TagProcessorBase<wchar_t>* mpTagProcessor;
    u16 mTextBufBytes;
    u16 mTextLen;
    u8 mTextPosition;
    struct {
        u8 allocFont : 1;
    } mTextBoxFlag;
};

}
}

#endif
