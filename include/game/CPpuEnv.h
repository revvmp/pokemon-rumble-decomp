#ifndef GAME_CPPUENV_H
#define GAME_CPPUENV_H

#include <game/CFileData.h>
#include <game/NPpuVM.h>
#include <map>
#include <memory>
#include <string>

class CPpuCode : public NPpu::CCode, public std::tr1::enable_shared_from_this<CPpuCode> {
public:
    CPpuCode() : mReserved0(0), mReserved1(0), mReserved2(0) {}

    void SetFile(const std::tr1::shared_ptr<CFileData>& rFile) { mFile = rFile; }
    bool Load(const std::tr1::shared_ptr<CFileData>& rFile, const char* pName);

private:
    int mReserved0;
    int mReserved1;
    int mReserved2;
    std::tr1::shared_ptr<CFileData> mFile;
    std::string mName;
};

class CPpuEnv : public NPpu::CEnvironment, public std::tr1::enable_shared_from_this<NPpu::CEnvironment> {
public:
    typedef std::map<std::string, std::tr1::shared_ptr<NPpu::CCode> > CodeMap;

    CPpuEnv(int globalCount, const NPpu::SNativeFunction* pNatives, u32 nativeCount)
        : NPpu::CEnvironment(globalCount, pNatives, nativeCount), mReserved(0), mLoading(false) {}
    virtual ~CPpuEnv() {}

    bool& Loading() { return mLoading; }
    CodeMap& Codes() { return mCodes; }

private:
    int mReserved;
    std::tr1::shared_ptr<std::string> mReservedString;
    bool mLoading;
    CodeMap mCodes;
};

#endif
