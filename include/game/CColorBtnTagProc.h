#ifndef GAME_CCOLORBTNTAGPROC_H
#define GAME_CCOLORBTNTAGPROC_H

#include <nw4r/ut/ut_CharWriter.h>
#include <nw4r/ut/ut_TagProcessorBase.h>

namespace nw4r {
namespace lyt {
class Pane;
}
}

class CColorBtnTagProc : public nw4r::ut::TagProcessorBase<wchar_t> {
public:
    CColorBtnTagProc() : mSavedGradation(nw4r::ut::CharWriter::GRADMODE_V) {}
    virtual ~CColorBtnTagProc();

    virtual Operation Process(u16 ch, ContextType* pContext);
    virtual Operation CalcRect(nw4r::ut::Rect* pRect, u16 ch,
                               ContextType* pContext);

    static void Attach(nw4r::lyt::Pane* pPane);
    static void SetColor(int index, nw4r::ut::Color start,
                         nw4r::ut::Color end);

private:
    static CColorBtnTagProc sInstance;
    static nw4r::ut::Color sStartColors[5];
    static nw4r::ut::Color sEndColors[5];

    nw4r::ut::CharWriter::GradationMode mSavedGradation;
    nw4r::ut::Color mSavedStart;
    nw4r::ut::Color mSavedEnd;
};

#endif
