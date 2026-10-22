#include <game/CPpuEnv.h>
#include <game/CFiber.h>
#include <game/CPpuEngine.h>
#include <string.h>

extern const NPpu::SNativeFunction gScriptNativeBindings[];
extern const u32 gScriptNativeBindingCount;

namespace {
struct ScopedFlag {
    bool* mpFlag;
    explicit ScopedFlag(bool* pFlag) : mpFlag(pFlag) { *mpFlag = true; }
    ~ScopedFlag() { *mpFlag = false; }
};
}

std::tr1::shared_ptr<NPpu::CEnvironment> CreateScriptEnvironment() {
    return std::tr1::shared_ptr<NPpu::CEnvironment>(new CPpuEnv(0x400, gScriptNativeBindings, gScriptNativeBindingCount));
}

bool CPpuCode::Load(const std::tr1::shared_ptr<CFileData>& rFile, const char* pName) {
    if (!rFile || !rFile->IsLoaded()) {
        return false;
    }
    mFile = rFile;
    mName = pName;
    return NPpu::CCode::Load(mName.c_str(), rFile->GetData(), rFile->GetSize(), 0, 0);
}

std::tr1::shared_ptr<NPpu::CCode> LoadScriptCode(NPpu::CEnvironment* pBaseEnv, const char* pName, int wait) {
    CPpuEnv* pEnv = static_cast<CPpuEnv*>(pBaseEnv);
    if (wait) {
        while (pEnv->Loading()) {
            YieldFiber();
        }
    }
    ScopedFlag guard(&pEnv->Loading());
    std::string key(pName);
    CPpuEnv::CodeMap::iterator it = pEnv->Codes().find(key);
    if (it != pEnv->Codes().end()) {
        std::tr1::shared_ptr<NPpu::CCode> cached(it->second);
        if (cached) {
            return cached;
        }
        pEnv->Codes().erase(it);
    }
    std::tr1::shared_ptr<CPpuCode> code(new CPpuCode);
    bool loaded;
    {
        std::string directory("script.arc/");
        std::tr1::shared_ptr<CFileData> file = LoadLocalizedFile((directory + pName).c_str(), 0, wait, CPpuEngine::GetScriptRoot());
        loaded = code->Load(file, pName);
    }
    if (!loaded) {
        return std::tr1::shared_ptr<NPpu::CCode>();
    }
    pEnv->Codes()[key] = code;
    return code;
}

void NPpu::CEnvironment::CollectGarbage() {
    static_cast<CPpuEnv*>(this)->ClearCodes();
}
