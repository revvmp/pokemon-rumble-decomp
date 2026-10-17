#include <nw4r/snd/snd_BankFile.h>

#include <nw4r/snd/snd_WaveArchive.h>

#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace snd {
namespace detail {

inline u8 ReadByte(const void* pAddr) {
    return *static_cast<const u8*>(pAddr);
}

bool BankFileReader::IsValidFileHeader(const void* pBankBin) {
    const ut::BinaryFileHeader* pHeader =
        static_cast<const ut::BinaryFileHeader*>(pBankBin);

    if (pHeader->signature != SIGNATURE) {
        return false;
    }

    if (pHeader->version < NW4R_VERSION(1, 0)) {
        return false;
    }

    if (pHeader->version > VERSION) {
        return false;
    }

    return true;
}

BankFileReader::BankFileReader(const void* pBankBin)
    : mHeader(nullptr), mDataBlock(nullptr), mWaveBlock(nullptr) {
    if (!IsValidFileHeader(pBankBin)) {
        return;
    }

    mHeader = static_cast<const BankFile::Header*>(pBankBin);

    if (mHeader->dataBlockOffset != 0) {
        mDataBlock = static_cast<const BankFile::DataBlock*>(
            ut::AddOffsetToPtr(mHeader, mHeader->dataBlockOffset));
    }

    if (mHeader->waveBlockOffset != 0) {
        mWaveBlock = static_cast<const BankFile::WaveBlock*>(
            ut::AddOffsetToPtr(mHeader, mHeader->waveBlockOffset));
    }
}

const BankFile::InstParam* BankFileReader::GetInstParam(int prgNo, int key,
                                                        int velocity) const {
    if (mHeader == nullptr) {
        return nullptr;
    }

    if (prgNo < 0 || prgNo >= static_cast<int>(mDataBlock->instTable.count)) {
        return nullptr;
    }

    const BankFile::DataRegion* pRef = &mDataBlock->instTable.items[prgNo];

    if (pRef->dataType == Util::DATATYPE_INVALID) {
        return nullptr;
    }

    if (pRef->dataType != Util::DATATYPE_T1) {
        pRef = GetReferenceToSubRegion(pRef, key);
        if (pRef == nullptr) {
            return nullptr;
        }
    }

    if (pRef->dataType == Util::DATATYPE_INVALID) {
        return nullptr;
    }

    if (pRef->dataType != Util::DATATYPE_T1) {
        pRef = GetReferenceToSubRegion(pRef, velocity);
        if (pRef == nullptr) {
            return nullptr;
        }
    }

    if (pRef->dataType != Util::DATATYPE_T1) {
        return nullptr;
    }

    return Util::GetDataRefAddress1(*pRef, &mDataBlock->instTable);
}

bool BankFileReader::ReadInstInfo(InstInfo* pInfo, int prgNo, int key,
                                  int velocity) const {
    const BankFile::InstParam* pParam = GetInstParam(prgNo, key, velocity);
    if (pParam == nullptr) {
        return false;
    }

    if (pParam->waveDataLocationType ==
        InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_INDEX) {
        if (pParam->waveIndex < 0) {
            return false;
        }

        pInfo->waveDataLocation.type =
            InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_INDEX;
        pInfo->waveDataLocation.index = pParam->waveIndex;
    } else if (pParam->waveDataLocationType ==
               InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_ADDRESS) {
        if (pParam->waveInfoAddress == nullptr) {
            return false;
        }

        pInfo->waveDataLocation.type =
            InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_ADDRESS;
        pInfo->waveDataLocation.address = pParam->waveInfoAddress;
    } else if (pParam->waveDataLocationType ==
               InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK) {
        if (pParam->waveDataLocationCallback == nullptr) {
            return false;
        }

        pInfo->waveDataLocation.type =
            InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK;
        pInfo->waveDataLocation.callback = pParam->waveDataLocationCallback;
    } else {
        return false;
    }

    pInfo->attack = pParam->attack;
    pInfo->hold = pParam->hold;
    pInfo->decay = pParam->decay;
    pInfo->sustain = pParam->sustain;
    pInfo->release = pParam->release;
    pInfo->originalKey = pParam->originalKey;
    pInfo->pan = pParam->pan;

    if (mHeader->fileHeader.version >= NW4R_VERSION(1, 1)) {
        pInfo->volume = pParam->volume;
        pInfo->tune = pParam->tune;
    } else {
        pInfo->volume = 127;
        pInfo->tune = 1.0f;
    }

    switch (pParam->noteOffType) {
    case 0:
        pInfo->noteOffType = InstInfo::NOTE_OFF_TYPE_RELEASE;
        break;
    case 1:
        pInfo->noteOffType = InstInfo::NOTE_OFF_TYPE_IGNORE;
        break;
    default:
        return false;
    }

    pInfo->alternateAssign = pParam->alternateAssign;
    return true;
}

const BankFile::DataRegion*
BankFileReader::GetReferenceToSubRegion(const BankFile::DataRegion* pRef,
                                        int splitKey) const {
    const BankFile::DataRegion* pSub = nullptr;

    switch (pRef->dataType) {
    case Util::DATATYPE_T0: {
        break;
    }

    case Util::DATATYPE_T1: {
        pSub = pRef;
        break;
    }

    case Util::DATATYPE_T2: {
        const BankFile::RangeTable* pTable =
            Util::GetDataRefAddress2(*pRef, &mDataBlock->instTable);

        if (pTable == nullptr) {
            return nullptr;
        }

        int index = 0;
        while (splitKey > ReadByte(pTable->key + index)) {
            if (++index >= pTable->tableSize) {
                return nullptr;
            }
        }

        u32 offset = index * sizeof(BankFile::DataRegion) +
                     ut::RoundUp(pTable->tableSize + 1, 4);

        pSub = static_cast<const BankFile::DataRegion*>(ut::AddOffsetToPtr(
            reinterpret_cast<const void*>(offset), ut::GetIntPtr(pTable)));
        break;
    }

    case Util::DATATYPE_T3: {
        const BankFile::IndexTable* pTable =
            Util::GetDataRefAddress3(*pRef, &mDataBlock->instTable);

        if (pTable == nullptr) {
            return nullptr;
        }

        if (splitKey < pTable->min || splitKey > pTable->max) {
            return nullptr;
        }

        pSub = reinterpret_cast<const BankFile::DataRegion*>(
            pTable->ref +
            (splitKey - pTable->min) * sizeof(BankFile::DataRegion));
        break;
    }
    }

    return pSub;
}

bool BankFileReader::ReadWaveInfo(WaveInfo* pWaveInfo,
                                  const InstInfo::WaveDataLocation& rLocation,
                                  const void* pWaveAddr,
                                  const WaveInfo** ppWaveInfoAddr) const {
    if (ppWaveInfoAddr != nullptr) {
        *ppWaveInfoAddr = nullptr;
    }

    if (mHeader == nullptr) {
        return false;
    }

    if (rLocation.type ==
        InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_INDEX) {
        int waveIndex = rLocation.index;

        if (mWaveBlock == nullptr) {
            WaveArchiveReader archiveReader(pWaveAddr);

            const WaveFile::FileHeader* pFileHeader =
                archiveReader.GetWaveFile(waveIndex);

            if (pFileHeader == nullptr) {
                return false;
            }

            WaveFileReader waveReader(pFileHeader);
            return waveReader.ReadWaveInfo(pWaveInfo);
        }

        if (waveIndex >= mWaveBlock->waveInfoTable.count) {
            return false;
        }

        const WaveFile::WaveInfo* pFileWaveInfo = Util::GetDataRefAddress0(
            mWaveBlock->waveInfoTable.items[waveIndex],
            &mWaveBlock->waveInfoTable);

        if (pFileWaveInfo == nullptr) {
            return false;
        }

        WaveFileReader waveReader(pFileWaveInfo);
        return waveReader.ReadWaveInfo(pWaveInfo, pWaveAddr);
    }

    if (rLocation.type ==
        InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_ADDRESS) {
        if (rLocation.address == nullptr) {
            return false;
        }

        if (ppWaveInfoAddr != nullptr) {
            *ppWaveInfoAddr = rLocation.address;
        }

        *pWaveInfo = *rLocation.address;
        return true;
    }

    if (rLocation.type ==
        InstInfo::WaveDataLocation::WAVE_DATA_LOCATION_CALLBACK) {
        if (rLocation.callback == nullptr) {
            return false;
        }

        const WaveInfo* pCallbackWaveInfo = rLocation.callback->GetWaveInfo();
        if (pCallbackWaveInfo == nullptr) {
            return false;
        }

        if (ppWaveInfoAddr != nullptr) {
            *ppWaveInfoAddr = pCallbackWaveInfo;
        }

        *pWaveInfo = *pCallbackWaveInfo;
        return true;
    }

    return false;
}

}
}
}
