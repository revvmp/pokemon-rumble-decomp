# F-362-1: Native FA path unit and typed wildcard iterator

## Scope

The first three original #362 entries are PFFILE_cinsert/cdelete/combine at80211D10/11E10/11F10 and belong to the preceding file unit. This issue owns the path unit's first function, PFPATH_DoSplitPath at80211FE0. The complete path unit also supplies four original #363 tails at80214960/149C0/14CD0/14E50, ending80214EB8. These neighboring inventories remain distinct; no issue is complete without every original row's source-selected proof.

No path split, source selection or matching ledger row is published by this diagnostic checkpoint. Its complete-source audit rejects any differing function.

## Pinned references

The initial CC0 Petari VF source at e0ab141914921726a595573dc48d3f13d0922efe is a related revision with a smaller iterator and four-argument matching wrapper. It also contains a marked pointer-based fake match in AdjustExtShortName, which is not reused. Original FA names/file ordering are independently available from the pinned IPL f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c symbol and split maps.

A closer primary reference is the CC0 TFP2 source/header at b62dfe78759133a85bf5ce451bcf496b87439b0c:

- src/vf/fatfs/pf_path.c: SHA256 c382881aa7ac2760ba89e8e40ec04a97a481bdbf906c7d21d637838d3c2927f1.
- include/private/vf/PrFILE2/fatfs/pf_path.h: SHA256 a5333c81d530e8ce291758ee585511dd022c939c824e402108555b24604c2855.
- Root LICENSE: CC0, SHA25636ffd9dc085d529a7e60e1276d73ae5a030b020313e6c5408593a6ae2af39673.

This revision supplies the actual three-argument matching wrapper and complete16-byte PF_FILE_NAME_ITER: buffer pointer0, dot_inserted word4, is_long_name word8, index halfwordC. Native MatchFileName initializes these exact fields and recursive DoMatch copies all four words. The field4 name is retained from the matching reference layout; native code initializes/copies it but does not establish a new active dot-insertion behavior.

## Diagnostic progress

Ordinary C probes with unchanged established Wii/1.0 SDK flags reproduce all27 path functions, including every independently resolved relocation target and both initialized allocations:42-byte sdata payload at804B8890 and10-byte sdata2 payload at804BA5C8. Every function has the native size and instruction stream. The complete diagnostic audit exits0. This per-function proof is not yet an actual source-selected link or acceptance of either issue's original inventory.

The final three differences shared character conversion. An independent review of GetNextCharOfPattern established the ordinary constant-left comparison `'a' <= wc`, which preserves the target's separate range comparisons instead of the compiler's subtraction-based range fold. The same source expression in both short-name and long-name helpers reproduces the recursive and public matching callers completely. The pattern's two-byte OEM output also uses two explicit byte stores, with the zero-high-byte branch first, matching the target accesses rather than a borrowed halfword type-pun. These changes require neither an extra helper nor optimization flags.

The cleaned production candidate preserves every existing canonical public spelling and const input contract. It reproduces all11824 native function bytes and223 independently resolved relocation sites. Moving SplitPathWildcard into native definition order also reproduces the complete11992-byte text extent and every inter-function padding byte. Only text, sdata and sdata2 are allocated. Their actual extents are80211FE0–80214EB8,804B8890–804B88BA and804BA5C8–804BA5D2, with alignments16,8 and8 respectively; following alignment remains outside the source unit.

The bounded canonical header adds the matching-reference iterator and path predicates, plus the missing CheckExtShortName declaration. The native short-name copy routines return zero and therefore use s32 rather than the borrowed void declarations. ParseEntryIndex returns an unsigned boolean word. Existing const source parameters and all other public contracts remain intact. Existing-consumer preservation and the actual source-selected build remain required before integration.

A bounded consumer control compiles the actual existing pf_entry.c and pf_entry_iterator.c twice with the original and proposed path headers, holding the complete volume headers and established flags constant. Both controls preserve every section except numeric compiler-local names in strtab; normalized strings, complete code/data, symbol records and relocation records are identical. This independently checks the three return-contract corrections at their actual current consumers. It supplements the complete existing-object comparison required by the final combined build.

The native revision omits several guards present in the references. Each removal was checked directly against its native function. GetVolumeFromPath also omits the VF empty-length precheck, and both short-name copy directions omit reference overflow/empty-name error exits. The current-context and configuration field accesses use the independently proven complete volume proposal pending canonical integration, with no guessed offset casts.

A signed32 scalar argument in the private full-width conversion operation reproduces both cmpNameImpl and parseShortName exactly; the related reference's ushort scalar produces a different range-check stream. The short-name iterator's unsigned32 character intermediate is also measured against complete caller streams. These are local target/source findings, not permission to widen storage records. AdjustExtShortName's ordinary indexed loop matches without the other reference's pointer-to-pointer trick.

Wii1.0a/1.0RC1/1.1/1.3 and GC3.0a3/3.0a5/3.0a5.2 comparisons did not improve the full unit. The published TFP2 release profile also uses O4,p/ipa file/inline auto/cats off; its default inlining mode does not resolve the remaining mismatches here. No compiler override or optimization workaround is accepted by this checkpoint.

## Traps

- A related VF iterator's8-byte size is wrong for this target. Do not fill the gap with an unexplained padding word: the closer pinned reference independently identifies the complete16-byte record.
- A matching function size is insufficient. Five initial functions had native sizes but differing instruction streams; the whole path gate continues to reject mismatches and audits every relocation.
- Preserve native guard behavior and output widths independently of borrowed names. The target-specific signed32 character-operation argument is a source finding, while the iterator index remains16 bits.
- Primary source text containing full-width character literals needs an explicit encoding strategy when probing CodeWarrior. ASCII Unicode codepoint constants avoid accidental multibyte-source interpretation; code generation still requires complete native proof.
- CodeWarrior can produce different range-check streams for equivalent operand orders. Here the original-looking constant-left lower bound reproduces all three formerly differing character callers; do not compensate with flags or generated assembly.
- A halfword cast over a byte array can hide the original byte order. The native pattern routine explicitly stores the high and low bytes separately; preserve those accesses and verify its complete callers.
