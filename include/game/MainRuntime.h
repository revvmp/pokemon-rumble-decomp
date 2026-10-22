#ifndef GAME_MAIN_RUNTIME_DECLARATIONS_H
#define GAME_MAIN_RUNTIME_DECLARATIONS_H

#include <game/CLayout.h>
#include <game/CPath.h>
#include <game/FileList.h>
#include <functional>

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

struct MainMessageFile {
    const char* mpPath;
    bool mConvertEscape;
};

extern const MainMessageFile gMainMessageFiles[9];
extern const unsigned char gSystemMessageArchive[];

void InitializeMainMessages();
std::tr1::shared_ptr<CLayout::FontMap> LoadMainFonts();
std::tr1::shared_ptr<FileList> LoadStartupArchives();
void AppendLoadedFile(FileList& rFiles, const std::tr1::shared_ptr<CFileData>& rFile);
void HandleFileError(CPath::Drive drive, const char* pPath, long error, unsigned long flags);
void SetLayoutInitializeExceptionHandler(const std::tr1::function<void(const CLayout::CInitializeException&)>& rHandler);
void InitializeResetCallbacks();
void RegisterResetCallback(const std::tr1::function<void()>& rCallback, int priority);
const char* GetResourceBasePath();
void InitializeResourceFiles(const ResourceFileConfig& rConfig);
void ShutdownResourceFiles();
void InitializeCalculationWorker();
void ShutdownCalculationWorker();
void InitLocalizedFiles();
void ResetLocalizedFiles();
void InitializeSystemMessages(const void* pArchive, const char* pLayoutName, const char* pFontName, unsigned long size, unsigned long flags);
void SetSystemMessageFonts(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts);
void RegisterSystemMessageCallback(const std::tr1::function<void()>& rCallback, int phase);
void SetDefaultFontMap(const std::tr1::weak_ptr<CLayout::FontMap>& rFonts);

#endif
