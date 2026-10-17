# Issue 469: RTC, semaphores, system calls, threads, and time

## F-469-1: Complete source units and target differences

Target `wpse-v1`, frozen `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Issue #469 owns 40 functions and 8,600 function bytes. Completing the OSTime source unit also reconstructs four functions in neighboring #470, for 44 functions and 10,272 function bytes in five units. #470 remains separate; its other units are unaffected.

RTC starts with `WriteSramCallback` at `802BAE30`, after OSReset ends at `802BAE2C` in #468. The callback inlines WriteSram; __OSInitSram inlines ReadSram and the GBS mode update. A distinct semaphore unit begins at `802BB8E0`, followed by the system-call vector at `802BBA60`, thread scheduling at `802BBAE0`, and timekeeping at `802BD060`. The survey's coarse reference-file list omitted semaphores.

| Unit | Functions | Function bytes | Sections |
| --- | --- | --- | --- |
| OSRtc.c | 9 | 2,700 | `.text` `802BAE30–802BB8E0`; `.bss` `804528E0–80452934` |
| OSSemaphore.c | 4 | 376 | `.text` `802BB8E0–802BBA60` |
| OSSync.c | 2 | 128 | `.text` `802BBA60–802BBAE0` |
| OSThread.c | 23 | 5,364 | `.text` `802BBAE0–802BD058`; `.bss` `80452938–80453018`; `.sdata` `804B8A98–804B8A9C`; `.sbss` `804B9500–804B950C` |
| OSTime.c | 6 | 1,704 | `.text` `802BD060–802BD730`; `.data` `803C8D10–803C8D70` |

RTC, system-call, thread, and forward timekeeping code follow pinned [Petari](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os/), commit `e0ab141914921726a595573dc48d3f13d0922efe`, CC0-1.0, with no file-level license exceptions. OSCalendarTimeToTicks and its GetYearDays helper follow [ogws OSTime.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/OS/OSTime.c), under that repository's CC0-1.0 license, with member names adapted to the existing calendar header. Semaphores are independently reconstructed from target load/store, interrupt, and thread-queue operations. Source uses ordinary C control flow; the time-base reads and system-call vector retain established SDK inline assembly rather than executable-byte data.

Wii/1.0 with existing `cflags_sdk` and default 16-byte function alignment reproduces all units. The target calls `__OSCancelAlarmsForUserData(thread)` immediately after disabling interrupts in OSCancelThread, before inspecting thread state; this is absent from Petari. GetDates is inline in this target, so its reference noinline annotation is omitted. The default switch callback is the original empty callback, corroborated by SwitchThreadCallback's pointer, not a fabricated implementation of an unknown function. Unused public reference functions are omitted from these source objects.

The semaphore unit disables interrupts for every operation. Initialization sets the count and clears its thread queue. Wait blocks while the observed count is nonpositive, then decrements and returns the old count. TryWait decrements only a positive count and returns the old value. Signal increments, wakes queued threads, and returns the old value. The reconstructed storage view uses a volatile signed 32-bit counter at zero and an eight-byte OSThreadQueue at four, total twelve bytes. The volatile view retains the observed counter reload in Wait; it does not establish an original source-level qualifier. Names follow the standard SDK semaphore interface and proven behavior, with medium confidence in the file name.

The OSMutex header merged in #619 includes OSThread.h instead of duplicating its identical queue/link declarations. This contribution preserves those canonical declarations, extends OSThread.h with semaphore storage and its interface, and extends OSTime.h with the reverse calendar conversion declaration. The full all_source target validates shared declarations across registered units.

## Validation

All five units are Matching and source-linked. Every code/data section scores 100% in focused objdiff-cli 3.8.2 comparisons. A split-only build passes before enabling source linking. The source-linked build then passes the configured SHA-1 check, frozen SHA-256 manifest, and direct DOL byte comparison. Function-byte counts exclude dtk alignment gap symbols.

Commands: `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, focused `objdiff-cli diff` for each unit, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. All exit zero. The final PR repeats these on its exact rebased commit; a fresh ignored build directory is used after shared-header conflict resolution. Original hashes and reference bytes are unchanged.

## Traps

- The imported OSMutex.h duplicated OSThreadQueue and OSMutexLink from OSThread.h, preventing the scheduler from including both. Reusing the identical canonical thread declarations resolves the error without altering field layouts; the full registered source build and linked DOL validate it.
- The reference places GetDates out of line, but this target incorporates it into OSTicksToCalendarTime. Removing only its noinline marker produces the observed 0x32C-byte routine, confirmed by full code and DOL comparison. Do not infer a different compiler from the initial shorter conversion routine.
