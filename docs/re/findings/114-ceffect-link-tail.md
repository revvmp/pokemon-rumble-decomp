# F-114-5: CEffect bit-vector cleanup and source link

2026-11-04, target `wpse-v1`, Wii/1.1 with the configured game flags. This follows the already-documented `InitializeEffect` fake match and unresolved link in [sprint-a17](sprint-a17.md#fake-match-wave-d-0016).

## Destructor hierarchy and exception actions

The artificial `std::__bitvec_base` layer reproduced the three redundant null branches in bit-vector deletion but made the `AllocEffectGroupID` exception action reference its 100-byte destructor. The target references the 96-byte `__bitvec_deleter` destructor and has no separate `__bitvec_base` body. Removing the intermediate layer alone fixes that action but removes one required branch from `AllocEffectGroupID`, the shared-pointer deleter's `dispose`, and the corresponding `move_ptr` destructor.

Rejected scratch experiments included moving allocation into a constructor on `__bitvec_base`, implicit versus explicit intermediate destructors, empty secondary bases, empty destructor-body branches, and flattened storage hierarchies. None reproduced both the code and the cleanup target. Scratch results remain under ignored `tools/local/session/114-bitctor/`; no rejected header was applied to the shared tree.

## Documented fake

The retained bit vector inherits directly from `__bitvec_deleter`. A local explicit specialization of `std::detail::default_delete<std::vector<bool>>::operator()` in `CEffect.cpp` performs:

1. A null check.
2. An explicit vector destructor call.
3. Global `operator delete` on the same pointer.

This is equivalent to the former delete expression for this concrete, nonvirtual vector type, which declares no class-specific deallocation function. The explicit destructor call preserves the additional compiler-generated null branch without introducing an intermediate base class or a new cleanup function. It is recorded as D-0016 source shaping, not original-source recovery.

A repository search found `vector<bool>` consumers only in `CEffect.cpp`. The change therefore has no other known instantiated consumer, although the integrating agent still performs the real complete build.

## Validation

Before the independent initializer-signature correction, the isolated candidate scored **58/58 functions at 100%** and emitted only the 96-byte `__bitvec_deleter`, with no `__bitvec_base` symbol. The member-function specialization version retained all matches; after the initializer's second and third parameters were corrected to `u16`, only its old diagnostic symbol spelling was temporarily unpaired against the stale split object. The new source/header edits preserve that separate signature correction.

Complete source-link verification is performed by the integrating agent. An object match alone is not full acceptance.

## Traps

An extra destructor level can hide an incorrect exception-cleanup target. Here a direct delete expression and a null-checked explicit destructor plus deallocation are semantically equivalent but produce different redundant branch counts. Match both the normal path and the exception-table destructor reference before accepting an inheritance model.
