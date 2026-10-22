#ifndef GAME_CFILEDATA_H
#define GAME_CFILEDATA_H

#include <functional>
#include <game/CPath.h>
#include <game/CScopedActivity.h>
#include <memory>
#include <string>
#include <revolution/os/OSMutex.h>
#include <revolution/os/OSThread.h>
#include <revolution/types.h>

class CFileData {
public:
    class CWorker;

    enum Flag {
        FLAG_DRIVE_MASK = 0x3FF,
        FLAG_KIND_MASK = 0xC00,
        FLAG_KIND_RESOURCE = 0x400,
        FLAG_KIND_RESOURCE_READY = 0x800,
        FLAG_TOP_DOWN = 0x4000,
        FLAG_PLACEMENT_MASK = 0x60000,
        FLAG_PLACEMENT_MEM1 = 0x20000,
        FLAG_PLACEMENT_PREFER_MEM2 = 0x40000,
        FLAG_PLACEMENT_PREFER_MEM1 = 0x60000
    };

    enum Error {
        ERROR_ARCHIVE_OPEN = -6000,
        ERROR_BAD_STATE = -6006,
        ERROR_OUT_OF_MEMORY = -6007,
        ERROR_BAD_COMPRESSION = -6010
    };

    CFileData();

    bool IsValid() const { return !mPending && mData != nullptr; }
    u32 GetSize() const { return mSize; }
    void* GetData() const { return mData; }
    const std::string& GetPath() const { return mPath; }
    bool IsLoaded() const { return !mPending && mData != nullptr; }

    void SetDrive(CPath::Drive drive);
    bool OpenArchiveEntry();

    static bool WaitUntilLoaded(CFileData* pFile);

private:
    friend class CWorker;

    static void* TryAllocateByPlacement(u32 size, int alignment, u32 flags);
    static void* AllocateByPlacement(u32 size, int alignment, u32 flags);
    void ValidateResource();

    u32 mFlags;
    std::string mPath;
    u32 mSize;
    std::tr1::shared_ptr<char> mBuffer;
    void* mData;
    std::tr1::shared_ptr<CFileData> mArchive;
    bool mPending;
    int mError;
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
