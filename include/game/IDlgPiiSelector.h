#ifndef GAME_IDLGPIISELECTOR_H
#define GAME_IDLGPIISELECTOR_H

#include <game/IDlg.h>
#include <memory>

class CPiiPersonalData;
class IController;

class IDlgPiiSelector : public IDlg {
public:
    virtual ~IDlgPiiSelector() {}
    virtual bool IsShown(int player) = 0;
    virtual void Show(int player, bool flag) = 0;
    virtual bool IsMultiPlayerEntranceShown() = 0;
    virtual void ShowMultiPlayerEntrance() = 0;
    virtual void SetEntrancePPD(IController* pController, const std::tr1::shared_ptr<CPiiPersonalData>& rPPD) = 0;

    static std::tr1::shared_ptr<IDlgPiiSelector> Create(bool flag);
};

#endif
