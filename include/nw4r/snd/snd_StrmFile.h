#ifndef NW4R_SND_STRM_FILE_H
#define NW4R_SND_STRM_FILE_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_adpcm.h"
#include <nw4r/snd/snd_Types.h>
#include "nw4r/snd/snd_Util.h"

#include "nw4r/ut/ut_binaryFileFormat.h"

namespace nw4r { namespace ut { class FileStream; }}

namespace nw4r { namespace snd { namespace detail
{
	struct StrmFile
	{

		struct Header
		{
			ut::BinaryFileHeader	fileHeader;
			u32						headBlockOffset;
			u32						headBlockSize;
			u32						adpcBlockOffset;
			u32						adpcBlockSize;
			u32						dataBlockOffset;
			u32						dataBlockSize;
		};

		struct StrmDataInfo
		{
			u8	format;
			u8	loopFlag;
			u8	numChannels;
			u8	sampleRate24;
			u16	sampleRate;
			u16	blockHeaderOffset;
			u32	loopStart;
			u32	loopEnd;
			u32	dataOffset;
			u32	numBlocks;
			u32	blockSize;
			u32	blockSamples;
			u32	lastBlockSize;
			u32	lastBlockSamples;
			u32	lastBlockPaddedSize;
			u32	adpcmDataInterval;
			u32	adpcmDataSize;
		};

		struct TrackInfo
		{
			u8	channelCount;
			u8	channelIndexTable[];
		};

		struct TrackInfoEx
		{
			u8		volume;
			u8		pan;
			u8	padding[2];
			u32	reserved;
			u8		channelCount;
			u8		channelIndexTable[];
		};

		struct TrackTable
		{
			u8										trackCount;
			u8										trackDataType;
			u8									padding[2];
			Util::DataRef<TrackInfo, TrackInfoEx>	refTrackHeader[];
		};

		struct AdpcmParamSet
		{
			AdpcmParam		adpcmParam;
			AdpcmLoopParam	adpcmLoopParam;
		};

		struct ChannelInfo
		{
			Util::DataRef<AdpcmParamSet>	refAdpcmInfo;
		};

		struct ChannelTable
		{
			u8							channelCount;
			u8						padding[3];
			Util::DataRef<ChannelInfo>	refChannelHeader[];
		};

		static u32 const SIGNATURE_HEAD_BLOCK =
			0x48454144;

		struct HeadBlock
		{
			ut::BinaryBlockHeader		blockHeader;
			Util::DataRef<StrmDataInfo>	refDataHeader;
			Util::DataRef<TrackTable>	refTrackTable;
			Util::DataRef<ChannelTable>	refChannelTable;
		};

		static u32 const SIGNATURE_FILE =
			0x5253544D;
		static int const FILE_VERSION = 0x0100;
	};
}}}

namespace nw4r { namespace snd { namespace detail
{
	class StrmFileReader
	{
	public:
		struct StrmInfo
		{
			SampleFormat	sampleFormat;
			bool			loopFlag;
			int				numChannels;
			int				sampleRate;
			u16				blockHeaderOffset;
			u32				loopStart;
			u32				loopEnd;
			u32				dataOffset;
			u32				numBlocks;
			u32				blockSize;
			u32				blockSamples;
			u32				lastBlockSize;
			u32				lastBlockSamples;
			u32				lastBlockPaddedSize;
			u32				adpcmDataInterval;
			u32				adpcmDataSize;
		};

		struct StrmTrackInfo
		{
			u8	volume;
			u8	pan;
			int	channelCount;
			u8	channelIndexTable[32];
		};

	public:
		StrmFileReader();

		void Setup(void const *strmData);

		bool IsAvailable() const { return mHeader != nullptr; }
		u32 GetAdpcBlockOffset() const
		{
			if (IsAvailable())
				return mHeader->adpcBlockOffset;

			return 0;
		}

		int GetTrackCount() const;
		int GetChannelCount() const;

		bool ReadStrmInfo(StrmInfo *strmInfo) const;
		bool ReadStrmTrackInfo(StrmTrackInfo *trackInfo, int trackIndex) const;
		bool ReadAdpcmInfo(AdpcmParam *adpcmParam,
		                   AdpcmLoopParam *adpcmLoopParam,
		                   int channelIndex) const;

	private:
		static bool IsValidFileHeader(void const *strmData);

		static SampleFormat GetSampleFormatFromStrmFileFormat(u8 format);

	public:
		static int const SUPPORTED_FILE_VERSION = 0x0100;

	private:
		StrmFile::Header	const *mHeader;
		StrmFile::HeadBlock	const *mHeadBlock;

	private:
		friend class StrmFileLoader;
	};

	class StrmFileLoader
	{
	public:
		StrmFileLoader(ut::FileStream &stream) : mStream(stream) {}

		bool LoadFileHeader(void *buffer, u32 size);

		int GetTrackCount() const;
		int GetChannelCount() const;

		bool ReadStrmInfo(StrmFileReader::StrmInfo *strmInfo) const;
		bool ReadStrmTrackInfo(StrmFileReader::StrmTrackInfo *trackInfo,
		                       int trackIndex) const;
		bool ReadAdpcmInfo(AdpcmParam *adpcmParam,
		                   AdpcmLoopParam *adpcmLoopParam,
		                   int channelIndex) const;
		bool ReadAdpcBlockData(u16 *yn1, u16 *yn2, int blockIndex,
		                       int channelCount);

	private:
		static u32 const HEADER_ALIGNED_SIZE = (sizeof(StrmFile::Header) + 31u) & ~31u;

		ut::FileStream	&mStream;
		StrmFileReader	mReader;
	};
}}}

#endif
