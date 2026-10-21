#ifndef GAME_DETAIL_CCOMBODISPLAY_H
#define GAME_DETAIL_CCOMBODISPLAY_H

#include <game/IComboDisplay.h>
#include <game/LayoutResources.h>
#include <game/StringFormat.h>
#include <game/IGameMap.h>

namespace {
class CComboDisplay : public IComboDisplay {
public:
    enum State { Hidden, Begin, Loop, End };
    CComboDisplay() : mState(Hidden), mComboCount(0), mComboGauge(0) {}
    virtual int GetComboGauge() const { return mComboGauge; }
    virtual void SetComboGauge(int gauge) { mComboGauge = gauge; }
    virtual int GetComboCount() const { return mComboCount; }
    virtual void SetComboCount(int count) { mComboCount = count; }
    virtual void ShowComboCount() {
        mState = Begin;
        mLayout.SetText("NUMBER", FormatString(L"%d", mComboCount).c_str());
        mMotion = mLayout.CreateMotion("begin");
    }
    virtual void Calc() {
        switch (mState) {
        case Begin:
            if (mMotion->IsEnded()) {
                mMotion.reset();
                mState = Loop;
                mLayout.CreateMotion("loop");
            }
            break;
        case Loop:
            if (mComboGauge <= mEndFrameCount) {
                mState = End;
                mLayout.CreateMotion("end", spDefaultMotionPane, mEndFrameCount - mComboGauge, 1.0f);
            }
            break;
        case End:
            if (mEndFrameCount < mComboGauge) {
                mState = Loop;
                mLayout.CreateMotion("loop");
            } else if (mComboGauge == 0) {
                mState = Hidden;
            } else {
                mLayout.CreateMotion("end", spDefaultMotionPane, mEndFrameCount - mComboGauge, 1.0f);
            }
            break;
        }
        mLayout.Calc();
    }
    virtual void Draw() {
        if (!GetCurrentGameMap()->IsPausing() && mState != Hidden) {
            mLayout.Draw();
        }
    }

    void Load(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts, bool background) {
        mLayout.Load(gSharedLayoutArchives, "combo", rFonts, background);
        mEndFrameCount = mLayout.GetMotionLength("end");
    }

private:
    State mState;
    CLayout mLayout;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
    int mComboCount;
    int mComboGauge;
    int mEndFrameCount;
};
}

#endif
