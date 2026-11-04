# F-766-1: FA boot-record validator investigation

Target: wpse-v1. The original issue owns two retained functions, 612 instruction bytes, in the existing pdm_mbr.c split at 0x801FD360 through 0x801FD5C4. The compiler remains the established Wii/1.0 SDK configuration with default 16-byte function alignment. Original hashes, source selection and compiler flags are unchanged.

The unchanged baseline passed configure --map, plain Ninja and explicit frozen executable verification. The boot-sector checker remains byte-exact at 276 bytes. The master-boot-record validator remains NonMatching at 336 bytes: twenty instruction words differ in the first loop's byte loads, shifts, adds and register assignments between offsets 0x4C and 0x9C. All remaining native instructions, function size, branch structure, returns and both partition-array stack locations agree. This is partial investigation, with no new reconstructed-source credit or issue completion.

## Expression and structure evidence

Isolated probes use the exact generated Ninja compile command, without production flag changes, and compare the complete 336-byte function against the immutable DOL. Direct byte-expression permutations with independent start/count ordering reduce the disagreement to eighteen words. Readable little-endian read helpers produce the same eighteen-word lower bound. No candidate is byte-identical, and none has been applied to tracked source.

Negative probes include 480 same-order expression/cast/parenthesization candidates; 576 independent field permutations; 91 array-type, asymmetric helper and storage candidates; 192 helper argument-order, offset and return-value candidates; 150 byte-local, halfword-local and accumulation candidates; 120 start-expression trees; 120 count-expression trees; eight partition-byte pointer/array views; 64 loop-index/ABI-equivalent storage type candidates; 27 ordinary for/while/do-loop variants; and 12 typed partition-record views. A complete 14,400-case joint search combining all 24 term orders and all five binary-addition tree shapes independently for both fields also stopped at eighteen differing words; twenty equivalent condition-ordering variants did not improve that bound. Earlier symmetric helper probes that added an unwanted count-field zero store or removed the native start-field zero store are not valid 336-byte comparisons and do not establish a lower bound. Compiler-version diagnostics did not recover the target; those results do not justify a compiler or flag change.

Pinned public VF implementations in [Petari](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/vf/pdm_mbr.c), [wii-ipl](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RevoEX/src/vf/dskmng/pdm_mbr.c), [TFP2](https://github.com/koopthekoopa/TFP2/blob/b62dfe78759133a85bf5ce451bcf496b87439b0c/src/vf/dskmng/pdm_mbr.c), [pbr-decomp](https://github.com/pbr-ret/pbr-decomp/blob/2f00145d6b48689c2d182c409828b13cdf1e55f8/libs/RVL_SDK/src/vf/pdm_mbr.c) and [mscharged-decomp](https://github.com/yannicksuter/mscharged-decomp/blob/734c3f65dcb5b59f01fda2cf4aaab8249c2da13f/src/RVL_SDK/vf/pdm_mbr.c) were inspected. Their validator has a different disk/MBR-table interface and extra operations; they do not supply this FA body. The older wii-ipl right-nested unsigned byte-read macro emits 308 bytes when used as written, compared with the native 336. Its all-add counterpart stays 336 bytes with twenty differing words. These are reference leads, not proof of target source.

# F-766-2: second investigation of the boot-record validator

A second session reran the validator against the unchanged baseline with `tools/re/unitdiff.py`. The baseline still has 21 differing rows (89.23%), in the first loop between offsets 0x4C and 0x9C. Every candidate was compiled through the generated Ninja command for the existing split. No candidate matched, none was applied to tracked source, and no source credit is claimed.

The target loads the start bytes in the order 0x1C9, 0x1C8, 0x1C6, 0x1C7, and the count bytes in the matching order. That is the textual order of the public `PDM_U32_FROM_BUF` macro, `(b3 << 24) | ((b2 << 16) | (b0 | (b1 << 8)))`. Written with additions, that nesting compiles exactly like the current left-to-right sum, because the compiler reassociates it. With any `|` operator it emits `rlwimi` or `or` and changes the function size (308 to 348 bytes). Casting the operands to `u32` or `u16` changed nothing.

Negative probes, all at the native 336 bytes unless noted:

- Four local declaration orders crossed with three second-loop comparison forms: 21 to 39 rows. Swapping the array declarations swaps their stack slots and breaks the second loop.
- Moving the zero store, and staging both fields through scalar locals in seven statement orders: unchanged at 21 rows, or 348 bytes. The intermediate representation is canonical across those orders.
- Indexing by `i * 0x10`, `i << 4` or `0x10 * i`, and a separate partition pointer, either incremented or indexed: 34 to 60 rows.
- Accumulating the start field (`start_sector[i] = 0; start_sector[i] += ...`) explains the zero store, but it keeps the intermediate stores and emits 352 bytes.
- Writing the byte weights as multiplications (`* 0x100`, `* 256`, with or without `UL`) changes the schedule, unlike a reordered shift sum. All 576 start-by-count term orders with multiplication reach no better than 20 rows. Mixing shift and multiplication per term, with or without `<< 0` on the low byte, over all 24 shared term orders (384 candidates) reaches 19 rows, with orders `(b1, b0, b2, b3)` and `(b1, b0, b3, b2)` all shifts. That is still above the 18-word bound of F-766-1.

The remaining difference is the scheduler's choice between independent byte loads, not instruction selection. Changing the expression text alone does not reach it. A future attempt should vary context outside the loop body, such as the parameter order or the second loop and return structure, or should test whether the source unit was built with other flags. Any flag change needs separate evidence.

## Traps

- Score probes by the candidate's own instruction count, not the reported target size. `unitdiff.py` prints the target function size, so a candidate that adds stores can look like a 336-byte comparison unless the inserted rows are counted.

- Comparing only changed-byte counts can hide invalid source transformations. An output helper used for both fields introduced an extra count-array zero store; a return helper removed the native start-array zero store. Require the exact native function extent and preserve both stores before interpreting a probe's difference count.
- A library-relative name does not prove an identical API revision. The public VF validator has a disk parameter and an MBR structure, while the FA target accepts a total-sector count directly. Inspect native accesses and callers before adapting that reference.

# F-766-3: Context and scheduling diagnostics

A third investigation kept both source functions, interfaces and production compiler flags unchanged. The actual generated Ninja compile command was redirected only to issue-specific temporary input/output files. Direct ELF text comparison gives 20 unequal 32-bit instruction words in the 336-byte validator, with the 276-byte checker byte-identical. This raw word count differs from objdiff's 21 aligned diff rows; those measurements must not be interchanged.

The following additional diagnostics produced no exact candidate and no improvement over F-766-1's eighteen-word result:

| Diagnostic | Candidates | Result |
| --- | --- | --- |
| Ordinary C register hints on parameters and loop index | 16 | Same baseline text |
| Nested shift/multiply little-endian expressions with unsigned type variants | 24 | No improvement; native-size forms retain 20 unequal words |
| Both arrays grouped in one local struct, both field orders and three element types | 12 | 612-byte unit, 77 unequal words |
| Shared success/error exits, return temporaries and equivalent condition forms | 72 | Best native-size result remains 20 unequal words |
| Documented optimization levels and speed/space ordering | 9 | Best remains baseline; space selection produces 628-byte text |
| Scheduler, individual optimization and processor diagnostics | 14 | No improvement; disabling scheduling produces 628-byte text |
| Packed input words with endian conversion, offsets and const views | 18 | 596 or 708 bytes; different instruction selection |
| Paired inline partition readers, all parameter orders and output staging | 48 | Same baseline text |
| Equivalent zero stores and small memset diagnostics | 8 | No improvement; memset introduces additional code |
| Ordinary byte term order, independently of qualifiers | 24 | No improvement |
| IPA, inline policy and multiple-register-save diagnostics | 6 | Same baseline text |
| Materialized validation flags of four ordinary integer types | 12 | 644-byte text; unsuitable |

No flag diagnostic justifies a production configuration change. In particular, `-ipa off`, `-ipa function`, disabled inlining, deferred automatic inlining, `-use_lmw_stmw on` and `-opt all` all leave the baseline text unchanged. Processor 740 and 750 also reproduce the same baseline. None recovers the target's first-loop register graph.

The public VF bodies already surveyed in F-766-1 still have another interface. The target FA reader idiom remains unidentified; repeating byte-expression permutations or these context changes has no demonstrated benefit. The issue remains open with the original object linked and no additional source credit. The next useful evidence is the target FA implementation or an independently supported reader structure that produces the native graph.

## Traps

- **Aligned diff rows and native instruction-word differences are different metrics.** The current baseline has 21 aligned objdiff rows but 20 unequal native words, confined to offsets `0x4C` through `0x9C`. Compare exact extents and raw words before claiming improvement; preserve the separately exact boot-sector checker as well.
- **Compilation failure is not a negative binary result.** A volatile-parameter diagnostic conflicts with the existing public prototype and fails compilation. Its 24 attempted expression orders are excluded from the successful candidate counts above and establish no code-generation result. No volatile qualifier is added to production source.

# F-766-4: D-0016 selective volatile byte reads

2026-11-04: a direct probe using the unchanged configured Wii/1.0 flags reaches exact instructions for both functions. The master-boot validator remains 336 bytes and the boot-sector checker remains 276 bytes.

The retained D-0016 construct casts the buffer to `volatile u8*` only for the loads at `0x1C6`, `0x1C7`, `0x1CA`, `0x1CB`, `0x1CC`, and `0x1CD`. Loads at `0x1C8` and `0x1C9` stay ordinary. No load is added or removed, and all arithmetic, stores and control flow remain the same. This is a scheduling fake, not evidence that the original FAT input had volatile semantics.

A sweep of the 256 possible volatile subsets of the eight existing byte reads stopped at mask `0xF3`, which matches all instructions. Mask `0x03` reached 96.04762%, `0x33` reached 96.07143%, and `0x73` reached 99.5%; only the final exact candidate was retained. Earlier ordinary expression and compiler-context probes remain useful negative evidence, but they did not include this selective source-cast schedule constraint. Scratch source and complete objdiff output are in ignored `tools/local/session/mbr-compile/volatile-win.c` and `volatile-win.json`.

The maintainer marked the unit Matching, rebuilt successfully with Ninja, and verified the full DOL against its frozen SHA: PASS. Both functions are now source-linked and exact.

## Traps

- **Selective volatile reads can constrain scheduling without adding instructions.** The first-loop discrepancy was not solved by ordinary expression-tree permutations. Under D-0016, volatile qualification on six of the eight existing byte reads produces the exact schedule and register choices. This is a narrowly verified fake and must not be generalized to ordinary memory declarations.
