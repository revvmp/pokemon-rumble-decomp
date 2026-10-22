#ifndef GAME_MAIN_RUNTIME_DECLARATIONS_H
#define GAME_MAIN_RUNTIME_DECLARATIONS_H

#include <game/CLayout.h>
#include <game/CPath.h>
#include <game/FileList.h>
#include <functional>
#include <game/ResourceFileConfig.h>

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
void SetLayoutInitializeExceptionHandler(const std::tr1::function<void(const CLayout::CInitializeException&)>& rHandler);
void InitializeResetCallbacks();
void RegisterResetCallback(const std::tr1::function<void()>& rCallback, int priority);
const char* GetResourceBasePath();
void InitializeCalculationWorker();
void ShutdownCalculationWorker();
void InitLocalizedFiles();
void ResetLocalizedFiles();
void InitializeSystemMessages(const void* pArchive, const char* pLayoutName, const char* pFontName, unsigned long size, unsigned long flags);
void SetSystemMessageFonts(const std::tr1::shared_ptr<CLayout::FontMap>& rFonts);
void RegisterSystemMessageCallback(const std::tr1::function<void()>& rCallback, int phase);
void SetDefaultFontMap(const std::tr1::weak_ptr<CLayout::FontMap>& rFonts);

#endif
