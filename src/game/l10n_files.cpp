#include <revolution/types.h>
#include <game/FileList.h>
#include <game/CPath.h>
#include <game/Locale.h>
#include <game/StringFormat.h>
#include <map>
#include <set>
#include <string.h>

class LoadedFileData : public FileList {};

struct FileLoadParam {
    const char* mpPath;
    u32 mFlags;
};

std::tr1::shared_ptr<FileList> LoadFiles(const FileLoadParam* pFiles, unsigned int count, bool background);
std::tr1::shared_ptr<FileList> LoadLocalizedFiles(const FileLoadParam* pFiles, unsigned int count, bool background);
extern const char* const* gSharedLayoutArchives;

namespace {

struct LessConstChar {
    bool operator()(const char* pLeft, const char* pRight) const { return strcmp(pLeft, pRight) < 0; }
};

struct CL10NFileSet {
    CL10NFileSet() : mCount(0) { mpFiles[0] = nullptr; }
    bool Add(std::tr1::shared_ptr<FileList> files, std::tr1::shared_ptr<CFileData> file);
    unsigned int mCount;
    const char* mpFiles[4];
};

typedef std::set<std::string> PathSet;
typedef std::map<const char*, CL10NFileSet, LessConstChar> FileSetMap;
typedef std::map<const char*, const char*, LessConstChar> ArchiveMap;

std::tr1::shared_ptr<PathSet> sPaths;
std::tr1::shared_ptr<FileSetMap> sFileSets;
std::tr1::shared_ptr<ArchiveMap> sArchives;

}

const char* InternPath(const std::string& rPath) {
    return sPaths->insert(rPath).first->c_str();
}

std::string JoinPath(const std::list<std::string>& rComponents) {
    if (rComponents.empty()) {
        return std::string();
    }
    return CPath(0, rComponents).ToString(false).substr(1);
}

const char* SplitArchivePath(const char* pPath, std::string& rEntryPath) {
    CPath path;
    path.Parse(pPath);
    std::list<std::string> remaining(path.GetComponents());
    std::list<std::string> prefix;
    while (!remaining.empty()) {
        const std::string& rComponent = remaining.back();
        if (rComponent.size() > 4 && strcmp(rComponent.c_str() + rComponent.size() - 4, ".arc") == 0) {
            rEntryPath = JoinPath(prefix);
            return InternPath(JoinPath(remaining));
        }
        prefix.push_front(rComponent);
        remaining.pop_back();
    }
    return nullptr;
}

void InitLocalizedFiles() {
    sPaths.reset(new PathSet);
    sFileSets.reset(new FileSetMap);
    sArchives.reset(new ArchiveMap);
}

void ResetLocalizedFiles() {
    sPaths.reset();
    sFileSets.reset();
    sArchives.reset();
}

std::tr1::shared_ptr<FileList> LoadFileList(void** ppEntry, const char* pPath, u32 flags, bool background,
                                         const char* pRegion) {
    pPath = InternPath(pPath);
    FileSetMap::iterator found = sFileSets->find(pPath);
    std::tr1::shared_ptr<FileList> files;
    if (found != sFileSets->end()) {
        FileLoadParam params[3];
        const CL10NFileSet& rFileSet = found->second;
        for (unsigned int i = 0; i < rFileSet.mCount; ++i) {
            params[i].mpPath = rFileSet.mpFiles[i];
            params[i].mFlags = flags;
        }
        files = LoadLocalizedFiles(params, rFileSet.mCount, background);
    } else {
        if (pRegion == nullptr) {
            pRegion = "5:";
        }
        std::string languagePath = FormatString("%s/%s/%s", pRegion, GetGameLocale(), pPath);
        std::string regionPath = FormatString("%s/%s/%s", pRegion, GetRegionName(GetGameRegion()), pPath);
        std::string commonPath = FormatString("%s/%s", pRegion, pPath);
        files.reset(new LoadedFileData);
        CL10NFileSet fileSet;
        fileSet.Add(files, LoadResourceFile(languagePath.c_str(), flags, background));
        fileSet.Add(files, LoadResourceFile(regionPath.c_str(), flags, background));
        if ((flags & 0x2000) != 0 || fileSet.mCount != 0) {
            fileSet.Add(files, LoadResourceFile(commonPath.c_str(), flags, background));
        } else {
            fileSet.Add(files, LoadFile(commonPath.c_str(), flags, background));
        }
        found = sFileSets->insert(FileSetMap::value_type(pPath, fileSet)).first;
        if (pRegion == "5:" && strcmp(pPath, "game2d.arc") == 0) {
            gSharedLayoutArchives = found->second.mpFiles;
        }
    }
    if (ppEntry != nullptr) {
        *ppEntry = found->second.mpFiles;
    }
    if (files && files->empty()) {
        return nullptr;
    }
    return files;
}

std::tr1::shared_ptr<FileList> LoadLocalizedFiles(const FileLoadParam* pFiles, unsigned int count, bool background) {
    return LoadFiles(pFiles, count, background);
}

bool CL10NFileSet::Add(std::tr1::shared_ptr<FileList> files, std::tr1::shared_ptr<CFileData> file) {
    if (!file || !file->IsLoaded()) {
        return false;
    }
    mpFiles[mCount] = InternPath(file->GetPath());
    ++mCount;
    mpFiles[mCount] = nullptr;
    files->push_back(file);
    return true;
}

std::tr1::shared_ptr<CFileData> LoadLocalizedFile(const char* pPath, u32 flags, bool background, const char* pRegion) {
    pPath = InternPath(pPath);
    ArchiveMap::iterator found = sArchives->find(pPath);
    std::tr1::shared_ptr<CFileData> file;
    if (found != sArchives->end()) {
        file = LoadFile(found->second, flags, background);
        return file;
    }
    std::string paths[3];
    std::string entryPath;
    const char* pArchive = SplitArchivePath(pPath, entryPath);
    int count;
    if (pArchive != nullptr) {
        const char** pArchives;
        std::tr1::shared_ptr<FileList> archives = LoadFileList(reinterpret_cast<void**>(&pArchives), pArchive,
                                                             flags, background, pRegion);
        if (!archives) {
            return nullptr;
        }
        if (entryPath.size() == 0) {
            return archives->front();
        }
        count = 0;
        int i = 0;
        while (pArchives[i] != nullptr) {
            paths[count] = pArchives[i] + ("/" + entryPath);
            ++i;
            ++count;
        }
    } else {
        if (pRegion == nullptr) {
            pRegion = "5:";
        }
        paths[0] = FormatString("%s/%s/%s", pRegion, GetGameLocale(), pPath);
        paths[1] = FormatString("%s/%s/%s", pRegion, GetRegionName(GetGameRegion()), pPath);
        paths[2] = FormatString("%s/%s", pRegion, pPath);
        count = 3;
    }
    for (int i = 0; ; ++i) {
        if (i == count - 1) {
            (*sArchives)[pPath] = InternPath(paths[i]);
            file = LoadFile(paths[i].c_str(), flags, background);
            return file;
        }
        file = LoadResourceFile(paths[i].c_str(), flags, background);
        if (file && file->IsLoaded()) {
            (*sArchives)[pPath] = InternPath(paths[i]);
            return file;
        }
    }
}
