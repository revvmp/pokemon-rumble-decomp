#include <game/io/DCIM.h>
#include <game/io/FaAsync.h>
#include <stdio.h>
#include <MSL_C/ctype.h>
#include <string.h>

namespace {

bool ParseDcfDirectoryName(const char* pName, int length, int* pNumber) {
    if (length < 0) length = strlen(pName);
    if (length != 8) return false;
    int number = 0;
    for (int i = 0; i < 3; ++i) {
        int c = pName[i];
        if (!isdigit(c)) return false;
        number = number * 10 + c - '0';
    }
    for (int i = 3; i < 8; ++i) {
        int c = pName[i];
        if (!isalnum(c) && c != '_') return false;
    }
    if (number < 100) return false;
    if (pNumber) *pNumber = number;
    return true;
}

bool ParseDcfFileName(const char* pName, int length, int* pNumber) {
    if (length < 0) length = strlen(pName);
    if (length < 8 || (length > 8 && pName[8] != '.')) return false;
    for (int i = 0; i < 4; ++i) {
        int c = pName[i];
        if (!isalnum(c) && c != '_') return false;
    }
    int number = 0;
    for (int i = 4; i < 8; ++i) {
        int c = pName[i];
        if (!isdigit(c)) return false;
        number = number * 10 + c - '0';
    }
    if (number < 1) return false;
    if (pNumber) *pNumber = number;
    return true;
}

}

DcfIndexResult FindNextDcfIndex(int* pDirectory, int* pFile, bool forceNewDirectory) {
    if (FAChdir("/DCIM") < 0) {
        if (FAErrnum() == 2) {
            *pDirectory = 100;
            *pFile = 1;
            return DcfRootMissing;
        }
        return DcfIoError;
    }
    FADta directories;
    int largestDirectory = 99;
    int largestNintendoDirectory = 99;
    bool duplicateDirectory = false;
    int result = WaitFaFsfirst("*", 0x10, &directories);
    if (result != 0 && FAErrnum() != 2) return DcfIoError;
    while (result == 0) {
        int number;
        if (ParseDcfDirectoryName(reinterpret_cast<const char*>(directories.FileName), -1, &number)) {
            if (largestDirectory < number) largestDirectory = number;
            if (largestNintendoDirectory < number && !(directories.Attribute & 0xf) &&
                strncasecmp(reinterpret_cast<const char*>(&directories.FileName[3]), "NIN01", 5) == 0) {
                largestNintendoDirectory = number;
            }
        }
        result = WaitFaFsnext(&directories);
        if (result != 0 && FAErrnum() != 2) return DcfIoError;
    }
    result = WaitFaFsfirst("*", 0x10, &directories);
    if (result != 0 && FAErrnum() != 2) return DcfIoError;
    while (result == 0) {
        int number;
        if (ParseDcfDirectoryName(reinterpret_cast<const char*>(directories.FileName), -1, &number) &&
            number == largestNintendoDirectory &&
            strncasecmp(reinterpret_cast<const char*>(&directories.FileName[3]), "NIN01", 5) != 0) {
            duplicateDirectory = true;
        }
        result = WaitFaFsnext(&directories);
        if (result != 0 && FAErrnum() != 2) return DcfIoError;
    }
    if (!forceNewDirectory && largestNintendoDirectory >= 100 && !duplicateDirectory) {
        char directory[32];
        snprintf(directory, sizeof(directory), "%03dNIN01", largestNintendoDirectory);
        if (FAChdir(directory) < 0) return DcfIoError;
        FADta files;
        int largestFile = 0;
        result = WaitFaFsfirst("*", 0x7f, &files);
        if (result != 0 && FAErrnum() != 2) return DcfIoError;
        while (result == 0) {
            int number;
            if (!(files.Attribute & 0x10) &&
                ParseDcfFileName(reinterpret_cast<const char*>(files.FileName), -1, &number) && largestFile < number) {
                largestFile = number;
                if (number == 9999) break;
            }
            result = WaitFaFsnext(&files);
            if (result != 0 && FAErrnum() != 2) return DcfIoError;
        }
        if (largestFile < 9999) {
            *pDirectory = largestNintendoDirectory;
            *pFile = largestFile + 1;
            return DcfUseDirectory;
        }
    }
    if (largestDirectory == 999) {
        *pFile = -1;
        *pDirectory = -1;
        return DcfIndexFull;
    }
    *pDirectory = largestDirectory + 1;
    *pFile = 1;
    return DcfCreateDirectory;
}
