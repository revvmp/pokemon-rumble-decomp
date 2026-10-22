#ifndef NW4R_LYT_TEXT_BOX_H
#define NW4R_LYT_TEXT_BOX_H

#include <nw4r/lyt/lyt_common.h>
#include <nw4r/lyt/lyt_pane.h>
#include <revolution/gd/GDBase.h>

namespace nw4r {
namespace ut {
class Font;
template <typename CharT> class TagProcessorBase;
template <typename CharT> class TextWriterBase;
}
namespace lyt {
namespace res {
struct TextBox;
}
struct ResBlockSet;

class TextBox : public Pane {
public:
    NW4R_UT_RTTI_DECL(TextBox);

    TextBox(const res::TextBox* pBlock, const ResBlockSet& rBlockSet);
    virtual ~TextBox();
    virtual void DrawSelf(const DrawInfo& rInfo);
    virtual ut::Color GetVtxColor(u32 index) const;
    virtual void SetVtxColor(u32 index, ut::Color color);
    virtual u8 GetVtxColorElement(u32 index) const;
    virtual void SetVtxColorElement(u32 index, u8 value);
    virtual void SetExtUserDataList(const res::ExtUserDataList* pList);
    virtual void LoadMtx(const DrawInfo& rInfo);
    virtual void AllocStringBuffer(u16 minLength);
    virtual void FreeStringBuffer();
    virtual u16 SetString(const wchar_t* pString, u16 dstIndex);
    virtual u16 SetString(const wchar_t* pString, u16 dstIndex, u16 stringLength);

    void Init(u16 allocStringLength);
    ut::Rect GetTextDrawRect() const;
    ut::Rect GetTextDrawRect(ut::TextWriterBase<wchar_t>* pWriter) const;
    u16 SetStringImpl(const wchar_t* pString, u16 dstIndex, u32 stringLength);
    const ut::Font* GetFont() const;

    const wchar_t* GetStringBuffer() const { return mTextBuf; }

    u16 GetStringBufferLength() const {
        if (mTextBufBytes == 0) {
            return 0;
        }
        return mTextBufBytes / sizeof(wchar_t) - 1;
    }

    void SetFont(const ut::Font* pFont);
    f32 GetTextMagH() const;
    f32 GetTextMagV() const;
    f32 GetTextAlignMag() const;

    void SetFontSize(const Size& rSize) {
        mFontSize = rSize;
        mDrawCacheFlags |= 1;
    }

    const Size& GetFontSize() const { return mFontSize; }
    f32 GetCharSpace() const { return mCharSpace; }

    ut::Color GetTextColor(u32 index) const {
        return mTextColors[index];
    }

    void SetTextColor(u32 index, ut::Color color) {
        mTextColors[index] = color;
    }

    u8 GetTextPositionH() const {
        return detail::GetHorizontalPosition(mTextPosition);
    }

    u8 GetTextPositionV() const {
        return detail::GetVerticalPosition(mTextPosition);
    }

    void SetTextPositionH(u8 value) {
        detail::SetHorizontalPosition(&mTextPosition, value);
    }

    void SetTextPositionV(u8 value) {
        detail::SetVerticalPosition(&mTextPosition, value);
    }

    void SetTagProcessor(ut::TagProcessorBase<wchar_t>* pTagProcessor) {
        mpTagProcessor = pTagProcessor;
        mDrawCacheFlags |= 8;
    }

protected:
    void UpdateBorderFromExtUserData();

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
        u8 bAllocFont : 1;
        u8 textAlignment : 2;
        u8 bEnableBorder : 1;
    } mBits;
    mutable u32 mDrawCacheFlags;
    mutable ut::Rect mCachedTextDrawRect;
    mutable Size mCachedPaneSize;
    ut::Color mBorderColors[2];
    f32 mBorderWidth;
    GDLObj mDisplayList;
    void* mpDisplayListBuffer;
};
}
}

#endif
