# Particle and archive-font native tail probes, 2026-11-04

This isolated pass completes [ParticleManager Calc](329-particlemanager-tail.md) and [ArchiveFontBase GLGR](806-archive-font-registers.md). Both source integrations passed the coordinator's normal production build and frozen-artifact verification. Experiments remained in private outputs until the coordinator installed each exact source/header pair.

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

## Exact ParticleManager functions

The font constructor-output result suggested making the three post-field size loads into named caller outputs. Ninety-six ordinary int/u32 output-reference combinations retained 99.96706%. A further 168 enum-output combinations found the missing control: use a CurveOffset enum covering the complete u32 range, declare nameSize, rangeSize and randomSize in that order in the post-field case, write range and random sizes through references inside GetPostFieldNames, and write name size through a separate inline output helper after obtaining the names pointer. This keeps the three named values instead of anonymous expression temporaries.

The resulting 99.992676% candidate differs only in the order of the two source registers of the final random-table addition. Returning the integer cursor plus randomSize, rather than randomSize plus the integer cursor, makes all 1,366 instructions of the 5,464-byte Calc function exact. The sixteen other owned functions remain exact.

The readable final package, recompiled after formatting, is:

- `tools/local/tail-20261104/nw4r_ef_ef_particlemanager/pf_ready/ef_particlemanager.cpp`.
- The two existing `pf_include` headers listed above.
- `tools/local/tail-20261104/pf_particle_exact.patch`, containing the complete source and header delta against the coordinator's current files.
- `pf_ready/ef_particlemanager.o`, `compile.log`, `diff.json` and `data-audit.json` beside the source.

The original .text inventory is 8,964 bytes and all seventeen real function comparisons are 100%. The original .ctors, .bss, .sbss and 72-byte .sdata2 also compare at 100%. The 72 target .data bytes and all their relocation targets/addends agree, including every switch-table entry and the ParticleManager vtable. The source object appends a 16-byte weak ReferencedObject vtable with no relocation references, which makes the raw .data section score 90%.

The source object's .text extent is 9,156 bytes. Four pre-existing extra function emissions, also present in the initial source baseline, are absent from the owned original inventory: weak ReferencedObject SendClosing and DestroyFunc, plus RetireParticle and Draw_GetMtxPMtoEM. Object scores do not establish whether those extra emissions are discarded correctly. The coordinator must run the native source-link and frozen-artifact gate before source acceptance.

The enum output references, mixed pointer/integer cursor step, and emitter-resource same-type assignments are D-0016 code-generation accommodations. Hoisting emitter scale, correcting matrix declaration order and using the existing random/vector helpers are ordinary source corrections. No object patch or inline assembly is involved.

The coordinator's isolated ParticleManager source-link trial passed the frozen SHA. The first production build rejected the shared emitter-resource header controls: CEffect::Create and ef_emitter::CalcEmitter regressed from the frozen initial report and the DOL differed. The coordinator restricted those controls to ParticleManager with PARTICLE_MANAGER_EMISSION_LAYOUT and restored the original default accessor bodies. The particle-field header did not produce a reported regression.

The scoped production build then passed frozen verification and restored both regressed functions. ParticleManager is accepted from source. This full-build result also confirms that the extra unreferenced helper functions and weak vtable are discarded correctly. At this checkpoint the coordinator's full native report had nineteen remaining real functions and 88.035164% source-linked code; those are historical checkpoint values, not a claim that the overall decompilation is finished.

An earlier private audit appeared unchanged, but it compiled its default comparison after the coordinator had installed the candidate headers. It therefore compared the candidate header against itself, not against the pre-change header. Its output, `tools/local/tail-20261104/pf_ef_header_audit.json`, is not valid regression evidence. The production build results supersede that audit.

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

These controls did not justify retaining an artificial partial match. Neither canonical source nor canonical header was changed at this stage. Drivers use the prefix `tools/local/tail-20261104/pf_font_`.

## Exact font object after constructor-output control

The later candidate supersedes the bitfield experiment. No bitfields are needed. Add an overload `FontGlyphGroupsAcs(const void*, int&)` which performs the original constructor calculations, writes GetNumSheet to its output immediately after assigning mpData, then uses that output for mSizeSheetFlags and offsetSizeCWDH. The ordinary one-argument constructor is unchanged, so other callers keep their original inline body.

ConstructOpAnalyzeGLGR declares the int count immediately before pBlockEnd, calls the new overload, uses u16 glyphsPerSheet and u32 sizeAdjustTable, uses the integer-address RoundDown form, and explicitly narrows the count to u16 in the final memmove expression. The count must be declared immediately before pBlockEnd: leaving it just before the accessor construction produces 99.958336%, differing only in the pBlockEnd register at 0xD4 and 0x1B8. Moving it earlier than the data-size declaration changes other saved-register choices.

The exact isolated package is:

- `tools/local/tail-20261104/nw4r_ut_ut_ArchiveFontBase/pf_blockend4/ut_ArchiveFontBase.cpp`.
- `tools/local/tail-20261104/nw4r_ut_ut_ArchiveFontBase/pf_countoutinclude3/nw4r/ut/ut_ArchiveFontBase.h`.
- `pf_blockend4/ut_ArchiveFontBase.o`, `compile.log` and `diff.json` beside that source.

The direct native comparison reports all 25 real functions at 100%, including the 960-byte ConstructOpAnalyzeGLGR. The complete target .text extent is 5,432 bytes and compares at 100%; .data is 96 bytes and also compares at 100%. The gap symbols have no function score and are not counted as reconstructed functions.

The coordinator installed this header/source pair and ran the isolated source-link gate on 2026-11-04. It passed the frozen SHA with zero differing bytes, and the native report retained all 25 exact functions. The subsequent Matching promotion and normal production build also passed the frozen SHA. This is the full-artifact acceptance evidence beyond object scoring; original hashes were not changed.

The constructor-output API is an intentional D-0016 source accommodation after the documented ordinary-source attempts. The new int output preserves the named caller count instead of the constructor's anonymous cached-load temporary. Once that local survives, declaration order gives count and the earlier block-end pointer the same r24 without adding an instruction. The existing accessor still reloads sheet count in the loops, matching the target.

Additional follow-up probes included 124 by-value aggregate addition controls and 48 by-value cursor-parameter forms for Particle, plus 108 by-value count forms, 48 pointer-wrapper forms, 180 compound-assignment/intrinsic forms, 216 enum-range/enum-size forms, and 120 constructor-input forms for Font. None improved the previous closest candidates. The constructor-input experiments did recover count r24 and glyph r31 but reordered layout arithmetic. A 144-candidate constructor-output sweep then found the 99.958336% precursor, and six count-declaration positions found the exact object.

Scoped c9x_alias_by_type and c99_alias_by_type on/off controls did not change Particle, the Font baseline, the 99.791664% Font candidate or the constructor-input diagnostic. They do not explain the remaining Particle arithmetic allocation.

## Traps

The local trace wrapper initially copied configured compiler flags including repository `-i` arguments, then passed an extra include to ra_trace.py. The tracer appends its include arguments after GAME_FLAGS. The repository header therefore won even though the probe runner correctly searched the private overlay first. This affected early trace interpretation, not native probe scores. The wrapper now removes the configured `-i` pairs before asking ra_trace.py to add the overlay first. The corrected trace must identify the private font-header path in its compiler diagnostic, and its final instruction stream must agree with the corresponding candidate diff. Both closest candidates were retraced after the correction.

Full-width bitfield reads can change surviving virtual-register identity while preserving load instructions. This is a verified local code-generation control for this font function, not an ABI or original-source inference. It remains unaccepted until the function, all allocated sections, and the frozen final artifact compare exactly.

A cached inline load can eliminate a caller's declared local, making declaration-order probes ineffective. The font constructor output restores a named int count; the particle enum output references restore named size values where int/u32 outputs did not. Trace the surviving nodes before permuting declarations. The final font solution needs no bitfield representation change.

A live checkout is not a frozen comparison baseline during coordinated header integration. Here, the coordinator installed the candidate emitter header before the private auditor compiled its default comparison, hiding two real regressions. Use the preserved initial report or a complete original-header overlay from the pre-change revision. Do not treat an unchanged fresh-default comparison as evidence against the original build once shared files have changed.
