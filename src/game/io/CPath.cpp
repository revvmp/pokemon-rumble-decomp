#include <game/io/CPath.h>

#include <revolution/nand.h>
#include <revolution/os/OSError.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

CPath::CPath() : mDevice(DEVICE_UNSPECIFIED) {}

CPath::CPath(int device, const std::list<std::string>& rComponents) : mDevice(device), mComponents(rComponents) {}

CPath::~CPath() {}

namespace {

std::string GetDeviceNameFor(int device) {
    if (CPath::DEVICE_MCS_BASE <= device && device < CPath::DEVICE_MCS_BASE + 26) {
        const char letter = static_cast<char>(device - CPath::DEVICE_MCS_BASE + 'A');
        return std::string(&letter, 1);
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
    if (2 <= device && device < CPath::DEVICE_MCS_BASE) {
        char buffer[5];
        snprintf(buffer, sizeof(buffer), "%d", device);
        return std::string(buffer);
    }
    return std::string("");
}

}

std::string CPath::ToString(bool includeDevice) const {
    std::string::size_type length = 0;
    std::string device;
    char separator[2];
    char separatorChar;
    if ((DEVICE_MCS_BASE <= mDevice && mDevice < DEVICE_MCS_BASE + 26) || mDevice == DEVICE_UNC) {
        separatorChar = '\\';
    } else {
        separatorChar = '/';
    }
    separator[0] = separatorChar;
    separator[1] = '\0';
    if (separatorChar == '\\') {
        includeDevice = true;
    }
    if (includeDevice) {
        device.reserve(5);
        device = GetDeviceName();
        device += ":";
        length = device.size();
    }
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
    if ('0' <= pPath[0] && pPath[0] <= '9') {
        if (pPath[1] == ':') {
            mDevice = atoi(pPath);
            return pPath + 2;
        }
        if ('0' <= pPath[1] && pPath[1] <= '9') {
            if (pPath[2] == ':') {
                mDevice = atoi(pPath);
                return pPath + 3;
            }
            if ('0' <= pPath[2] && pPath[2] <= '9' && pPath[3] == ':') {
                mDevice = atoi(pPath);
                if (mDevice <= 511) {
                    return pPath + 4;
                }
            }
        }
    }
    if ('A' <= pPath[0] && pPath[0] <= 'Z' && pPath[1] == ':') {
        mDevice = DEVICE_MCS_BASE + pPath[0] - 'A';
        return pPath + 2;
    }
    if ('a' <= pPath[0] && pPath[0] <= 'z' && pPath[1] == ':') {
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
    const char* pStart = pPath;
    mComponents.clear();
    if (*pStart == '/' || *pStart == '\\') {
        ++pStart;
    } else if (mDevice == DEVICE_DVD) {
        OSPanic("CPath.cpp", 155, "NANDAPP cannot access DVD.\n");
    } else if (mDevice == DEVICE_NAND) {
        char currentDir[64];
        if (NANDGetCurrentDir(currentDir) != 0) {
            OSPanic("CPath.cpp", 168, "NANDGetCurrentDir failed\n");
        }
        Parse(currentDir);
    } else {
        OSPanic("CPath.cpp", 175, "CNT, MCS cannot use relative directory.\n");
    }
    Parse(pStart);
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
        if (component != ".") {
            if (component == "..") {
                if (!mComponents.empty()) {
                    mComponents.pop_back();
                }
            } else if (component.size() != 0) {
                mComponents.push_back(component);
            }
        }
    }
}
