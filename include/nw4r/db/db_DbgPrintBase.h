#ifndef NW4R_DB_DBG_PRINT_BASE_H
#define NW4R_DB_DBG_PRINT_BASE_H

#include <nw4r/ut/ut_Color.h>
#include <nw4r/ut/ut_Font.h>
#include <nw4r/ut/ut_list.h>
#include <revolution/mem/expHeap.h>
#include <stdarg.h>
#include <stddef.h>

namespace nw4r {
namespace db {

template <typename T> class DbgPrintBase {
public:
    struct Entry {
        int x;
        int y;
        int frames;
        int length;
        ut::Link link;
        T text[];
    };

    static DbgPrintBase* GetInstance();
    static void Create(void* pBuffer, u32 size, ut::Color color);
    static void Create(void* pBuffer, u32 size, const ut::Font* pFont, ut::Color color);
    void SetFont(const ut::Font* pFont);
    const ut::Font* GetFont() const;
    void SetColor(GXColor color);
    ut::Color GetColor() const;
    void SetFontSize(f32 scale);
    f32 GetFontSize() const;
    void SetVisible(bool visible);
    bool IsVisible() const;
    void Printf(int x, int y, const T* pFormat, ...);
    void Printf(int x, int y, int frames, const T* pFormat, ...);
    void VPrintf(int x, int y, int frames, const T* pFormat, va_list args);
    void Append(int x, int y, int frames, const T* pText, int length) __attribute__((never_inline));
    void PrintfCopy(int x, int y, const T* pFormat, ...);
    void PrintfCopy(int x, int y, int frames, const T* pFormat, ...);
    void VPrintfCopy(int x, int y, int frames, const T* pFormat, va_list args);
    void AppendCopy(int x, int y, int frames, const T* pText, int length);
    void Clear();
    void Draw();
    void Draw(int x, int y, int width, int height);
    DbgPrintBase(ut::Color color, const ut::Font* pFont);
    ~DbgPrintBase();
    Entry* GetFirst();
    Entry* GetNext(Entry* pEntry);
    void Remove(Entry* pEntry);
    static void CreateHeap(void* pBuffer, u32 size);
    static void* DestroyHeap();
    static void* operator new(size_t size);
    static void operator delete(void* pBuffer);

private:
    const ut::Font* mFont;
    ut::List mEntries;
    GXColor mColor;
    f32 mFontSize;
    bool mVisible;
    static MEMHeapHandle sHeap;
    static DbgPrintBase* sInstance;
};

}
}

#endif
