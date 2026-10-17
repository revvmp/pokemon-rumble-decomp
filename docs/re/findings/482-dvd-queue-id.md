# Issue 482: DVD queue and disc identification

## F-482-1: Two DVD translation units

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. Evidence uses dtk 1.8.4 split assembly, Wii/1.0 compiler, objdiff-cli 3.8.2 and full DOL verification. Reference source is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, CC0: `src/RVL_SDK/dvd/dvdqueue.c`, `dvdidutils.c`, and the corresponding declarations in `libs/RVL_SDK/include/revolution/dvd.h`.

`dvdqueue.c` owns `.text` `0x802D66A0`-`0x802D6920` and `.bss` `0x80458F30`-`0x80458F50`. Its six external functions match the reference's exact order and operations. `PopWaitingQueuePrio` is inlined into `__DVDPopWaitingQueue`. The existing local `WaitingQueue` symbol is four pairs of next/previous pointers. No constants or other data sections are required. The section contains 640 bytes including alignment; objdiff counts 604 function bytes.

`dvdidutils.c` owns `.text` `0x802D7340`-`0x802D7430`, the single 240-byte `DVDCompareDiskID` function, and no data. Target calls at `0x802D7378` and `0x802D73B0` compare respectively four game-name bytes and two company bytes through `fn_80345EF8`. The reference identifies that callee as `strncmp`. The target's company condition rejects either empty company string, unlike the game-name wildcard condition; the source preserves this behavior.

Both units compile with the existing `cflags_sdk`, Wii/1.0, and default 16-byte function alignment. objdiff reports all seven functions at 100%. `dvdqueue.c` is source-linked as `Matching`; full frozen-manifest verification passes. `dvdidutils.c` is source-linked after restoring the evidenced external `strncmp` name at `0x80345EF8`, permitted by the updated collaboration procedure. Its 0x40-byte target routine compares unsigned bytes up to the supplied count, returns their difference on mismatch, and stops at NUL, confirming the reference call-site identification. Only the name changes; its size, type, split and fallback implementation remain unchanged.

## Recovered symbols and interfaces

All function names below are high confidence original SDK spellings from reference source and matching instruction behavior. Their original names were `fn_<address>` except for the already named clear function. File offsets equal text addresses minus `0x8004FD40`.

| Address | Size | Function |
| --- | --- | --- |
| `0x802D66A0` | `0x38` | `__DVDClearWaitingQueue` |
| `0x802D66E0` | `0x68` | `__DVDPushWaitingQueue` |
| `0x802D6750` | `0xA0` | `__DVDPopWaitingQueue` |
| `0x802D67F0` | `0x58` | `__DVDCheckWaitingQueue` |
| `0x802D6850` | `0x64` | `__DVDGetNextWaitingQueue` |
| `0x802D68C0` | `0x60` | `__DVDDequeueWaitingQueue` |
| `0x802D7340` | `0xF0` | `DVDCompareDiskID` |

The units use imported `include/revolution/dvd.h` and OS declarations per D-0013; the provisional duplicate header was removed. The shared header import lacked the referenced stdbool.h and OSRoundUp32B definition; the original Petari declarations from the same pinned revision restore those dependencies. The new strncmp declaration uses the existing size_t type in string.h. `DVDCommandBlock` is 0x30 bytes with four-byte alignment: next/prev pointers at 0/4, unsigned command at 8, signed state at 0xC, unsigned offset/length at 0x10/0x14, address pointer at 0x18, transfer sizes at 0x1C/0x20, disk-ID pointer at 0x24, callback at 0x28, and user-data pointer at 0x2C. Only the first two members are accessed by these queue routines; the remainder is reference-derived ABI, not newly independently verified layout.

`DVDDiskID` is 0x20 bytes, aligned to four: char gameName[4] at 0, company[2] at 4, unsigned-byte diskNumber/gameVersion at 6/7, streaming fields at 8/9, 14 reserved bytes at 0xA, and unsigned magic words at 0x18/0x1C. The comparison verifies offsets 0, 4, 6, and 7 with signed char extension for the names and unsigned-byte comparison for the version fields. Other fields follow the reference ABI. BOOL is int; queue priority is signed 32-bit long. Callbacks take signed 32-bit result and a block pointer.

## Initial partial-work scope (superseded by F-482-3)

This is partial work on #482, not completion of its 30 functions. The first three functions are `dvd.c` tail routines: `__DVDTestAlarm` at `0x802D6660`, `__DVDStopMotorAsync` at `0x802D6680`, and `__DVDRestartMotor` at `0x802D6690`. Reference order and the reset-alarm comparison establish these names, but the containing unit begins in earlier blocks (#480/#481); no split or source for that unit is added here. Their three symbol names are recovered within this claim; __DVDTestAlarm also supplies the naming dependency recorded by OSAlarm in #464.

`dvderror.c` spans `0x802D6920`-`0x802D7340`. The target differs materially from the older Petari reference: the callback at `0x802D69B0` checks `NextOffset == 0` and computes a modulo-seven update before writing, behavior absent from that reference callback. Reconstruct from target evidence rather than assuming that source transfers unchanged. Its NAND call symbols and low-level DVD dependencies also require owner coordination. Fatal/device-error routines from `0x802D7430` onward remain unreconstructed. No source coverage is claimed for any of those functions.

## Validation and session handoff

Split-only configure, ninja and frozen-manifest verification exited 0 before source registration. Source compilation reports queue 6/6, disc ID 1/1 functions at 100%; queue BSS 32/32 bytes matches. A clean build after the shared-header migration links both units from source; objdiff marks both complete with 100% functions and data, and the independent full-DOL verifier exits 0. Source, symbols, split ownership and documentation were reviewed without adding comments, docstrings or unit tests. No new general tooling trap emerged.

Coordinate the `dvd.c` tail through its owning issue. Investigate the newer DVD error-ring implementation before importing its reference source.

## Traps

A union rebase reintroduced obsolete OSAlarm rows whose legacy encoding differed from the corrected upstream text. Inspect appended documentation for duplicates even when Git reports no conflict; preserve upstream rows and remove only the redundant additions. Squashing the issue-only delta removed the stale context from subsequent rebases. Worktrees share remote refs: pin the inspected base SHA before history operations, because another worker can advance origin/main between commands.

## F-482-2: Shared header prerequisites for the remaining DVD functions

The target callbacks at `0x802D71C0` and `0x802D7250` store the status register at `DVDErrorInfo + 0x14` and cover register at `+0x10`, respectively. The imported older header places status at `+0x10` and reserves the next four bytes. The corrected `cover`/`status` fields retain the record size of 128 bytes and every following offset. The call to `DVDLowPrepareStatusRegister` at `0x802D7290` tests its return with `cmpwi r3,0`; the imported byte-sized `bool` declaration instead compiles to `clrlwi.`. Returning `BOOL` reproduces the instruction. No existing reconstructed source outside this issue accesses these fields or calls this function.

Device-check reconstruction requires the missing `OSIOSRev` declaration and `__OSGetIOSRev`/`OSGetPhysicalMem2Size` prototypes. These are imported from Petari `libs/RVL_SDK/include/revolution/os.h` at the pinned CC0 revision used in F-482-1. The target loads `major` from byte offset 1 after calling `__OSGetIOSRev`, consistent with the eight-byte reference structure. This prerequisite changes no source coverage; remaining implementation validation will be recorded separately.

## F-482-3: Complete issue source linkage

All 30 functions listed in #482, totaling 4,404 function bytes, now compile at 100% objdiff and link from C source. This continuation adds the remaining 23 functions / 3,560 bytes. The existing queue and disc-ID units remain exact. Configure, ninja and the independent frozen-manifest verifier all exit 0 with the six DVD build objects marked Matching. No instruction bytes, assembly fallbacks or original code blobs were introduced into reconstructed source.

### Original units and partial integration

Reference dvd.c begins with DVDInit at 0x802D1B40 and extends through __DVDRestartMotor ending at 0x802D6694. That original unit crosses neighboring #480/#481 and this issue. With explicit maintainer authorization for the minimal prerequisite integration, src/RVL_SDK/dvd/dvd.c currently implements only this issue's three final functions (0x802D6660-0x802D6694). This split is an implementation fragment, not a claim that 0x802D6660 is an original translation-unit boundary. Earlier functions remain original-object fallbacks. The fragment references original ResetAlarm at 0x80458EA0 externally; its storage remains in the earlier fallback range. Future owners should extend this same source and consolidate its split when reconstructing the preceding functions. No neighboring functions count toward this issue's reconstructed total.

| Build object | Text extent | Functions / bytes | Other sections |
| --- | --- | --- | --- |
| dvd.c fragment | 802D6660-802D6694 | 3 / 44 | None |
| dvderror.c | 802D6920-802D7338 | 14 / 2524 | data 803D6C50-803D6C7C; bss 80458F60-804591C0; sbss 804B97B8-804B97C4 |
| dvdFatal.c | 802D7430-802D7598 | 4 / 340 | rodata 80371670-803716A8; data 803D6C80-803D7448; sdata 804B8BA8-804B8BB0; sbss 804B97C8-804B97CC; sdata2 804BB3E8-804BB3EC |
| dvdDeviceError.c | 802D75A0-802D782C | 2 / 652 | rodata 803716A8-803716C4; data 803D7448-803D75E0; bss 804591C0-804591E0; sdata 804B8BB0-804B8BB4; sbss 804B97D0-804B97D4; sdata2 804BB3F0-804BB3F4 |

All use Wii/1.0, existing cflags_sdk, default function alignment. All functions compare at 100%; all fatal/device data and error-log BSS/SBSS compare at 100%. The error-log compiler emits 43 bytes of string data, while dtk's aligned split contains 44. Its last zero byte is linker padding, not a missing source object: raw object inspection confirms the first 43 bytes are identical and the complete DOL verifies. Objdiff therefore scores that data section 98.85057%, despite exact linked artifact bytes. Source was not padded artificially to manipulate this score.

### Newer error-log behavior

The target adds an update of __ErrorInfo.nextOffset to (NextOffset + 1) % 7 when NextOffset is zero in cbForNandSeek. CodeWarrior inlines that callback into the write-completion failure path, explaining the corresponding larger function. The target adds cbForPrepareCoverRegister before __DVDStoreErrorCode; it stores cover, requests status, then begins NAND directory creation. F-482-2 records the requisite header corrections merged separately in #595. OSGetTime must be declared as a 64-bit function through OSTime.h; implicit C return typing instead generates a wrong 32-bit division.

NandInfo is 0x8C bytes at 80458F60; NandCb is 0xBC at 80458FEC, followed by 0x18 alignment bytes. __ErrorInfo/__FirstErrorInfo occupy 804590C0/80459140 and each are 128 bytes aligned to 32. ExistFlag, NextOffset and Callback are four-byte objects at 804B97B8/BC/C0. Their declarations and observed ordering reproduce the pooled BSS addressing.

### Messages and device validation

The target contains revised English messages and regional French/Spanish variants compared with Petari. Literal strings were recovered from the target pointer tables, represented as readable Japanese UTF-8 source converted by sjiswrap and Latin-1 escapes where appropriate. They are text assets, not copied executable code. Default/Europe tables each have seven entries; error104 has two. Regions 4/5 select error104 in this target regardless of the older reference's version conditional. Device-check error display is a static helper fully inlined into __DVDCheckDevice; the source object contains exactly the two target functions. The device code performs the original unencrypted-read/report-key checks and preserves volatile completion state.

### External symbol evidence

Only names, not sizes/types/splits, changed for external dependencies: NANDCloseAsync at 802E5FE0 follows the reference call from cbForNandWrite; DVDLowGetCoverRegister/StatusRegister at 802D98C0/D0 match corresponding target stores; DVDLowPrepareCoverRegister/StatusRegister at 802D9A50/9BC0 match callback chains; __DVDLowTestAlarm at 802D9ED0 is the reference alarm-test stub; SCGetProductGameRegion at 802E9C90 supplies the region switch; __div2i at 803405B4 is the emitted signed 64-bit runtime division helper for OSTicksToSeconds. ResetAlarm is named without moving or reconstructing its earlier-unit storage. All names reproduce the pinned reference SDK interfaces, while callback and register behavior is independently observed in this target.

### Continuation traps

A data split ending on a non-four-byte boundary is rejected by dtk. Extending only the split without correcting the inferred string symbol can leave a gap symbol crossing its end. Use the aligned split extent, inspect actual section bytes, and distinguish final linker padding from reconstructed content. Do not add dummy source padding to improve a fuzzy score when exact full verification already proves the link.

The imported aggregate OS header is incomplete. Include OSTime.h/OSCache.h for error logging and OSThread.h before SC for its queue type. Including OSAddress.h together with this aggregate OS header redefines its address macros; the device-check absolute declaration uses the evidenced address directly.
