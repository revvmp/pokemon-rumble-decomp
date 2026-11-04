# CModel native emission reconstruction

The three remaining functions are accepted as exact in the native build: root forced CModel.o regeneration, the native report showed all 158 functions exact, ninja exited zero, and frozen DOL verification passed with the unit still NonMatching. Initial native output was stale despite a later timestamp; four independent fresh compiles of the actual tracked source, including native relative paths and -MMD, established that the flags were not responsible.

The current source-link candidate is scratch-only: tools/local/session/329-model-emission/pairmarker0.cpp with header overlay implicitdtor0.h at include/game/CModel.h. The direct runner is 329-model-emission.py; prepend the scratch include directory. audit-exacteh.o is the stable object snapshot. All 158 functions remain exact and all 156 nonempty target functions exist. Only four order inversions remain.

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
