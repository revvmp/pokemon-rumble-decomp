#ifndef NW4R_SND_WSD_FILE_H
#define NW4R_SND_WSD_FILE_H

#include <nw4r/types_nw4r.h>

#include "nw4r/snd/snd_Util.h"

#include "nw4r/ut/ut_binaryFileFormat.h"

namespace nw4r { namespace snd { namespace detail {
	struct WaveSoundInfo
	{
		f32	pitch;
		u8	pan;
		u8	surroundPan;
		u8	fxSendA;
		u8	fxSendB;
		u8	fxSendC;
		u8	mainSend;
	};

	struct WaveSoundNoteInfo
	{
		s32	waveIndex;
		u8	attack;
		u8	hold;
		u8	decay;
		u8	sustain;
		u8	release;
		u8	originalKey;
		u8	pan;
		u8	surroundPan;
		u8	volume;
		f32	pitch;
	};

}}}

namespace nw4r { namespace snd { namespace detail { struct WaveInfo; }}}
namespace nw4r { namespace snd { namespace detail { struct WaveSoundInfo; }}}
namespace nw4r { namespace snd { namespace detail { struct WaveSoundNoteInfo; }}}

namespace nw4r { namespace snd { namespace detail
{
	struct WsdFile
	{

		struct Header
		{
			ut::BinaryFileHeader	fileHeader;
			u32						dataBlockOffset;
			u32						dataBlockSize;
			u32						waveBlockOffset;
			u32						waveBlockSize;
		};

		struct WsdInfo
		{
			f32					pitch;
			u8					pan;
			u8					surroundPan;
			u8					fxSendA;
			u8					fxSendB;
			u8					fxSendC;
			u8					mainSend;
			u8				padding[2];
			Util::DataRef<void>	graphEnvTablevRef;
			Util::DataRef<void>	randomizerTableRef;
			u32				reserved;
		};

		struct TrackInfo;

		typedef Util::Table<Util::DataRef<TrackInfo> > TrackInfoTable;

		struct NoteInfo
		{
			s32					waveIndex;
			u8					attack;
			u8					decay;
			u8					sustain;
			u8					release;
			u8					hold;
			u8				padding[3];
			u8					originalKey;
			u8					volume;
			u8					pan;
			u8					surroundPan;
			f32					pitch;
			Util::DataRef<void>	lfoTableRef;
			Util::DataRef<void>	graphEnvTablevRef;
			Util::DataRef<void>	randomizerTableRef;
			u32				reserved;
		};

		typedef Util::Table<Util::DataRef<NoteInfo> > NoteInfoTable;

		struct Wsd
		{
			Util::DataRef<WsdInfo>			refWsdInfo;
			Util::DataRef<TrackInfoTable>	refTrackTable;
			Util::DataRef<NoteInfoTable>	refNoteTable;
		};

		static u32 const SIGNATURE_DATA_BLOCK =
			FOURCC('D', 'A', 'T', 'A');

		struct DataBlock
		{
			ut::BinaryBlockHeader	blockHeader;
			u32						wsdCount;
			Util::DataRef<Wsd>		refWsd[];
		};

		static u32 const SIGNATURE_WAVE_BLOCK =
			FOURCC('W', 'A', 'V', 'E');

		struct WaveBlock
		{
			ut::BinaryBlockHeader	blockHeader;
			u32						waveCount;
			u32						offsetTable[];
		};

		struct WaveBlockOld
		{
			ut::BinaryBlockHeader	blockHeader;
			u32						offsetTable[];
		};

		static u32 const SIGNATURE_FILE =
			FOURCC('R', 'W', 'S', 'D');
		static int const FILE_VERSION = NW4R_VERSION(1, 3);
	};
}}}

namespace nw4r { namespace snd { namespace detail
{
	class WsdFileReader
	{
	public:
		WsdFileReader(void const *wsdData);

		bool ReadWaveInfo(int waveIndex, WaveInfo *waveData,
		                  void const *waveDataAddress) const;
		bool ReadWaveSoundInfo(WaveSoundInfo *info, int index) const;
		bool ReadWaveSoundNoteInfo(WaveSoundNoteInfo *noteInfo, int index,
		                           int noteIndex) const;

	private:
		bool IsValidFileHeader(void const *wsdData);

	private:
		static int const SUPPORTED_FILE_VERSION = NW4R_VERSION(1, 3);

	private:
		WsdFile::Header		const *mHeader;
		WsdFile::DataBlock	const *mDataBlock;
		WsdFile::WaveBlock	const *mWaveBlock;
	};
}}}

#endif
