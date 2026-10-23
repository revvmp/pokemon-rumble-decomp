#ifndef GAME_HOME_BUTTON_H
#define GAME_HOME_BUTTON_H

#include <functional>
#include <string>
#include <revolution/hbm/HBMBase.h>
#include <game/DisableHBM.h>

class CHBMParam {
public:
    enum Flags { NoSaveWiiMenu = 1, NoSaveReset = 2, NoSaveAll = 4, PreloadResources = 16 };

    CHBMParam();
    ~CHBMParam();
    void SetResourceRoot(const char* pPath) { mResourceRoot = pPath; }
    void SetSoundRoot(const char* pPath) { mSoundRoot = pPath; }
    void SetExtensionRoot(const char* pPath) { mExtensionRoot = pPath; }
    void SetNoSaveMessagePath(const char* pPath) { mNoSaveMessagePath = pPath; }
    void SetMessagePath(const char* pPath) { mMessagePath = pPath; }
    void SetNetworkMessagePath(const char* pPath) { mNetworkMessagePath = pPath; }
    void SetModelPath(const char* pPath) { mModelPath = pPath; }
    void SetTextureName(const char* pName) { mTextureName = pName; }
    void SetFlags(unsigned long flags) { mFlags |= flags; }
    void SetOpenCallback(const std::tr1::function<void()>& rCallback) { mOpenCallback = rCallback; }
    void SetCloseCallback(const std::tr1::function<bool(HBMSelectBtnNum)>& rCallback) { mCloseCallback = rCallback; }
    const std::string& GetResourceRoot() const { return mResourceRoot; }
    const std::string& GetSoundRoot() const { return mSoundRoot; }
    const std::string& GetExtensionRoot() const { return mExtensionRoot; }
    const char* GetNoSaveMessagePath() const { return mNoSaveMessagePath.c_str(); }
    const char* GetMessagePath() const { return mMessagePath.c_str(); }
    const char* GetNetworkMessagePath() const { return mNetworkMessagePath.c_str(); }
    const char* GetModelPath() const { return mModelPath.c_str(); }
    const char* GetTextureName() const { return mTextureName.c_str(); }
    bool HasFlags(unsigned long flags) const { return (mFlags & flags) != 0; }
    const std::tr1::function<void()>& GetOpenCallback() const { return mOpenCallback; }
    const std::tr1::function<bool(HBMSelectBtnNum)>& GetCloseCallback() const { return mCloseCallback; }
    float GetFrameRate() const { return mFrameRate; }

private:
    unsigned long mFlags;
    float mFrameRate;
    std::string mResourceRoot;
    std::string mSoundRoot;
    std::string mExtensionRoot;
    std::string mNoSaveMessagePath;
    std::string mMessagePath;
    std::string mNetworkMessagePath;
    std::string mModelPath;
    std::string mTextureName;
    std::tr1::function<void()> mOpenCallback;
    std::tr1::function<bool(HBMSelectBtnNum)> mCloseCallback;
};

inline CHBMParam::Flags operator|(CHBMParam::Flags left, CHBMParam::Flags right) {
    return static_cast<CHBMParam::Flags>(static_cast<unsigned long>(left) | static_cast<unsigned long>(right));
}

bool InitializeHomeButton(const CHBMParam& rParam, bool allowYield);

#endif
