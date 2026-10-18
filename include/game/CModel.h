#ifndef GAME_CMODEL_H
#define GAME_CMODEL_H

#include <game/ICalcDraw.h>
#include <memory>
#include <nw4r/g3d/g3d_calcworld.h>
#include <nw4r/g3d/g3d_scnobj.h>
#include <nw4r/math/math_types.h>
#include <vector>

namespace nw4r {
namespace g3d {
class AnmObjChrRootScale;
}
}

class CFileData;

class CModel : private ICalcDraw, private nw4r::g3d::ICalcWorldCallback {
public:
    class CMotionProgram;

    int GetMotionLength(const char* pName);
    std::tr1::shared_ptr<CMotionProgram> Play(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> PlayLoop(const char* pName, f32 frame, f32 rate);
    std::tr1::shared_ptr<CMotionProgram> PlayOnce(const char* pName, f32 frame, f32 rate);
    void Pause(bool pause);
    void UpdateFrame(f32 step);
    void SetRootScale(const std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale>& rRootScale);

    nw4r::g3d::ScnObj* GetScnObj() const { return mScnObj.get(); }

    std::tr1::shared_ptr<nw4r::g3d::ScnObj> mScnObj;
    std::vector<std::tr1::shared_ptr<CFileData> > mFiles;
    std::tr1::shared_ptr<nw4r::g3d::AnmObjChrRootScale> mRootScale;
};

#endif
