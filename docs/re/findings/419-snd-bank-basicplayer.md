# F-419-1: NW4R snd Bank, BankFile and BasicPlayer units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #419 owns `.text` `0x80276FA0–0x80278374`. Its first two functions, `AxfxImpl::Alloc` and `AxfxImpl::Free`, close `snd_AxfxImpl.cpp` ([F-418-1](418-snd-axvoicemanager-axfximpl.md)). This finding adds the next three units. `snd_BasicSound.cpp` starts at `0x80277BC0`, inside this issue's range, and continues to `0x802792F8`, inside #420's range. It is not yet reconstructed, so #419 remains partial.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_Bank.cpp` | `.text` | `0x80277040–0x802772B8` | `Bank::Bank`, `Bank::~Bank`, `Bank::NoteOn` |
| | `.sdata2` | `0x804BADC0–0x804BADE0` | `127.0f`, `63.0f`, `0.0f`, and the signed and unsigned int-to-float doubles |
| `nw4r/snd/snd_BankFile.cpp` | `.text` | `0x802772C0–0x80277980` | `BankFileReader::BankFileReader`, `GetInstParam`, `ReadInstInfo`, `GetReferenceToSubRegion`, `ReadWaveInfo` |
| | `.sdata2` | `0x804BADE0–0x804BADE8` | `1.0f` and padding |
| `nw4r/snd/snd_BasicPlayer.cpp` | `.text` | `0x80277980–0x80277BC0` | `PlayerParamSet::Init`, `BasicPlayer::BasicPlayer`, `InitParam`, `SetFxSend`, `GetFxSend`, `SetBiquadFilter`, `SetRemoteFilter`, `SetRemoteOutVolume`, `GetRemoteOutVolume`, the weak deleting destructor |
| | `.data` | `0x803C6198–0x803C61C0` | `BasicPlayer` vtable and padding |
| | `.sdata2` | `0x804BADE8–0x804BADF0` | `1.0f`, `0.0f` |

`BankFileReader::IsValidFileHeader` is inlined into the constructor and removed by the linker. The `.text` of `snd_Bank.cpp` ends at the last instruction of `NoteOn`; the eight bytes of padding before `0x802772C0` stay with the next object.

All three units are `Matching`: objdiff reports 100% code for 3, 5, and 10 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision

The three files match the zeldaret/ss revision (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`), written in the ogws style (CC0, commit `27fa94a593096eed7442efc4e70981fd52025b9c`). Differences from ogws:

- Bank file version 1.2. `InstParam` adds `hold`, `waveDataLocationType`, `noteOffType`, and `alternateAssign`, and the wave reference becomes a union of index, `WaveInfo` address, and `WaveDataLocationCallback`.
- `InstInfo` carries a `WaveDataLocation`, `hold`, `noteOffType`, and `alternateAssign`. `ReadWaveParam` becomes `ReadWaveInfo`. It reads a wave archive through `WaveArchiveReader` when the bank has no wave block. It also returns the source `WaveInfo` address.
- `Bank::NoteOn` calls `Channel::SetHold`, stores the alternate assign and the release-ignore flag, and installs the wave-data-location callback.
- The decoded wave description is `WaveInfo`, 0x80 bytes (ogws `WaveData`): `sampleFormat`, `loopFlag`, `numChannels`, `sampleRate`, `loopStart`, `loopEnd`, and two `ChannelParam` (data address and ADPCM state, 0x34 bytes each).
- `BasicPlayer` holds its parameters in a 0xAC-byte `PlayerParamSet` at 0x4, initialised by `PlayerParamSet::Init`, followed by `mId` at 0xB0. The set adds `biquadValue` (0x14), `biquadType` (0x18), and four `VoiceOutParam` (0x18 bytes each, from 0x4C). It removes the ogws remote send arrays.

## Headers

`snd_Bank.h`, `snd_BankFile.h`, `snd_WaveFile.h`, `snd_WaveArchive.h`, `snd_Channel.h`, `snd_EnvGenerator.h`, `snd_Lfo.h`, `snd_NoteOnCallback.h`, and `snd_BasicPlayer.h` are new. They were vendored from ogws through `tools/vendor/vendor_strip.py` and adapted to the target revision.

`Channel` follows the layout that `NoteOn` writes: `mReleaseIgnoreFlag` 0x3A, `mInitVolume` 0x90, `mInitPan` 0x94, `mInitSurroundPan` 0x98, `mTune` 0x9C, `mKey` 0xA8, `mOriginalKey` 0xAC, `mAlternateAssign` 0xBC, `mWaveDataLocationCallback` 0xC8, `mWaveInfo` 0xCC. The other members follow zeldaret/ss with four remote volumes, which places the list node at 0xD8. Its layout beyond the members `NoteOn` writes is confirmed only when `snd_Channel.cpp` is reconstructed. `EnvGenerator` gains `SetHold`, `mHold`, and `mHoldCounter`.

`snd_Types.h` gains `VoiceOutParam`. `snd_Voice.h` now forward-declares `WaveInfo` for `Voice::Setup`.

`WaveDataLocationCallback` has two virtual functions. `ReadWaveInfo` calls slot 0x8 to obtain the `WaveInfo`; zeldaret/ss frees it through slot 0xC. No original names are known. `GetWaveInfo` and `ReleaseWaveInfo` are descriptive names with low confidence.

`BasicPlayer::BasicPlayer` initialises `mId` to `0xFFFFFFFF`. ogws and zeldaret/ss spell it `BasicSound::INVALID_ID`. Switch to that constant once `snd_BasicSound.h` exists with a layout validated against the target.

## Outside symbols

Renamed in `symbols.txt` because the new units reference them: `Util::GetDataRefAddressImpl` (`0x80291070`), `WaveArchiveReader::WaveArchiveReader` (`0x802910A0`) and `GetWaveFile` (`0x80291120`), both `WaveFileReader` constructors (`0x802911B0`, `0x802911D0`), `WaveFileReader::ReadWaveInfo` (`0x802911E0`), `Channel::AllocChannel` (`0x8027A6E0`) and `Start` (`0x8027A2B0`), and `EnvGenerator::SetAttack`, `SetHold`, `SetDecay`, `SetSustain`, `SetRelease` (`0x8027AF40`, `0x8027AF60`, `0x8027AF80`, `0x8027B020`, `0x8027B030`). The envelope setters are identified by `NoteOn`'s call order and arguments.

## Traps

- **The range-table offset needs `ut::AddOffsetToPtr` with swapped operands.** `GetReferenceToSubRegion` computes `table + index * 8` before adding the rounded table size. Every direct pointer or integer expression tried (seven variants) adds them in the other order or allocates registers differently. Only `ut::AddOffsetToPtr(reinterpret_cast<const void*>(offset), ut::GetIntPtr(pTable))`, as in zeldaret/ss, reproduces the target.
