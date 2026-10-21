#ifndef GAME_CNULLOBJ_H
#define GAME_CNULLOBJ_H

#include <game/BWorldObj.h>
#include <list>
#include <memory>

class CFileData;

class CNullObj : public BWorldObj {
public:
    enum { TYPE = 1 << 17 };
    class CMotionProgram;
    typedef std::tr1::shared_ptr<CMotionProgram> MotionPtr;
    typedef std::list<MotionPtr> MotionList;

    explicit CNullObj(u32 type);
    virtual ~CNullObj();
    virtual void Calc();
    void SetTarget(const std::tr1::shared_ptr<BWorldObj>& rTarget);
    bool LoadAnm(const char* pPath, bool background);

private:
    u32 mNextMotionID;
    std::tr1::shared_ptr<CFileData> mAnmFile;
    MotionList mMotionPrograms;
    f32 mMotionFrame;
    std::tr1::weak_ptr<BWorldObj> mTarget;
};

#endif
