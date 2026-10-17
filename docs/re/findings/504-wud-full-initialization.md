# WUD complete initialization source

## F-504-15: All native bodies present; importer matching remains open

The remaining mismatch below was subsequently resolved in [F-504-16](504-wud-importer-match.md). This record preserves the earlier diagnostics and rejected approaches.

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. This pass adds the remaining nine native bodies, totaling 5,124 function bytes. All 86 native extents now total 25,028 bytes with Wii/1.0 and unchanged `cflags_sdk`. This is source coverage, not matching acceptance.

| Added native function | Address | Extent | Current non-relocation result |
| --- | --- | --- | --- |
| `__wudInitDevInfo` | `0x8030C020` | `0x638` | 82 instruction differences remain |
| `__wudNandResultCallback` | `0x8030C660` | `0x80` | Exact |
| `__wudNandFlushCallback` | `0x8030C6E0` | `0x18` | Exact |
| `__wudGetDevInfoFromWiiFit` | `0x8030C700` | `0x530` | Exact |
| `__wudInitHandler` | `0x8030CC30` | `0x114` | Exact |
| `__wudInitHandler0` | `0x8030CD50` | `0x20` | Exact |
| `__wudClearControlBlock` | `0x8030CEA0` | `0x394` | Exact |
| `WUDInit` | `0x8030D240` | `0x11C` | Exact |
| `WUDShutdown` | `0x8030D3B0` | `0x220` | Exact |

The actual `all_source` object has 85 exact non-relocation streams / 23,436 function bytes. Across those 85 bodies, all 1,128 relocation entries have matching positions, types and instruction opcodes/registers. Call/callback names and data/BSS destinations agree. Native small-data constants agree by bytes; source-only catalog entries still require final linker stripping. The importer's 57 destinations agree as an ordered relocation sequence, but some occur at different instruction offsets and its 82 differing words remain a matching blocker. No normalized register comparison is accepted as proof. A separate source-selected candidate link isolates the remaining final-byte mismatch to this importer, as recorded below.

## Initialization and recovery behavior

The dispatcher waits until more than 500 milliseconds have elapsed since system start and settings are available, then clears the control block. It uses the low 32-bit tick difference and the observed bus-clock conversion. A linked Balance Board in Japan region 0 takes the Wii Fit recovery path before device import. The observed private initialization values are recovery 2, import 3, waiting for flush 4, flush complete 5 and ready 6; the shared older enum is not changed.

The NAND result callback clears the lock, advances open/seek/read on their distinct success predicates, closes after a failed seek/read and ends after close. Read success is `0x4B000` bytes. The flush callback clears the lock and advances the phase. Recovery validates a non-null, 32-byte-aligned work buffer, preserving `OSPanic` file `WUD.c` and line 2704 on failure.

Recovery reads the Wii Fit save, copies 128 pairing bytes and its stored checksum, calculates the prefix checksum seed and then the pairing checksum. Both original checksum diagnostics are retained. It rejects an invalid checksum, an empty address and an empty link key for a different application. Valid pairing data populates settings slot 14 using the existing typed pairing record and the proven settings profile view. It preserves address offset 0, key offset 6 and profile offset 22 in the workspace, and key offset 20 in the settings profile. The asynchronous close and settings-flush continuations match the target.

Device import compacts standard entries, preserves the Balance Board settings slot, imports companion devices and flushes the updated arrays. Its functional source is present at the original `0x638` extent. Diagnostic placement was corrected to precede the Balance Board settings-copy operations, but register-allocation differences remain. Current hypotheses about those differences are not acceptance evidence.

Follow-up scratch compilations tested all 120 orders of the five scalar/pointer declarations, preserving their types and behavior. None improved on the then-current 128 differing non-relocation words. Moving the zero-address array declaration, using unsigned remaining-count storage, changing the outer index from `int` to `long`, and giving each import branch its own device-pointer scope also left 128 differences. Widening the compacted-device count increased differences to 173; introducing a control-block pointer for its sync/state writes increased differences or changed the function extent. These are rejected approaches, not source changes. Keep the canonical importer declarations.

A subsequent comparison of 64 declaration/initialization forms across the two inlined allocator helpers identified an improvement: declare the result pointer before the control-block pointer in `__wudGetNewDevInfo`. This preserves functional behavior, produces the same native extents and leaves the other 85 functions and all 1,128 paired relocations exact. It reduces importer differences from 128 to 82 non-relocation words. The real source-selected link reduces final differing bytes from 171 to 116, all within the importer. Repeating the 120 importer declaration orders with this helper form gives no further improvement. The source keeps only the verified helper declaration reorder; compiler flags and all external interfaces are unchanged.

Reordering declarations in `__wudGetDevInfoByIndex` or delaying its control-block pointer assignment leaves the importer at 82 differences; some forms regress search preparation. Replacing that helper's pointer with direct global accesses also regresses three other native functions. Early returns from the standard allocator change native extents and regress four callers. Those scratch forms are rejected; keep the original loop, result variable and common return.

Extending the allocator comparison to 256 combinations, including separate interrupt-result assignment and null-initialized result pointers, gives no improvement below the accepted 82-word importer result without regressions elsewhere. One late control-block initializer reduces importer differences to 70 but introduces seven differences in each of `__wudSyncVirginStandard` and its inlined copy in `__wudSyncHandler`. Reordering or separating the outer virgin-standard pointer declarations does not remove those regressions. This alternative remains rejected.

An instruction-shape diagnostic that ignores register names leaves four differing positions: importer offsets `0x290`, `0x294`, `0x298` and `0x29C`. The target calls `OSDisableInterrupts` before forming the control-block alias, then saves the interrupt result before reading the sync type. Current source schedules the alias before the call and the sync-type load before saving its result. All remaining raw differences involve register operands. This narrows the next investigation to allocator scheduling and register lifetimes; removing register operands is diagnostic only and does not reduce the 82-word acceptance mismatch.

## Compiler cross-check

The complete current source was compiled with the same SDK flags under six installed compiler versions. The table counts exact non-relocation instruction streams only, using all 86 native functions and their target extents; it does not establish final matching for any alternate compiler. No compiler setting is changed.

| Compiler | Exact raw native bodies | Importer extent / differing non-relocation words | Additional differences |
| --- | --- | --- | --- |
| Wii/1.0 | 85 | `0x638` / 82 | None |
| Wii/1.0a | 84 | `0x638` / 82 | `__wudInitSub`, five words |
| Wii/1.0RC1 | 84 | `0x638` / 82 | `__wudInitSub`, five words |
| Wii/1.1 | 82 | `0x638` / 82 | Virgin-standard sync, database cleanup and Wii Fit recovery |
| Wii/1.3 | 82 | `0x638` / 82 | Same three additional functions as Wii/1.1 |
| GC/3.0a3 | 55 | `0x630` / 281 | Thirty other native functions |

The measured Wii/1.0 selection remains supported. Later Wii versions leave the importer mismatch unchanged while regressing other bodies; switching compilers is not a remedy for this source.

`WUDInit` retains the BTA trace setup, optional linked-board initialization hook, alarm setup and dirty-flag clearing. It starts at state 1, without the newer reference's additional device-up wait. Shutdown saves standard devices and optionally companion devices, then starts the existing timed shutdown driver. The clear-control-block routine uses the original typed handle setters, producing the observed unsigned-byte indexing and native unrolling.

## Complete data and storage evidence

Ten SDK messages survive from APIs or diagnostics absent from the native function inventory. Two ordinary constant-pointer catalogs preserve those exact strings: `gWudUnlinkedCallbackMessages` and `gWudUnlinkedDiagnosticMessages`. They are source-only objects and are not added to target metadata. Both are absent from the source-selected candidate ELF. No fake function bodies or calls are introduced.

The actual original `WUDStartClearDevice` API body is also reconstructed with normal external linkage. The compiler inlines it into vendor-event handling and emits a standalone source copy, matching the reference source structure. This copy is not one of the target's 86 retained functions and is absent from the source-selected candidate ELF. It places the original clear-device message at its evidenced pool position.

All 4,519 emitted `.data` bytes agree with the frozen DOL after resolving the nine compiler-generated jump-table entries to their observed native destinations. The target's final extra byte is zero alignment padding. All 22 emitted `.sdata` bytes also agree; its target range has two final zero padding bytes. The complete BSS and small-BSS extents and native operand positions agree. Prelink `.sdata2` contains the native seven constant bytes and additional catalog pointers. The candidate link strips the catalogs and reproduces the final constant bytes and alignment exactly.

The following small-data names come from the reference and paired target operands: `__bte_trace_level` at `0x804B9B35`, `_wudNandBufPtr` at `0x804B9B38`, weak `_initWBC` at `0x804B9B3C`, and `_wudNandPhase` at `0x804B9B48`. Canonical scalar declarations are kept in reference order as tentative zero-initialized definitions. Their compiled positions now reproduce the target.

Outside functions `BTM_RegisterForDeviceStatusNotif` at `0x8031D6C4` and `BTM_RegisterForVSEvents` at `0x8031D844` receive names from paired calls in `__wudInitSub`. Their splits, extents and types do not change.

## Source-selected candidate comparison

A scratch copy of the real link response selects the compiled WUD object in place of its original object, without changing the `NonMatching` setting or normal build artifacts. The actual Wii/1.1 linker and dtk ELF-to-DOL conversion both exit zero. The candidate and frozen DOL are both 4,094,880 bytes. They differ at exactly 116 bytes in 87 instruction words, all inside `__wudInitDevInfo` at `0x8030C020`-`0x8030C658`. Every other executable byte agrees, including the other 85 WUD functions, relocated calls, initialized data, small data and alignment.

The candidate ELF places all 86 retained WUD functions at their original addresses. Both source-only message catalogs and the standalone `WUDStartClearDevice` body are absent. This proves stripping and placement for this candidate; it does not make the remaining importer acceptable or pass frozen verification.

The candidate comparison also corrected F-504-14's raw-split payload interpretation. At virtual address `0x803DF1FC`, the frozen DOL and pinned SDK reference contain `80 01 01 38`. The original split object has an inferred `R_PPC_ADDR32` relocation to `@etb_80010138`, so its raw four bytes are zeros. Copying those placeholders into the controller payload introduced four final-byte differences. Restoring the frozen/reference bytes removes those differences. Ordinary compiler jump tables likewise require relocation-aware data comparison. No original file, expected hash or manifest changes.

## Validation and acceptance boundary

The real `all_source` target compiles successfully. Full Ninja and explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1` pass with the original WUD object selected. All 340 baseline source objects remain byte-identical. Whole-unit objdiff diagnostics are text 99.77785%, data 11.763171%, BSS 100%, sdata 100%, sbss 100% and sdata2 63.636364%. The data diagnostic uses coarse reference metadata and relocation placeholders; the resolved comparison and candidate DOL establish the initialized data bytes instead.

WUD remains `NonMatching` and gains zero linked WUD source credit. Resolve the importer and rerun the complete source-selected frozen verification before acceptance. A source-coverage count of 86 does not close #504.

## Provenance

Initialization, control-block clearing, device import and shutdown use functional [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. Balance Board recovery, hook names and private helpers are corroborated by [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Target operands determine the state values, diagnostic placement and panic line. Comments and annotations are removed. No decompiler text is included.

## Traps

- **Zero initialization spelling changes CodeWarrior layout.** Explicit `= FALSE` definitions are emitted differently from tentative scalar definitions. The reference's declaration order and tentative definitions reproduce the complete observed small-BSS layout; do not infer physical offsets from a 100% zero-data score alone.
- **An API may survive only through inlining and its message.** Treat the original clear-device API body separately from the 86 retained native functions. A standalone source copy and source-only message catalogs require a stripping proof before acceptance.
- **An available-looking tick macro may actually be an undefined call.** This include graph has no `OSDiffTick` or `OS_TICKS_DELTA`. Explicit low-word subtraction reproduces the target's arithmetic. Casting a completed 64-bit subtraction still emitted unnecessary high-word work.

- **Raw split bytes are not resolved executable data.** The inferred controller-word relocation and the genuine compiler jump table both contain placeholders. The scratch source-selected link exposed the incorrect payload interpretation and proves all final data bytes only after relocation.
