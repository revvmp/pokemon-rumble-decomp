#ifndef GAME_CPATH_H
#define GAME_CPATH_H

#include <revolution/types.h>
#include <list>
#include <string>

class CPath {
public:
    enum Device {
        DEVICE_DVD,
        DEVICE_NAND,
        DEVICE_MCS_BASE = 512,
        DEVICE_UNC = 539,
        DEVICE_UNSPECIFIED = 540
    };

    CPath();
    CPath(int device, const std::list<std::string>& rComponents);
    ~CPath();
    std::string ToString(bool includeDevice) const;
    std::string GetDeviceName() const;
    void Set(const char* pPath);
    void Parse(const char* pPath);
    const std::list<std::string>& GetComponents() const { return mComponents; }

private:
    const char* ParseDevice(const char* pPath);
    void ParseRelative(const char* pPath, const char* pOriginal);

    int mDevice;
    std::list<std::string> mComponents;
};

#endif
