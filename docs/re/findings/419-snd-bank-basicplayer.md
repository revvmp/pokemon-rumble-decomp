# F-419-1: NW4R snd Bank, BankFile and BasicPlayer units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #419 owns `.text` `0x80276FA0–0x80278374`. Its first two functions, `AxfxImpl::Alloc` and `AxfxImpl::Free`, close `snd_AxfxImpl.cpp` ([F-418-1](418-snd-axvoicemanager-axfximpl.md)). F-419-1 adds the next three units. F-419-2 adds `snd_BasicSound.cpp`, which starts at `0x80277BC0`, inside this issue's range, and continues to `0x8027930C`, inside #420's range. Together they complete #419.

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

`BasicPlayer::BasicPlayer` initialises `mId` with `BasicSound::INVALID_ID`, as in ogws and zeldaret/ss. The first pull request used the literal `0xFFFFFFFF` until F-419-2 added `snd_BasicSound.h`.

## Outside symbols

Renamed in `symbols.txt` because the new units reference them: `Util::GetDataRefAddressImpl` (`0x80291070`), `WaveArchiveReader::WaveArchiveReader` (`0x802910A0`) and `GetWaveFile` (`0x80291120`), both `WaveFileReader` constructors (`0x802911B0`, `0x802911D0`), `WaveFileReader::ReadWaveInfo` (`0x802911E0`), `Channel::AllocChannel` (`0x8027A6E0`) and `Start` (`0x8027A2B0`), and `EnvGenerator::SetAttack`, `SetHold`, `SetDecay`, `SetSustain`, `SetRelease` (`0x8027AF40`, `0x8027AF60`, `0x8027AF80`, `0x8027B020`, `0x8027B030`). The envelope setters are identified by `NoteOn`'s call order and arguments.

## Traps

- **The range-table offset needs `ut::AddOffsetToPtr` with swapped operands.** `GetReferenceToSubRegion` computes `table + index * 8` before adding the rounded table size. Every direct pointer or integer expression tried (seven variants) adds them in the other order or allocates registers differently. Only `ut::AddOffsetToPtr(reinterpret_cast<const void*>(offset), ut::GetIntPtr(pTable))`, as in zeldaret/ss, reproduces the target.

# F-419-2: NW4R snd BasicSound unit

`nw4r/snd/snd_BasicSound.cpp` is the zeldaret/ss revision, written in the ogws style.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80277BC0–0x8027930C` | 36 functions and the static initialiser at `0x80279300` |
| `.ctors` | `0x80352A54–0x80352A58` | static initialiser entry |
| `.data` | `0x803C61C0–0x803C61F8` | `BasicSound` vtable |
| `.sbss` | `0x804B9348–0x804B9350` | `BasicSound::typeInfo` and padding |
| `.sdata2` | `0x804BADF0–0x804BAE00` | `1.0f`, `0.0f`, and the int-to-float double |

The unit is `Matching`: objdiff reports 100% code for all 36 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`.

The vtable confirms the zeldaret/ss virtual order: `GetRuntimeTypeInfo`, the destructor, `Shutdown`, `IsPrepared`, `IsAttachedTempSpecialHandle`, `DetachTempSpecialHandle`, `InitParam`, both `GetBasicPlayer` overloads, `OnUpdatePlayerPriority`, `UpdateMoveValue`, and `UpdateParam`. `typeInfo` is a non-const static, so the compiler initialises it in `__sinit_\snd_BasicSound_cpp`.

The linker removes `SetAutoStopCounter`, `IsPause`, `SetPitch`, `SetPan`, `SetSurroundPan`, `SetLpfFreq`, `SetOutputLineFlag`, `SetFxSend`, and `GetRemainingFadeFrames`; the source keeps them because other units call them.

## Layout

`BasicSound` is 0x110 bytes, as in zeldaret/ss: handles and owners 0x4 to 0x18, `mAmbientInfo` 0x1C, `mAmbientParam` 0x30 (0x28 bytes), `mActorParam` 0x58, `mFadeVolume` 0x64, `mPauseFadeVolume` 0x74, flags 0x84 to 0x8C, `mPriority` 0x98, `mVoiceOutCount` 0x99, `mBiquadFilterType` 0x9A, `mId` 0x9C, `mExtMoveVolume` 0xA0, the per-sound parameters 0xB0 to 0xEC, and four list nodes 0xF0 to 0x108.

`SoundParam` in `snd_Types.h` now has the 0x28-byte shape used for `mAmbientParam`. It adds `biquadFilterValue`, `biquadFilterType`, and a final word. That word is zeroed before the first ambient update and otherwise passed through the ambient callback unchanged; it is named `userData` with low confidence. `SoundAmbientParam` adds the four `VoiceOutParam`. `snd_Voice.h` stores `VoiceOutParam` rather than `SoundParam`.

The new headers `snd_SoundPlayer.h`, `snd_SoundActor.h`, `snd_SoundStartable.h`, `snd_ExternalSoundPlayer.h`, `snd_SoundHandle.h`, `snd_PlayerHeap.h`, and `snd_SoundMemoryAllocatable.h` follow zeldaret/ss. The target confirms these offsets: `SoundPlayer` volume 0x2C, LPF 0x30, output line 0x34, main output volume 0x38, biquad type 0x3C and value 0x40, remote volumes from 0x44 (`GetRemoteOutVolume` at `0x80288060`), main send 0x54, and effect sends 0x58. It also confirms `SoundActor::mActorParam` at 0x48. Their other members are provisional until those units are reconstructed. `snd_MoveValue.h` gains `GetRemainingTime`.

The ambient callback names follow zeldaret/ss: `detail_UpdateAmbientParam`, `detail_GetAmbientPriority`, `detail_GetRequiredVoiceOutCount`, `detail_AllocAmbientArg`, and `detail_FreeAmbientArg`. zeldaret/ss leaves the argument-update callback unnamed; `detail_UpdateAmbientArg` is a descriptive name with low confidence.

## Outside symbols

Renamed because the unit calls them: `SoundHandle::DetachSound` (`0x80287D10`), `SoundPlayer::GetRemoteOutVolume` (`0x80288060`), `detail_SortPriorityList(BasicSound*)` (`0x80288070`), `detail_RemoveSound` (`0x80288500`), `detail_FreePlayerHeap` (`0x80288830`), and `ExternalSoundPlayer::RemoveSound` (`0x8027B4C0`). Each is identified by its call site in `Shutdown`, `Stop`, `SetPlayerPriority`, or `UpdateParam`.

## Traps

- **A tail call leaves an unreachable `blr` that dtk splits off.** `GetAmbientPriority` ends in `bctr` followed by the epilogue's `blr`. dtk inferred a separate 4-byte function `fn_80279204`, and objdiff scored `GetAmbientPriority` at 92%. Setting its size to 0x38 in `symbols.txt` and removing `fn_80279204` gives 100%. The unaligned start address is the sign.
