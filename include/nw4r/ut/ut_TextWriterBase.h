#ifndef NW4R_UT_TEXT_WRITER_BASE_H
#define NW4R_UT_TEXT_WRITER_BASE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/ut/ut_CharWriter.h>
#include <nw4r/ut/ut_TagProcessorBase.h>

#include <nw4r/math/math_types.h>

#include <stdio.h>
#include <wchar.h>

namespace nw4r {
namespace ut {

template <typename T> class TextWriterBase : public CharWriter {
public:
    typedef TagProcessorBase<T> TagProcessorType;

public:
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

    f32 GetWidthLimit() const {
        return mWidthLimit;
    }
    void SetWidthLimit(f32 limit) {
        mWidthLimit = limit;
    }
    void ResetWidthLimit() {
        mWidthLimit = NW4R_MATH_FLT_MAX;
    }

    f32 GetCharSpace() const {
        return mCharSpace;
    }
    void SetCharSpace(f32 space) {
        mCharSpace = space;
    }

    f32 GetLineSpace() const {
        return mLineSpace;
    }
    void SetLineSpace(f32 space) {
        mLineSpace = space;
    }

    int GetTabWidth() const {
        return mTabWidth;
    }
    void SetTabWidth(int width) {
        mTabWidth = width;
    }

    u32 GetDrawFlag() const {
        return mDrawFlag;
    }
    void SetDrawFlag(u32 flag) {
        mDrawFlag = flag;
    }

    TagProcessorBase<T>* GetTagProcessor() const {
        return mTagProcessor;
    }
    void SetTagProcessor(TagProcessorBase<T>* pProcessor) {
        mTagProcessor = pProcessor;
    }
    void ResetTagProcessor() {
        mTagProcessor = &mDefaultTagProcessor;
    }

    void SetLineHeight(f32 height);
    f32 GetLineHeight() const;
    f32 CalcFormatStringWidth(const T* str, ...) const;
    f32 CalcFormatStringHeight(const T* str, ...) const;
    void CalcFormatStringRect(Rect* rect, const T* str, ...) const;
    void CalcVStringRect(Rect* rect, const T* str, va_list args) const;
    f32 CalcStringWidth(const T* pStr, int len) const;
    f32 CalcStringHeight(const T* str, int len) const;
    void CalcStringRect(Rect* pRect, const T* pStr, int len) const;
    f32 Printf(const T* str, ...);
    f32 VPrintf(const T* pStr, va_list args);
    f32 Print(const T* pStr, int len);
    f32 PrintfMutable(const T* str, ...);
    f32 VPrintfMutable(const T* str, va_list args);
    f32 PrintMutable(const T* pStr, int len);
    f32 PrintMutableToDisplayList(const T* pStr, int len);
    static int VSNPrintf(char* buffer, u32 count, const char* str, va_list args) { return vsnprintf(buffer, count, str, args); }
    static int VSNPrintf(wchar_t* buffer, u32 count, const wchar_t* str, va_list args) { return vswprintf(buffer, count, str, args); }
    f32 CalcLineWidth(const T* pStr, int len);

    static T* GetBuffer() {
        return mFormatBuffer;
    }
    static T* SetBuffer(T* pBuffer, u32 size) {
        T* pOldBuffer = mFormatBuffer;
        mFormatBuffer = pBuffer;
        mFormatBufferSize = size;
        return pOldBuffer;
    }

    static u32 GetBufferSize() {
        return mFormatBufferSize;
    }

private:
    static const int DEFAULT_FORMAT_BUFFER_SIZE = 256;

    static const u32 DRAWFLAG_MASK_ALL = DRAWFLAG_MASK_ALIGN_TEXT |
                                         DRAWFLAG_MASK_ALIGN_H |
                                         DRAWFLAG_MASK_ALIGN_V;

private:

    bool CalcLineRectImpl(Rect* pRect, const T** ppStr, int len);
    void CalcStringRectImpl(Rect* pRect, const T* pStr, int len);

    f32 PrintImpl(const T* pStr, int len, bool mutableCursor);
    f32 PrintToDisplayListImpl(const T* pStr, int len, bool mutableCursor);
    f32 AdjustCursor(f32* pX, f32* pY, const T* pStr, int len);
    bool IsDrawFlagSet(u32 mask, u32 flag) const;

private:
    f32 mWidthLimit;
    f32 mCharSpace;
    f32 mLineSpace;
    int mTabWidth;
    u32 mDrawFlag;
    TagProcessorBase<T>* mTagProcessor;

    static T* mFormatBuffer;
    static int mFormatBufferSize;
    static TagProcessorBase<T> mDefaultTagProcessor;
};


template <typename T> bool TextWriterBase<T>::IsDrawFlagSet(u32 mask, u32 flag) const {
    return (mDrawFlag & mask) == flag;
}

}
}

#endif
