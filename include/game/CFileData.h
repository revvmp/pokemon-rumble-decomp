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

class CFileData;
std::tr1::shared_ptr<CFileData> OpenFileData(const CPath& rPath, u32 flags, long& rError);
std::tr1::shared_ptr<CFileData> OpenFileData(const char* pPath, u32 flags);

class CFileData {
public:
    class CWorker;

    enum Flag {
        FLAG_DRIVE_MASK = 0x3FF,
        FLAG_KIND_MASK = 0xC00,
        FLAG_KIND_RESOURCE = 0x400,
        FLAG_KIND_RESOURCE_READY = 0x800,
        FLAG_RAW = 0x1000,
        FLAG_OPTIONAL = 0x2000,
        FLAG_TOP_DOWN = 0x4000,
        FLAG_ALLOW_OUT_OF_MEMORY = 0x8000,
        FLAG_ALLOW_EMPTY = 0x10000,
        FLAG_PLACEMENT_MASK = 0x60000,
        FLAG_PLACEMENT_MEM1 = 0x20000,
        FLAG_PLACEMENT_PREFER_MEM2 = 0x40000,
        FLAG_PLACEMENT_PREFER_MEM1 = 0x60000
    };

    enum Error {
        ERROR_ARCHIVE_OPEN = -6000,
        ERROR_NOT_FOUND = -6003,
        ERROR_BAD_JOB = -6004,
        ERROR_BAD_DECODE_JOB = -6005,
        ERROR_BAD_STATE = -6006,
        ERROR_OUT_OF_MEMORY = -6007,
        ERROR_SHUTDOWN = -6008,
        ERROR_DECOMPRESS = -6009,
        ERROR_BAD_COMPRESSION = -6010
    };

    CFileData();
    ~CFileData();

    bool IsValid() const { return !mPending && mData != nullptr; }
    u32 GetSize() const { return mSize; }
    long GetError() const { return mError; }
    void* GetData() const { return mData; }
    const std::string& GetPath() const { return mPath; }
    bool IsLoaded() const { return !mPending && mData != nullptr; }

    void ReportError();
    void SetDrive(CPath::Drive drive);
    CPath::Drive GetDrive() const { return static_cast<CPath::Drive>(mFlags & FLAG_DRIVE_MASK); }
    bool OpenArchiveEntry();

    static bool WaitUntilLoaded(CFileData* pFile);

private:
    static void* TryAllocateByPlacement(u32 size, int alignment, u32 flags);
    friend class CWorker;
    friend std::tr1::shared_ptr<CFileData> OpenFileData(const CPath& rPath, u32 flags, long& rError);
    friend std::tr1::shared_ptr<CFileData> OpenFileData(const char* pPath, u32 flags);

    static void* AllocateByPlacement(u32 size, int alignment, u32 flags);
    void ValidateResource();

    u32 mFlags;
    std::string mPath;
    u32 mSize;
    std::tr1::shared_ptr<char> mBuffer;
    void* mData;
    std::tr1::shared_ptr<CFileData> mArchive;
    volatile bool mPending;
    int mError;
};

std::tr1::shared_ptr<CFileData> CreateFileData(const char* pPath, u32 flags, int background);
std::tr1::shared_ptr<CFileData> LoadFile(const char* pPath, u32 flags, int background);
std::tr1::shared_ptr<CFileData> LoadResourceFile(const char* pPath, u32 flags, int background);
std::tr1::shared_ptr<CFileData> RequestFile(const char* pPath, u32 flags, bool background);
std::tr1::shared_ptr<CFileData> LoadLocalizedFile(const char* pPath, u32 flags, int background, const char* pRegion);

inline std::tr1::shared_ptr<CFileData> LoadResourceFile(const char* pPath, u32 flags, int background, const char* pRegion) {
    return LoadLocalizedFile(pPath, flags | 0x2000, background, pRegion);
}

#endif
