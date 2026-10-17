# Issue 290: HBM homebutton core, block 4

## F-290-1: HBMRemoteSpk.cpp spans 0x8017AC20 to 0x8017B53C

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The first 13 functions of this block (`0x8017A6F0`–`0x8017AC1C`) end `RVL_SDK/hbm/HBMController.cpp`, matched by #615; see [F-289-1](289-hbm-controller.md#f-289-1-hbmcontrollercpp-spans-0x8017a010-to-0x8017ac20). The remaining 10, together with the five `RemoteSpk` members named in #615 (`0x8017B3A0`–`0x8017B53C`), form `RVL_SDK/hbm/HBMRemoteSpk.cpp`, the `homebutton::RemoteSpk` unit. Its last function, `isPlayReady`, ends at `0x8017B53C`, past #290's listed range, and the next unit starts at `0x8017B540`.

Boundaries:

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x8017AC20`–`0x8017B53C` | 15 functions |
| `.data` | `0x803BA2D0`–`0x803BA300` | `__vt__Q210homebutton9RemoteSpk` (`0xC`), the `"homebutton::RemoteSpk"` RTTI string (`0x803BA2DC`), `__RTTI__Q210homebutton9RemoteSpk` (`0x803BA2F8`) |
| `.bss` | `0x803E9B80`–`0x803E9B84` | `RemoteSpk::spInstance` (dtk guessed 8 bytes; the object is 4) |

The unit has no `.rodata`. `SetInstance`, `GetInstance`, `GetPCMFromSeID` and the file-static `MakeVolumeData` are inlined everywhere and absent from the executable. The linker drops the out-of-line copies of the three members.

Evidence: the vtable and RTTI string give the class. The constructor stores the vtable at `0x1F0` and registers the instance at `0x803E9B80`, which `Controller` already reads. Every function's control flow matches the CC0 references: TP `zeldaret/tp` for `UpdateSpeaker`, and xenoblade `ea25691b` for the rest. Compiler and flags are `homebuttonLib` per [F-287-1](287-hbm-core.md#f-287-1-hbm-compiler-flags-and-first-unit-boundaries).

This revision differs from both references:

- `UpdateSpeaker` drops a stream after more than 60 consecutive send failures, which is TP's later revision.
- `ClearPcm` clears all four channels of `this` instead of going through `GetInstance()`.
- `Start` also recreates every channel alarm and resets each channel, the same way the constructor does.
- `Stop` cancels the channel alarms as well as the speaker alarm.
- `SpeakerOnCallback` handles `WPAD_ERR_TRANSFER` by clearing `playReady`.
- `SpeakerPlayCallback` handles `WPAD_ERR_TRANSFER` and `WPAD_ERR_NO_CONTROLLER` in separate cases with identical bodies.
- `Connect` clears `wencinfo` with an inline word loop instead of calling `memset`. A `u32` loop index is required; an `int` index swaps the zero and base registers.

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with the unit `Matching`, and objdiff reports `.text`, `.data` and `.bss` at 100%:

| Address | Size | Name |
| --- | --- | --- |
| `0x8017AC20` | `0x26C` | `UpdateSpeaker__Q210homebutton9RemoteSpkFP7OSAlarmP9OSContext` |
| `0x8017AE90` | `0x2C` | `ClearPcm__Q210homebutton9RemoteSpkFv` |
| `0x8017AEC0` | `0xBC` | `__ct__Q210homebutton9RemoteSpkFPv` |
| `0x8017AF80` | `0xA4` | `__dt__Q210homebutton9RemoteSpkFv` |
| `0x8017B030` | `0xD4` | `Start__Q210homebutton9RemoteSpkFv` |
| `0x8017B110` | `0x54` | `Stop__Q210homebutton9RemoteSpkFv` |
| `0x8017B170` | `0x30` | `DelaySpeakerOnCallback__Q210homebutton9RemoteSpkFP7OSAlarmP9OSContext` |
| `0x8017B1A0` | `0xD8` | `SpeakerOnCallback__Q210homebutton9RemoteSpkFll` |
| `0x8017B280` | `0x30` | `DelaySpeakerPlayCallback__Q210homebutton9RemoteSpkFP7OSAlarmP9OSContext` |
| `0x8017B2B0` | `0xE4` | `SpeakerPlayCallback__Q210homebutton9RemoteSpkFll` |
| `0x8017B3A0` | `0x8C` | `Connect__Q210homebutton9RemoteSpkFl` |
| `0x8017B430` | `0x98` | `Play__Q210homebutton9RemoteSpkFliSc` |
| `0x8017B4D0` | `0x1C` | `isPlaying__Q210homebutton9RemoteSpkCFl` |
| `0x8017B4F0` | `0x30` | `isPlayingId__Q210homebutton9RemoteSpkCFli` |
| `0x8017B520` | `0x1C` | `isPlayReady__Q210homebutton9RemoteSpkCFl` |

The first 10 rows are this issue's; the last five follow its listed range.

Outside callees renamed for linking: `WPADCanSendStreamData` (`0x802FC730`) and `WPADSendStreamData` (`0x802FC7C0`).

Layout, now proven by every member access: `ChanInfo` is 0x68 bytes. It holds `alarm` at 0x0, `wencinfo` at 0x30 (0x20 bytes), `in_pcm` 0x50, `length` 0x54, `seId` 0x58, `first` 0x5C, `vol` 0x5D, `cannotSendCnt` 0x5E and `playReady` 0x62. `RemoteSpk` holds `info[4]` at 0x0, `speakerAlarm` at 0x1A0, the ARC handle at 0x1D0, `available` at 0x1EC and the vtable pointer at 0x1F0.

## Traps

- Petari `os.h` has no `OSNanosecondsToTicks`; it is now defined beside the other tick macros. The `revolution/wpad.h` include chain also needs `OSThreadQueue` from `revolution/os/OSMutex.h`, which `os.h` does not provide. Without it, `sc.h` fails to compile unless `mem.h` happens to be included first.
- A `switch` whose `cmpwi` chain is not in sorted order keeps the source's case order. Here that order exposed an extra `WPAD_ERR_TRANSFER` case that neither reference has.
