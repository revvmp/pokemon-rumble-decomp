#ifndef NW4R_UT_TAG_PROCESSOR_BASE_H
#define NW4R_UT_TAG_PROCESSOR_BASE_H
#include <nw4r/types_nw4r.h>

namespace nw4r {
namespace ut {

struct Rect;
template <typename T> class TextWriterBase;

template <typename T> struct PrintContext {
    enum Flags {
        FLAGS_CHARSPACE = (1 << 0),
    };

    TextWriterBase<T>* writer;
    const T* str;
    f32 x;
    f32 y;
    u32 flags;
};

template <typename T> class TagProcessorBase {
public:
    typedef PrintContext<T> ContextType;

    enum Operation {
        OPERATION_DEFAULT,
        OPERATION_NO_CHAR_SPACE,
        OPERATION_CHAR_SPACE,
        OPERATION_NEXT_LINE,
        OPERATION_END_DRAW
    };

public:
    TagProcessorBase();
    virtual ~TagProcessorBase();

    virtual Operation Process(u16 ch, ContextType* pCtx);
    virtual Operation CalcRect(Rect* pRect, u16 ch,
                               ContextType* pCtx);

private:
    void ProcessLinefeed(ContextType* pCtx);
    void ProcessTab(ContextType* pCtx);
};

}
}

#endif
