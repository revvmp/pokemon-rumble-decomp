# F-796-1: NW4R snd SeqTrack unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #796 owns `.text` `0x80280FA0–0x8028329C`: `snd_SeqTrack.cpp`, `snd_Sound3DActor.cpp`, and `snd_Sound3DCalculator.cpp`. This finding covers the first unit.

## Unit and boundaries

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80280FA0–0x802823F8` | 17 functions from `SetPlayerTrackNo` to `NoteOn` |
| `.rodata` | `0x8036E138–0x8036E160` | `NoteOn`'s `NoteOnInfo` initializer template (0x24) and padding |
| `.data` | `0x803C6660–0x803C6670` | vtable |
| `.sdata2` | `0x804BAF20–0x804BAF50` | float constants |

The unit is `Matching`: objdiff reports 100% code for all 17 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. objdiff scores the data at 61.5% because of how it pairs the local jump-table and constant symbols; the linked bytes are identical. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Reconstruction

The code is the zeldaret/ss revision (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`) without changes to code generation; every linked function has the reference size. `UpdateChannelRelease`, `StopAllChannel`, and `AddChannel` are inlined; `SetSilence`, `SetVolume`, `SetPitch`, `SetPan`, `SetSurroundPan`, `SetPanRange`, `SetLpfFreq`, `SetBiquadFilter`, `SetModDepth`, `SetModSpeed`, and `SetTrackVariable` are dead-stripped. The existing `snd_SeqTrack.h` from F-426-2 needed no change. The remote output loop runs over `WPAD_MAX_CONTROLLERS` instead of the reference's literal 4.

## Outside symbols

Renamed: the `SeqTrack` vtable (`0x803C6660`). The function names were already present from the reference symbol import.

## Traps

None beyond F-425-1 and F-794-1.

# F-796-2: NW4R snd Sound3DActor and Sound3DCalculator units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. This finding covers the rest of issue #796.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `snd_Sound3DActor.cpp` | `.text` | `0x80282400–0x802827C8` | constructor, destructor, `SetupSound`, `SetPosition`, `detail_UpdateAmbientArg`, two `@84@` thunks |
| | `.rodata` | `0x8036E160–0x8036E178` | `AmbientInfo` initializer template (0x14) and padding |
| | `.data` | `0x803C6670–0x803C66A0` | vtable |
| | `.sdata2` | `0x804BAF50–0x804BAF58` | constant |
| `snd_Sound3DCalculator.cpp` | `.text` | `0x802827D0–0x8028329C` | seven static functions from `CalcVolumeAndPriority` to `CalcAngleAndDistance` |
| | `.sdata2` | `0x804BAF58–0x804BAFA0` | float and conversion constants |

Both units are `Matching`: objdiff reports 100% code for all 14 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. objdiff scores Sound3DActor data at 60% because it pairs the weak `AmbientArgUpdateCallback` vtable with the original symbols; the linked bytes are identical.

## Reconstruction

The code follows the zeldaret/ss revision with the class layouts of F-797-1:

- The import named `0x80282770` `at_0x0c`; it overrides `AmbientArgUpdateCallback::detail_UpdateAmbientArg(void*, const BasicSound*)`. The imported thunk names were also swapped: `0x802827B0` adjusts `this` by 0x54 and branches to `detail_UpdateAmbientArg`, and `0x802827C0` branches to the destructor.
- The destructor inlines `SoundActor::ForEachSound` over the four actor players with `ClearUpdateCallback`. This library build attaches each sound with `SoundHandle::detail_AttachSound` and has no reverse iteration, so `ExternalSoundPlayer::ForEachSound` and `SoundActor::ForEachSound` take only the function object.
- `CalcPan` loads every argument of the output-mode dispatch before it calls `AxManager::GetInstance`, so the dispatch is an inline helper that receives them as parameters.
- `CalcPanDpl2` averages `cosf` results. Calling the double `cos` adds a 0.5 double constant and makes `.sdata2` eight bytes longer, which moves every later small-data address.

## Outside symbols

None.

## Traps

- A double math call where the original used the float form can leave `.text` the same size but grow `.sdata2`; check section sizes when the DOL header differs.
