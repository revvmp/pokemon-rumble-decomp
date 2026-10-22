#ifndef GAME_CDIVIDEDGROUND_H
#define GAME_CDIVIDEDGROUND_H

#include <game/BModelObj.h>
#include <list>
#include <memory>
#include <nw4r/g3d/g3d_scnobj.h>
#include <vector>

class CPpuEnv;

namespace nw4r {
namespace g3d {
class ScnGroupEx;
}
}

namespace NPpu {
class CEnvironment;
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
    static std::tr1::shared_ptr<CDividedGround> Create(u32 type);
    virtual ~CDividedGround();

    virtual void Calc();
    virtual CPpuEngine* GetPpuEngine() const { return mPpuEngine.get(); }
    virtual void OnEnterWorld();
    virtual void OnLeaveWorld(BWorld* pWorld);
    virtual void UpdateVisible() {}

    bool Load(const StageID* pStageID, int stageNo, bool background);
    bool LoadPkc(int mode, const std::tr1::shared_ptr<CPpuEnv>& rEnv, const char* pPrefix);
    bool LoadAttr(const char* pName, bool background);
    bool IsOutOfBounds(const nw4r::math::VEC3& rPos);
    CBSPTree* GetBSPTree(int index) { return mBSPTrees[index].get(); }

private:
    void BuildScene(const StageID* pStageID, int stageNo, bool background);
    void LoadFileLists(const StageID* pStageID, int stageNo, bool background);
    bool LoadAttr(const StageID* pStageID, int stageNo, const char* pName, bool background);

    static const char* FindIndexSuffix(const char* pName);

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
