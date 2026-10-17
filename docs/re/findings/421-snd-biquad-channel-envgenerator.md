# F-421-1: NW4R snd BiquadFilterPreset through ExternalSoundPlayer

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #421 owns `.text` `0x80279250–0x8027A414` and issue #422 owns `0x8027A420–0x8027B3F4`. #421's first six functions close `snd_BasicSound.cpp` ([F-420-1](420-snd-basicsound-coverage.md)). The rest of both ranges holds six further units. The last of them, `snd_ExternalSoundPlayer.cpp`, ends with the first three functions of #423's range. This completes #421 and #422.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_BiquadFilterPreset.cpp` | `.text` | `0x80279310–0x80279750` | `GetCoef` of `BiquadFilterLpf`, `Hpf`, `Bpf512`, `Bpf1024`, `Bpf2048`, then their five weak deleting destructors in the same order |
| | `.rodata` | `0x8036C9D8–0x8036DE10` | the five `coefTable` arrays (112, 97, 122, 93, 93 entries of 10 bytes) and padding |
| | `.data` | `0x803C61F8–0x803C6248` | the five vtables, `Bpf2048` first |
| | `.sdata2` | `0x804BAE00–0x804BAE10` | the int-to-float double, `2.0f` |
| `nw4r/snd/snd_Channel.cpp` | `.text` | `0x80279750–0x8027A8E8` | `ChannelManager::GetInstance`, its weak destructor, `GetRequiredMemSize`, `Setup`, `Shutdown`, `UpdateAllChannel`; `Channel::InitParam`, `Update`, `Start`, `Release`, `NoteOff`, `Stop`, `UpdateSweep`, `SetSweepParam`, `SetBiquadFilter`, `VoiceCallbackFunc`, `AllocChannel`, `FreeChannel` |
| | `.bss` | `0x8044D3C8–0x8044D3F8` | destructor chain entry (0x10) and the local `instance` (0x20) |
| | `.sbss` | `0x804B9350–0x804B9358` | `instance` guard and padding |
| | `.sdata2` | `0x804BAE10–0x804BAE38` | `0.0f`, `1.0f`, `255.0f`, `6.0f`, `256.0f`, the unsigned and signed int-to-float doubles |
| `nw4r/snd/snd_DisposeCallbackManager.cpp` | `.text` | `0x8027A8F0–0x8027AC7C` | `GetInstance`, the weak destructor, `RegisterDisposeCallback`, `UnregisterDisposeCallback`, `Dispose`, `DisposeWave` |
| | `.bss` | `0x8044D3F8–0x8044D410` | destructor chain entry and the local `instance` |
| | `.sbss` | `0x804B9358–0x804B9360` | `instance` guard and padding |
| `nw4r/snd/snd_debug.cpp` | `.text` | `0x8027AC80–0x8027AD00` | `Debug_SetWarningFlag` |
| | `.sdata` | `0x804B89D0–0x804B89D8` | `gWarningFlag` (initially `0x1F`) and padding |
| `nw4r/snd/snd_EnvGenerator.cpp` | `.text` | `0x8027AD00–0x8027B0D0` | constructor, `Init`, `Reset`, `GetValue`, `Update`, `SetAttack`, `SetHold`, `SetDecay`, `SetSustain`, `SetRelease` |
| | `.rodata` | `0x8036DE10–0x8036E110` | `DecibelSquareTable` (128 `s16`) and the local `attackTable` (128 `f32`) |
| | `.sdata2` | `0x804BAE38–0x804BAE68` | `VOLUME_INIT` (`-90.4f`) and the constants of `Update` and `CalcRelease` |
| `nw4r/snd/snd_ExternalSoundPlayer.cpp` | `.text` | `0x8027B0D0–0x8027B5D8` | constructor, destructor, `StopAllSound`, `DetachSoundActorAll`, `AppendSound`, `SetPlayableSoundCount`, `RemoveSound`, `detail_CanPlaySound` |

`snd_BiquadFilterCallback.cpp` contributes nothing. The weak destructors of the preset filters are emitted in `snd_BiquadFilterPreset.cpp` because it holds the key functions. The linker removes the functions without an out-of-line copy in the target: the `ChannelManager` and `Channel` constructors, `Channel::~Channel`, `ChannelManager::Alloc` and `Free`, `Channel::GetSweepValue`, the `DisposeCallbackManager` constructor, `EnvGenerator::CalcRelease` and `CalcDecibelSquare`, `ExternalSoundPlayer::PauseAllSound` and `GetLowestPrioritySound`, and the unused weak helper destructors. The next function, `0x8027B5E0`, starts another unit in #423's range.

All six units are `Matching`: objdiff reports 100% code for 10, 18, 6, 1, 10, and 8 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision

All six files match the zeldaret/ss revision (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`), written in the ogws style (CC0, commit `27fa94a593096eed7442efc4e70981fd52025b9c`). Differences from ogws:

- `ChannelManager::GetRequiredMemSize` takes the channel count, `(count + 1) * sizeof(Channel)`. `Channel` is 0xE0 bytes.
- `Channel` adds `mBiquadType` (0x3B), `mBiquadValue` (0x54), four remote output volumes (0x70), `mPanMode` (0xB4), `mPanCurve` (0xB8), `mAlternateAssign` (0xBC), and the wave-data-location callback (0xC8, 0xCC). `Update` forwards the biquad filter and remote volumes to the voice. `Stop` and `VoiceCallbackFunc` release the wave information through the callback's slot 0xC. `NoteOff` is new and only releases when the release-ignore flag is clear.
- `EnvGenerator` adds the hold stage (`mHold` 0x16, `mHoldCounter` 0x18, `SetHold`). As in ogws, `VOLUME_INIT` is a `const volatile f32` member defined in this file, so `Channel` loads it from `EnvGenerator`'s `.sdata2`.
- `ExternalSoundPlayer` adds `DetachSoundActorAll` and `AppendSound`, which holds the sound thread lock and drops lower-priority sounds while the player is full.
- `Debug_SetWarningFlag` is the only function of `snd_debug.cpp` in the target. zeldaret/ss does not contain it; its body follows the zeldaret/ss `GetWarningBitFlag` and `gWarningFlag`. Its name and signature follow the public NW4R API and are confirmed only by the exact match.

## Headers

New: `snd_InstancePool.h`, `snd_ChannelManager.h`, `snd_DisposeCallbackManager.h`, and `snd_SoundThread.h`, from ogws stripped and adapted, and `snd_debug.h`. `snd_Channel.h` gains `NoteOff` and `SetBiquadFilter`; `snd_Voice.h` gains `SetBiquadFilter`. `snd_Channel.cpp` defines the inline placement `operator new` locally, as `snd_AxVoiceManager.cpp` does, because `InstancePool::Alloc` constructs in place.

`SoundThread` follows zeldaret/ss: 0x398 bytes, with `mMutex` at 0x354 (used by `SoundThread::AutoLock` in `Dispose`, `DisposeWave`, and `AppendSound`) and a second flag at 0x395, which `GetInstance` clears with `mCreateFlag`. zeldaret/ss shows that `AxCallbackProc` sends no message while that flag is set; `mPauseFlag` is a descriptive name with low confidence. The other members are provisional until `snd_SoundThread.cpp` is reconstructed.

## Outside symbols

Renamed because the units call them: `PoolImpl::CreateImpl`, `DestroyImpl`, `AllocImpl`, `FreeImpl` (`0x8027C3C0`, `0x8027C510`, `0x8027C600`, `0x8027C660`), `LfoParam::Init` (`0x8027C6B0`), `Lfo::Reset`, `Update`, `GetValue` (`0x8027C6E0`, `0x8027C700`, `0x8027C7A0`), `Util::CalcPitchRatio` (`0x80290BC0`) and `CalcVolumeRatio` (`0x80290D70`), `VoiceManager::AllocVoice` (`0x802906B0`), `SoundThread::GetInstance` (`0x80288CC0`), and twenty `Voice` functions from `0x8028E5F0` to `0x8028ED70` (`Free`, `Setup`, `Start`, `Stop`, `SetVolume`, `SetVeVolume`, `SetPitch`, `SetPanMode`, `SetPanCurve`, `SetPan`, `SetSurroundPan`, `SetLpfFreq`, `SetBiquadFilter`, `SetRemoteFilter`, `SetOutputLine`, `SetMainOutVolume`, `SetMainSend`, `SetFxSend`, `SetRemoteOutVolume`, `SetPriority`). Each is identified by its call site in `Channel::Update`, `Start`, `Release`, `Stop`, `AllocChannel`, or `ChannelManager::Setup` and `Shutdown`, in the zeldaret/ss call order. Their splits and sizes are unchanged.

## Traps

- **A `const volatile` member constant lives in its owner's `.sdata2`.** `Channel::InitParam` and `Start` load `-90.4f` from `0x804BAE38`, which is `EnvGenerator::VOLUME_INIT`, not a `Channel` literal. Ending `snd_Channel.cpp`'s `.sdata2` at `0x804BAE38` and naming the symbol lets both units link.
- **Bash heredocs can fail on long embedded sources.** A heredoc holding several generated C++ files failed with `unexpected EOF while looking for matching '`, and wrote nothing. Write generator scripts and templates to files first.
