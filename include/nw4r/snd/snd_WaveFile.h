#ifndef NW4R_SND_WAVE_FILE_H
#define NW4R_SND_WAVE_FILE_H
#include <nw4r/types_nw4r.h>

#include <nw4r/snd/snd_Types.h>

#include <nw4r/ut/ut_binaryFileFormat.h>

namespace nw4r {
namespace snd {
namespace detail {

namespace WaveFile {

enum Format { FORMAT_PCM8, FORMAT_PCM16, FORMAT_ADPCM };

struct FileHeader {
    ut::BinaryFileHeader fileHeader;
    u32 infoChunkOffset;
    u32 infoChunkSize;
    u32 dataChunkOffset;
    u32 dataChunkSize;
};

struct WaveInfo {
    u8 format;
    u8 loopFlag;
    u8 numChannels;
    u8 sampleRate24;
    u16 sampleRate;
    u8 dataLocationType;
    u8 padding;
    u32 loopStart;
    u32 loopEnd;
    u32 channelInfoTableOffset;
    u32 dataLocation;
    u32 reserved;
};

struct WaveChannelInfo {
    u32 channelDataOffset;
    u32 adpcmOffset;
    u32 volumeFrontLeft;
    u32 volumeFrontRight;
    u32 volumeRearLeft;
    u32 volumeRearRight;
    u32 reserved;
};

struct InfoBlock {
    ut::BinaryBlockHeader blockHeader;
    WaveInfo waveInfo;
};

}

struct ChannelParam {
    void* dataAddr;
    AdpcmInfo adpcmInfo;
};

struct WaveInfo {
    SampleFormat sampleFormat;
    bool loopFlag;
    int numChannels;
    int sampleRate;
    u32 loopStart;
    u32 loopEnd;
    ChannelParam channelParam[CHANNEL_MAX];
};

class WaveFileReader {
public:
    explicit WaveFileReader(const WaveFile::FileHeader* pFileHeader);
    explicit WaveFileReader(const WaveFile::WaveInfo* pWaveInfo);

    bool ReadWaveInfo(WaveInfo* pWaveInfo) const {
        return ReadWaveInfo(pWaveInfo, nullptr);
    }
    bool ReadWaveInfo(WaveInfo* pWaveInfo, const void* pWaveAddr) const;
    const void* GetWaveDataAddress(const WaveFile::WaveChannelInfo* pChannelInfo,
                                 const void* pWaveAddr) const;
    static SampleFormat GetSampleFormatFromWaveFileFormat(u8 format);

private:
    const WaveFile::WaveInfo* mWaveInfo;
};

class WaveDataLocationCallback {
public:
    virtual const WaveInfo* GetWaveInfo();
    virtual void ReleaseWaveInfo(const WaveInfo* pWaveInfo);
};

}
}
}

#endif
