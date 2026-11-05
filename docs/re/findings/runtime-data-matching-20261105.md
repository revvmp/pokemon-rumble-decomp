# Runtime and MSL object-data audit

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

The existing report measures 20,877 matched data bytes out of 21,705 for Runtime and MSL, or 96.18521%, despite all 107 units being source-selected. This audit distinguishes actual retained objects, native alignment, and compiler-generated material discarded during the final link. It does not replace the frozen executable verifier or change its expected hashes.

## Initial inventory

| Unit | Data bytes lacking object-level credit | Failing sections |
| --- | ---: | --- |
| Runtime/NMWException | 112 | extab, extabindex |
| Runtime/MWRTTI | 176 | .data, .rodata, .sdata, extab, extabindex |
| Runtime/Gecko_ExceptionPPC | 488 | .data, .rodata, extab, extabindex |
| MSL_C/strtold | 16 | .rodata |
| MSL_C/wstrtold | 16 | .rodata |
| MSL_CPP/msl_thread | 20 | extab, extabindex |
| Total | 828 | |

These are whole-section lost-credit amounts. They are not counts of wrong retained bytes. The baseline global report measures 1,245,840 matched data bytes out of 1,788,360, while its complete_data count is 1,788,360. The earlier [main acceptance](main-source-completion-20261104.md) explicitly preserved that distinction.

## INFINITY literal boundaries

Both scanner objects contain exactly the nine-byte sequence `INFINITY` followed by its terminating zero. The original split symbols at 0x80398990 and 0x803989A0 had size 0x10, incorrectly including seven alignment bytes each. The actual source ELF `.rodata` sections each have size nine and alignment eight. The following wide_ctype_data `.rodata` also has alignment eight.

The exact literal and alignment extents are:

| Owner | Retained range | Native alignment range |
| --- | --- | --- |
| MSL_C/strtold.c | .rodata 0x80398990–0x80398999 | 0x80398999–0x803989A0 |
| MSL_C/wstrtold.c | .rodata 0x803989A0–0x803989A9 | 0x803989A9–0x803989B0 |

The accepted proposal sets both existing literal symbols' sizes to 0x9 and preserves the original sixteen-byte split extents. No source declaration, literal value, compiler flag, frozen artifact, or data denominator changes. Explicit skip ranges were tested first but are unnecessary: objdiff correctly recognizes the padding once the actual symbol sizes are supplied.

An isolated native partial link used the accepted Wii/1.1 linker with `-nodefaults -fp hardware -r` and the unchanged strtold, wstrtold, and wide_ctype_data source objects. It exited zero and produced a 1,568-byte `.rodata` section with alignment eight. Its first 32 bytes are precisely the two nine-byte literals, each followed by seven zero bytes. This demonstrates that ordinary native alignment regenerates both gaps.

Copied symbols, splits, and config under `tools/local/runtime-data-20261105/` were processed by `dtk dol split --no-update` into a separate scratch output. Splitting exited zero and wrote 897 objects. An initial trial shortened the split sections and verified both literal payloads. The subsequent, smaller trial restored both original split extents and changed only the two symbol sizes. Both sixteen-byte `.rodata` sections then compare at 100% against unchanged source objects; their `.sdata2` sections remain at 100%. This recovers 32 bytes of diagnostic credit without changing the denominator. Shared configure/ninja and full frozen-DOL verification remain the coordinator's integration gate.

## Exception records and weak definitions

Direct objdiff JSON and extabindex relocation targets identify the additional source records below. Their corresponding records are absent from the original retained unit; removing legitimate source bodies merely to suppress their object-level data is not justified by this audit.

| Owner | Additional source exception-record owners |
| --- | --- |
| NMWException | __destroy_new_array |
| MWRTTI | __get_typeid, std::exception destructor, std::bad_typeid destructor |
| Gecko_ExceptionPPC | ExPPC_DestroyLocalArray, ExPPC_DestroyMemberArray, ExPPC_IsInSpecification, ExPPC_HandleUnexpected, std::exception destructor |
| msl_thread | std::tr1::detail::shared_ptr_deleter_common destructor |

NMWException's four retained extab records and four retained index records match in order and contents. Its source object additionally emits an eight-byte extab record and twelve-byte index for __destroy_new_array. The shared_ptr unit likewise retains the release record and emits an additional eight-byte extab record and twelve-byte index for its inline virtual destructor. The latter is not release_weak: release_weak is a real retained function at 0x8034D8C4.

MWRTTI's raw object emits additional bad_typeid and exception type names, base lists, vtables, RTTI, and what strings. Gecko similarly emits an additional std::exception family. The final native link selects the necessary definitions and discards duplicates or unused material. Anonymous original base-list names also differ from compiler-generated names. These facts explain why matching final artifact bytes do not imply raw input-section equality.

The existing inline exception definitions and library helper bodies are preserved. No source was changed by this investigation.

## Explicit metadata correspondences

The exception-record mapper pairs each original index record with the compiler record identified by the same function relocation, then pairs the exception body referenced by that index. Isolated real objdiff trials using those correspondences make all NMWException and msl_thread data sections 100%, despite the legitimate additional source records. No record is deleted or rewritten.

Two Gecko records, `@etb_800441B8` and `@etb_80044200`, have true size 0x12, while the inferred original symbols included the following two-byte native alignment and claimed size 0x14. Changing only those two symbol sizes in scratch, with the original extab extent unchanged, makes Gecko extab 100% under the verified mappings.

These additional explicit correspondences recover the retained type names, base lists, string pools, and jump tables:

| Unit | Original split symbol | Current compiler symbol |
| --- | --- | --- |
| MWRTTI | lbl_803982B8 | @308 |
| MWRTTI | lbl_803982C8 | @stringBase0 |
| MWRTTI | lbl_803E5D00 | @309 |
| Gecko_ExceptionPPC | lbl_80398328 | @1062 |
| Gecko_ExceptionPPC | lbl_8039833C | @stringBase0 |
| Gecko_ExceptionPPC | jumptable_803E5D18 | @278 |
| Gecko_ExceptionPPC | jumptable_803E5D5C | @772 |
| Gecko_ExceptionPPC | lbl_803E5DB0 | @1063 |

With these pairings, both units' `.rodata` and `.data` sections score 100%. The compiler-generated numbers are diagnostic identities for the currently compiled source, not recovered original names. Their source records are independently distinguished by literal contents or jump-table/base-list relocations.

The isolated trials leave only 116 bytes of section credit unresolved: MWRTTI extabindex (24 bytes, 80%), MWRTTI `.sdata` (eight bytes, 50%), and Gecko extabindex (84 bytes, 88.57143%). In each case the mapped record has moved within the compiler input section because earlier unretained records exist. The remaining relocation scores need investigation of the comparator's offset treatment; no false credit is awarded here. The successful mappings and four symbol-size corrections account for 712 of the initial 828 diagnostic bytes without source changes or denominator changes.

## Traps

- A nine-byte C string can appear as a sixteen-byte original split object when its symbol absorbs the next input's alignment. Inspect native source section alignment and demonstrate regeneration with the native linker. Correcting only the symbol size can restore the comparison while preserving the full split extent and denominator; here explicit skip ranges and padded source arrays are unnecessary.
- Extra extabindex entries must be identified through their function relocations. In msl_thread the extra record belongs to the inline virtual destructor, while release_weak is retained. Inferring ownership from neighboring source definitions would misdiagnose the gap.
- Plausible original inline and weak definitions can emit records that the final linker discards. Preserve the distinction between original executable bytes, raw compiler inputs, and retained output; a full-file match does not turn the raw-object diagnostic into 100%.
