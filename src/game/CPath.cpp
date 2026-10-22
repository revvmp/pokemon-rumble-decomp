#include <game/CPath.h>

#include <revolution/nand.h>
#include <revolution/os/OSError.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace {

bool IsMCS(int d) {
    if (d < 512) return false;
    if (d >= 538) return false;
    return true;
}

std::string GetDeviceNameFor(int device) {
    if (device >= CPath::DEVICE_MCS_BASE && device < CPath::DEVICE_MCS_BASE + 26) {
        return std::string(std::single_char_init(), static_cast<char>(device - CPath::DEVICE_MCS_BASE + 'A'));
    }
    if (device == CPath::DEVICE_UNC) {
        return std::string("\\");
    }
    if (device == CPath::DEVICE_DVD) {
        return std::string("dvd");
    }
    if (device == CPath::DEVICE_NAND) {
        return std::string("nand");
    }
    if (device >= 2 && device < CPath::DEVICE_MCS_BASE) {
        char buffer[5];
        snprintf(buffer, sizeof(buffer), "%d", device);
        return std::string(buffer);
    }
    return std::string("");
}

}

CPath::CPath() : mDevice(DEVICE_UNSPECIFIED) {}

CPath::CPath(int device, const std::list<std::string>& rComponents) : mDevice(device), mComponents(rComponents) {}

CPath::~CPath() {}

std::string CPath::ToString(bool includeDevice) const {
    std::string device;
    char separator[2];
    separator[0] = IsMCS(mDevice) || mDevice == DEVICE_UNC ? '\\' : '/';
    separator[1] = '\0';
    if (separator[0] == '\\') {
        includeDevice = true;
    }
    if (includeDevice) {
        device.reserve(5);
        device = GetDeviceName();
        device += ":";
    }
    std::string::size_type length = device.size();
    for (std::list<std::string>::const_iterator it = mComponents.begin(); it != mComponents.end(); ++it) {
        length += (*it).size() + 1;
    }
    std::string result;
    result.reserve(length);
    if (includeDevice) {
        result += device;
    }
    for (std::list<std::string>::const_iterator it = mComponents.begin(); it != mComponents.end(); ++it) {
        result += separator;
        result += *it;
    }
    return result;
}

std::string CPath::GetDeviceName() const {
    return GetDeviceNameFor(mDevice);
}

const char* CPath::ParseDevice(const char* pPath) {
    if (strncmp(pPath, "nand:", 5) == 0) {
        mDevice = DEVICE_NAND;
        return pPath + 5;
    }
    if (strncmp(pPath, "dvd:", 4) == 0) {
        mDevice = DEVICE_DVD;
        return pPath + 4;
    }
    if ((int)pPath[0] >= '0' && (int)pPath[0] <= '9') {
        if (pPath[1] == ':') {
            mDevice = atoi(pPath);
            return pPath + 2;
        }
        if ((int)pPath[1] >= '0' && (int)pPath[1] <= '9') {
            if (pPath[2] == ':') {
                mDevice = atoi(pPath);
                return pPath + 3;
            }
            if ((int)pPath[2] >= '0' && (int)pPath[2] <= '9' && pPath[3] == ':') {
                mDevice = atoi(pPath);
                if (mDevice <= 511) {
                    return pPath + 4;
                }
            }
        }
    }
    if ((int)pPath[0] >= 'A' && (int)pPath[0] <= 'Z' && pPath[1] == ':') {
        mDevice = DEVICE_MCS_BASE + pPath[0] - 'A';
        return pPath + 2;
    }
    if ((int)pPath[0] >= 'a' && (int)pPath[0] <= 'z' && pPath[1] == ':') {
        mDevice = DEVICE_MCS_BASE + pPath[0] - 'a';
        return pPath + 2;
    }
    if ((pPath[0] == '/' || pPath[0] == '\\') && (pPath[1] == '/' || pPath[1] == '\\')) {
        mDevice = DEVICE_UNC;
        return pPath + 1;
    }
    mDevice = DEVICE_DVD;
    return pPath;
}

void CPath::Set(const char* pPath) {
    ParseRelative(ParseDevice(pPath), pPath);
}

void CPath::ParseRelative(const char* pPath, const char* pOriginal) {
    mComponents.clear();
    if (*pPath == '/' || *pPath == '\\') {
        ++pPath;
    } else if (mDevice == DEVICE_DVD) {
        OSPanic("CPath.cpp", 155, "NAND APP cannot access DVD.\n");
    } else if (mDevice == DEVICE_NAND) {
        char currentDir[64];
        if (NANDGetCurrentDir(currentDir) != 0) {
            OSPanic("CPath.cpp", 168, "NANDGetCurrentDir failed\n");
        }
        Parse(currentDir);
    } else {
        OSPanic("CPath.cpp", 175, "CNT, MCS cannot use relative directory.\n");
    }
    Parse(pPath);
}

void CPath::Parse(const char* pPath) {
    const char* pEnd = pPath + strlen(pPath);
    while (pPath < pEnd) {
        const char* pSeparator = pPath;
        while (*pSeparator != '\0') {
            if (*pSeparator == '/' || *pSeparator == '\\') {
                break;
            }
            ++pSeparator;
        }
        if (*pSeparator == '\0') {
            mComponents.push_back(std::string(pPath));
            return;
        }
        std::string component(pPath, pSeparator);
        pPath = pSeparator + 1;
        while (*pPath == '/' || *pPath == '\\') {
            ++pPath;
        }
        if (component.compare(".") != 0) {
            if (component.compare("..") == 0) {
                if (!mComponents.empty()) {
                    mComponents.pop_back();
                }
            } else if (component.size() != 0) {
                mComponents.push_back(component);
            }
        }
    }
}
