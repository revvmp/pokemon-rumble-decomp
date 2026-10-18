#ifndef GAME_CDIVIDEDGROUND_H
#define GAME_CDIVIDEDGROUND_H

#include <game/BModelObj.h>
#include <list>
#include <memory>
#include <nw4r/g3d/g3d_scnobj.h>
#include <vector>

namespace nw4r {
namespace g3d {
class ScnGroupEx;
}
}

class CBSPTree;
class CFileData;
class CModel;
class CPpuEngine;
class ScnMdlSimpleData;
struct StageID;

class CDividedGround : public BModelObj {
public:
    enum { TYPE = 1 << 15 };

    enum { BSP_COUNT = 2 };

    explicit CDividedGround(u32 type);
    virtual ~CDividedGround();

    virtual void Calc();
    virtual BWorldObjOwner* GetOwner() const;
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld();
    virtual void UpdateVisible();

private:
    std::tr1::shared_ptr<CPpuEngine> mPpuEngine;
    const StageID* mStageID;
    int mStageNo;
    std::list<std::tr1::shared_ptr<CFileData> > mFiles;
    std::vector<const char*> mFileNames;
    std::tr1::shared_ptr<CFileData> mBSPFiles[BSP_COUNT];
    std::tr1::shared_ptr<CBSPTree> mBSPTrees[BSP_COUNT];
    std::tr1::shared_ptr<nw4r::g3d::ScnGroup> mScnGroup;
    std::list<std::tr1::shared_ptr<nw4r::g3d::ScnGroupEx> > mScnGroups;
    std::list<std::tr1::shared_ptr<CModel> > mModels;
    std::list<std::tr1::shared_ptr<ScnMdlSimpleData> > mSimpleModels;
};

#endif
