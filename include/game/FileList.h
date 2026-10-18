#ifndef GAME_FILELIST_H
#define GAME_FILELIST_H

#include <game/CFileData.h>
#include <list>
#include <memory>

typedef std::list<std::tr1::shared_ptr<CFileData> > FileList;

std::tr1::shared_ptr<FileList> LoadFileList(void** ppEntry, const char* pPath, u32 flags, bool background,
                                            const char* pRegion);
std::tr1::shared_ptr<FileList> LoadResourceFileList(void** ppEntry, const char* pPath, u32 flags, bool background,
                                                    const char* pRegion);

#endif
