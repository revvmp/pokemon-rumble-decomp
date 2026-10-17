#ifndef NW4HBM_UT_TEXT_WRITER_BASE_H
#define NW4HBM_UT_TEXT_WRITER_BASE_H
#include "nw4hbm/types_nw4hbm.h"

#include "nw4hbm/ut/ut_CharWriter.h"
#include "nw4hbm/ut/ut_TagProcessorBase.h"

#include <stdarg.h>

namespace nw4hbm {
namespace ut {

template <typename T> class TextWriterBase : public CharWriter {
public:
    typedef TagProcessorBase<T> TagProcessorType;

    enum DrawFlag {
        DRAWFLAG_ALIGN_TEXT_BASELINE = 0,
        DRAWFLAG_ALIGN_TEXT_CENTER = (1 << 0),
        DRAWFLAG_ALIGN_TEXT_RIGHT = (1 << 1),

        DRAWFLAG_ALIGN_H_BASELINE = 0,
        DRAWFLAG_ALIGN_H_CENTER = (1 << 4),
        DRAWFLAG_ALIGN_H_RIGHT = (1 << 5),

        DRAWFLAG_ALIGN_V_BASELINE = 0,
        DRAWFLAG_ALIGN_V_CENTER = (1 << 8),
        DRAWFLAG_ALIGN_V_TOP = (1 << 9),

        DRAWFLAG_MASK_ALIGN_TEXT = DRAWFLAG_ALIGN_TEXT_BASELINE |
                                   DRAWFLAG_ALIGN_TEXT_CENTER |
                                   DRAWFLAG_ALIGN_TEXT_RIGHT,

        DRAWFLAG_MASK_ALIGN_H = DRAWFLAG_ALIGN_H_BASELINE |
                                DRAWFLAG_ALIGN_H_CENTER |
                                DRAWFLAG_ALIGN_H_RIGHT,

        DRAWFLAG_MASK_ALIGN_V = DRAWFLAG_ALIGN_V_BASELINE |
                                DRAWFLAG_ALIGN_V_CENTER | DRAWFLAG_ALIGN_V_TOP,
    };

public:
    TextWriterBase();
    ~TextWriterBase();

    void SetLineHeight(f32 height);
    f32 GetLineHeight() const;

    void SetLineSpace(f32 space);
    void SetCharSpace(f32 space);
    f32 GetLineSpace() const;
    f32 GetCharSpace() const;

    void SetTabWidth(int width);
    int GetTabWidth() const;

    void SetDrawFlag(u32 flag);
    u32 GetDrawFlag() const;

    void SetTagProcessor(TagProcessorType* pProcessor);
    void ResetTagProcessor();
    TagProcessorType& GetTagProcessor() const;

    f32 CalcFormatStringWidth(const T* pFormat, ...) const;
    f32 CalcFormatStringHeight(const T* pFormat, ...) const;
    void CalcFormatStringRect(Rect* pRect, const T* pFormat, ...) const;
    void CalcVStringRect(Rect* pRect, const T* pFormat, va_list args) const;

    f32 CalcStringWidth(const T* pStr, int len) const;
    f32 CalcStringWidth(const T* pStr) const;
    f32 CalcStringHeight(const T* pStr, int len) const;
    f32 CalcStringHeight(const T* pStr) const;
    void CalcStringRect(Rect* pRect, const T* pStr, int len) const;
    void CalcStringRect(Rect* pRect, const T* pStr) const;

    f32 Printf(const T* pFormat, ...);
    f32 VPrintf(const T* pFormat, va_list args);
    f32 Print(const T* pStr, int len);
    f32 Print(const T* pStr);

    static T* SetBuffer(T* pBuffer, u32 size);
    static T* SetBuffer(u32 size);
    static u32 GetBufferSize();
    static T* GetBuffer();

    static int VSNPrintf(T* pBuffer, u32 count, const T* pFormat, va_list args);
    static int StrLen(const T* pStr);

private:
    static const int DEFAULT_FORMAT_BUFFER_SIZE = 256;

    static const u32 DRAWFLAG_MASK_ALL = DRAWFLAG_MASK_ALIGN_TEXT |
                                         DRAWFLAG_MASK_ALIGN_H |
                                         DRAWFLAG_MASK_ALIGN_V;

    f32 CalcLineWidth(const T* pStr, int len);
    int CalcLineRectImpl(Rect* pRect, const T* pStr, int len);
    void CalcStringRectImpl(Rect* pRect, const T* pStr, int len);
    f32 PrintImpl(const T* pStr, int len);
    f32 AdjustCursor(f32* pX, f32* pY, const T* pStr, int len);

    bool IsDrawFlagSet(u32 mask, u32 flag) const {
        return (mDrawFlag & mask) == flag;
    }

private:
    f32 mCharSpace;
    f32 mLineSpace;
    int mTabWidth;
    u32 mDrawFlag;
    TagProcessorType* mTagProcessor;

    static T* mFormatBuffer;
    static u32 mFormatBufferSize;
    static TagProcessorType mDefaultTagProcessor;
};

}
}

#endif
