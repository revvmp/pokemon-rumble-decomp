#ifndef GAME_CPLAYER_H
#define GAME_CPLAYER_H

class IController;

class CPlayer {
public:
    void SetController(IController* pController);
};

#endif
