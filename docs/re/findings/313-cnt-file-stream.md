# F-313-1: CNT handle and file-stream unit

## Scope and ownership

Issue #313 originally lists 35 entries / 8,020 instruction bytes. The survey's `0x801AF870` CntFileStream start omits its preceding CNT handle implementation. The independently established unit begins at `0x801AF780`, with CNTHandleWithMutex construction, destruction and initialization, and ends at `0x801AFC7C`, after its static initializer. It contains 23 retained functions / 1,128 instruction bytes. No preceding CPath or CFileData functions are accepted by this finding.

The exact class names `nw4r::ut::CNTHandleWithMutex` and `nw4r::ut::CntFileStream` occur in the executable's shared-pointer deleter names and RTTI. The implementation is independently reconstructed from target instructions, vtables, exception tables and existing canonical CNT/FileStream declarations. No third-party implementation is copied. `Init`, member names and nested `ScopedLock` are descriptive names where original spellings are not established.

## Baseline and diagnostic evidence

The unchanged isolated worktree passes configure, fresh plain Ninja (523 targets) and explicit frozen main.dol verification, each exit zero. The verifier covers main.dol only, excluding the boot executable, data contents and WAD packaging.

An external probe using established `GAME_COMPILER` Wii/1.1 and `cflags_game` reproduces every retained function's complete instruction stream after resolving all relocations to their independently established native targets. The probe also reproduces the complete initialized data and small-data sections, constructor pointer, every retained exception record and exception index. There are 82 paired relocation sites. An additional unretained IOStream destructor and its exception metadata are explicitly excluded; they do not receive native byte credit.

The split-only and subsequent NonMatching builds both pass plain Ninja and frozen verification. The source-selected build also passes plain Ninja and explicit frozen verification, each exit zero. Actual final ELF symbols retain all 23 original addresses, every function byte and all initialized tables. Its linker input contains the compiled source object exactly once and excludes the original split object. The unused IOStream destructor is absent from the final ELF. All 516 pre-existing compiled source objects remain whole-file byte-identical to the fresh baseline.

The actual object comparison reports 100% for every retained function and for .text, .ctors, .data, .sdata and .sbss. Raw extab and extabindex section scores are zero because the source object includes the additional unused destructor's eight-byte exception entry and twelve-byte index. This metadata shifts later raw section offsets and is removed during linking. The independent comparison pairs all nine retained exception records and indexes by their actual function/cleanup relocation targets; every retained byte matches, and the complete final native exception extents are exact. The raw section scores are preserved as a diagnostic limitation, not rounded or claimed as 100%.

The current-main rebase onto `09fd2b0a` passes a separate fresh 525-target default plain Ninja build, explicit frozen verification and both actual-source/native and final-ELF audits, all exit zero. Of the 516 historical control objects, 512 remain whole-file identical. MWRTTI differs only in compiler-local metadata, with all allocated bytes and canonical relocations unchanged. The upstream shared-pointer header adds an unretained base destructor to msl_thread; both original function bodies and their exception prefixes/relocations remain exact, and the added destructor is absent from the final ELF. The only other changed old objects are canonical main's pf_dir and HBMBase; main also adds pf_entry. All of those upstream files are preserved without edits. The only owned source addition is the CNT stream unit.

The complete standalone unit is source-selected Matching. The remaining twelve original #313 functions / 6,892 instruction bytes before this unit are unfinished, so the issue remains open. This acceptance does not establish complete original-inventory coverage.

## Native layout

| Type | Proven layout |
| --- | --- |
| CNTHandleWithMutex | initialized boolean `0`, OSMutex `4`, CNTHandle `0x1C`, total `0x44` |
| CntFileStream | FileStream base `0` / `0x14`, FilePosition `0x14`, handle pointer `0x1C`, CNTFileInfo `0x20`, close-on-destroy flag `0x64`, close-enable flag `0x65`, total `0x68` |
| ScopedLock | one handle reference; construction locks its mutex and destruction unlocks it |

Opening closes a prior owned stream, locks the supplied handle, calls CNTOpen, initializes size and position, and sets both close flags and availability. Closing locks the handle, closes the CNT file, clears availability and releases its handle pointer. Reading serializes CNTReadAt with the handle mutex and advances FilePosition only for a positive result. Seeking uses the canonical FilePosition implementation. The virtual table preserves the existing FileStream order; reads and seeks are supported, asynchronous access, writes and cancellation are not. Buffer and size alignment are 32, offset alignment is 4.

| Section | Native extent |
| --- | --- |
| extab | `0x80037BEC`–`0x80037C80` |
| extabindex | `0x80050640`–`0x800506AC` |
| .text | `0x801AF780`–`0x801AFC7C` |
| .ctors | `0x80352A14`–`0x80352A18` |
| .data | `0x803BF2E0`–`0x803BF3A8`, last initialized byte before `0x803BF3A3` |
| .sdata | `0x804B81D8`–`0x804B81F0` |
| .sbss | `0x804B9130`–`0x804B9134` |

## Traps

The survey labels this as a library file linked in the game range, but native exception records and CodeWarrior RTTI require the established game flags for this unit. Do not apply the exceptions-disabled NW4R defaults merely because its namespace is nw4r::ut.

The existing AutoLock inherits NonCopyable. Using it adds a guard-address lifetime and extra saved register around exception cleanup, changing Open, Close and Read. A simple scoped guard with private copy operations and the evidenced single reference reproduces all three complete functions and the retained guard destructor. This is evidence for this unit's guard structure, not a reason to change the shared AutoLock class.

The emitted empty IOStream destructor has identical code to the retained FileStream destructor. Only the latter is referenced by the native CntFileStream exception cleanup. Identify retained function ownership through those relocation targets, rather than counting both identical candidate streams.

Compiler-generated local names such as `@635` collide across translation units. Never resolve their addresses against unrelated names in a linked ELF; map source-local objects through their owning section or paired relocation evidence.

The compiler's static-initializer symbol contains one literal backslash before the filename. Doubling it in symbols.txt made the initializer's object comparison unmatched even though its bytes were exact. Preserve the actual symbol spelling from the compiled ELF, then regenerate the split and confirm the retained function scores 100%.
