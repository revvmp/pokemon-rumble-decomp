#ifndef GAME_RESOURCEFILECONFIG_H
#define GAME_RESOURCEFILECONFIG_H

#include <functional>
#include <game/CPath.h>

typedef std::tr1::function<void(CPath::Drive, const char*, long, unsigned long)> FileErrorHandler;

struct ResourceFileConfig {
    ResourceFileConfig();
    void SetErrorHandler(const FileErrorHandler& rHandler);
    FileErrorHandler mErrorHandler;
    int mReadPriority;
    int mDecodePriority;
    unsigned long mStackSize;
    bool mUseDriveWorker;
};

void HandleFileError(CPath::Drive drive, const char* pPath, long error, unsigned long flags);
void InitializeResourceFiles(const ResourceFileConfig& rConfig);
void ShutdownResourceFiles();

#endif
