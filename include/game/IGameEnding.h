#ifndef GAME_IGAMEENDING_H
#define GAME_IGAMEENDING_H

#include <game/IGameMap.h>

class IGameEnding : public IGameMap {
public:
    virtual ~IGameEnding() {}
    virtual void ShowCongratulations() = 0;
    virtual void LocalizeCongratulations() = 0;
    virtual void BuildStaffRollText() = 0;
    virtual void StartStaffRoll() = 0;
    virtual void HideStaffRoll() = 0;
    virtual bool IsStaffRollFinished() = 0;
    virtual void ShowNext() = 0;
    virtual void LocalizeNext() = 0;
    virtual bool IsNextVisible() = 0;
};

#endif
