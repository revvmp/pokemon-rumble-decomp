#ifndef GAME_CSTAGECAMERA_H
#define GAME_CSTAGECAMERA_H

#include <game/world/BWorldCamera.h>
#include <game/script/CPpuEngine.h>
#include <memory>

class CPpuEnv;

class IUIInput;

class CStageCamera : public BWorldCamera {
public:
    enum { TYPE = 1 << 3 };

    explicit CStageCamera(u32 type);
    virtual ~CStageCamera();

    virtual void Calc();
    virtual CPpuEngine* GetPpuEngine() const { return mEngine.get(); }
    virtual void OnLeaveWorld(BWorld* pWorld);
    virtual void UpdateIndex(int prevIndex);

    void LoadScript(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv);
    void LoadScript(const std::tr1::shared_ptr<CPpuEnv>& rEnv) { LoadScript(0, rEnv); }
    f32 GetStickWorldAngle(const IUIInput* pInput);

private:
    void UpdateLightPosition();

    u32 mReserved;
    std::tr1::shared_ptr<CPpuEngine> mEngine;
};

#endif
