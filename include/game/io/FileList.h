#ifndef GAME_FILELIST_H
#define GAME_FILELIST_H

#include <game/io/CFileData.h>
#include <list>
#include <memory>

typedef std::list<std::tr1::shared_ptr<CFileData> > FileList;

class LoadedFileData : public FileList {};

std::tr1::shared_ptr<FileList> LoadFileList(void** ppEntry, const char* pPath, u32 flags, int background,
                                            const char* pRegion);
inline std::tr1::shared_ptr<FileList> LoadResourceFileList(void** ppEntry, const char* pPath, u32 flags,
                                                           int background, const char* pRegion) {
    return LoadFileList(ppEntry, pPath, flags | 0x2000, background, pRegion);
}

#endif
