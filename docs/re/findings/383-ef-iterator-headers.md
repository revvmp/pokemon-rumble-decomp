# F-383-3: const particle iterator forwarding

## Scope and ownership

This prerequisite changes only the bodies of the existing const `DrawStrategyImpl::GetElderParticle` and `GetYoungerParticle` overloads. Each forwards to its existing nonconst overload after removing const qualification from the manager pointer. Those overloads only read the activity-list link; neither mutates the manager. Signatures, class layout, virtual methods, object registration, splits, and compiler options remain unchanged.

The first retained stripe function at `0x80242820` belongs to #383. The complete stripe file extends into #384. The #384 owner separately verified the existing smooth-stripe source and approved this shared prerequisite and subsequent coordinated source acceptance. The reviewed change does not take ownership of the externally assigned #385 bodies. The original #382 and #383 inventories remain incomplete.

The forwarding form is inferred source structure supported by the native instruction and relocation results below. It is not claimed to be uniquely recovered original spelling. The parent reviewed the concrete two-body diff and its readonly callees before publication.

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

## Acceptance boundary

These are individual native proofs. They do not establish source-selected linking, a new Matching designation, issue completion, or measured progress. The proposed header still requires a current-main full existing-source comparison, actual `all_source` and default Ninja builds, and explicit frozen verification before it can be marked ready. Every existing Matching output must remain exact; any intended NonMatching difference must be identified precisely rather than omitted from the comparison.

After this prerequisite merges, coordinated PR #908 can register and verify the complete stripe source and accept the unchanged smooth-stripe source through its #384 owner. That source delivery must independently prove actual source-only link inputs, final native addresses, full frozen DOL identity, and the original #384 inventory of 25 functions and 18,856 bytes. Header publication alone carries no reconstruction credit.

## Traps

An older diagnostic header included a direct particle-count access to correct an earlier canonical accessor. PR #917 has since corrected that accessor itself. Reapplying the historical hunk would unnecessarily change canonical source structure. Start from current main and retain only the two iterator-body changes.

Individual native comparison can expose the exact intended NonMatching improvement without proving global header safety. Preserve the current all-source inventory and compare every existing source object; do not infer complete baseline coverage from an object-directory scan or claim all NonMatching objects unchanged.
