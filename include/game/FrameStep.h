#ifndef GAME_FRAMESTEP_H
#define GAME_FRAMESTEP_H

extern int gFrameStep;

class FrameStepScope {
public:
    FrameStepScope() : mPrevious(gFrameStep) {}
    ~FrameStepScope() { gFrameStep = mPrevious; }

private:
    int mPrevious;
};

#endif
