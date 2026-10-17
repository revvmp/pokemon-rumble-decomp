#ifndef NW4R_SND_BANK_FILE_H
#define NW4R_SND_BANK_FILE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Util.h>
#include <nw4r/snd/snd_WaveFile.h>

#include <nw4r/ut/ut_binaryFileFormat.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace BankFile {

struct InstParam {
    union {
        s32 waveIndex;
        const WaveInfo* waveInfoAddress;
        WaveDataLocationCallback* waveDataLocationCallback;
    };
    u8 attack;
    u8 decay;
    u8 sustain;
    u8 release;
    u8 hold;
    u8 waveDataLocationType;
    u8 noteOffType;
    u8 alternateAssign;
    u8 originalKey;
    u8 volume;
    u8 pan;
    u8 padding;
    f32 tune;
    Util::DataRef<void> lfoTableRef;
    Util::DataRef<void> graphEnvTableRef;
    Util::DataRef<void> randomizerTableRef;
    u32 reserved;
};

struct RangeTable {
    u8 tableSize;
    u8 key[];
};

struct IndexTable {
    u8 min;
    u8 max;
    u16 reserved;
    u8 ref[];
};

typedef Util::DataRef<void, InstParam, RangeTable, IndexTable> DataRegion;
typedef Util::DataRef<WaveFile::WaveInfo> WaveRegion;

struct Header {
    ut::BinaryFileHeader fileHeader;
    u32 dataBlockOffset;
    u32 dataBlockSize;
    u32 waveBlockOffset;
    u32 waveBlockSize;
};

struct DataBlock {
    ut::BinaryBlockHeader blockHeader;
    Util::Table<DataRegion> instTable;
};

struct WaveBlock {
    ut::BinaryBlockHeader blockHeader;
    Util::Table<WaveRegion> waveInfoTable;
};

}

struct InstInfo {
    enum NoteOffType { NOTE_OFF_TYPE_RELEASE, NOTE_OFF_TYPE_IGNORE };

    struct WaveDataLocation {
        enum WaveDataLocationType {
            WAVE_DATA_LOCATION_INDEX,
            WAVE_DATA_LOCATION_ADDRESS,
            WAVE_DATA_LOCATION_CALLBACK
        };

        WaveDataLocationType type;
        union {
            s32 index;
            const WaveInfo* address;
            WaveDataLocationCallback* callback;
        };
    };

    WaveDataLocation waveDataLocation;
    u8 attack;
    u8 hold;
    u8 decay;
    u8 sustain;
    u8 release;
    NoteOffType noteOffType;
    u8 alternateAssign;
    u8 originalKey;
    u8 pan;
    u8 volume;
    f32 tune;
};

class BankFileReader {
public:
    static const u32 SIGNATURE = FOURCC('R', 'B', 'N', 'K');
    static const int VERSION = NW4R_VERSION(1, 2);

public:
    explicit BankFileReader(const void* pBankBin);

    bool ReadInstInfo(InstInfo* pInfo, int prgNo, int key, int velocity) const;
    bool ReadWaveInfo(WaveInfo* pWaveInfo,
                      const InstInfo::WaveDataLocation& rLocation,
                      const void* pWaveAddr,
                      const WaveInfo** ppWaveInfoAddr) const;

private:
    static bool IsValidFileHeader(const void* pBankBin);

    const BankFile::InstParam* GetInstParam(int prgNo, int key,
                                            int velocity) const;
    const BankFile::DataRegion*
    GetReferenceToSubRegion(const BankFile::DataRegion* pRef,
                            int splitKey) const;

private:
    const BankFile::Header* mHeader;
    const BankFile::DataBlock* mDataBlock;
    const BankFile::WaveBlock* mWaveBlock;
};

}
}
}

#endif
