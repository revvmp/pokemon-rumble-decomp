#ifndef GAME_CDISPLAYPHASECTRL_H
#define GAME_CDISPLAYPHASECTRL_H

#include <game/ICalc.h>
#include <game/CLayout.h>

class CDisplayPhaseCtrl : public ICalc {
public:
    explicit CDisplayPhaseCtrl(CLayout* pLayout);
    virtual ~CDisplayPhaseCtrl();
    void SetTarget(CLayout* pLayout, nw4r::lyt::Pane* pPane);
    void SetTarget(CLayout* pLayout, const char* pPaneName);
    void SetForceVisible(bool visible);
    void Show();
    void Hide();
    bool IsVisible() const;
    bool IsActive() const { return mPhase != PHASE_HIDDEN; }
    virtual void Calc();

private:
    enum Phase {
        PHASE_HIDDEN,
        PHASE_BEGIN,
        PHASE_BEGIN_HIDE,
        PHASE_VISIBLE,
        PHASE_END,
        PHASE_END_SHOW
    };

    void Begin();
    void End();

    Phase mPhase;
    bool mShowRequested;
    bool mForceVisible;
    CLayout* mpLayout;
    nw4r::lyt::Pane* mpPane;
    std::tr1::shared_ptr<CLayout::CMotionProgram> mMotion;
};

#endif
