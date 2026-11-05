# Particle and archive-font native tail probes, 2026-11-04

This isolated pass continues [ParticleManager Calc](329-particlemanager-tail.md) and [ArchiveFontBase GLGR](806-archive-font-registers.md). Neither unit is accepted from source. No shared header, source, configuration, original hash, or build output was changed by this investigation.

The coordinator owns configure, ninja and frozen-artifact verification. All comparisons below are direct native Wii/1.1 compiler experiments using the configured unit command and separate output paths. They are object-comparison evidence, not final-link evidence. The helper is `tools/local/tail-20261104/probe.py`.

## ParticleManager::Calc

The directly compiled current source baseline is 96.931915%. The missing older scratch candidate was reconstructed from the earlier findings. Its replacement package is:

- Source: `tools/local/tail-20261104/nw4r_ef_ef_particlemanager/pf_recovered_best/ef_particlemanager.cpp`.
- Private field header: `tools/local/tail-20261104/nw4r_ef_ef_particlemanager/pf_include/nw4r/ef/ef_particlemanagerfield.h`.
- Private resource header: `tools/local/tail-20261104/nw4r_ef_ef_particlemanager/pf_include/nw4r/ef/ef_res_emitter.h`.
- Native object, log and full diff JSON are beside the candidate source.

This reproduces 99.96706% for Calc. All sixteen other functions compare at 100%. There are seven differing instruction words, all register operands. The retained source remains unchanged.

The reconstructed source hoists the emitter-scale calculation before the inverse-matrix setup, reverses the final rotation-only matrix declarations, declares the track index immediately before the post-field pointer, and uses the documented explicit vector helpers. The random-field overlay uses HashCurveSeed and NextCurveSeed. The emitter-resource overlay retains the same-type size and index assignments described in the earlier findings. The gravity scale and field-space mode-3 scale must retain their original operators; making the mode-3 scale an explicit VEC3Scale adds six floating-point operand differences.

The closest post-field helper advances a byte pointer past key data, converts the pointer to u32 for the header-size addition, advances past range data, and returns random size plus the integer pointer. This is a source code-generation accommodation, not recovered original source.

| Calc offset | Target | Candidate |
| --- | --- | --- |
| 0x100c | add r4,r23,r0 | add r7,r23,r0 |
| 0x1010 | lwz r7,20(r23) | lwz r4,20(r23) |
| 0x1014 | addi r0,r4,32 | addi r7,r7,32 |
| 0x1018 | lwz r4,24(r23) | lwz r0,24(r23) |
| 0x101c | add r0,r0,r6 | add r7,r7,r6 |
| 0x1020 | add r22,r0,r7 | add r22,r4,r7 |
| 0x1024 | add r19,r22,r4 | add r19,r22,r0 |

The corrected allocation trace is `pf_recovered_best/trace.txt`. At allocation input, the key sum is virtual r82 and the cursor is virtual r49; these are separate nodes which both receive physical r7. The random-size and name-size loads are virtual r410 and r411. They receive r4 and r0 before the cursor is coloured. This distinguishes shared physical allocation from literal merging of the key-sum and cursor nodes. The desired target allocation would give the cursor r0, random size r7, and key sum/name size r4.

The following additional bounded probes did not improve the recovered candidate:

- 98 typed expression and cast combinations, plus sixteen volatile-read masks.
- 82 discovered per-function optimizer switches. The prior opt_prelinearize compiler failure was excluded from the Particle sweep.
- 72 aggregate return-value forms carrying both post-field pointers.
- 128 redundant control-flow forms around individual pointer steps.
- 31 full-width bitfield masks on the curve-header size fields.
- 128 wide-integer cursor and narrowing combinations.
- 128 void-pointer, byte-pointer and same-type assignment combinations, including the void-pointer assignment control independently proven by the SetupShadowMaterial agent.
- 72 scalarized cursor classes with copy-constructor, temporary-return and inline-step variations.

These are local negative results. They do not establish that the controls fail for other functions. Drivers use the prefix `tools/local/tail-20261104/pf_particle_`.

## ArchiveFontBase::ConstructOpAnalyzeGLGR

The direct current-source baseline is 99.291664%. A new private representation control improves the comparison: declare the existing adjacent fields as full-width `u16 numSheet : 16` and `u16 numCWDH : 16`. This leaves the accessed offsets unchanged in the compiled candidate and changes the compiler's value representation. It is a D-0016 candidate, not evidence that the original declaration used bitfields.

The bitfield-only candidate `pf_field4` scores 99.604164%. Combining that view with a u32 adjustment-table size and integer-address RoundDown produces the closest candidate, 99.791664%, with ten differing register operands and no instruction-structure differences. All 24 other functions remain at 100%.

The closest package is:

- Source: `tools/local/tail-20261104/nw4r_ut_ut_ArchiveFontBase/pf_bitcount252/ut_ArchiveFontBase.cpp`.
- Private header: `tools/local/tail-20261104/nw4r_ut_ut_ArchiveFontBase/pf_fieldinclude4/nw4r/ut/ut_ArchiveFontBase.h`.
- Native object, log, full diff JSON and corrected allocation trace are beside the candidate source.

The scalar sheet count remains u16; glyphs per sheet remains int. The candidate keeps count in r31 and glyphs per sheet in r24, while the target uses r24 and r31. An earlier block-end pointer is r28 in the candidate and r24 in the target. Those two early uses plus the eight count/glyph uses account for all ten differing words, at offsets 0xD4, 0x1B8, 0x1BC, 0x1D8, 0x1DC, 0x1E0, 0x214, 0x368, 0x398 and 0x39C.

In the bitfield-only corrected trace, the cached count is an expression temporary, virtual r118, coloured first into r31. The ordinary field view instead represents it as an earlier inline temporary. The new control therefore changes the compiler's surviving node, but does not yet give the target's named-local priority.

Additional probes, all discarded, include:

- 180 local count-bitfield forms, 24 reference-return accessor forms, and both baseline/type-variant sweeps of 82 discovered optimizer switches. The best option-only baseline was 99.379166% with opt_common_subs disabled and changed instruction scheduling.
- Scoped optimizer resets, 48 file-pointer wrapper forms and sixty accessor-object lifetime forms.
- 56 actual field-view/accessor-return combinations, which found the new full-width bitfield control.
- 336 count-type, adjustment-type, pointer-expression and assignment combinations, which found the 99.791664% candidate.
- 72 count-before-constructor and constructor-count-input combinations.
- 48 volatile/register qualifier combinations and 96 wide-count forms.
- Both glyph-type variants of the improved candidate under 82 optimizer switches.
- 144 pointer-typed count storage forms, 72 mixed layout/metadata accessor views, and 144 cached-count member placement forms.
- 48 accessor-factory and inline-depth forms.

These controls do not justify retaining an artificial partial match. Neither canonical source nor canonical header was changed. Drivers use the prefix `tools/local/tail-20261104/pf_font_`.

## Traps

The local trace wrapper initially copied configured compiler flags including repository `-i` arguments, then passed an extra include to ra_trace.py. The tracer appends its include arguments after GAME_FLAGS. The repository header therefore won even though the probe runner correctly searched the private overlay first. This affected early trace interpretation, not native probe scores. The wrapper now removes the configured `-i` pairs before asking ra_trace.py to add the overlay first. The corrected trace must identify the private font-header path in its compiler diagnostic, and its final instruction stream must agree with the corresponding candidate diff. Both closest candidates were retraced after the correction.

Full-width bitfield reads can change surviving virtual-register identity while preserving load instructions. This is a verified local code-generation control for this font function, not an ABI or original-source inference. It remains unaccepted until the function, all allocated sections, and the frozen final artifact compare exactly.
