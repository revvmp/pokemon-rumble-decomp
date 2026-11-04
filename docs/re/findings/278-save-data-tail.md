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
