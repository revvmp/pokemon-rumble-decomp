# F-416-1: NW4R snd AxManager unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #416 owns `.text` `0x80271E10–0x80273694`, the first block of the NW4R sound library. Its first 23 functions form `snd_AxManager.cpp`. The last function, `0x80273650`, is the `AxVoice` constructor and starts `snd_AxVoice.cpp`, which continues through #417.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80271E10–0x80273650` | 23 functions, constructor through the static initializer |
| `.ctors` | `0x80352A50–0x80352A54` | static initializer entry |
| `.bss` | `0x8044CF80–0x8044D390` | `sZeroBuffer` (0x100), `sBiquadFilterCallbackTable` (0x200), the destructor chain entry of the local `instance` (0x10), and `instance` (0x100) |
| `.sbss` | `0x804B9320–0x804B9338` | the five preset biquad filter objects, the `instance` guard, `finishedFlag`, and two bytes of padding |
| `.sdata2` | `0x804BAD80–0x804BAD98` | `0.0f`, `1.0f`, the int-to-float conversion double, `32768.0f`, `32000.0f` |

The NW4R `g3d` code ends at `0x80271E0C`, and the snd link order starts with `snd_AxManager.cpp`; `snd_AnimSound.cpp` and `snd_AnimSoundReader.cpp`, which precede it in later NW4R revisions, are absent. dtk had already split the static initializer at `0x80273610` from its `.ctors` entry. AxManager has no `.data`: the five `BiquadFilter*` vtables at `0x803C61F8–0x803C6248` follow other snd data and belong to `snd_BiquadFilterPreset.cpp`.

The unit is `Matching`: objdiff reports 100% for all 23 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Revision

The 2009 `AxManager` lies between the two CC0 references. Compared with ogws `src/nw4r/snd/snd_AxManager.cpp` (commit `27fa94a593096eed7442efc4e70981fd52025b9c`, the 2006 Wii Sports revision), it:

- has no disk-error member at offset 0x1A, and `Update` has no `DVDGetDriveStatus` check;
- keeps a per-bus `mEffectProcessTick` at 0xF4, measured with `OSGetTick` in `AuxCallbackFunc` and cleared by `ShutdownEffect` and by a change to DPL2 output;
- owns a 128-entry `BiquadFilterCallback` table and five preset filters, installed into slots 1 to 5 by `Init`;
- returns early from `SetOutputMode` when the mode is unchanged, takes the interrupt lock, and calls the new `FxBase::OnChangeOutputMode` virtual (slot 0x18) for every effect;
- clamps only the lower bound in `SetMasterVolume`, and `ClearEffect(bus, 0)` also stops an unfinished fade.

The Skyward Sword decompilation (zeldaret/ss, CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`) has all of these, plus a `mSrcType` member at 0x100 that this target lacks: the constructor's last store is at 0xFC and the `instance` object is 0x100 bytes. The source follows ss for the function bodies, without `mSrcType`. `SetBiquadFilterCallback` has no out-of-line copy in the target; it is inlined into `Init` and the unreferenced copy is removed by the linker.

`Init` needs `#pragma ppc_iro_level 0`. With the default level the interrupt-lock state stays in a register; the target stores it on the stack and tests the lock's address before the destructor call. ss uses the same pragma for this function with a different compiler (Wii/1.6). No other source form tested (the ss `if (!mInitialized)` form, an early return) reproduces it.

## Headers

New headers under `include/nw4r/snd/` come from ogws `include/nw4r/snd` at the commit above, stripped with `tools/vendor/vendor_strip.py`: `snd_Types.h`, `snd_MoveValue.h`, `snd_FxBase.h`, `snd_AxManager.h`, `snd_VoiceManager.h`, `snd_Voice.h`, `snd_AxVoice.h`, and `snd_DisposeCallback.h`, plus `include/nw4r/ut/ut_Lock.h`. Adaptations:

- `snd_AxManager.h` follows the layout above; `AIDMACallback` became the project's `AIDCallback`.
- `FxBase` gains the inline `OnChangeOutputMode` virtual.
- `snd_BiquadFilterCallback.h` and `snd_BiquadFilterPreset.h` are rewritten from ss in the ogws style.
- Umbrella includes became individual headers (`ut_LinkList.h`, `ut_algorithm.h`, `ut_NonCopyable.h`, `revolution/ax.h`, `ai.h`, `wpad.h`, `os.h`, `limits.h`). `ut_Lock.h` also includes `revolution/os/OSMutex.h`.
- `snd_Voice.h`, `snd_AxVoice.h`, and `snd_VoiceManager.h` are the 2006 interfaces. Only `VoiceManager::GetInstance`, `UpdateAllVoicesSync`, and the `Voice::SYNC_AX_VE` (8) and `SYNC_AX_MIX` (0x10) flags are verified by this unit; the owners of those units must reconcile the rest with the target.

Shared-header changes: `include/revolution/ax.h` gains `AX_VOICE_STOP`, `AX_PB_LPF_ON`, `AX_PB_BIQUAD_ON`, `AX_SAMPLE_RATE`, `AX_VOICE_MAX`, `AXSetVoiceState`, `AXSetVoiceAddr`, and `AXSetMaxDspCycles`, with the ogws values and signatures. `include/revolution/sc.h` now includes `revolution/os/OSThread.h`, because it declares an `OSThreadQueue` member and failed to compile when included through `wpad.h` alone.

## Outside symbols

Renamed in `symbols.txt` so the unit links: `VoiceManager::GetInstance` `0x80290420` (its body constructs a bool and two link lists, matching the reference layout), `VoiceManager::UpdateAllVoicesSync` `0x80290B40`, `AXSetMaxDspCycles` `0x802DD780` (stores `__AXMaxDspCycles`), and the vtables `BiquadFilterBpf2048` `0x803C61F8`, `Bpf1024` `0x803C6208`, `Bpf512` `0x803C6218`, `Hpf` `0x803C6228`, and `Lpf` `0x803C6238`. The vtable pairing comes from the static initializer, which stores each vtable into the matching filter object in declaration order. Splits and sizes of these symbols are unchanged.

## Remaining work in #416

`0x80273650` (0x44 bytes) is `AxVoice::AxVoice`: it initializes the embedded `AxVoiceParamBlock` (VPB pointer, sync flags, the previous volume envelope at 0x8 with `0x8000`, the first-update flag, and the volume at 0xE), the wave pointer at 0x10, the two flags at 0x1C and 0x1D, and the callback and list node from 0x38. `snd_AxVoice.cpp` therefore starts in #416 and runs through #417's range.

## Traps

- **A `.sbss` split must end on an aligned address.** Ending AxManager's `.sbss` at its last object (`0x804B9336`) made `dtk dol split` fail with `Invalid alignment for split`. Ending it at `0x804B9338`, including the two padding bytes, splits and links.
- **A null test of a local lock's address means reduced optimization.** When the target stores `OSDisableInterrupts`'s result on the stack and checks `&lock` before restoring interrupts, `#pragma ppc_iro_level 0` around the function reproduces it.
- **`revolution/sc.h` needs `OSThread.h`.** Including `revolution/wpad.h` without first including `revolution/os/OSThread.h` failed with an undefined `OSThreadQueue`; `sc.h` now includes it.
