# F-278-1: SaveData vector-copy helpers

Target: `wpse-v1`, `main.dol`, remaining SaveData functions. The 2026-11-04 baseline compiled 87 of the unit's 100 real functions identically. The five vector-copy constructors below were not emitted or paired in the baseline.

| Address | Recovered function | Size |
| --- | --- | --- |
| `0x8016B2E0` | `std::vector<SD_PiiPersonalData>` copy constructor | `0x204` |
| `0x8016B4F0` | `std::vector<SD_PiiCollectionData>` copy constructor | `0x1CC` |
| `0x8016B6C0` | `std::vector<SD_BattleRoyalData>` copy constructor | `0x1A4` |
| `0x8016B870` | `std::vector<SD_StatisticValue>` copy constructor | `0x1A4` |
| `0x8016BA20` | `std::vector<UsedPassword>` copy constructor | `0x1A4` |

The target's element sizes, vtable stores, copy fields, allocator calls and destruction paths identify these instances. Explicit instantiation initially produced their exact sizes but scores of 99.41–99.52%. The count occupied a different register, and the allocation/construction empty-tag slots were reversed.

The retained source specializes the five `__vec_constructor` copy constructors in `SaveData.cpp`. Each forwards its source range to `CopySaveVector`, which uses separate `AllocateSaveVector` and `ConstructSaveVector` inline forwarders. This reproduces the count register and both tag slots without changing the shared vector header. The explicit `SlotData` copy constructor, defined after the specializations, emits the five enclosing `vector` constructors. Each specialization retains four dead counter locals to cross the compiler inline-cost threshold; these produce no runtime instructions. These source-shape specializations are recorded as D-0016 fake matches, because the exact library layering has not been recovered.

Validation: direct compilation with the flags from the existing `build.ninja`, then objdiff-cli 3.8.2 against the original split object with the five evidenced symbol names, gave 92/100 real functions at 100%, preserving all 87 baseline matches. The coordinator rebuilt the integrated main branch and reported successful full `ninja` and frozen-manifest SHA verification. SaveData remains `NonMatching`; this is function reconstruction progress, not a source-linked unit completion.

## F-278-2: SlotData copy diagnosis

Controlled probes reproduced the previous three-part inline-budget experiment in [sprint-a03](sprint-a03.md#savedata-slotdata-chain-measured): scalar field widths, extra copy-constructor cost, and trivial-destruction dispatch. With target names mapped diagnostically, `DeserializeVector<SlotData>`, append, capacity, construction and append-reallocation helpers matched, but the generated `SlotData::Data` copy was 468 bytes instead of the target's 428. The unretained global vector changes also removed target erase helpers and changed two other functions in this unit, so they were not applied.

The target copy at `0x8016B130` uses serialized scalar loads/stores and no frame. A const-source generated constructor instead preloads many scalars and saves r23–r31. Giving `CMapRecord` a non-const copy constructor makes the compiler synthesize a non-const `Data` copy. An explicit `SlotData` copy constructor then forwards its `Data` member through a non-const reference. In this controlled probe the Data copy became exactly 428 bytes; separating the two counters into scalar members made its complete instruction stream exact. This demonstrates a non-const-copy route to the target, not evidence that the original `CMapRecord` had such a constructor.

An empty private copy-tag base can induce the same non-const generated signature without altering `CMapRecord`. A union storage view preserving all existing public array aliases compiled but changed the enclosing inline structure and regressed ResetSlot, construct-at-end and append-reallocation. Local vector-destructor specializations also changed their callers' instruction and stack structure. The final retained header uses the empty `SlotDataCopyTag` base and scalar fields. Nine consumers were mechanically updated with unsigned casts where old byte-array reads were unsigned. The shared vector implementation and CMapRecord remain unchanged.

Wii/1.0, 1.1, 1.3 and 1.5 produced the same problematic const-source Data copy; 1.0a and GC/3.0a5.2 differed more. Disabling pointer analysis, common-subexpression optimization, lifetimes, propagation, scalarization, aggressive load/store scheduling or C99 alias-by-type did not reproduce the target. Disabling scheduling removed the frame but left instruction-order and register differences. Volatile scalar members and full-width bitfields did not fix the compiler-generated copy.

## Traps

- A source file with an explicit `SlotData` copy constructor can instantiate its vector copy constructors before later specializations. Defining the SlotData copy only after those specializations avoids a redefinition error. Explicit vector instantiations after that definition can cause a second redefinition and must be reconsidered together with the source ordering.
- The shared vector destructor's apparent dead loops affect the inline budget. Replacing their dispatch globally can remove required out-of-line erase helpers and alter otherwise matching deserialization functions. Keep the probe isolated until all callers are checked.
- These experiments use copied headers and direct compiler output under ignored `tools/local/session/278-*`. They never rewrite the original binary, manifests, or expected hashes. The initial five-function change needed no header modifications; the final SlotData batch changes its header and nine mechanical consumers.

## F-278-3: All functions exact; source-link trial remains open

The integrated final source reproduces all 100 target functions. In addition to the copy shape above, TU-local `erase_at_end` specializations select trivial destruction for the five record types. The original inferred erase-tail names used a false tag; the true-tag names reproduce the same target instructions. The explicit SlotData copy passes its Data member through `const_cast<Data&>` to the compiler-generated non-const copy, without mutating the source. These constructs are D-0016 fake matches rather than recovered original source.

Load required a signed `GetSaveSize` inline forwarder and Unscramble local declarations ordered `i`, `blockOffset`, `pBlock`, `pDst`, `shift`. These affect register allocation only. The equivalent filename conditional was inverted to reproduce target branch orientation.

Evidence: direct compilation of the actual `SaveData.cpp` filename and main headers produced 100/100 exact target functions in `tools/local/session/278-verify/report.json`. The coordinator then ran the broad main build successfully and verified the frozen DOL SHA, with SaveData still NonMatching and no regressions among unchanged function identities. The initial 87/100 baseline therefore gains thirteen exact functions.

A subsequent Matching trial linked successfully but failed SHA (first difference DOL header offset 0x1E). The trial retains four additional weak vector-deleter destructors and reorders several weak getters, clear helpers, append helpers and exception records. Its final text displacement is +0x210. Source RTTI also occupies more small-data space than the original. The coordinator restored NonMatching. No source-linked SaveData completion is claimed; remaining work is emission, exception-table and data ordering.

The first four target vector-copy exception cleanups refer to already-emitted deleter bodies with different inferred template identities (including allocator-reference and nested-Data instances); the source uses its own four distinct deleters. This is evidence of shared identical bodies, but does not establish whether the original used linker folding. Output-only full-link probes with the exact configured Wii/1.1 linker and `-code_merging safe`, `all`, and `all,aggressive` all linked but did not recover the target. Safe merging shrank text to 0x2F65B4 and moved its start to 0x80050000 while retaining the four extra deleters; aggressive merging removed their symbols but changed many unrelated functions. No merging flag was retained.

## F-278-4: Complete function-order candidate

The later CGameSession exception-owner investigation corrected the inferred identities of four record-vector deleters at 0x800BAE60, 0x800BAED0, 0x800BAF40 and 0x800BAFB0. Current SaveData exception relocations resolve to these correctly named earlier functions, superseding the unresolved-identity diagnosis above. This correction does not imply use of global linker code folding.

The new direct-compiler candidate preserves all 100 function instruction streams and reproduces their complete order. It uses SAVEDATA_EMISSION_LAYOUT to declare the five GetDataSize and ClearData methods in the header, then defines them at their target emission positions in SaveData.cpp. The five trivial erase-tail methods are declared under the same scoped vector gate and defined as local inline specializations between the ClearData definitions. A non-inline specialization preserved order but changed 17 functions; the inline definition preserves all target instructions.

Six early sizeof probes request vector-deleter types in reverse target order, correcting the deferred append-method order. The final destructor swap is corrected by placing the complete shared_ptr_holder definition before move_ptr for this translation unit. The unchanged holder and reference helper definitions were extracted into MSL_CPP/detail/shared_ptr_holder.h; other translation units include them at their original location. Merely forward-declaring the holder, probing move_ptr size, or specializing the move_ptr destructor did not reproduce its required slot.

These source timing controls are intentional D-0016 reconstruction shims. They neither replace original instructions nor alter hashes. Ignored tools/local/session/278-ppu/ready.cpp and ready.json record the 100/100 function and order comparison; the shared-header candidate still requires the coordinator's broad build and frozen-SHA acceptance before SaveData may be marked Matching.

The complete native source-link trial succeeds and differs from the frozen executable in only 166 data bytes. All function addresses, executable instructions, exception sections, section sizes and small-data bytes are exact. The remaining difference is a 28-byte retained function-pointer type-name string at 0x803B7C10; the target places it at 0x803B7CA4, after four basic_string diagnostic/exception strings. These strings are not live instruction-reference differences in the final image.

Moving FormatWide's definition, deferring the generic get_deleter body, changing equivalent typeid operands, probing string methods from dead code, and method declaration pragmas did not change this data order. Explicit reserve instantiation moved its string before class RTTI instead and regressed three functions. These experiments are unretained; source-link acceptance remains open.

## F-278-5: Scoped RTTI emission shim

The final 166-byte discrepancy has a D-0016 source-only candidate. SaveDataEmission declares the existing C ABI object __RTTI__PFPv_v in its own namespace and uses that same metadata object for function-pointer deleter comparisons. Other deleter types retain normal typeid(D) behavior. This is a deliberate ABI reference, not a recovered original source declaration.

CopySaveVector takes unused addresses of small template emitter functions. Each emitter returns the normal compiler-generated typeid of a throw() function pointer. Their deferred instantiation places the compiler's retained type-name string after the basic_string diagnostic block, at the required data offset 0x8C4. All 100 target functions and their complete order remain exact. The unreferenced eight-byte emitter functions must be dead-stripped from the final executable; they are not original game functions and are not counted as reconstruction gains.

This shim is scoped by SAVEDATA_EMISSION_LAYOUT in shared_ptr_deleter.h. It does not construct RTTI bytes manually, substitute executable bytes, alter hashes, or change runtime comparison semantics. Direct compilation is recorded in ignored tools/local/session/278-ppu/rttiready.json; full native-link acceptance and helper removal remain mandatory.

## Final acceptance

The coordinator retained SaveData as Matching after a fresh native ninja build exited 0 and frozen main.dol verification passed. All 100 original functions now link from C++ together with their exact exception metadata, data and executable layout. Inspection of the accepted build/wpse-v1/main.elf with powerpc-eabi-nm found no EmitFreeDeleterTypeInfo or FreeDeleterType helper symbols: the emission probes are fully dead-stripped. Original hashes are unchanged.

The scoped declaration-order controls and RTTI emission shim remain intentional D-0016 fake matches. Their acceptance proves byte-identical reconstruction; it does not establish that the original developers used these helper declarations or emission probes.
