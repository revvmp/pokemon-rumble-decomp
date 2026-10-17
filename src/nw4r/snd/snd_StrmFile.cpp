#include <nw4r/snd/snd_StrmFile.h>
#include <nw4r/snd/snd_WaveFile.h>
#include <nw4r/ut/ut_FileStream.h>
#include <nw4r/ut/ut_algorithm.h>
namespace nw4r { namespace snd { namespace detail {

bool StrmFileReader::IsValidFileHeader(void const *strmData)
{


	ut::BinaryFileHeader const *fileHeader =
		static_cast<ut::BinaryFileHeader const *>(strmData);



	if (fileHeader->signature != StrmFile::SIGNATURE_FILE)
		return false;



	if (fileHeader->version < 0x0100)
		return false;



	if (fileHeader->version > SUPPORTED_FILE_VERSION)
		return false;

	return true;
}

StrmFileReader::StrmFileReader() :
	mHeader		(nullptr),
	mHeadBlock	(nullptr)
{
}

void StrmFileReader::Setup(void const *strmData)
{


	if (!IsValidFileHeader(strmData))
		return;

	mHeader		= static_cast<StrmFile::Header const *>(strmData);
	mHeadBlock	= static_cast<StrmFile::HeadBlock const *>(
		ut::AddOffsetToPtr(mHeader, mHeader->headBlockOffset));



	StrmFile::StrmDataInfo const *info = Util::GetDataRefAddress0(
		mHeadBlock->refDataHeader, &mHeadBlock->refDataHeader);


}

int StrmFileReader::GetTrackCount() const
{


	StrmFile::TrackTable const *trackTable = Util::GetDataRefAddress0(
		mHeadBlock->refTrackTable, &mHeadBlock->refDataHeader);

	return trackTable->trackCount;
}

int StrmFileReader::GetChannelCount() const
{


	StrmFile::ChannelTable const *channelTable = Util::GetDataRefAddress0(
		mHeadBlock->refChannelTable, &mHeadBlock->refDataHeader);

	return channelTable->channelCount;
}

bool StrmFileReader::ReadStrmInfo(StrmInfo *strmInfo) const
{


	StrmFile::StrmDataInfo const *info = Util::GetDataRefAddress0(
		mHeadBlock->refDataHeader, &mHeadBlock->refDataHeader);





	strmInfo->sampleFormat			= GetSampleFormatFromStrmFileFormat(info->format);
	strmInfo->loopFlag				= info->loopFlag;
	strmInfo->numChannels			= info->numChannels;
	strmInfo->sampleRate			= (info->sampleRate24 << 16) + info->sampleRate;
	strmInfo->blockHeaderOffset		= info->blockHeaderOffset;
	strmInfo->loopStart				= info->loopStart;
	strmInfo->loopEnd				= info->loopEnd;
	strmInfo->dataOffset			= info->dataOffset;
	strmInfo->numBlocks				= info->numBlocks;
	strmInfo->blockSize				= info->blockSize;
	strmInfo->blockSamples			= info->blockSamples;
	strmInfo->lastBlockSize			= info->lastBlockSize;
	strmInfo->lastBlockSamples		= info->lastBlockSamples;
	strmInfo->lastBlockPaddedSize	= info->lastBlockPaddedSize;
	strmInfo->adpcmDataInterval		= info->adpcmDataInterval;
	strmInfo->adpcmDataSize			= info->adpcmDataSize;

	return true;
}

bool StrmFileReader::ReadStrmTrackInfo(StrmTrackInfo *trackInfo,
                                       int trackIndex) const
{


	StrmFile::TrackTable const *trackTable = Util::GetDataRefAddress0(
		mHeadBlock->refTrackTable, &mHeadBlock->refDataHeader);
	if (trackIndex >= trackTable->trackCount)
		return false;

	switch (trackTable->trackDataType)
	{
	case 0:
	{
		StrmFile::TrackInfo const *src = Util::GetDataRefAddress0(
			trackTable->refTrackHeader[trackIndex], &mHeadBlock->refDataHeader);
		if (!src)
			return false;

		trackInfo->volume		= 127;
		trackInfo->pan			= 64;
		trackInfo->channelCount	= src->channelCount;

		int count = ut::Min(trackInfo->channelCount, 32);

		for (int i = 0; i < count; i++)
			trackInfo->channelIndexTable[i] = src->channelIndexTable[i];
	}
		break;

	case 1:
	{
		StrmFile::TrackInfoEx const *src = Util::GetDataRefAddress1(
			trackTable->refTrackHeader[trackIndex], &mHeadBlock->refDataHeader);
		if (!src)
			return false;

		trackInfo->volume		= src->volume;
		trackInfo->pan			= src->pan;
		trackInfo->channelCount	= src->channelCount;

		int count = ut::Min(trackInfo->channelCount, 32);

		for (int i = 0; i < count; i++)
			trackInfo->channelIndexTable[i] = src->channelIndexTable[i];
	}
		break;

	default:

		break;
	}

	return true;
}

bool StrmFileReader::ReadAdpcmInfo(AdpcmParam *adpcmParam,
                                   AdpcmLoopParam *adpcmLoopParam,
                                   int channelIndex) const
{




	StrmFile::StrmDataInfo const *info = Util::GetDataRefAddress0(
		mHeadBlock->refDataHeader, &mHeadBlock->refDataHeader);
	if (info->format != 2)
		return false;

	StrmFile::ChannelTable const *channelTable = Util::GetDataRefAddress0(
		mHeadBlock->refChannelTable, &mHeadBlock->refDataHeader);
	if (channelIndex >= channelTable->channelCount)
		return false;

	StrmFile::ChannelInfo const *channelInfo =
		Util::GetDataRefAddress0(channelTable->refChannelHeader[channelIndex],
	                               &mHeadBlock->refDataHeader);

	StrmFile::AdpcmParamSet const *src = Util::GetDataRefAddress0(
		channelInfo->refAdpcmInfo, &mHeadBlock->refDataHeader);

	*adpcmParam		= src->adpcmParam;
	*adpcmLoopParam	= src->adpcmLoopParam;

	return true;
}

SampleFormat StrmFileReader::GetSampleFormatFromStrmFileFormat(u8 format)
{
	switch (format)
	{
	case 2:
		return SAMPLE_FORMAT_DSP_ADPCM;

	case 1:
		return SAMPLE_FORMAT_PCM_S16;

	case 0:
		return SAMPLE_FORMAT_PCM_S8;

	default:

		return SAMPLE_FORMAT_DSP_ADPCM;
	}
}

bool StrmFileLoader::LoadFileHeader(void *buffer, u32 size)
{
	u8 buffer2[32 + HEADER_ALIGNED_SIZE];

	mStream.Seek(0, ut::FileStream::SEEK_ORIGIN_BEG);

	s32 readSize = mStream.Read(ut::RoundUp(buffer2, 32),
	                            HEADER_ALIGNED_SIZE);
	if (readSize != HEADER_ALIGNED_SIZE)
		return false;

	StrmFile::Header *header =
		static_cast<StrmFile::Header *>(ut::RoundUp(buffer2, 32));

	StrmFileReader reader;
	if (!reader.IsValidFileHeader(header))
		return false;

	if (header->adpcBlockOffset > size)
		return false;

	u32 loadSize = header->headBlockOffset + header->headBlockSize;

	mStream.Seek(0, ut::FileStream::SEEK_ORIGIN_BEG);

	readSize = mStream.Read(buffer, loadSize);
	if (readSize != loadSize)
		return false;

	mReader.Setup(buffer);

	return true;
}

int StrmFileLoader::GetTrackCount() const
{
	if (!mReader.IsAvailable())
		return 0;

	return mReader.GetTrackCount();
}

int StrmFileLoader::GetChannelCount() const
{
	if (!mReader.IsAvailable())
		return 0;

	return mReader.GetChannelCount();
}

bool StrmFileLoader::ReadStrmInfo(StrmFileReader::StrmInfo *strmInfo) const
{
	if (!mReader.IsAvailable())
		return false;

	mReader.ReadStrmInfo(strmInfo);
	return true;
}

bool StrmFileLoader::ReadStrmTrackInfo(StrmFileReader::StrmTrackInfo *trackInfo,
                                       int trackIndex) const
{
	if (!mReader.IsAvailable())
		return false;

	mReader.ReadStrmTrackInfo(trackInfo, trackIndex);
	return true;
}

bool StrmFileLoader::ReadAdpcmInfo(AdpcmParam *adpcmParam,
                                   AdpcmLoopParam *adpcmLoopParam,
                                   int channelIndex) const
{
	if (!mReader.IsAvailable())
		return false;

	mReader.ReadAdpcmInfo(adpcmParam, adpcmLoopParam, channelIndex);
	return true;
}

bool StrmFileLoader::ReadAdpcBlockData(u16 *yn1, u16 *yn2, int blockIndex,
                                       int channelCount)
{
	if (!mReader.IsAvailable())
		return false;

	s32 readOffset = mReader.GetAdpcBlockOffset()
	               + blockIndex * channelCount * (sizeof(u16) * 2)
	               + sizeof(ut::BinaryBlockHeader);

	mStream.Seek(readOffset, ut::FileStream::SEEK_ORIGIN_BEG);

	u32 readDataSize = channelCount * (sizeof(u16) * 2);


	__attribute__((aligned(32))) u16 buffer[2 * 8];

	int readSize = mStream.Read(buffer, sizeof buffer);
	if (readSize != 32u)
		return false;

	for (int i = 0; i < channelCount; i++)
	{
		yn1[i] = buffer[i * 2];
		yn2[i] = buffer[i * 2 + 1];
	}

	return true;
}

}}}
