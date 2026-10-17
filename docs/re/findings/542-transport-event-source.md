# F-542-2: MetroTRK transport, notification and event sources

Issue #542 owns 25 functions and 2,132 native code bytes from `0x8034E380` through `0x8034EBD4`. The first six functions, totaling 652 bytes, belong to the `dolphin_trk.c` translation unit whose first function belongs to #541. That owner supplies the complete unit. This delivery reconstructs the remaining nineteen functions in three complete translation units, totaling 1,480 code bytes. The six shared functions receive no duplicate reconstruction credit here.

## Source and compiler evidence

The reference is Petari revision `e0ab141914921726a595573dc48d3f13d0922efe`, under its CC0 license. The corresponding MetroTRK files have no separate file-level license notice. Their names and control flow are retained where the target agrees; target differences determine the transport selection, UART interface, queue layout, notification exchange and diagnostic call. Source includes the canonical shared interfaces established in [F-542-1](542-transport-headers.md).

All three units use Wii/1.1, four-byte function alignment, automatic inlining and `-use_lmw_stmw on`. The shared MetroTRK policy disables small data and uses readonly strings. The transport unit overrides both small-data thresholds to eight and uses writable pooled strings. The queue unit also uses writable pooled strings. These are per-unit settings proven against the target, rather than a compiler change for the surrounding library.

| Unit | Native text | Functions | Code bytes | Other split sections |
| --- | --- | --- | --- | --- |
| `Os/dolphin/dolphin_trk_glue.c` | `0x8034E60C`–`0x8034E9A8` | 13 | 924 | `.data` `0x803E6320`–`0x803E6408`; `.bss` `0x804B3A70`–`0x804B3A98`; `.sbss` `0x804B9C10`–`0x804B9C18`; `.sdata2` `0x804BBBF8`–`0x804BBBFC` |
| `Portable/notify.c` | `0x8034E9A8`–`0x8034EA38` | 1 | 144 | None |
| `Portable/nubevent.c` | `0x8034EA38`–`0x8034EBD4` | 5 | 412 | `.data` `0x803E6408`–`0x803E6428`; `.bss` `0x804B3A98`–`0x804B3AC0` |

The transport strings occupy 225 initialized bytes and the queue diagnostic occupies 29; the following bytes belong to alignment. The end-of-program trap is the four-byte constant `0x00454E44`. Its compiler-generated weak local symbol is `@LOCAL@InitializeProgramEndTrap__Fv@EndofProgramInstruction`. The communication table is 40 bytes, `TRK_Use_BBA` is one byte, and the queue is 36 bytes; their splits preserve the observed padding before the following units.

## Target behavior and naming

`InitMetroTRKCommTable` selects the GDEV callback table for hardware ID 1. ID 2 returns success immediately after clearing the BBA flag; the older reference's UDP initialization is absent. Unknown hardware emits the original diagnostics and returns one. The UART initializer accepts two unused flag arguments and a `void**` input-pending address, as proven by the #543 caller. The read/write wrappers normalize errors to zero or minus one. `InitializeProgramEndTrap` copies the trap word into `PPCHalt + 4` and flushes both caches.

The queue has no mutex prefix or mutex calls. Its count, ring index, two events and event ID use the complete layout in F-542-1. Posting to a full queue emits the target's `MetroTRK - Event Queue full` diagnostic and returns `DS_EventQueueFull`. Event IDs start at 0x100 and reset there on wraparound. `TRKDoNotifyStopped` obtains a request buffer, adds stop or exception information, exchanges it through the two-argument `TRKRequestSend`, and releases any successful response and the request.

Outside names are supported by native calls and the coordinated #543/#545 implementations: `TRK_memcpy`, `TRKGetFreeBuffer`, `TRKReleaseBuffer`, `TRKRequestSend`, `TRKTargetAddStopInfo` and `TRKTargetAddExceptionInfo`. Their existing sizes, types and unit ownership remain intact.

## Validation

The split-only original-object build passed configure, Ninja and the unchanged frozen-manifest verifier before source selection. Actual reconstructed source objects then scored 100% in every text, data, BSS and small-data section in objdiff. An independent relocation audit resolved all 89 text relocations to their native targets and compared every instruction word for all nineteen functions, all 258 initialized data bytes and all storage extents. The audit neither modifies objects nor supplies linker inputs.

The source-selected build passes `python configure.py --build-dir build/542-merge2`, `.venv/Scripts/ninja` and the explicit frozen-manifest verifier for `build/542-merge2/wpse-v1`, each exiting zero. The rebuilt DOL is byte-identical to the original and has SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The actual linker inputs select all three reconstructed objects; the final ELF retains every owned function's native address and extent. Of 361 baseline source objects, 359 retain identical code, data and relocation targets. Merged #721 adds only the four-byte `strncasecmp` forwarding branch to `extras.c`; its original 160 bytes and relocations remain identical. Merged #740 moves the empty weak `FxBase::OnChangeOutputMode` definition out of the AxManager object. All 32 remaining AxManager function bodies and normalized relocations, all data sections and the constructor target remain identical; only that four-byte weak body and twelve alignment bytes disappear. These independently audited changes originate on main, and the full frozen DOL remains exact.

Merged #721 supplies the six shared functions from the #541-owned Dolphin unit. An independent audit of its actual compiled Wii/1.1 object checks the complete seven-function unit, including the six supplied #542 bodies, with 804 text bytes, 39 paired relocations and 60 interrupt-vector table bytes exact. The final linker selects that reconstructed source object too. The complete original #542 inventory therefore has all 25 functions and 2,132 bytes at their native final ELF addresses and extents. The six shared functions receive no duplicate reconstruction credit here.

## Traps

- CodeWarrior string suboptions accumulate. Appending `-str reuse,pool` to `-str reuse,readonly` leaves strings readonly. The actual transport and queue objects need `-str reuse,pool,noreadonly` to reproduce the native `.data` placement; their instructions alone cannot detect the wrong section.
- The queue unit uses the legacy fixed one-argument `OSReport` declaration. Including the variadic OS interface introduces a CR1-clearing instruction absent from its native diagnostic call. The transport unit uses the normal variadic interface and retains its native CR1 handling.
- A redundant nested success check in the older notification reference emits an extra branch. The target performs one initial buffer check and one response check; expressing that control flow directly reproduces all 144 bytes.
