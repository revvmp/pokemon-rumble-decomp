#ifndef GAME_GAMELOOP_H
#define GAME_GAMELOOP_H

#include <revolution/types.h>

class IGameLoopCallback {
public:
    virtual bool IsRunning() = 0;
    virtual void ResetTiming() = 0;
    virtual void Update() = 0;
    virtual bool ShouldRender() = 0;
    virtual void Prepare() = 0;
    virtual void Calc() = 0;
    virtual void Draw() = 0;
    virtual void Render() = 0;
    virtual void Present() = 0;
};

extern int gGameFrame;

void RunGameLoop(IGameLoopCallback* pCallback);

#endif
