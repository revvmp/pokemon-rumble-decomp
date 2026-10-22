#ifndef GAME_CFILEDATA_H
#define GAME_CFILEDATA_H

#include <memory>
#include <string>
#include <revolution/types.h>

class CFileData {
public:
    bool IsValid() const { return !mFailed && mData != nullptr; }
    u32 GetSize() const { return mSize; }
    void* GetData() const { return mData; }
    const std::string& GetPath() const { return mPath; }
    bool IsLoaded() const { return !mFailed && mData != nullptr; }

private:
    u32 mFlags;
    std::string mPath;
    u32 mSize;
    std::tr1::shared_ptr<u8> mBuffer;
    void* mData;
    std::tr1::shared_ptr<CFileData> mArchive;
    bool mFailed;
    int mState;
};

std::tr1::shared_ptr<CFileData> CreateFileData(const char* pPath, u32 flags, bool background);
std::tr1::shared_ptr<CFileData> LoadFile(const char* pPath, u32 flags, bool background);
std::tr1::shared_ptr<CFileData> LoadResourceFile(const char* pPath, u32 flags, bool background);
std::tr1::shared_ptr<CFileData> RequestFile(const char* pPath, u32 flags, bool background);
std::tr1::shared_ptr<CFileData> LoadLocalizedFile(const char* pPath, u32 flags, u32 mode, const char* pRegion);

inline std::tr1::shared_ptr<CFileData> LoadResourceFile(const char* pPath, u32 flags, u32 mode, const char* pRegion) {
    return LoadLocalizedFile(pPath, flags | 0x2000, mode, pRegion);
}

#endif
