# Issue 472: EXI tail, SI, and DB translation units

Findings from the RVL_SDK block `0x802C23C0`–`0x802C4218` (#472), established with dtk 1.8.4, CodeWarrior Wii/1.0, objdiff-cli 3.8.2, and GNU objdump 2.42 on `wpse-v1`.

## F-472-1: Unit boundaries

Five C translation units cover the block up to `0x802C4080`. Each matches at 100% in objdiff and the full DOL verifies with all five linked from source.

| Unit | `.text` | `.data` | `.bss` | `.sdata` | `.sbss` |
| --- | --- | --- | --- | --- | --- |
| `RVL_SDK/exi/EXIUart.c` | `0x802C23C0`–`0x802C2700` | | | | `0x804B9598`–`0x804B95A8` |
| `RVL_SDK/exi/EXICommon.c` | `0x802C2700`–`0x802C2890` | | | | |
| `RVL_SDK/si/SIBios.c` | `0x802C2890`–`0x802C3E90` | `0x803D5218`–`0x803D5288` | `0x804537D0`–`0x804539F0` | `0x804B8AD8`–`0x804B8AE0` | `0x804B95A8`–`0x804B95B8` |
| `RVL_SDK/si/SISamplingRate.c` | `0x802C3E90`–`0x802C3F80` | `0x803D5288`–`0x803D5320` | | | `0x804B95B8`–`0x804B95C0` |
| `RVL_SDK/db/db.c` | `0x802C3F80`–`0x802C4080` | `0x803D5320`–`0x803D5338` | | | `0x804B95C0`–`0x804B95C8` |

Evidence:
- `EXIGetID` at `0x802C2010` closes `EXIBios.c`; `ProbeBarnacle` and `__OSEnableBarnacle` are the only `EXIUart.c` functions kept by the linker. `EXICommon.c` holds only `EXIWriteReg`; its `__EXIFreq` constant is not present.
- The SI banner `<< RVL_SDK - SI \trelease build: Feb 27 2009 10:04:44 (0x4302_145) >>` at `0x803D5218` is the first `.data` object of `SIBios.c`, followed by `Si` and `Type`. `__SIVersion` must be defined before `Si` in source for the string to be emitted first.
- Every `.bss` object of `SIBios.c` is addressed relative to `Packet` in `SIInterruptHandler`, `SITransfer`, and `GetTypeCallback`, so the layout is fixed: `Packet` +0x0, `XferTime` +0x80, `TypeTime` +0xA0, `Alarm` +0xC0, `InputBuffer` +0x180, `InputBufferValid` +0x1A0, `InputBufferVcount` +0x1B0, `RDSTHandler` +0x1C0, `GetTypeCallback`'s `cmdFixDevice` +0x1D0, `TypeCallback` +0x1E0.
- `SIGetStatus` and `SIGetResponseRaw` are defined after `__SITransfer` in the original file order, although `SIInterruptHandler` and `SIGetResponse` inline them; `SITransferNext`, `SIClearTCInterrupt`, `SIGetResponseRaw`, and `CallTypeAndStatusCallback` exist only inlined. `SIRegisterPollingHandler` is dead-stripped; `SIUnregisterPollingHandler` and `SIEnablePollingInterrupt` survive.
- `SIRefreshSamplingRate` at `0x802C3F70` is an 8-byte tail call to `SISetSamplingRate`. `XYPAL` at `0x803D52B8` is followed by the `SISetSamplingRate: unknown TV format. Use default.` string at `0x803D52E8`.
- `DBVerbose` and `__DBInterface` close `db.c`; the `DBExceptionDestination\n` string at `0x803D5320` is its only `.data`.

## F-472-2: `fn_802C4080` is `OnShutdown`, the first function of `vi.c`

The last function in the #472 block, `0x802C4080` (size `0x198`), is the static `OnShutdown(BOOL final, u32 event)` of `RVL_SDK/vi/vi.c`. It switches on events 0 to 6, inlines `VISetRGBModeImm` and `VIFlush` (the 64-bit `changed` mask loop with `__shl2i` at `0x803408DC`), and uses its static `first` at `.sdata` `0x804B8AE4`, immediately after the VI banner pointer `__VIVersion` at `0x804B8AE0`. The rest of `vi.c` lies in #473 (`0x802C4220` onward, including `VIInit` with the VI banner at `0x803D5338`).

Splitting `vi.c` requires its full section ranges, almost all inside #473, so this block stops at `0x802C4080`. The symbol is renamed `OnShutdown` in `config/wpse-v1/symbols.txt`; its reconstruction belongs with the `vi.c` unit.

## F-472-3: CodeWarrior names C function statics with C++ mangling

Wii/1.0 compiling C names function-local statics `@LOCAL@<function>__<mangled signature>@<name>`, for example `@LOCAL@SIInterruptHandler__FsP9OSContext@cmdTypeAndStatus` and `@LOCAL@GetTypeCallback__FlUlP9OSContext@cmdFixDevice`. The mangled signature encodes the parameter types (`s`, `l`, `Ul`, `P9OSContext`), so `__OSInterrupt` must be `s16`, `s32` must be `long`, and the `.sbss` object at `0x804B95A8` must carry this exact name in `symbols.txt` for objdiff to pair it. The names `@LOCAL@SIInit__Fv@Initialized` and `@LOCAL@SIGetType__Fl@cmdTypeAndStatus` already in `symbols.txt` follow the same scheme.

## F-472-4: Verification can pass on a stale `main.dol`

With all five units first marked `Matching`, the link failed on undefined `EXIAttach`, `EXIDetach`, `OSCancelAlarm`, `OSGetWirelessID`, `OSSetWirelessID`, and `VIGetCurrentLine`, because those callees still carried `fn_` names. `ninja` exited 1, but `tools/verify/verify.py verify` then printed `PASS main.dol` because the previous `main.dol` was still on disk. Renaming the six callees in `symbols.txt` fixed the link; the identities follow from the matched call sites. Check the `ninja` exit status before running verification.

## F-472-5: `vi.c` unit and 2009 differences

`RVL_SDK/vi/vi.c` covers `.text` `0x802C4080`-`0x802C6A30`, `.data` `0x803D5338`-`0x803D5890`, `.bss` `0x804539F0`-`0x80453B60`, `.sdata` `0x804B8AE0`-`0x804B8B00` and `.sbss` `0x804B95C8`-`0x804B9678`. The split alone verified. With the unit `Matching`, all 25 linked functions and the `.data`, `.bss`, `.sdata` and `.sbss` sections compare at 100% in objdiff-cli 3.8.2, and the full DOL verifies.

Boundaries: `OnShutdown` opens the text, and `__VIResetSIIdle` ends exactly at `i2c.c`'s `0x802C6A30`. The `vi3in1.c` code from `0x802C7350` owns `.data` from `0x803D5890`, `.bss` from `0x80453B60`, `.sdata` from `0x804B8B08` and `.sbss` from `0x804B9680`.

Differences from Petari's 2007 `vi.c` needed for the match, each read from the target:
- Banner `<< RVL_SDK - VI \trelease build: Feb 27 2009 10:04:46 (0x4302_145) >>`.
- `OnShutdown` calls `__VISetGamma1_0` for events 0 and 4-6 before returning `TRUE`. That function at `0x802C8450` passes `gammaSet + 340`, entry 10 (`VI_GM_1_0`) of the 34-byte `VIGammaObj` table, to `__VISetGammaImm`. The name is the one used by the CC0 wii-ipl and pbr decompilations.
- `__VIResetSIIdle` follows `__VIResetRFIdle`. `VIResetDimmingCount` inlines `__VIResetDev0Idle` and returns `TRUE`.
- The `VIConfigure` panic line is 2651.
- `encoderType` is never written, so `VIInit` has no encoder query.

Static layout follows declaration order. Explicitly initialized `.sbss` statics come first in declaration order. Function statics follow in definition order: `OnShutdown`'s `count`, the retrace handler's `DimmingON_Pending` and `DimmingOFF_Pending`, then `PrintDebugPalCaution`'s `message`. Uninitialized statics come last in reverse declaration order. The `.bss` arrays are also emitted in reverse declaration order: declaring `__VIDimmingFlag_DEV_IDLE`, `HorVer`, `regs`, `shdwRegs` places `shdwRegs` first. The wii-ipl declaration order reproduces the `.sbss` layout exactly. CodeWarrior gives function statics after the first an `@n` suffix, for example `@LOCAL@__VIRetraceHandler__FsP9OSContext@old_tvtype@0`. These names must appear exactly so in `symbols.txt` for objdiff to pair them.

`symbols.txt` needed three structural edits inside the unit. The 8-byte `changed` and `shdwChanged` replace separate 4-byte halves. The merged `0xEC` object at `0x803D5524` is split into `taps` and the three render modes. The `0x154` string object at `0x803D56F4` is split into its seven `OSReport` and `OSPanic` strings, so that the string-relative `addi` operands pair.

### Traps

- **A merged string object scores below 100% with identical bytes.** dtk merges adjacent string literals into one object, and objdiff then reports each string-relative `addi` as an argument mismatch. Split the object at each literal in `symbols.txt` before judging the function.
- **dtk refuses overlapping symbols after a resize.** Widening a symbol to 8 bytes fails the split with `Symbol ... overlaps with symbol lbl_...`. Delete the inner label too.
