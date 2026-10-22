#ifndef GAME_ISAVEMANAGER_H
#define GAME_ISAVEMANAGER_H

#include <game/ICalcDraw.h>
#include <memory>

class ISaveManager : public ICalcDraw {
public:
    virtual ~ISaveManager() {}
    virtual void LoadSync() = 0;
    virtual void RequestSave() = 0;
    virtual void RequestSaveWithoutBackup() = 0;
    virtual void RequestSaveCommitOnly() = 0;
    virtual void RequestDelete() = 0;
    virtual bool IsIdle() = 0;

    static ISaveManager* GetInstance();
    static std::tr1::shared_ptr<ISaveManager> Create();
};

#endif
