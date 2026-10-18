# F-383-3: particle iterator forwarding and cooked view return

## Scope and ownership

This prerequisite changes the bodies of the existing const `DrawStrategyImpl::GetElderParticle` and `GetYoungerParticle` overloads. Each forwards to its existing nonconst overload after removing const qualification from the manager pointer. Those overloads only read the activity-list link; neither mutates the manager. A separately evidenced two-line correction makes `DrawInfo::GetViewMtxCooked` return its destination by const reference. Class layout, virtual methods, object registration, splits, and compiler options remain unchanged.

The first retained stripe function at `0x80242820` belongs to #383. The complete stripe file extends into #384. The #384 owner separately verified the existing smooth-stripe source and approved this shared prerequisite and subsequent coordinated source acceptance. The reviewed change does not take ownership of the externally assigned #385 bodies. The original #382 and #383 inventories remain incomplete.

The forwarding form is inferred source structure supported by the native instruction and relocation results below. It is not claimed to be uniquely recovered original spelling. The parent reviewed the concrete two-body diff and its readonly callees before publication.

Before the first global gate, the parent also reviewed and approved the exact two-line view-return expansion with the independent reference and consumer proof below. It changes the existing return type and adds `return *pMtx`; it introduces no competing overload or copied implementation.

## Current canonical inputs

The diagnostic control is main `95d4ee867cd60be6c9aac6048888789fc3485ed3`. The proposed header SHA-256 is `5db14c4314980052797bc825cfd7a964b1029d56650b5f9554eff45b727af36a`. `383-reference/header-95d4ee86-native/commands.json` preserves the actual generated Ninja compiler command. Both probes use the unchanged Wii/1.1 compiler and canonical NW4R flags.

The stripe probe compiles the complete ordinary C++ source retained by draft PR #908, with its owned class declaration and the proposed canonical include tree. The smooth-stripe probe compiles the existing canonical source with that same proposed include tree. Neither probe uses an alternative `DrawStrategyImpl` declaration, compiler pragma, or instruction-control flag.

The canonical `ActivityList::GetNumActive` correction from PR #917 is already present and is preserved. The earlier counter-access investigation in F-383-2 is historical evidence, not an additional hunk in this prerequisite. The canonical three-argument `CalcAhead_NoDesign` declaration is also preserved.

## Retained native proof

| Complete unit | Functions | Code bytes | Text relocation sites | Initialized data and constants |
| --- | --- | --- | --- | --- |
| Stripe | 21 | 13,908 | 369 | 80 bytes, including five vtable pointers |
| Smooth stripe | 28 | 20,196 | 513 | 28-byte vtable with five pointers and 52 constant bytes |

Every retained function size and non-relocation instruction word matches the frozen executable. Independently resolved call and data targets also match. The smooth-stripe BSS and SBSS extents are respectively 48 and 4 bytes. Its thirteen additional weak definitions are unretained diagnostic output and receive no source credit.

The decisive native origin callbacks are `DrawStripeStrategy::CalcAhead_ParticleBoth_Origin` at `0x80245C80` and `DrawSmoothStripeStrategy::CalcAhead_ParticleBoth_Origin` at `0x8024AC10`. Both 552-byte bodies match completely with the forwarding form. The expected canonical NonMatching change is the smooth-stripe origin callback becoming native-exact; this is not an assertion that all NonMatching objects stay unchanged.

The outside proof outputs are `383-reference/header-95d4ee86-native/stripe-paired-native.json` and `smooth-forward-paired-native.json`. They are produced by `383-header-current-stripe-paired.py` and `383-header-current-smooth-paired.py`; the latter independently decodes targets from the frozen executable and current canonical symbols. The #384 owner's separate `384-reference/smooth-forward-paired-native.json` confirms all 28 smooth-stripe functions, 513 text sites, vtable, constants, and storage extents.

A paired original-header control, compiled with the same command and only the canonical header restored through an isolated include overlay, emits 41 functions. Forty retain identical instruction bytes and relocation records. Only `CalcAhead_ParticleBoth_Origin` changes, from 564 bytes to the native 552 bytes. Its proposed instruction stream is independently verified above. `383-compare-smooth-control.py` records the complete function comparison and both body hashes in `383-reference/header-95d4ee86-native/smooth-original-control.json`. This bounds the expected smooth-stripe exception without excluding the rest of that object from review.

## Cooked view return evidence

The immutable native helper at `0x80240170` ends by returning the destination matrix address in `r3`. An isolated pointer-return experiment reproduced the complete 464-byte body, but did not distinguish the original pointer or reference spelling. A complete frozen-DOL direct-call scan finds only the view-space caller at `0x80240BA0`, which discards `r3`. That scan alone therefore did not justify choosing a C++ return type.

An independent [Pulsar DrawInfo declaration](https://github.com/MelgMKW/Pulsar/blob/820ad929c3c7141a0396692d8b0896d1546240fd/GameSource/core/nw4r/ef/DrawInfo.hpp) declares `const nw4r::math::MTX34& GetViewMtxCooked(nw4r::math::MTX34* dest) const`. The MIT-licensed reference is pinned to commit `820ad929c3c7141a0396692d8b0896d1546240fd`, blob `5ce0da790631e74c521518fe4522e1c2173888b6`, SHA-256 `edd4b574f9202aa5695fc897f09b7cf1fa2dbb8d47f537fdcfe1e8046c775214`. Only its signature supplies evidence; the existing canonical class and function body are retained, with the destination returned by reference.

The precise const-reference signature and `return *pMtx` reproduce all 464 native bytes and five independently paired relocation sites. The complete directional probe improves from 22 to 23 exact retained functions, 5,556 bytes and 152 text relocation sites. Its remaining `DrawDirectional` is 3,352 bytes against native 3,404 and remains NonMatching.

Same-command original/proposed controls for directional, free, line and point compare 57 emitted functions. Fifty-six instruction streams and canonical relocation records are unchanged. Only the directional weak view helper changes from 460 to 464 bytes. Numeric local labels are compared by section and offset rather than compiler-generated spelling. Separate controls compare all 69 emitted Stripe/Smooth functions unchanged; fresh strict paired audits also preserve their complete 49 retained functions, 882 text sites, ten vtable pointers and initialized payloads.

The outside outputs are under `382-reference/const-reference-return`: `provenance.json`, `caller-control.json`, `paired-native.json`, `stripe-smooth-consumers.json`, `stripe-paired-native.json` and `smooth-forward-paired-native.json`. Reproducers are `382-reference-return-proof.py`, `382-paired-reference-return.py` and `383-reference-return-consumers.py`. The helper improvement above belongs to the completed directional candidate, not a claim about present canonical object changes.

A separate same-command control compiles the actual current canonical directional source, which still lacks the recovered view-space and selector definitions. It emits only eight functions and no out-of-line cooked-view helper. All eight instruction streams and canonical relocation records remain unchanged by the return correction. `383-current-directional-return-control.py` records this distinction under `383-reference/current-directional-return-control/result.json`. The current global comparison therefore expects no directional object change beyond possible numeric local-name strings; only the smooth-stripe origin correction requires an allocated-code exception. Every existing Matching object must preserve its allocated bytes and relocations.

## Original inventory limits

Complete diagnostic joins read the original issue tables rather than inferred recut filenames. `383-audit-original-diagnostics.py` checks all 24 original #383 rows and 9,976 bytes: 23 rows and 7,820 bytes are individually exact, with only the 2,156-byte native Free draw still different. Six inherited accepted Line/Point rows and 2,748 bytes are not new work. Free's other three rows, 436 bytes, and its initialized tables, vtable and constants are independently exact against immutable accepted-source objects and the frozen executable.

With the reference-return candidate, `382-audit-original-diagnostics.py` checks all 26 original #382 rows and 10,072 bytes: 25 rows and 6,668 bytes are individually exact, with only the 3,404-byte native directional draw still different. Nine inherited accepted Billboard rows and 3,724 bytes receive no new credit. Its accepted object payloads and targets are checked independently; the complete directional source still requires its own source-selected acceptance.

These audits do not close either issue. Their original functions must reside in complete Matching translation units. The remaining directional draw therefore also prevents accepting its seven exact neighboring #383 rows in isolation; the free draw prevents complete Free-unit acceptance. Stripe's final source-selected gate remains pending separately. The outside outputs are `383-reference/original383-diagnostic-audit.json` and `382-reference/const-reference-return/original382-diagnostic-audit.json`.

## Acceptance boundary

The initial complete preservation gate verifies committed head `6d88a0d06cc6bd428e6eadb1ca1af849f1ae740b` after rebasing onto actual PR #919. The independently preserved control is `705972f8b0e61db58f5a3add1ff9e3331b85dae1`, whose complete tracked tree equals the accepted merge. Every raw control object, linked artifact and tracked-tree manifest hash was revalidated before use. A fresh output directory avoids retaining prior metadata or source objects.

| Actual command or comparison | Exit | Result |
| --- | --- | --- |
| `python configure.py --map --no-progress` | 0 | Fresh configured build |
| `.venv/Scripts/ninja all_source -j16` | 0 | All 551 declared source objects compiled |
| `.venv/Scripts/ninja -j16` | 0 | Actual LINK, DOL and frozen CHECK succeeded |
| Plain Ninja and explicit `all_source` repeats | 0 | Both report no work |
| Explicit `tools/verify/verify.py verify` with frozen manifest and reference | 0 | Frozen DOL SHA-256 unchanged |
| Complete 551-object preservation comparison | 0 | 544 whole files identical; six only numeric local-name strings; one bounded NonMatching correction |
| Six original-header recompilation controls | 0 | Each reproduces its exact old whole-object SHA-256 |
| Fresh complete retained native audits | 0 | Actual built Smooth 28/20,196/513; Stripe 21/13,908/369 plus five data pointers; completed directional 23/5,556/152 |

The six string-table-only objects are Billboard, Directional, Free, Line, Point and DrawStrategyImpl. All their section metadata, allocated bytes and relocation sections are unchanged. The sole allocated-code exception is the measured Smooth origin change, 564 to 552 bytes, with four fewer zero alignment bytes. Its other 40 emitted function bodies, initialized data and canonical nontext relocations remain unchanged. This preserves every existing Matching source output; it does not claim the NonMatching object unchanged.

All 15 allocated final ELF sections are also identical to the control. The whole ELF file has changed symbol-name metadata, so whole-file ELF identity is not claimed. The actual final DOL remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Complete commands, checked exit codes, raw comparison, original-header controls and native outputs are preserved under `383-reference/header-global-20261017-180457`. The operational driver is `383-run-header-gate.py`; it contains no merge action.

These header and diagnostic proofs do not establish source-selected Stripe/Smooth acceptance, a new Matching designation, issue completion, or measured progress. Subsequent rebases still require the exact-head current-main gates before merging. The supported no-progress build defers only the whole-project aggregate report; the actual report must be generated after merging, before the normal progress-image refresh.

After this prerequisite merges, coordinated PR #908 can register and verify the complete stripe source and accept the unchanged smooth-stripe source through its #384 owner. That source delivery must independently prove actual source-only link inputs, final native addresses, full frozen DOL identity, and the original #384 inventory of 25 functions and 18,856 bytes. Header publication alone carries no reconstruction credit.

## Incremental 577-object preservation checkpoint

After workflow update `ccfd461c`, the build directory was retained through metadata rebases. At committed head `77372015fca6f3a8210507a3c22d447b30bc2d2b`, configure, explicit `all_source`, default Ninja, explicit frozen verification, no-work repeats and all retained native audits exited 0. Only the prior `main.dol` was removed before Ninja.

The independent actual PR #897 control is `bef39d01891f7a6d20a3ca06c55faddc99f901f9`, whose complete tracked tree is identical to actual merge `b4205e4acd00b5129ec9ce0a99a092a918ee8853`. Its declared and present inventories both contain 577 source objects, and `ninja -n all_source` reports no work. Raw objects, linked artifacts and the tracked tree were captured independently under `383-reference/current-main-b4205e4a-control` and their manifest hashes revalidated.

The complete comparison passes: 570 whole objects are identical, the same six objects differ only in numeric local-name strings, and the sole allocated-code exception remains the measured NonMatching Smooth origin correction. Six same-command original-header controls reproduce the exact old whole-object hashes. All 15 allocated final ELF sections and the complete frozen DOL remain identical; whole-file ELF identity is not claimed. The proposed tree differs from this control only in the two reviewed headers and this findings file.

Commands, exit codes, native results, object comparison, original-header controls and linked-section confirmation are preserved in `383-reference/header-global-20261017-181659`. This is a historical checkpoint: later PRs #904 and #954 advanced main, so it does not establish the final latest-main merge gate.

## Traps

An older diagnostic header included a direct particle-count access to correct an earlier canonical accessor. PR #917 has since corrected that accessor itself. Reapplying the historical hunk would unnecessarily change canonical source structure. Start from current main and retain only the two iterator-body changes.

Individual native comparison can expose the exact intended NonMatching improvement without proving global header safety. Preserve the current all-source inventory and compare every existing source object; do not infer complete baseline coverage from an object-directory scan or claim all NonMatching objects unchanged.

The initial clean 551-object gate preceded workflow update `ccfd461c`. That historical build is not advice to discard outputs on later rebases. The current procedure preserves verified outputs and incrementally regenerates metadata; remove only the old `main.dol` before Ninja, then check its exit and explicit frozen verification. The original immutable control and complete gate evidence remain preserved independently.
