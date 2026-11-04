# GameUtil preset initializer scheduling probes

2026-11-04, `wpse-v1`. No GameUtil source or build-configuration change was retained.

`InitBgmFxPresets` remains at 92.22632%. Its stores reproduce the intended preset values, but the target initially loads sixteen floating constants, while the current implementation loads twelve and introduces six more near the first chorus preset. The target later replaces three short-lived constants near preset five. This is a load-placement and allocation difference, not evidence of missing preset behavior.

Wii 1.0, 1.3, and 1.5 produce the same initializer score; Wii 1.0a also regresses `StageDesc::Init`. Const-reference float helper parameters regress to approximately 71?74%; double helper parameters make no change. Disabling instruction scheduling regresses to 65.70526%. Scheduler models 601, 602/603, 604, 7450, and generic do not improve the result; 7400 and 750 retain the baseline.

An experimental flattened implementation with separately named constant scalars and explicitly ordered volatile reads regresses to 88.14737%, uses another saved floating register, and does not reproduce allocation. The corresponding nonvolatile scalar variant reaches 89.88421%. Neither is a retained fake match. Earlier direct helper expansion attempts are recorded in [sprint-a17cx](sprint-a17cx.md).

Private scripts and outputs are under `tools/local/session/gameutil*`. They are diagnostics only; no full source-link trial was requested because no object candidate improved the retained source.

## Traps

Identical constant values and stores do not establish identical scheduling dependencies. Prescribing the load order with volatile reads changes virtual-register construction and saved-register requirements, so it is not a substitute for recovering the original expression dependencies.
