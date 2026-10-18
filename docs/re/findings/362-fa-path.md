# F-362-1: Native FA path unit and typed wildcard iterator

## Scope

The first three original #362 entries are PFFILE_cinsert/cdelete/combine at80211D10/11E10/11F10 and belong to the preceding file unit. This issue owns the path unit's first function, PFPATH_DoSplitPath at80211FE0. The complete path unit also supplies four original #363 tails at80214960/149C0/14CD0/14E50, ending80214EB8. These neighboring inventories remain distinct; no issue is complete without every original row's source-selected proof.

No path split, source selection or matching ledger row is published by this diagnostic checkpoint. Its complete-source audit deliberately exits nonzero while any function differs.

## Pinned references

The initial CC0 Petari VF source at e0ab141914921726a595573dc48d3f13d0922efe is a related revision with a smaller iterator and four-argument matching wrapper. It also contains a marked pointer-based fake match in AdjustExtShortName, which is not reused. Original FA names/file ordering are independently available from the pinned IPL f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c symbol and split maps.

A closer primary reference is the CC0 TFP2 source/header at b62dfe78759133a85bf5ce451bcf496b87439b0c:

- src/vf/fatfs/pf_path.c: SHA256 c382881aa7ac2760ba89e8e40ec04a97a481bdbf906c7d21d637838d3c2927f1.
- include/private/vf/PrFILE2/fatfs/pf_path.h: SHA256 a5333c81d530e8ce291758ee585511dd022c939c824e402108555b24604c2855.
- Root LICENSE: CC0, SHA25636ffd9dc085d529a7e60e1276d73ae5a030b020313e6c5408593a6ae2af39673.

This revision supplies the actual three-argument matching wrapper and complete16-byte PF_FILE_NAME_ITER: buffer pointer0, dot_inserted word4, is_long_name word8, index halfwordC. Native MatchFileName initializes these exact fields and recursive DoMatch copies all four words. The field4 name is retained from the matching reference layout; native code initializes/copies it but does not establish a new active dot-insertion behavior.

## Diagnostic progress

Ordinary C probes with unchanged established Wii/1.0 SDK flags reproduce24 of27 path functions, including all independently resolved relocation targets and both initialized allocations:42-byte sdata payload at804B8890 and10-byte sdata2 payload at804BA5C8. Every function currently has the native size. The incomplete functions are GetNextCharOfPattern, recursive DoMatchFileNameWithPattern and public MatchFileNameWithPattern; their inlined character-classification streams still differ. The complete gate remains failed, not partially accepted as matching source.

The native revision omits several guards present in the references. Each removal was checked directly against its native function. GetVolumeFromPath also omits the VF empty-length precheck, and both short-name copy directions omit reference overflow/empty-name error exits. The current-context and configuration field accesses use the independently proven complete volume proposal pending canonical integration, with no guessed offset casts.

A signed32 scalar argument in the private full-width conversion operation reproduces both cmpNameImpl and parseShortName exactly; the related reference's ushort scalar produces a different range-check stream. The short-name iterator's unsigned32 character intermediate is also measured against complete caller streams. These are local target/source findings, not permission to widen storage records. AdjustExtShortName's ordinary indexed loop matches without the other reference's pointer-to-pointer trick.

Wii1.0a/1.0RC1/1.1/1.3 and GC3.0a3/3.0a5/3.0a5.2 comparisons did not improve the full unit. The published TFP2 release profile also uses O4,p/ipa file/inline auto/cats off; its default inlining mode does not resolve the remaining mismatches here. No compiler override or optimization workaround is accepted by this checkpoint.

## Traps

- A related VF iterator's8-byte size is wrong for this target. Do not fill the gap with an unexplained padding word: the closer pinned reference independently identifies the complete16-byte record.
- A matching function size is insufficient. Five initial functions had native sizes but differing instruction streams; the whole path gate continues to reject mismatches and audits every relocation.
- Preserve native guard behavior and output widths independently of borrowed names. The target-specific signed32 character-operation argument is a source finding, while the iterator index remains16 bits.
- Primary source text containing full-width character literals needs an explicit encoding strategy when probing CodeWarrior. ASCII Unicode codepoint constants avoid accidental multibyte-source interpretation; code generation still requires complete native proof.
