#ifndef GAME_CPPUENV_H
#define GAME_CPPUENV_H

#include <game/CFileData.h>
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

    void SetFile(const std::tr1::shared_ptr<CFileData>& rFile) { mFile = rFile; }
    bool Load(const std::tr1::shared_ptr<CFileData>& rFile, const char* pName);

private:
    SelfReference<CPpuCode> mRef;
    std::tr1::shared_ptr<CFileData> mFile;
    std::string mName;
};

class CPpuEnv : public NPpu::CEnvironment, public std::tr1::enable_shared_from_this<CPpuEnv> {
public:
    typedef std::tr1::shared_ptr<NPpu::CEnvironment> SelfPtr;
    typedef std::map<std::string, std::tr1::shared_ptr<NPpu::CCode> > CodeMap;

    CPpuEnv(int globalCount, const NPpu::SNativeFunction* pNatives, u32 nativeCount)
        : NPpu::CEnvironment(globalCount, pNatives, nativeCount), mRef(), mLoading(false) {}
    virtual ~CPpuEnv() {}

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

    bool& Loading() { return mLoading; }
    CodeMap& Codes() { return mCodes; }
    void ClearCodes() { mCodes.clear(); }

private:
    SelfReference<NPpu::CEnvironment> mRef;
    bool mLoading;
    CodeMap mCodes;
};

#endif
