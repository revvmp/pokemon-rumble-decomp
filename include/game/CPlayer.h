#ifndef GAME_CPLAYER_H
#define GAME_CPLAYER_H

class CPlayerPii;
class IController;

class CPlayer {
public:
    void SetController(IController* pController);
    bool IsJoined() const;
    CPlayerPii* GetPii();
};

#endif
