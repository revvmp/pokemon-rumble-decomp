#include <nw4r/snd/snd_WaveFile.h>

#include <nw4r/snd/snd_AxVoice.h>
#include <nw4r/ut/ut_algorithm.h>

namespace nw4r {
namespace snd {
namespace detail {

WaveFileReader::WaveFileReader(const WaveFile::FileHeader* pFileHeader)
    : mWaveInfo(nullptr) {
    const WaveFile::InfoBlock* pInfoBlock =
        static_cast<const WaveFile::InfoBlock*>(
            ut::AddOffsetToPtr(pFileHeader, pFileHeader->infoChunkOffset));
    if (!pInfoBlock) {
        return;
    }
    mWaveInfo = &pInfoBlock->waveInfo;
}

WaveFileReader::WaveFileReader(const WaveFile::WaveInfo* pWaveInfo)
    : mWaveInfo(pWaveInfo) {}

SampleFormat WaveFileReader::GetSampleFormatFromWaveFileFormat(u8 format) {
    switch (format) {
    case WaveFile::FORMAT_ADPCM:
        return SAMPLE_FORMAT_DSP_ADPCM;
    case WaveFile::FORMAT_PCM16:
        return SAMPLE_FORMAT_PCM_S16;
    case WaveFile::FORMAT_PCM8:
        return SAMPLE_FORMAT_PCM_S8;
    default:
        return SAMPLE_FORMAT_DSP_ADPCM;
    }
}

bool WaveFileReader::ReadWaveInfo(WaveInfo* pWaveInfo,
                                  const void* pWaveDataOrigin) const {
    SampleFormat sampleFormat =
        GetSampleFormatFromWaveFileFormat(mWaveInfo->format);
    pWaveInfo->sampleFormat = sampleFormat;
    pWaveInfo->numChannels = mWaveInfo->numChannels;
    pWaveInfo->sampleRate =
        (mWaveInfo->sampleRate24 << 16) + mWaveInfo->sampleRate;
    pWaveInfo->loopFlag = mWaveInfo->loopFlag;
    pWaveInfo->loopStart = AxVoice::GetSampleByDspAddress(
        nullptr, mWaveInfo->loopStart, sampleFormat);
    pWaveInfo->loopEnd = AxVoice::GetSampleByDspAddress(
        nullptr, mWaveInfo->loopEnd, sampleFormat) + 1;

    const u32* pChannelInfoOffsets = static_cast<const u32*>(
        ut::AddOffsetToPtr(mWaveInfo, mWaveInfo->channelInfoTableOffset));
    for (int channelIndex = 0; channelIndex < mWaveInfo->numChannels;
         channelIndex++) {
        if (channelIndex >= CHANNEL_MAX) {
            continue;
        }
        ChannelParam& rChannelParam = pWaveInfo->channelParam[channelIndex];
        const WaveFile::WaveChannelInfo* pChannelInfo =
            static_cast<const WaveFile::WaveChannelInfo*>(
                ut::AddOffsetToPtr(mWaveInfo, pChannelInfoOffsets[channelIndex]));
        if (pChannelInfo->adpcmOffset) {
            const AdpcmInfo* pAdpcmInfo = static_cast<const AdpcmInfo*>(
                ut::AddOffsetToPtr(mWaveInfo, pChannelInfo->adpcmOffset));
            rChannelParam.adpcmParam = pAdpcmInfo->param;
            rChannelParam.adpcmLoopParam = pAdpcmInfo->loopParam;
        }
        rChannelParam.dataAddr = const_cast<void*>(
            GetWaveDataAddress(pChannelInfo, pWaveDataOrigin));
    }
    return true;
}

const void* WaveFileReader::GetWaveDataAddress(
    const WaveFile::WaveChannelInfo* pChannelInfo,
    const void* pWaveDataOrigin) const {
    const void* pWaveDataAddress = nullptr;
    bool offsetIsDataBlock = pWaveDataOrigin == nullptr;
    if (!pWaveDataOrigin) {
        pWaveDataOrigin = mWaveInfo;
    }
    switch (mWaveInfo->dataLocationType) {
    case 0:
        pWaveDataAddress =
            ut::AddOffsetToPtr(pWaveDataOrigin, mWaveInfo->dataLocation);
        if (offsetIsDataBlock) {
            pWaveDataAddress = ut::AddOffsetToPtr(pWaveDataAddress, 8);
        }
        break;
    case 1:
        pWaveDataAddress = reinterpret_cast<const void*>(mWaveInfo->dataLocation);
        break;
    default:
        return nullptr;
    }
    pWaveDataAddress =
        ut::AddOffsetToPtr(pWaveDataAddress, pChannelInfo->channelDataOffset);
    return pWaveDataAddress;
}

}
}
}
