#ifndef GAME_CPATH_H
#define GAME_CPATH_H

#include <revolution/types.h>
#include <list>
#include <string>

class CPath {
public:
    CPath();
    CPath(int device, const std::list<std::string>& rComponents);
    ~CPath();
    void Parse(const char* pPath);
    std::string ToString(bool includeDevice) const;
    const std::list<std::string>& GetComponents() const { return mComponents; }

private:
    int mDevice;
    std::list<std::string> mComponents;
};

#endif
