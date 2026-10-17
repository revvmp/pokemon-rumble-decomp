# F-794-1: NW4R snd RemoteSpeakerManager, SeqFile and SeqPlayer units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #794 owns `.text` `0x8027E700–0x80280758`. Its first seven functions continue `snd_RemoteSpeaker.cpp`, whose first function and split belong to #426; they are not covered here. This finding adds the three units that follow.

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_RemoteSpeakerManager.cpp` | `.text` | `0x8027EE60–0x8027F0F8` | `GetInstance`, `GetRemoteSpeaker`, `Setup`, `Shutdown`, `RemoteSpeakerAlarmProc` |
| | `.sbss` | `0x804B9368–0x804B9370` | `GetInstance` guard and padding |
| | `.bss` | `0x8044D410–0x8044D6E8` | `GetInstance` function-local instance (0x2D8) |
| `nw4r/snd/snd_SeqFile.cpp` | `.text` | `0x8027F100–0x8027F238` | `SeqFileReader` constructor, `GetBaseAddress`, `ReadOffsetByLabel` |
| `nw4r/snd/snd_SeqPlayer.cpp` | `.text` | `0x8027F240–0x80280758` | 25 functions from `InitSeqPlayer` to the weak `ChannelCallback` and the destructor thunks |
| | `.data` | `0x803C65A8–0x803C6610` | vtable and padding |
| | `.sdata2` | `0x804BAF08–0x804BAF20` | float constants |
| | `.bss` | `0x8044D6E8–0x8044D708` | `mGlobalVariable` |

All three units are `Matching`: objdiff reports 100% code for 5, 3, and 25 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. objdiff scores `SeqPlayer`'s data at 20% and `RemoteSpeakerManager`'s at 98.9% because of how it pairs the vtable's thunk symbols and the local-static objects; the linked bytes are identical. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## Reconstruction

All three follow zeldaret/ss (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`). `RemoteSpeakerManager`'s constructor and `SeqFileReader::IsValidFileHeader` are inlined and dead-stripped; so are `SeqPlayer::Shutdown`, `SetTempoRatio`, `GetLocalVariable`, `SetLocalVariable`, `SetGlobalVariable`, `SetTrackMute`, `SetTrackSilence`, `SetTrackVolume`, `CloseTrack`, `SetPlayerTrack`, `FinishPlayer`, and `UpdateChannelParam`. The two unnamed 8-byte setters are `SetChannelPriority` (`0x8027FCC0`, stores `mParserParam.priority` at 0xE9) and `SetReleasePriorityFix` (`0x8027FCD0`, stores 0xCF). The vtable at `0x803C65A8` confirms the `snd_SeqPlayer.h` virtual order and the two `@180@`/`@192@` destructor thunks at `0x80280740` and `0x80280750`.

`SeqFileReader` accepts only version 1.0 (`SUPPORTED_FILE_VERSION`), as in the reference. `ReadOffsetByLabel` reads the label count, label offsets, name lengths, and offsets through `Util::ReadBigEndian` and casts with `reinterpret_cast`, which reproduce the target's register assignment.

## Headers

New: `snd_SeqFile.h`, following zeldaret/ss: the file header's data-block offset at 0x10 and label-block offset at 0x18, label entries with offset, name length, and name, and the data block's base offset at 0x8.

## Outside symbols

Renamed: `RemoteSpeaker`'s constructor (`0x8027E4D0`, in #426's range), called by `GetInstance`; the `GetInstance` guard (`0x804B9368`) and instance (`0x8044D410`); `SeqPlayer::mGlobalVariable` (`0x8044D6E8`) and vtable; the two setters and the two destructor thunks above.

## Traps

- **`static_cast` and `reinterpret_cast` can allocate registers differently.** `ReadOffsetByLabel` matched every instruction except a swap of two saved registers until the label-block and label-info pointers were cast with `reinterpret_cast`, as in the reference, instead of `static_cast`. Wrapping the reads in `Util::ReadBigEndian`, an identity on this target, was also needed.
