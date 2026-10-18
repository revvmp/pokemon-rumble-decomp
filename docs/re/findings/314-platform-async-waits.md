# F-314-1: Platform timing and asynchronous file waits

Issue #314 has 25 surveyed entries and 4,528 instruction bytes between `0x801AFC80` and `0x801B0EE8`. The unchanged isolated baseline configures, completes plain Ninja, and passes the frozen executable verifier. All 25 entries are now reconstructed and source-selected in three complete units. F-314-3 records the initial fourteen-function acceptance; F-314-5 records the complete issue inventory and real final source link.

## Native boundaries and dependencies

The first four functions are timing wrappers at `0x801AFC80`, `0x801AFC90`, `0x801AFCA0`, and `0x801AFCE0`. They call `VIGetRetraceCount`, call `VIWaitForRetrace`, return `3000 / (VIGetTvFormat() == 1 ? 50 : 60)`, and return the nominal interval 50 respectively. The division is signed. The wrapper names remain descriptive because no original spelling has been recovered.

The external `314-reference/video-timing.cpp` probe and `314-audit-video.py` independently prove all four function sizes, 72 instruction bytes and three SDK call targets under the game compiler. Initializing the rate with the conditional expression preserves the native lifetime; initializing it to 60 before calling the SDK introduces a saved register and eight extra bytes. No compiler override is needed.

The callback at `0x801AFCF0` and ten wrappers through `0x801B0638` use PrFILE2 asynchronous commands. Their command work area is `0x170` bytes, followed by an `OSSemaphore` at `0x170` and signed result at `0x17C`. The callback stores the result before signaling the semaphore. The first partial delivery left this SDK ABI unresolved. The independently authored public work-area contract and exact additional consumer proof are now recorded in F-314-4 and owner prerequisite F-370-1.

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

This first delivery selected 14 of the original 25 functions / 2,220 of 4,528 instruction bytes. It retained the 11 PrFILE2 entries / 2,308 bytes as automatic fallbacks and used a partial-work issue reference. The additional consumer proof below supersedes the initial layout limitation; actual source selection still requires its own full build and frozen verification.

## F-314-4: Typed FA asynchronous consumers

The #370 owner independently reconstructed and approved the canonical public `revolution/fa.h` interface in PR #920. F-370-1 records native caller/callee evidence, public ABI provenance and the restricted reference's license limitation. No restricted implementation or header text is imported here. The public command explicitly contains a user pointer, seventy opaque `u32` SDK work words and `FANandInfo`, rather than exposing guessed internal fields. This documented SDK work-area contract independently explains the native `0x170` size and the backing-file record at `0x11C`, with size at `0x16C`. No internal volatile qualifier is introduced.

The complete `FaWaitContext` derives from canonical `FACommandBlock`, appending `mSemaphore` at `0x170` and signed `mResult` at `0x17C`, for size `0x180`. The ordinary callback stores the result then signals the semaphore. All ten wrappers preserve the same submission-failure, fiber-yield, completion and exception-wait behavior as the NAND wrappers. SDK `FAFile`, `FADta` and `FADrvTbl` aliases retain the actual canonical PrFILE2 types; no alternate ABI or guessed opaque object layout is introduced by the consumers.

| Address | Size | Native SDK operation |
| --- | --- | --- |
| `0x801AFCF0` | `0xC` | Completion callback |
| `0x801AFD00` | `0xF0` | `FAAttachAsync` |
| `0x801AFDF0` | `0xD8` | `FAMountAsync` |
| `0x801AFED0` | `0xE8` | `FAUnmountAsync` |
| `0x801AFFC0` | `0xF8` | `FACreateAsync` |
| `0x801B00C0` | `0xF8` | `FAFwriteAsync` |
| `0x801B01C0` | `0xF8` | `FAFsfirstAsync` |
| `0x801B02C0` | `0xD8` | `FAFsnextAsync` |
| `0x801B03A0` | `0xD8` | `FARemoveAsync` |
| `0x801B0480` | `0xD8` | `FAMkdirAsync` |
| `0x801B0560` | `0xD8` | `FARmdirAsync` |

Compiling the actual `src/game/FaAsync.cpp` with only its tracked include tree and the exact owner-approved temporary header reproduces all eleven functions / 2,308 instruction bytes and 115 independently paired text relocation targets. The strict external `314-audit-fa.py` also verifies all ten exception records at `0x80037C88`–`0x80037F80`, all ten indices at `0x800506B8`–`0x80050730`, 880 combined exception/index bytes and 40 independently paired table/index relocation targets. Compilation and audit exit zero. The producer owner independently repeated the same real consumer proof against its tracked canonical header. The header content is preserved unchanged and will be deduplicated after its separate prerequisite merges.

This completes individual native reconstruction evidence for all 25 original entries / 4,528 instruction bytes, with 218 text and 78 exception/index relocation targets and 1,692 exception/index bytes. It does not replace the remaining required split-only, NonMatching, Matching, source-selected link, baseline-control and exact committed-head frozen gates.

### Canonical prerequisite integration checkpoint

After #910 and #920 merged, the source branch was restacked onto canonical main `fb63a24627e72e30f4f48badb7e1b1b82cb04bef`. The temporary activity/FA header commits were dropped; both interfaces are inherited unchanged from main. All six owned source/header files remain byte-identical to source checkpoint `9136d953d0be1deb6bdeff31eb1173b13274669a`. Shared config entries combine with all current main objects and names, and union-appended ledger/symbol records were checked for duplicate rows. The header's actual 550-object integration control remains recorded above.

Before restacking, an independent immutable snapshot preserved the exact tracked checkpoint, all 526 compiled source objects from the accepted fourteen-function `986c3977f37e174bef7362c918c67a207946a073` output, its ELF/DOL, and the separate individual FA evidence. These are distinct evidence stages: the newer checkpoint still registers FA as NonMatching and has not completed its source-selected full-artifact gate.

The actual tracked FA source was recompiled using only canonical main declarations after restacking. `314-compile-fa-source.py` and `314-audit-fa.py 314-reference/fa-actual-source.o` both exit zero: all eleven functions / 2,308 bytes, 115 text targets, ten exception records / 880 exception-and-index bytes, and forty exception targets remain exact. No configure, Ninja or whole-artifact verifier was run for this integration checkpoint; the mandatory fresh metadata/source-selection gate remains pending.

## F-314-5: Complete original inventory and source selection

The staged pipeline on main `82571e1dcdb33589073ef500ecf425835813239a` completes the original #314 inventory. Split-only commit `447757eb480542fa70510c82b314c56d8ae97533` has no FA source registration. Fresh default configure and plain Ninja (`-j16`, 560 targets) exit zero, followed by the explicit frozen verifier. An independent control captures all 553 actual compiled source objects and both artifact hashes at that stage.

The actual FA object is then registered NonMatching at `979a3f2a`. Configure, plain Ninja and frozen verification exit zero. The real Ninja-produced object independently reproduces eleven functions / 2,308 bytes, 115 actual code targets, all ten exception records and indices / 880 bytes, and forty exception targets. The complete report shows all 25 functions and all nine allocated sections of the three owned units at 100 percent.

Matching source selection at `0f6b21f08eb80dc1dda7d49c89720d762c739633` also completes configure, plain Ninja and explicit frozen verification with exit zero. The linker selects each of the three real compiled source objects exactly once and selects none of their original fallback objects. Every original issue entry is checked against the actual GitHub inventory: all 25 native addresses and sizes, all 4,528 linked instruction bytes, and all 1,692 exception/index bytes match the original executable. The complete independent relocation inventory contains 218 code sites and 78 exception/index sites. All 553 previous whole source objects remain SHA-256 identical; only `game/FaAsync.o` is added, bringing the compiled source inventory to 554. The rebuilt DOL remains identical to the split-stage DOL and the frozen manifest.

| Acceptance command | Exit | Evidence |
| --- | --- | --- |
| `python configure.py` then `.venv/Scripts/ninja -j16` at each stage | 0 | Actual split-only, NonMatching and Matching pipeline |
| `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` after each successful Ninja | 0 | Frozen executable identity |
| `python C:/MassStorage/GitHub/314-audit-fa.py build/wpse-v1/src/game/FaAsync.o` | 0 | Actual FA object instructions, targets and exception records |
| `python C:/MassStorage/GitHub/314-audit-report.py matching` | 0 | 25 functions and nine allocated sections at 100 percent |
| `python C:/MassStorage/GitHub/314-audit-linked.py` | 0 | Source-only linker inputs, 25 native addresses and all owned linked bytes |
| `python C:/MassStorage/GitHub/314-audit-original-inventory.py` | 0 | Every original issue row, exact size and native bytes |
| `python C:/MassStorage/GitHub/314-final-stage-control.py check` | 0 | All 553 previous whole objects unchanged and exact DOL |

All original #314 functions are covered, with no neighboring source contribution or duplicate byte credit. The activity and fiber implementations and FA SDK producers remain with their respective owners. Newer skill guidance is adopted for the final integration loop: retain build outputs, remove only the old DOL before Ninja, then require successful Ninja and exact frozen verification after each rebase. The earlier fresh staged evidence remains valid and preserved.

Later canonical main integration adds the FA standard wrappers, CNT stream and BWorld independently of this issue. The canonical ResTexture header additions change eleven EF objects' compiler metadata. Their old raw files are independently preserved and have the exact hashes from the 553-object split control; comparing them proves identical allocated section types, flags, sizes, alignments, bytes and canonical relocation targets. Thus 542 previous whole object files remain identical and eleven are allocated-equivalent, rather than claiming all 553 final files are byte-identical. Newer main objects are separately classified against their actual tracked main source paths. The DOL remains identical. `314-audit-external-object-metadata.py` and the extended stage control both exit zero for this classification.

The final landing uses the repository's unchanged `land.py` workflow. An outside issue-prefixed wrapper runs the original frozen verifier and then the full original-inventory, source-only link, section, relocation and object-control audits before the helper can push or merge each candidate. It also rejects duplicate or stale owned ledger rows and updates the pull request's verified-head evidence. No tracked tool is modified and no verification or ancestry check is bypassed.

After the canonical #912 integration, the final control independently revalidates all 599 accepted raw source-object hashes and requires every one to remain whole-file identical in this issue's 602-object build. Against the historical 553-object split control, 539 files are whole-identical and twelve differ only outside allocated sections. Two further files are independently preserved current-main contributions: SmoothStripe has changed code, and pf_dir has a different historical hash without an available matching old raw file. Neither is called allocated-equivalent. Their actual candidate bytes must equal the separately rehashed accepted current-main raw files. The earlier eleven-object metadata classification describes its preceding main checkpoint, not these newer canonical changes. The full frozen DOL remains exact.

## Traps

Independent assistance to #313 and the canonical list investigation: the CPath deleting destructor at `0x801AE820` / `0xCC` tests its list member at `this + 4` with `addic. r31,r3,4` at `0x801AE844`, followed by three identical conditional branches at `0x801AE848`, `0x801AE84C` and `0x801AE850` to `0x801AE8C0`. This corroborates three nested destruction layers independently of the BUI shared-pointer list; it does not justify adding artificial null guards. Each 20-byte node contains two link pointers followed by a twelve-byte narrow string. The string destructor loads word zero, then executes `rlwinm. r0,r0,1,31,31` at `0x801AE890`. This rotates the original word's HIGH bit into the low bit and tests it before freeing the pointer at string offset eight. An earlier chat description incorrectly called this the low bit. The corrected interpretation agrees with the independently exact wide-string diagnostic in F-53-3; generic string growth and short-string behavior remain unresolved. No canonical string or list implementation is added here.

The survey's NAND description does not identify every wrapper: the earlier callback has result offset `0x17C`, whereas the actual NAND callback has result offset `0xC8`. Their direct callees and canonical command sizes prove two different APIs. Do not apply the NAND layout to the PrFILE2 work area.

The exception handler calls `OSWaitSemaphore`, not a semaphore destructor. Its role is to preserve a live asynchronous request across a fiber exception; removing it or modeling it as destruction changes both behavior and exception tables.
