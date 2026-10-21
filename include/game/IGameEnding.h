#ifndef GAME_IGAMEENDING_H
#define GAME_IGAMEENDING_H

#include <game/IGameMap.h>

class IGameEnding : public IGameMap {
public:
    virtual ~IGameEnding() {}

    static std::tr1::shared_ptr<IGameEnding> Create(const MapDesc* pDesc, bool background);

    virtual void ShowCongratulations() = 0;
    virtual void StopCongratulations() = 0;
    virtual void ParseStaffRollText(const wchar_t* pText) = 0;
    virtual void StartStaffRoll() = 0;
    virtual void HideStaffRoll() = 0;
    virtual bool IsStaffRollFinished() = 0;
    virtual void ShowNext() = 0;
    virtual void StopNext() = 0;
    virtual bool IsNextVisible() = 0;
};

#endif
