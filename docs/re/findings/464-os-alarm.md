# F-464-1: OSAlarm boundaries and reconstruction

Target `wpse-v1`, original `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

## Scope and provenance

Issue #464 owns the complete alarm unit. The OS unit begins in #463 and remains with that owner; OSAlloc begins at `0x802B3810` and continues into #465. This change covers only OSAlarm, leaving those neighboring units untouched.

Reference: Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/os/OSAlarm.c` and the OSAlarm, OSContext, OSException, OSReset, OSTime and types headers under `libs/RVL_SDK/include/revolution/`. Its root LICENSE declares CC0 1.0 Universal. The source is adapted from that reference, with minimal declarations restricted to the alarm unit and a new implementation of the final routine from disassembly. The original assembly exception-entry routine remains explicit; its 80 bytes must not be reported as C reconstruction.

## Established ranges

| Section | Runtime range, exclusive end | DOL file offset | Evidence |
| --- | --- | --- | --- |
| `.text` | `0x802B2F50`–`0x802B3810` | `0x263210` | All 12 functions in reference order, including the newer final routine; allocator list insertion begins at the end |
| `.data` | `0x803C7760`–`0x803C7770` | `0x3C3860` | `__OSInitAlarm` passes this 16-byte shutdown registration record to `OSRegisterShutdownFunction` |
| `.sbss` | `0x804B9488`–`0x804B9490` | No file bytes | The queue head and tail use these two words throughout the unit |

No other data sections are referenced as unit-owned storage. The split-only build and exact frozen-manifest verifier passed before source registration. Commands used decomp-toolkit 1.8.4, binutils 2.42-2, objdiff-cli 3.8.2, and Wii/1.0 MWCC with the F-0007 SDK flags, including default 16-byte function alignment, `-inline auto,level=3`, `-ipa file`, `-sdata 8`, and `-sdata2 8`.

## Names and behavior

Six previously generic symbols were renamed inside the claim: `OSSetPeriodicAlarm` at `0x802B3280`, `OSCancelAlarm` at `0x802B3310`, `OnReset` at `0x802B36B0`, `OSSetAlarmUserData` at `0x802B3740`, `OSGetAlarmUserData` at `0x802B3750`, and `__OSCancelAlarmsForUserData` at `0x802B3760`. The first five names are recovered from the reference source and matching instruction sequences. The last is a descriptive chosen name, not a recovered original spelling.

The final routine disables interrupts, walks the alarm queue while saving the next node before cancellation, cancels nodes whose tag is `0xFFFFFFFF` and whose user data equals the argument, then restores interrupts. Petari does not contain it. It belongs to OSAlarm, rather than OSAlloc: it accesses AlarmQueue, the alarm tag and user-data fields, and OSCancelAlarm. Its generated register allocation matches when the saved-interrupt-state declaration precedes the alarm pointer declarations; the initial ordering exchanged r30 and r31. This observation is specific to this routine and compiler.

The reference's OSSetAlarmTag is absent from this linked unit and is not emitted. The context-saving exception entry is retained as the reference's inline assembly, not substituted with executable byte data.

## Validation and remaining work

The reconstructed unit has 12/12 functions at objdiff 100%, 2,180 function bytes and 24 data bytes; the complete `.text` range is 2,240 bytes including padding. These are object-diff results, not a claim that the source is linked. The final registration remains `NonMatching`, so exact DOL verification uses the original alarm object.

Three external names remain outside this claim: the call at `0x802B32C4` targets `0x802BD100` (`__OSTimeToSystemTime`, owned by #470), and the call at `0x802B36EC` targets `0x802D6660` (`__DVDTestAlarm`, owned by #482). The compiler-generated signed 64-bit division call in InsertAlarm also requires `__div2i` at `0x803405B4`, owned by #536. Their original generic symbols are preserved. The source uses recovered SDK names rather than generic address labels. Coordinate the names with those owners before promoting the unit to Matching; then rebuild and verify the full DOL. A fully source-linked acceptance result is still required even though objdiff already reports 100%.

A temporary source-link attempt failed with precisely these three undefined names (`ninja` exit 1); the final registration was restored to NonMatching.

The PR records clean build and verifier commands and exit codes. No claim is made for completion of #464: its OS and OSAlloc portions remain unreconstructed here.
