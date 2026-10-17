# F-797-1: NW4R snd Sound3D, SoundActor, and SoundArchive units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #797 owns `.text` `0x802832A0–0x80285400`: 79 functions in six source files. All six are reconstructed and linked.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `snd_Sound3DEngine.cpp` | `.text` | `0x802832A0–0x80283630` | constructor, both `UpdateAmbientParam` overloads, `GetAmbientPriority`, `GetRequiredVoiceOutCount`, weak destructor |
| | `.data` | `0x803C66A0–0x803C66C0` | vtable |
| | `.sdata2` | `0x804BAFA0–0x804BAFB8` | `CalcPanParam` defaults and constants |
| `snd_Sound3DListener.cpp` | `.text` | `0x80283630–0x80283898` | constructor, `SetMatrix`, three setters |
| | `.sdata2` | `0x804BAFB8–0x804BAFC8` | constants |
| `snd_Sound3DManager.cpp` | `.text` | `0x802838A0–0x80283B68` | constructor, nine members, `Sound3DParam` constructor, `__sinit`, three `@4@` thunks |
| | `.ctors` | `0x80352A5C–0x80352A60` | `__sinit` entry |
| | `.data` | `0x803C66C0–0x803C66F8` | vtable |
| | `.sdata2` | `0x804BAFC8–0x804BAFD0` | constants |
| | `.bss` | `0x8044D708–0x8044D720` | default `Sound3DEngine` |
| `snd_SoundActor.cpp` | `.text` | `0x80283B70–0x80283E08` | eight members and the weak `SoundArchivePlayer::detail_ConvertLabelStringToSoundId` |
| | `.data` | `0x803C66F8–0x803C6718` | vtable |
| | `.sdata2` | `0x804BAFD0–0x804BAFD8` | constants |
| `snd_SoundArchive.cpp` | `.text` | `0x80283E10–0x802841F8` | 23 functions |
| | `.data` | `0x803C6718–0x803C6738` | vtable |
| `snd_SoundArchiveFile.cpp` | `.text` | `0x80284200–0x80285400` | 23 `SoundArchiveFileReader` functions |

Every unit is `Matching`: objdiff reports 100% code for all 79 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. objdiff scores Sound3DManager and SoundActor data at 13% and 20% because it pairs weak base-class vtables and thunks with the original symbols; the linked bytes are identical. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Reconstruction

The code follows the zeldaret/ss revision (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`), adjusted to this older library build (`Sep 24 2009`, `0x4302_151`):

- The DOL contains no `SoundArchive::Shutdown`, `GetSoundLabelString`, player, group, or bank label conversion, `detail_OpenGroupStream`, `detail_OpenGroupWaveDataStream`, or `SetExternalFileRoot`; they are unreferenced or absent and are not defined. `OpenExtStreamImpl` is inlined twice into `detail_OpenFileStream`, which therefore has two 256-byte path buffers; the source defines it `inline`. `IsAvailable` returns the pointer comparison directly.
- The DOL contains no `Sound3DListener` biquad-filter setters and no `SoundActor::PauseAllSound` or `GetPlayingSoundCount`. `SoundActor` has `SetPlayableSoundCount(int, int)`, which indexes the actor player without a range check.
- For sound archives older than version 1.1, `impl_GetSoundInfoOffset` adds 28 to the table entry, not the offset of `soundInfoRef` in the current `SoundCommonInfo` (0x18).
- `SoundArchiveFileReader` inlines `IsValidFileHeader`, `GetString`, and all `impl_Get*Info` helpers; their emitted copies are dead-stripped.

New headers: `snd_Sound3DCalculator.h`, `snd_Sound3DEngine.h`, `snd_Sound3DListener.h`, `snd_Sound3DManager.h`, and a first `snd_SoundArchivePlayer.h` with the base classes, the virtual functions in vtable order, `detail_SetupSoundImpl`, and `mSoundArchive` at 0x10. Its remaining members depend on sound, stream-buffer, and instance-manager headers that are still open; issue #798 owns their completion. `include/string.h` gains `strncat`.

Field names without an original symbol follow the NintendoWare CTR successor of these classes and are recorded in [symbols](../SYMBOLS.md).

## Outside symbols

The first copy of the implicit `Sound3DManager` destructor is a weak game-side copy at `0x801E7790`. It is renamed to `__dt__Q34nw4r3snd14Sound3DManagerFv`, so the linker discards the unit's identical copy and the `@4@` destructor thunk branches to it, as in F-424-2.

## Traps

- The linker dead-strips unreferenced strong functions as well as weak ones: the emitted copy of `IsValidFileHeader` disappears from the link. A reference function missing from the DOL therefore does not prove the original source lacked it, so the shared headers keep those declarations; only functions present in the DOL are defined.
- A version-gated file-format constant can differ from the current structure layout; check the immediate in the original instead of deriving it with `offsetof`.
