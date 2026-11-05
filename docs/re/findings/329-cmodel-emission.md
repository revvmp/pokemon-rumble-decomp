# CModel native emission reconstruction

The three remaining functions are accepted as exact in the native build: root forced CModel.o regeneration, the native report showed all 158 functions exact, ninja exited zero, and frozen DOL verification passed with the unit still NonMatching. Initial native output was stale despite a later timestamp; four independent fresh compiles of the actual tracked source, including native relative paths and -MMD, established that the flags were not responsible.

The earlier source-link candidate was tools/local/session/329-model-emission/pairmarker0.cpp with header overlay implicitdtor0.h at include/game/CModel.h. That scratch directory was unavailable on the later native host. The direct runner is 329-model-emission.py; prepend the scratch include directory. audit-exacteh.o is the stable object snapshot. All 158 functions remain exact and all 156 nonempty target functions exist. At that stage, four order inversions remained.

## Recovered ordering

Moving Unload out of the header to an inline definition before Load places vector clear after AddResourceFile. Moving FindVis definitions after UpdateAnimations restores their ordinary method order. Moving the generic CreateAnimObj body to the end defers its eight instances to the target phase. Moving empty virtual callback bodies to the source preserves vtable declaration order while placing their code after LoadBounds.

Removing the explicitly declared/defined TPartialAnm destructor allows the native implicit virtual destructor to land at both original positions: true immediately after constructor cleanup helpers and false in the later implicit destructor wave. No replacement destructor bodies are required.

The three matched true methods can be explicitly instantiated under opt_optimizecontrolflow off and then restore on. This narrower form preserves the CreateAnimObj instances when those are deferred. Explicit instantiations of the four false methods, the AnmObj vector deleter destructor, and motion-tree insert_node_at place the first deferred wave without changing bodies or library headers.

An explicit instantiation of the original generic find_leaf<u32> after GetPartialAnm preserves its body and positions it correctly. An unused ordinary RequestModelMotionPairDestructorEmission helper contains the named-false pair-destructor request. Keeping this request outside find_leaf is required for exact exception metadata. The helper must be shown stripped during the eventual native link trial.

Named-false probes reproduce early list destruction, list front, playback-frame access, map iterator helpers, pair destruction, and vector iterator comparison. A sizeof seed for shared_ptr<AnmObj> before CModel.h restores that destructor's position. These are D-0016 emission accommodations; all are removed from executable caller instruction streams.

## Remaining order and EH

Target vector iterator inequality at rank 59 still follows reset<AnmObjChrOverride> rank 60 in the source. A local explicit inline inequality specialization instead moves it too early, before other dependency helpers. Named comparison result, volatile result in the dead probe, direct operator invocation, and early vector include do not fix the adjacent inversion.

The ChrRes shared_ptr_holder destructor at target rank 123 precedes the three move_ptr destructors at ranks 120 through 122. Early holder includes move the whole holder group; a partial specialization does not isolate the desired phase. Those experiments are not retained. Root's EH audit supports the inferred cleanup names, so no symbol rename is justified.

Root audited audit-exacteh.o: every comparable extab record and cleanup relocation matches the original. GetPartialAnm is now exactly 840 bytes. Full source linkage and data layout remain unaccepted.

## Exception metadata trap

A named-false pair-destructor call inside find_leaf adds an eight-byte GetPartialAnm unwind scope at caller offset 0x760, although all function instructions remain exact. Removing only that dead call restores the original 840-byte record. Explicit throw() on both declaration and definition, a throw() helper, literal false, const bool, and enum-false conditions do not suppress the inferred throwing state. A sizeof destructor expression avoids the unwind difference but also loses the emission request. Moving the request to an unused standalone function retains the desired pair destructor order and exact caller EH. No inferred-name changes were justified.

## Native section experiment

The initial six-section prototype remains scratch-only and is rejected. A late explicit specialization declaration with __declspec(section) moves the inline iterator inequality body, but analogous declarations for deferred non-inline generic destructors and tree.destroy suppress their bodies entirely. Explicit instantiation declarations preserve those bodies but ignore their section attributes. A code_type pragma placed before a definition does not move most functions already declared in a header; implicit constructors and member-template instances follow different timing. Original declaration attributes or carefully scoped specializations are needed before this can be a native-link candidate. The original overlay was restored after this experiment.

## Native reconstruction of the missing scratch candidate

On 2026-11-04, the earlier scratch directory was unavailable on the current PC. `tools/local/tail-20261104/model_restore.py` reconstructs the documented source and private header views from the tracked source. Its `game_CModel/restored_requests/CModel.o` compares all 158 real functions exactly, with a 156-function common ordered sequence. The only displaced functions are the vector iterator inequality versus `shared_ptr<AnmObjChrOverride>::reset` at ordinals 59/60 and the ChrRes holder destructor versus the three move-pointer destructors at ordinals 120â€“123. The explicitly requested motion-pair destructor remains in an unused standalone requester, keeping all 104 original exception records and their cleanup relocations exact.

The coordinator's first ordinary native source-link trial succeeded but produced 32 extra code bytes and 32 extra data bytes. The trial exposed a structural source error that per-function comparison did not identify.

## Empty callbacks belong to the inherited interface

The original `CModel` vtable contains its secondary base at offset `0x20`. The callback-B and callback-C slots at offsets `0x30` and `0x34` point directly to four-byte empty bodies at `0x801BCA70` and `0x801BCA60`. Only callback A needs an adjusting thunk, at secondary slot `0x2C`, and an additional primary-table entry at `0x38`. The original payload therefore ends at `0x3C`; its inferred 64-byte split symbol includes four bytes of trailing alignment.

The reconstructed class incorrectly declared B and C overrides. The compiler consequently emitted two additional eight-byte adjusting thunks and additional primary-table entries at offsets `0x3C` and `0x40`, giving a 68-byte table. Removing the two overrides produces the original 60-byte payload, direct inherited B/C slots, and only the observed callback-A adjusting thunk. All existing function instructions and all 104 exception records remain exact. The corrected identities are:

| Address | Original owner inferred from vtable |
| --- | --- |
| `0x801BCA60` | `nw4r::g3d::ICalcWorldCallback::ExecCallbackC` |
| `0x801BCA70` | `nw4r::g3d::ICalcWorldCallback::ExecCallbackB` |

This is an evidence-backed class and symbol correction, not a fake match. The private implementation is `game_CModel/inherited_callbacks` with `inherited_include/game/CModel.h`. The coordinator's `links/cmodel-inherited` native trial restores every executable section size and the exact DOL file length, while remaining emission and data differences still reject full SHA acceptance. The unit remains NonMatching.

## Literal emission is a separate remaining constraint

The first retained RTTI/vtable group must precede the `tree::insert length error` literal, which starts at original unit data offset `0x4B4`, immediately after the `CModel::BPartialAnm` type string. The animation-resource RTTI group follows it. The explicit `insert_node_at` instantiation that fixes the function's ordinal emits the literal too early, at source data offset `0x7C`. Removing that instantiation leaves the first metadata group correctly ordered but moves the literal after the animation metadata. The final ordinary trial differs in 1,187 `.data` bytes as a result of this ordering, even though `.data` has its original total size.

Bounded controls are retained as ignored scratch scripts under `tools/local/tail-20261104`: moving the tree definition out of the header without adding `inline` preserves the function and exception comparisons but does not fix literal order; adding `inline` changes `GetPartialAnm` and is rejected. An inline message getter prevents the required inlining and is rejected. A custom `data_type` pragma requires both initialized and uninitialized section names; leaving it active redirects all pooled strings, while restoring `.data` leaves the original ordering. It does not isolate this one literal. No compiled data was moved or rewritten.

## Additional traps

- A four-byte empty virtual body can have the wrong inferred class owner. Inspect secondary-vtable slots and adjusting thunks: the absence of B/C thunks and extra primary entries proves inheritance here.
- An explicit template instantiation can fix function order while moving its string literal ahead of metadata. Compare retained data order as well as code order before accepting the unit.
- Making generic ownership destructors explicitly inline preserves all 158 measured function bodies in this unit but changes their emission positions. This is a bounded observation, not a global header recommendation; no such header edit is retained.


## Delayed member-pointer request restores the literal

The independent data-emission investigation by `vm_properties_tail` removed the direct `insert_node_at` instantiation and introduced a dependent template that returns `&Tree::insert_node_at`. `CMotionProgram::Wait` discards the returned member-function pointer. The compiler removes the requester from the caller's machine instructions but requests the insertion body in its later generation pass. A simple depth-zero request is sufficient; deeper wrappers and other tested request locations did not improve the emission phase.

The candidate `tools/local/tail-20261104/game_CModel/model_literal_helper_0_wait` retains all 158 function bodies under the two corrected inherited callback identities and all 104 original exception records. The coordinator's native `links/cmodel-delayed-literal` trial restored all 1,784 retained CModel data bytes, including the literal at original unit offset `0x4B4`. The whole linked `.data` section compared exactly. The remaining 1,928 differing DOL bytes were confined to code order and exception references. This request is D-0016 emission scaffolding, not a recovered original use of the member-function pointer.

## Superseded three-source section candidate

A private three-source candidate reproduced the full 158-function sequence using native compiler sections. Its main compiler input keeps ordinary template bodies for inlining and exception analysis, and its data sections are byte-for-byte and relocation-for-relocation identical to the delayed-literal candidate. Small typed source fragments provide strong implementations for the methods whose section attributes the compiler otherwise ignores. The source implementations are compiled normally; no object bytes, executable bytes, or relocation records are rewritten.

The main source is `game_CModel/clean_main/CModel.cpp`, the ownership/animation helper source is `clean_helpers/CModel.cpp`, and the resource-copy helper is `clean_copy/CModel.cpp`. The private, scoped header proposal is `clean_include`. Its scopes are `CMODEL_EMISSION_LAYOUT`, `CMODEL_HELPER_EMISSION_LAYOUT`, and `CMODEL_COPY_EMISSION_LAYOUT`. `CModelAnimationTraits.h` gives the formerly local traits the shared descriptive namespace `ModelAnimation`, and `CModelAnimationFactory.h` shares the unchanged factory body across the primary template and its explicit emission specializations. Eight function symbol names consequently change namespace; `fragment_join/aliases.json` lists those diagnostic correspondences together with the two inherited callback corrections.

The compiled clean inputs have identical allocated-section bytes to the earlier `fragment_main`, `fragment_helpers`, and `fragment_copy` inputs. With their native code sections selected in target order, all 158 functions have the original order and all 104 exception records and cleanup relocations compare exactly. Objdiff's supported target-to-base symbol mappings pair every function; its only residual score difference is the eight renamed call relocations inside `UpdateAnimations`, which the function mapper does not also rename during relocation scoring. That remains a diagnostic result, not full-link acceptance.

A plain native `-r` join produces the expected diagnostic object, but the final Wii/1.1 linker rejects that joined custom-section object with an internal error in `LK_symbol.c`, line 2693. The coordinator then tested the three genuine compiled source objects directly in the final native link, using their unique CModel section names to interleave the code spans while preserving main-before-helper metadata ownership. No stale DOL from the failed link is evidence of acceptance. The unit remains NonMatching until a fresh native link and frozen-manifest verification pass.

## Section-control traps confirmed by these probes

- A `section` attribute on an ordinary class-template member works, but the tested member-template declarations and explicit instantiation attributes ignore it. Their generation request can select a native code section, at the cost of changing metadata timing. This is why the helper source retains typed bodies instead of forcing the main input's generation phase.
- A late declaration of an inline virtual template member suppresses its body in this compiler. Adding `inline` does not restore it. A helper-only declaration view plus a strong exact-body specialization preserves the main input's inference and native weak-definition ownership.
- Explicitly defining `ResMdl`'s copy constructor in the main input changes trivial-copy analysis and alters `Load`, `UpdateAnimations`, and the eight factory bodies. The isolated resource-copy fragment provides the same copy implementation while the main input retains the original implicit copy semantics.
- Late destructor declarations can change data even when every code body and unwind record remains exact. Three animation-control-block destructor declarations reversed the later vtable/RTTI wave. Leaving those declarations out of the main input and supplying their strong definitions in the helper restored the complete original data prefix.
- The generic [Power Architecture Build Tools Reference](https://community.nxp.com/pwmxy87654/attachments/pwmxy87654/cw-development-tools/8896/1/Power%20Arch%20Build%20Tools%20Reference.pdf) documents an `OBJECT(function, file)` linker command. The tested Wii/1.1, 1.3, 1.5, 1.6, 1.7, and 0x4201_127 linkers do not implement it. Wii/1.1 treats `OBJECT` as an input filename, and the documented two-argument syntax fails at its comma. The generic manual cannot justify assuming that feature exists in this pinned Wii toolchain.


## Complete isolated source link, 2026-11-04

The final four-source candidate passed the frozen `wpse-v1` main-DOL manifest through the native final linker. The coordinator's `tools/local/tail-20261104/links/cmodel-resource-guard/verification.json` records linker success followed by `PASS`: all 4,094,880 bytes match, with SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The link uses genuine compiled C++ objects. No assembler, copied original executable content, object rewriting, or relaxed comparison is involved. The coordinator subsequently integrated the sources and ran the normal configure/ninja pipeline: it exited successfully, passed the same frozen SHA, retained all 158 exact function comparisons, and introduced no project regressions. All 1,392 native-link aliases compared byte-for-byte with their compiler inputs. Clean-build repeatability is recorded by the coordinator in the shared validation documents.

The retained sources are `src/game/CModel.cpp`, `CModelOwnership.cpp`, `CModelResource.cpp`, and `CModelThunks.cpp`. Their final-link order is thunks, main, ownership, resource. Native code sections independently select the original function sequence. This leaves the retained exception records in their original input-object sequence. The plain native `-r` join remains suitable for objdiff analysis, but is not a final-link input because of the pinned linker's internal error described above.

The cleaned files were compiled under those actual basenames with scoped header views. Every nonempty allocated section, including its relocation sequence, matches the corresponding passing trial object exactly. `CModel.cpp` retains its anonymous animation traits, so no trait namespace renames remain. `CModelThunks.cpp` uses the supported `#pragma RTTI off`; it does not require a special compiler command. The coordinator also exercised the production linker-preparation CLI with grouped raw inputs and obtained the same frozen SHA before integrating the normal build.

### Early ownership and exception order

Keeping all eight `CreateAnimObj` member-template bodies in the main compiler input avoids the extra local vector-error literal and RTTI strings retained by the superseded three-source candidate. That rejected candidate had exact function addresses but added 96 DOL bytes. Its individual exception records also matched, while their placement did not. Per-record equality therefore was not sufficient.

The main input now supplies a typed Override `shared_ptr::reset` definition after `UpdateAnimations`. The animation-vector `reserve` and `push_back` declarations retain their wrapper signatures, and their identical typed wrapper bodies follow the reset definition. The FileData-vector `push_back` specialization remains inline in the header at the original request phase. This preserves all original caller instructions, all exception ranges, the vector cleanup order, and the complete main data emission. Scoped code sections place these early helpers at their original addresses. All 104 original exception records are reproduced, including their cleanup relocations.

A rejected control disabled exceptions only for helper bodies whose instruction bytes remained identical. The native link removed their exception records instead of selecting the discarded weak main copies, shrinking the DOL by 320 bytes. `links/cmodel-helper-eh-ownership` is failure evidence, not a usable build. Exception ownership follows the selected function definition in this trial.

### Synthetic resource guard

The remaining ChrRes cleanup helper at `0x801BFBF0` required a separate template declaration phase. The original guard type spelling is unknown; the earlier `std::tr1::detail::shared_ptr_holder` name was an inferred reconstruction. Under D-0016, `ModelEmission::ResourceGuard<T, D>` is a synthetic source emission type, not a recovered original name.

Its sole stored member is the same `Metrowerks::compressed_pair<T*, D&>`: a pointer and a deleter reference. Its constructor, `release`, and destructor perform the same operations as the inferred holder. A scoped type selection applies it only to the ChrRes custom-deleter constructor. Declaring this template before `move_ptr` places its 108-byte destructor after the three move-pointer destructors, as required by both code and exception-record order. It adds no RTTI, vtable, or retained data.

The synthetic symbol is `__dt__Q213ModelEmission64ResourceGuard<Q34nw4r3g3d12AnmObjChrRes,PFPQ34nw4r3g3d6G3dObj_v>Fv`. After this explicit identity mapping, all 158 selected functions have their original addresses and sizes; all 104 exception records, cleanup relocations, and their retained sequence match. The native map confirms the guard destructor at `0x801BFBF0`.

### Compiler-generated thunks and stripped requesters

The thunk fragment contains inline copies of the real CModel destructor and callback-A implementation. The main input's strong definitions own those methods; the first fragment supplies only the compiler-generated adjusting thunks. Its scoped declaration views prevent it from taking ownership of inherited callbacks, shared-pointer/vector cleanup bodies, or partial-animation destructors. The main input still sees ordinary bodies for inlining and exception analysis.

An unused requester originally constructed an empty `ResMdl`. This emitted a weak `ResMdl(void*)` body that stole a later unit's definition, adding 16 bytes at the end of CModel and shifting neighboring code. Accepting an existing `ResMdl` reference removed that unwanted request. The two inherited empty callbacks likewise had to be declarations in the thunk fragment so that the main input retained their earlier positions. The successful final map has no retained `RequestModel` helpers.

All named-false requests, template-size seeds, the discarded tree member-pointer request, scoped header declaration placement, explicit typed ownership fragments, synthetic guard, and native section scaffolding are D-0016 controls. The source bodies remain typed C++; the B/C inheritance correction is separately supported by the original vtable evidence and is not a fake.

### Final traps

- Exact instructions and exact individual exception records can still yield the wrong exception-table order. Audit the retained record sequence after weak/strong definition selection.
- An unused source requester can instantiate a weak body retained by another live caller. Check the full native map for accidental ownership, especially in a fragment placed before the main unit.
- Disabling exception generation for a strong helper does not preserve its discarded weak counterpart's metadata in the tested native link.
- A separate typed guard can control one cleanup emission phase without changing its pointer/deleter layout. Its synthetic identity must be documented and validated through the full frozen artifact, not presented as an original class-name recovery.
