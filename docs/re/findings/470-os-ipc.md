# Issue 470: OS IPC buffer and time conversion

## F-470-1: OSIpc boundaries and original names

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. The three functions at `0x802BD980`, `0x802BD990`, and `0x802BD9A0` correspond to Petari `src/RVL_SDK/os/OSIpc.c` at commit `e0ab141914921726a595573dc48d3f13d0922efe` (CC0; no file-level license exception). Source follows that reference with direct cached-address constants and a minimal project header.

The first getter loads the existing local `IpcBufferHi` at `.sbss:0x804B9510`; the second loads `IpcBufferLo` at `.sdata:0x804B8AA0`. Initialization copies the words at `0x80003130` and `0x80003134` into Lo and Hi respectively. Lo starts as `0xFFFFFFFF`; Hi starts as zero. Getter names are `__OSGetIPCBufferHi` and `__OSGetIPCBufferLo`, high confidence. IPCInit calls them at `0x802F1248` and `0x802F1250`, corroborating issue #496.

The unit owns `.text` `0x802BD980-0x802BD9B8`, `.sdata` `0x804B8AA0-0x804B8AA8`, and `.sbss` `0x804B9510-0x804B9518`, including trailing alignment. dtk 1.8.4 split assembly establishes the ranges. Function sizes are 8, 8, and 24 bytes; the remaining 16 text bytes are alignment. Subtract `0x8004FD40` for text DOL offsets. Data objects are 4-byte pointers with 4 trailing padding bytes each.

## F-470-2: Time-conversion dependency

`fn_802BD100` is `__OSTimeToSystemTime` (high confidence). Its 0x58 bytes disable interrupts, add the caller's 64-bit value to the 64-bit OS time offset at `0x800030D8`, restore interrupts, and return the sum. This matches Petari OSTime.c at the behavioral level and its use in OSSetPeriodicAlarm at `0x802B32C4` (#464). This change recovers the name only; no source-match claim is made for it.

OSTime.c starts with OSGetTime at `0x802BD060` and OSGetTick at `0x802BD080`, owned by #469. That issue owns the split and header under the collaboration rules. Its remaining functions in #470 stay in their existing fallback until coordination with that owner. OSUtf, OSStateTM, OSPlayRecord, OSStateFlags, OSNet, and OSNandbootInfo also remain unreconstructed in this contribution; #470 stays open.

## Validation

Split-only build and verifier passed. With the corrected boundary, `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exited 0. Objdiff 3.8.2 reports all three functions, 40 code bytes, and 16 data bytes at 100%. The `Matching` unit is linked from source and the full DOL passes the frozen manifest and direct byte comparison.

## Traps

A first split ending at `0x802BD9C0` scored 100% in objdiff but failed full verification: the source object ends at offset `0x38` and the next fallback unit aligned to 8 bytes, placing its first function at `0x802BD9B8`. Ending the source split at its real last instruction boundary retains the original inter-unit padding in the fallback. This is another instance of the alignment caution in F-0010, not a change to expected hashes.
