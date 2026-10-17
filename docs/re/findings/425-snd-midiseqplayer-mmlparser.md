# F-425-1: NW4R snd MidiSeqPlayer and MmlParser units

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #425 owns `.text` `0x8027CC00–0x8027E054`. Its first functions close `snd_MemorySoundArchive.cpp` ([F-424-2](424-snd-instancepool-lfo.md#f-424-2-nw4r-snd-memorysoundarchive-unit)). This finding adds the next two units. The last two functions, `MmlSeqTrack`'s constructor and `Parse` (`0x8027E000–0x8027E054`), start `snd_MmlSeqTrack.cpp`, which the user approved for issue #426 together with its weak destructor ([F-426-2](426-snd-shared-interfaces.md)).

## Units and boundaries

| Unit | Section | Range | Contents |
| --- | --- | --- | --- |
| `nw4r/snd/snd_MidiSeqPlayer.cpp` | `.text` | `0x8027CD70–0x8027CE18` | weak `SeqPlayer` virtuals and their `@180@`/`@192@` adjustor thunks |
| `nw4r/snd/snd_MmlParser.cpp` | `.text` | `0x8027CE20–0x8027DFFC` | `Parse`, `CommandProc`, `NoteOnCommandProc`, `ReadArg`, `ParseAllocTrack` |
| | `.data` | `0x803C6340–0x803C6570` | `printvar` format string, `CommandProc` jump table, vtable |
| | `.sdata` | `0x804B89D8–0x804B89E8` | `"T"`, `"G"`, `""` variable-class prefixes |
| | `.sbss` | `0x804B9360–0x804B9368` | `mPrintVarEnabledFlag` and padding |
| | `.sdata2` | `0x804BAEE8–0x804BAF08` | float constants |

Both units are `Matching`: objdiff reports 100% code for 11 and 5 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`. objdiff scores `snd_MmlParser.cpp` data at 96.1% because of how it pairs the local string and jump-table symbols; the linked bytes are identical. Compiler and flags are those of F-25-1 (Wii/1.1, `cflags_nw4r`).

## MidiSeqPlayer

zeldaret/ss (CC0, commit `57a709b2128eb163a18e2969d561c9abb7d1fcee`) and ogws (CC0, commit `27fa94a593096eed7442efc4e70981fd52025b9c`) place `snd_MidiSeqPlayer.cpp` between `snd_MemorySoundArchive.cpp` and `snd_MmlParser.cpp`. Its linked contents are the first copies of `SeqPlayer`'s inline virtuals, in the same order: `OnUpdateFrameSoundThread`, `OnShutdownSoundThread`, `InvalidateWaveData`, `SoundThread::PlayerCallback::OnUpdateVoiceSoundThread`, `IsPause`, `IsStarted`, `IsActive`, and the four adjustor thunks for the `DisposeCallback` (`-0xB4`) and `PlayerCallback` (`-0xC0`) bases. `MidiSeqPlayer`'s own code is dead-stripped. The source, like both references, declares only the constructor and the `ChannelCallback` override, enough to emit `MidiSeqPlayer`'s vtable and with it these weak functions. The class's other members are unknown.

## MmlParser

The code is the zeldaret/ss revision without changes to code generation; all five linked function sizes equal those of the reference. `Read16`, `Read24`, `ReadVar`, and `GetVariablePtr` are inlined everywhere and dead-stripped. The variable-range bounds are file-local constants (`PLAYER_VARIABLE_MIN`, `GLOBAL_VARIABLE_MIN`, `TRACK_VARIABLE_MIN`, `TRACK_VARIABLE_MAX`) instead of the reference's macros. `ReadArg` keeps the reference's uninitialized result when a variable reference is invalid.

## Headers

New: `snd_SeqPlayer.h` and `snd_MidiSeqPlayer.h`, following zeldaret/ss. The target confirms the `SeqPlayer` bases and flag offsets: `IsActive`, `IsStarted`, and `IsPause` read bytes 0xCC, 0xCD, and 0xCE, and the thunks adjust by 0xB4 and 0xC0 for the second and third bases. The other members are provisional until `snd_SeqPlayer.cpp` is reconstructed.

## Outside symbols

Renamed: the `MmlParser` vtable (`0x803C6560`) and `mPrintVarEnabledFlag` (`0x804B9360`). The function names were already present from the reference symbol import.

## Traps

- **A unit can contribute only weak functions.** The 11 `SeqPlayer` functions before `MmlParser::Parse` are not `MmlParser`'s; they come from `snd_MidiSeqPlayer.cpp`, whose own code is all dead-stripped. Check the reference split list for a unit whose strong code is absent before assigning leading weak functions to the next unit.
