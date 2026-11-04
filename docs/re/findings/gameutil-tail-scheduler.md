# GameUtil preset initializer scheduling probes

2026-11-04, `wpse-v1`. No GameUtil source or build-configuration change was retained.

`InitBgmFxPresets` remains at 92.22632%. Its stores reproduce the intended preset values, but the target initially loads sixteen floating constants, while the current implementation loads twelve and introduces six more near the first chorus preset. The target later replaces three short-lived constants near preset five. This is a load-placement and allocation difference, not evidence of missing preset behavior.

Wii 1.0, 1.3, and 1.5 produce the same initializer score; Wii 1.0a also regresses `StageDesc::Init`. Const-reference float helper parameters regress to approximately 71?74%; double helper parameters make no change. Disabling instruction scheduling regresses to 65.70526%. Scheduler models 601, 602/603, 604, 7450, and generic do not improve the result; 7400 and 750 retain the baseline.

An experimental flattened implementation with separately named constant scalars and explicitly ordered volatile reads regresses to 88.14737%, uses another saved floating register, and does not reproduce allocation. The corresponding nonvolatile scalar variant reaches 89.88421%. Neither is a retained fake match. Earlier direct helper expansion attempts are recorded in [sprint-a17cx](sprint-a17cx.md).

Private scripts and outputs are under `tools/local/session/gameutil*`. They are diagnostics only; no full source-link trial was requested because no object candidate improved the retained source.

## Traps

Identical constant values and stores do not establish identical scheduling dependencies. Prescribing the load order with volatile reads changes virtual-register construction and saved-register requirements, so it is not a substitute for recovering the original expression dependencies.

## Exact initializer through two float lifetimes

A later 2026-11-04 pass resolves InitBgmFxPresets at 0x80125360, size 0x2F8. SetReverbParam names preDelayTime as a local initialized to 0.02f and then assigns it through static_cast<f32>. SetDelayParam similarly assigns its feedback parameter through static_cast<f32> before storing it. These same-type reassignments preserve the target floating-point virtual-register lifetimes while leaving every stored preset value unchanged.

The combination restores all sixteen initial literal loads, the three later replacements, all instruction scheduling, and every FPR operand. Direct compilation of formatted tracked source gives 13/13 exact GameUtil functions with 13/13 order, preserving the twelve previous exact functions. Evidence: tools/local/session/gameutil-ppu/accepted.cpp and accepted.json. This supersedes the earlier no-gain conclusion for the initializer, while leaving its failed-probe history useful.

These two redundant source reassignments are an intentional D-0016 fake match. Neither assignment performs a numeric conversion: each source and destination is already f32. Reassigning fusedTime and feedback first reached 97.23684%, restoring instruction order but choosing the wrong FPRs. Moving the first preserved lifetime to preDelayTime instead resolves the allocation. Reassigning only one value does not match.

The coordinating maintainer owns native Matching selection and full frozen-SHA validation; that acceptance is pending at this point.

## Source-link startup constructor correction

The first Matching trial exposed an extra 0x308-byte __sinit_GameUtil_cpp (0x310 including alignment), shifting the following DCIM code. The three shared NW4R effect-parameter headers supply default constructors. Their presence turned the zero-filled static preset records into dynamic initialization even though the target GameUtil has no corresponding startup constructor.

The coordinating maintainer approved a GAMEUTIL_STATIC_FX_PRESETS gate around only those default constructors in snd_FxReverbStd.h, snd_FxDelay.h and snd_FxChorus.h. Only GameUtil defines the gate, preserving every field and its type/layout while allowing native aggregate initialization. Other consumers keep the existing constructors. This scoped declaration view is documented D-0016 source shaping to reproduce the observed static initialization; it is not a global claim that the NW4R types lacked constructors.

A fresh direct compile of the tracked gated source preserves all 13 functions and their order. Its sBgmFxPresets storage is byte-identical to all 1,160 bytes of the original table, and it emits neither __sinit_GameUtil_cpp nor a .ctors entry. Evidence: tools/local/session/gameutil-ppu/static-accepted.cpp and static-accepted.json. Full native verification remains with the coordinating maintainer.

The coordinating maintainer subsequently confirmed a fresh native link and frozen SHA PASS with GameUtil selected Matching, accepted in cbb5f0e0. All thirteen functions and the static preset data are source-linked.
