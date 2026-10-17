# F-314-1: Platform timing and asynchronous file waits

Issue #314 has 25 surveyed entries and 4,528 instruction bytes between `0x801AFC80` and `0x801B0EE8`. The unchanged isolated baseline configures, completes plain Ninja, and passes the frozen executable verifier. The fourteen timing/NAND entries below are now source-selected and accepted; the eleven PrFILE2 entries remain unfinished, so this does not complete the issue.

## Native boundaries and dependencies

The first four functions are timing wrappers at `0x801AFC80`, `0x801AFC90`, `0x801AFCA0`, and `0x801AFCE0`. They call `VIGetRetraceCount`, call `VIWaitForRetrace`, return `3000 / (VIGetTvFormat() == 1 ? 50 : 60)`, and return the nominal interval 50 respectively. The division is signed. The wrapper names remain descriptive because no original spelling has been recovered.

The external `314-reference/video-timing.cpp` probe and `314-audit-video.py` independently prove all four function sizes, 72 instruction bytes and three SDK call targets under the game compiler. Initializing the rate with the conditional expression preserves the native lifetime; initializing it to 60 before calling the SDK introduces a saved register and eight extra bytes. No compiler override is needed.

The callback at `0x801AFCF0` and ten wrappers through `0x801B0638` use PrFILE2 asynchronous commands. Their command work area is `0x170` bytes, followed by an `OSSemaphore` at `0x170` and signed result at `0x17C`. The callback stores the result before signaling the semaphore. The #369 owner independently proves a typed 64-byte common command at work-area offset four, and the attach API stores a NAND backing-file record at `0x11C`, with a 78-byte path and size at `0x16C`. At the initial diagnostic checkpoint, the intervening storage was unresolved. Subsequent independent #370 evidence establishes the actual public opaque work-area contract; its separately owned header prerequisite is tracked in #920. This bounded activity-header prerequisite adds no FA declaration, guessed storage or PrFILE2 implementation.

The final callback and nine wrappers use the existing canonical `NANDCommandBlock`, whose size is `0xBC`. A derived wait context appends `OSSemaphore` at `0xBC` and signed result at `0xC8`, giving size `0xCC`. This exactly accounts for the native stack and callback offsets without redefining an SDK structure.

All asynchronous wrappers construct an empty activity scope, initialize a zero-count semaphore, and submit the operation. Submission failure returns immediately. Successful submission tries the semaphore repeatedly, yielding through the existing fiber routine while it remains unavailable. If yielding throws, the catch handler blocks in `OSWaitSemaphore` before rethrowing; the request and its stack context therefore remain alive until completion. Normal completion reads the result stored by the callback.

The empty scope constructor at `0x801B5A80`, size `0x10`, increments one global activity count and never accesses object storage. Its deleting destructor at `0x801B5A90`, size `0x6C`, decrements that count, clamps it against the observed zero bound, and conditionally invokes operator delete. The neighboring zero-count query at `0x801B5A60` and reset at `0x801B5A70` corroborate the count interpretation. These routines belong to #317; this issue does not take their source, split, or object registration. A shared declaration must distinguish descriptive naming from recovered original spelling.

## Exact NAND diagnostic

An ordinary C++ probe using canonical SDK declarations and Wii/1.1 with the established game flags produces every native instruction and independently paired call/address target for these entries:

| Address | Size | Proven behavior |
| --- | --- | --- |
| `0x801B0640` | `0xC` | Completion result store and semaphore signal |
| `0x801B0650` | `0xF8` | Wait for `NANDCreateAsync` |
| `0x801B0750` | `0xF8` | Wait for `NANDOpenAsync` |
| `0x801B0850` | `0xD8` | Wait for `NANDCloseAsync` |
| `0x801B0930` | `0xF8` | Wait for `NANDWriteAsync` |
| `0x801B0A30` | `0xF8` | Wait for `NANDCreateDirAsync` |
| `0x801B0B30` | `0xD8` | Wait for `NANDDeleteAsync` |
| `0x801B0C10` | `0xE8` | Wait for `NANDMoveAsync` |
| `0x801B0D00` | `0xF8` | Wait for `NANDCheckAsync` |
| `0x801B0E00` | `0xE8` | Wait for `NANDGetHomeDirUsageAsync` |

The diagnostic proves ten functions, 2,148 instruction bytes, and 100 independently paired text relocation targets. All nine exception records and their indices are also exact: 684 exception bytes at `0x80037F80`–`0x8003822C`, 108 index bytes at `0x80050730`–`0x8005079C`, and 36 independently paired table/index relocations. No target byte is copied into the source. The code was reconstructed from this executable rather than imported from another project.

Reproduction tools are outside the repository: `314-reference/nand-wait.cpp`, its compiled object, and `314-audit-nand.py` in the shared investigation directory. The audit reads the frozen native ELF, requires exact sizes and instructions, resolves each call/address relocation to the independently identified SDK or game entry, and compares relocated exception/index records. Compilation and the strict audit both exit zero. The initial diagnostic was followed by the complete split/source-selected acceptance recorded in F-314-3.

## F-314-2: Bounded activity-scope declaration

The header-only prerequisite adds `include/game/CScopedActivity.h`, containing only a class with a declared constructor and nonvirtual destructor. The complete empty declaration has no invented fields. `CScopedActivity` is a descriptive name for the proven count behavior, not a claim to have recovered the original class spelling. No implementation, symbol rename, split, object registration, source credit, or #317 acceptance is included. The coordinating maintainer approved this bounded declaration while leaving #317 source ownership intact.

The NAND diagnostic includes this actual tracked header, rather than an external replacement. Recompiling with it preserves all ten exact function bodies, all 100 text relocation targets, all nine exception records and indices, and their 36 relocation targets. This consumer evidence supports the declaration independently of an otherwise unused header's unchanged baseline build.

The header candidate also completes a fresh default 531-target plain Ninja build. All 524 existing compiled source objects have identical complete SHA-256 hashes before and after adding the declaration; no allocated section, relocation, compiler metadata, or object inventory changes. Configure, Ninja, the object control and the explicit frozen executable verifier exit zero. The header contributes zero reconstructed instruction bytes.

The later integration control is actual main `ba3bf8dd03f37dc60726198a95bb223743e36622`, including the newly added NonMatching BModelObj source and canonical interfaces from #943. A fresh default 557-target plain Ninja and explicit frozen verification both exit zero on that clean commit. All 550 compiled source objects, their hashes, the ELF, DOL and tracked source/header/config input hashes are independently preserved before applying this declaration. This replaces the earlier 524-object control for the final header gate; no superseded interface proposal is substituted for canonical main.
## F-314-3: Source-selected timing and NAND units

Two descriptive source units now replace fourteen original #314 entries, totaling 2,220 native instruction bytes. `game/VideoTiming.cpp` owns `.text` `0x801AFC80`–`0x801AFCE8`, `extab` `0x80037C80`–`0x80037C88`, and `extabindex` `0x800506AC`–`0x800506B8`. `game/NandAsync.cpp` owns `.text` `0x801B0640`–`0x801B0EE8`, `extab` `0x80037F80`–`0x8003822C`, and `extabindex` `0x80050730`–`0x8005079C`. The final function ends establish text boundaries, leaving subsequent alignment to the next object. dtk attaches the timing function's eight-byte exception record and twelve-byte index automatically; these owned ranges are retained in the split metadata.

The existing Game library registers both with Wii/1.1 and unchanged `cflags_game`. Split-only fresh configure/plain Ninja/frozen verification passes first. Both are then compiled as NonMatching, and every retained function and every allocated section scores 100 percent in objdiff. After changing only these two registrations to Matching, plain Ninja and explicit frozen verification pass again. The actual link selects each compiled source object exactly once and selects neither automatic fallback object. All fourteen final function addresses, all 2,220 relocated instruction bytes and all 812 exception/index bytes equal the original executable. The timing exception/index pair contributes two additional paired relocation targets, bringing the complete independent audit to 103 text and 38 exception/index targets.

`NandWaitContext` is a complete `0xCC`-byte derived SDK context: canonical `NANDCommandBlock` at zero, `mSemaphore` at `0xBC`, and signed `mResult` at `0xC8`. Its header preserves all canonical SDK field types. The callback downcasts the SDK base pointer to its actual derived wait context; there are no integer-address accessors or invented padding fields. The public wrappers use proven SDK argument types. The scope and fiber callees are renamed only to resolve actual native references; their source/splits/sizes remain with their original owners.

All 524 compiled source objects from the unchanged baseline remain identical as complete files; the candidate adds only the two owned source objects. The source names, wrapper names, context/member names, `CScopedActivity`, and `YieldFiber` are descriptive, with medium spelling confidence and high confidence in their native behavior and ABI. No reference implementation was copied. No compiler flag, original hash, expected artifact, maintainer-only progress state, or neighboring source ownership changes.

This delivers 14 of the original 25 functions / 2,220 of 4,528 instruction bytes. The 11 PrFILE2 entries / 2,308 bytes remain automatic fallbacks. Their incomplete outer SDK command layout is not concealed by an arbitrary storage array, and the pull request uses a partial-work issue reference.

## Traps

Independent assistance to #313 and the canonical list investigation: the CPath deleting destructor at `0x801AE820` / `0xCC` tests its list member at `this + 4` with `addic. r31,r3,4` at `0x801AE844`, followed by three identical conditional branches at `0x801AE848`, `0x801AE84C` and `0x801AE850` to `0x801AE8C0`. This corroborates three nested destruction layers independently of the BUI shared-pointer list; it does not justify adding artificial null guards. Each 20-byte node contains two link pointers followed by a twelve-byte narrow string. The string destructor loads word zero, then executes `rlwinm. r0,r0,1,31,31` at `0x801AE890`. This rotates the original word's HIGH bit into the low bit and tests it before freeing the pointer at string offset eight. An earlier chat description incorrectly called this the low bit. The corrected interpretation agrees with the independently exact wide-string diagnostic in F-53-3; generic string growth and short-string behavior remain unresolved. No canonical string or list implementation is added here.

The survey's NAND description does not identify every wrapper: the earlier callback has result offset `0x17C`, whereas the actual NAND callback has result offset `0xC8`. Their direct callees and canonical command sizes prove two different APIs. Do not apply the NAND layout to the PrFILE2 work area.

The exception handler calls `OSWaitSemaphore`, not a semaphore destructor. Its role is to preserve a live asynchronous request across a fiber exception; removing it or modeling it as destruction changes both behavior and exception tables.
