#ifndef GAME_CPPUENV_H
#define GAME_CPPUENV_H

#include <game/CFileData.h>
#include <game/CPpuEngine.h>
#include <game/NPpuVM.h>
#include <map>
#include <memory>
#include <string>

template <typename T> struct SelfReference {
    SelfReference() : mCount(0) {}
    ~SelfReference() {}

    int mCount;
    std::tr1::shared_ptr<T> mSelf;
};

class CPpuCode;

class CPpuEnv : public NPpu::CEnvironment, public std::tr1::enable_shared_from_this<CPpuEnv> {
    friend std::tr1::shared_ptr<NPpu::CCode> LoadScriptCode(NPpu::CEnvironment* pBaseEnv, const char* pName, bool wait);

public:
    typedef std::tr1::shared_ptr<CPpuEnv> SelfPtr;
    typedef std::map<std::string, std::tr1::shared_ptr<CPpuCode> > CodeMap;

    CPpuEnv(int globalCount, const NPpu::SNativeFunction* pNatives, u32 nativeCount);
    virtual ~CPpuEnv();

    virtual void AddRef() {
        if (mRef.mCount == 0) {
            mRef.mSelf = shared_from_this();
        }
        ++mRef.mCount;
    }
    virtual void Release() {
        if (--mRef.mCount == 0) {
            SelfPtr keepAlive = mRef.mSelf;
            mRef.mSelf.reset();
            keepAlive.reset();
        }
    }

    void ClearCodes() { mCodes.clear(); }

private:
    SelfReference<CPpuEnv> mRef;
    bool mLoading;
    CodeMap mCodes;
};

class CPpuCode : public NPpu::CCode, public std::tr1::enable_shared_from_this<CPpuCode> {
public:
    typedef std::tr1::shared_ptr<CPpuCode> SelfPtr;

    CPpuCode() : mRef() {}

    virtual void AddRef() {
        if (mRef.mCount == 0) {
            mRef.mSelf = shared_from_this();
        }
        ++mRef.mCount;
    }
    virtual void Release() {
        if (--mRef.mCount == 0) {
            SelfPtr keepAlive = mRef.mSelf;
            mRef.mSelf.reset();
            keepAlive.reset();
        }
    }

    bool Load(const char* pName, bool wait) {
        std::tr1::shared_ptr<CFileData> file = LoadLocalizedFile((std::string("script.arc/") + pName).c_str(), 0, wait, CPpuEngine::GetScriptRoot());
        if (!file || !file->IsLoaded()) {
            return false;
        }
        mFile = file;
        mName = pName;
        return NPpu::CCode::Load(mName.c_str(), mFile->GetData(), mFile->GetSize(), 0, 0);
    }

private:
    SelfReference<CPpuCode> mRef;
    std::tr1::shared_ptr<CFileData> mFile;
    std::string mName;
};

#endif
