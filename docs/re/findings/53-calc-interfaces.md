# F-53-4: native calculation and drawing interfaces

## Canonical interface and confidence

The bounded ICalc and ICalcDraw headers are exact copies of the #327 owner's approved diagnostic interfaces. They add no state beyond a virtual-table pointer. ICalcDraw inherits ICalc at offset zero. Native RTTI at 804B5AA0 names ICalc and has no bases; RTTI at 804B5B40 names ICalcDraw and identifies ICalc at offset zero.

| Vtable byte offset | Interface method | Evidence |
| --- | --- | --- |
| 08 | virtual destructor | Native derived vtable order and ordinary C++ inheritance |
| 0C | Calc() | Exact BUI loop and independent display target |
| 10 | PrepareDraw() | Exact BUI loop and default empty hook |
| 14 | DrawBefore() | Exact BUI loop and default empty hook |
| 18 | Draw() | Exact BUI loop and independent display target |

Calc takes no float argument. An early paired-single save instruction in the display was mistaken for an incoming f1 use; the actual body does not consume f1. The BUI loop reproduces the target with a no-argument virtual call, independently resolving that false lead.

ICalc and ICalcDraw are recovered original names. PrepareDraw and DrawBefore are descriptive project method names supported by caller behavior, not claims of recovered spelling. BWorld's first hook performs world/vertex/material calculation, its second dispatches pre-render callbacks, and its Draw performs view, gather, sort and draw passes. CMiniMap and CMsgBalloon consumers independently support the pre-render role. No private class layout or unrelated virtual slots are introduced.

The pure Calc and Draw declarations express the inferred interface abstraction. The I-prefix convention and native use support that model, but no retained base vtable establishes the original pure keyword. This inference is explicitly separate from the proven zero-offset inheritance and virtual call order. Constructors, destructor implementation identity and unrelated BUI source acceptance are not claimed by this header work.

## Canonical-only native consumer

The #327 owner supplied an independently expressed BUI diagnostic fixture and approved read-only reuse. The #53 driver compiles that fixture with the actual Game BController Ninja command and only the project include path. It does not use the #327 header overlay or any reference fallback. The complete BUI/container fixture remains outside the repository under its owner's control; none of those implementations is published here.

Seven BUI method bodies reproduce every native instruction byte with no instruction relocations:

| Address | Method | Bytes |
| --- | --- | --- |
| 801C8CE0 | GetManager | 36 |
| 801C90D0 | OnAttached | 4 |
| 801C90E0 | OnDetached | 4 |
| 801C9410 | Calc | 88 |
| 801C9470 | PrepareDraw | 88 |
| 801C94D0 | DrawBefore | 100 |
| 801C9540 | Draw | 100 |

These seven bodies total 420 bytes. The two ordinary empty base hooks each reproduce their four-byte native BLR, at 80063DC0 and 80063DD0. This is a diagnostic consumer proof, not credit or acceptance of the whole BUI unit.

The display independently uses vtable 8039A5A4: destructor8006BE20, Calc8006B7D0, PrepareDraw80063DC0, DrawBefore80063DD0 and Draw8006B770. The RTTI/base list, all five targets and both empty hook words are read directly from the frozen DOL rather than inferred from the fixture.

Header SHA-256 values are dc1fd714d122b82f6e4105c10ba7020226a03cbc84bfefb13a7817c798d0e139 for ICalc.h and ede3eb61be64021864da3d26c4b1e508f798b02dfae35a09f4771e039df63fef for ICalcDraw.h. They are byte-identical to the approved owner copies.

## Existing-source and build proof

The actual same-base control is main c8b460b0, including external PR900's complete HBMBase source. A pre-control snapshot taken before rebuilding that inherited source was rejected when its HBMBase object differed. The accepted control explicitly removes the two newly added headers, runs configure, all_source, plain Ninja and the frozen verifier, then restores the exact proposed headers and repeats all_source, plain Ninja and verification. All commands exit zero.

All 517 existing source objects are whole-file byte-identical between the accepted control and proposed headers. The complete ELF and DOL are also whole-file byte-identical. No existing production unit includes the new interfaces yet; the independent diagnostic directly consumes them. No object status, split, symbol metadata or source coverage changes.

The whole DOL matches the original frozen SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da. No expected hash changes.

## Rebased complete baseline

A fresh control/proposed build on `e70743b5fdbef570d06974387bfa4e40838e7d7d` contains 535 compiled source objects. Both configurations explicitly run `all_source`, plain default Ninja, and the actual-directory frozen verifier, all exiting zero. All 535 whole object files and the complete ELF/DOL remain byte-identical. The proposed headers are saved and restored as the exact committed copies, without depending on a mutable outside overlay.

`53-check-icalc-latest.py`, `53-icalc-latest-baseline.log`, `53-icalc-latest-baseline.json`, and the latest control ELF/DOL reproduce this checkpoint in `build-53-icalc-latest`. The canonical-only consumer compiler takes `build-53-icalc-latest/wpse-v1/src/game/BController.o` as its explicit Ninja target. Its compiler exits zero before both independent audits repeat the complete 428 native instruction bytes and display RTTI/vtable target proof. The final committed-head repeat and ancestry guard remain separate merge requirements.

## Accepted owner baseline on main 813a7c74

The next rebase inherits main `813a7c745d081a82bf86e3ef76d7fbd54fc02e68` without metadata or header conflicts. The earlier e707 object hashes differ for nineteen inherited outputs and are not used as this base's preservation control. Instead, the accepted PR879 owner snapshot at `a70262465efb08731e782639dfbb20d1056ac232` supplies its complete declared 535-object control and linked artifacts. An independent empty git diff and byte-identical `git ls-tree -r -z` snapshot prove that owner's verified tree is exactly actual main 813a7c74, including images. Every saved object and artifact manifest hash is independently verified before reuse.

Against that exact accepted control, all 535 proposed whole object files and the entire ELF/DOL are byte-identical. Configure, explicit all_source, plain default Ninja and the actual-directory frozen verifier all exit zero. The canonical-only consumer compiles zero before both instruction and RTTI/vtable audits repeat zero. `53-final-icalc-813.py` and its log reproduce this gate using the immutable owner snapshot copied under `315-reference/905-control-813a7c74`. This reuses an independently verified control without rebuilding it or inserting original/control bytes into the proposed build.

## Reproduction

The following files are outside the checkout in C:/MassStorage/GitHub/53-reference:

- 53-icalc-consumer.cpp, 53-compile-icalc.py and 53-audit-icalc.py: canonical-only compile and exact 428-byte native comparison.
- 53-audit-display-icalc.py and 53-icalc-display-proof.json: independent RTTI, vtable targets, header hashes and default hook words.
- 53-check-icalc-baseline.py and 53-icalc-baseline.log: accepted same-base control, explicit all_source, plain Ninja and frozen verification.
- 53-icalc-control-main.elf and 53-icalc-control-main.dol: retained-output controls.
- C:/MassStorage/GitHub/53-icalc-baseline.json: all 517 whole-object hashes.

No restricted implementation is imported. Header interfaces are independently expressed from native evidence; the owner-authored BUI fixture and standard-template experiments remain outside this pull request.

## Traps

- A generic virtual call at offset10 or14 does not identify an ICalcDraw method: ICalc-only classes can use those offsets for unrelated methods. Establish the RTTI/inheritance family before naming a slot.
- A paired-single save is not evidence of a float parameter. Inspect actual incoming register use and independently compile callers.
- Shared origin/main refs can advance between commands. Record the actual branch base and build its inherited source before capturing a same-base baseline.
- Header preservation gates must explicitly run all_source before capturing all source objects, even when a no-progress configuration is used. Plain Ninja and frozen retained-output verification remain separate requirements.
