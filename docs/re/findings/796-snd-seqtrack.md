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
