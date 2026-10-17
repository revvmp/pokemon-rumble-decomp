# F-296-1: Complete HBM inventory acceptance

## Scope and attribution

Issue #296 contains 40 surveyed entries and 5,396 instruction bytes at `0x8018C620`–`0x8018DC14`. All entries now reside in the existing canonical Matching Material, Pane and Picture source units. External PR871 supplies the final Material constructor correction; this issue's acceptance changes none of those implementations, splits or interfaces. It adds zero decompiled bytes and no ledger row. Existing ledger coverage remains counted once.

The final five Material entries total 528 bytes. F-295-1's earlier prose calling that tail nine functions is inconsistent with the original issue inventory. Pane's surveyed entries at `0x8018CC10` and `0x8018CC40` are four-byte return-instruction tails within the preceding 36-byte accessors. Their ranges are checked separately without inventing extra functions: the 40 entries occupy 38 real source functions.

## Exact original inventory

| Original address | Bytes | Canonical unit | Containing source function |
| --- | --- | --- | --- |
| `0x8018C620` | `120` | `material` | `Animate__Q36nw4hbm3lyt8MaterialFv` |
| `0x8018C6A0` | `52` | `material` | `AddAnimationLink__Q36nw4hbm3lyt8MaterialFPQ36nw4hbm3lyt13AnimationLink` |
| `0x8018C6E0` | `8` | `material` | `FindAnimationLink__Q36nw4hbm3lyt8MaterialFPQ36nw4hbm3lyt13AnimTransform` |
| `0x8018C6F0` | `76` | `material` | `SetAnimationEnable__Q36nw4hbm3lyt8MaterialFPQ36nw4hbm3lyt13AnimTransformb` |
| `0x8018C740` | `272` | `material` | `GetTextureSize__Q36nw4hbm3lyt6detailFPQ36nw4hbm3lyt8MaterialUc` |
| `0x8018C850` | `280` | `pane` | `__ct__Q36nw4hbm3lyt4PaneFPCQ46nw4hbm3lyt3res4Pane` |
| `0x8018C970` | `308` | `pane` | `__dt__Q36nw4hbm3lyt4PaneFv` |
| `0x8018CAB0` | `80` | `pane` | `AppendChild__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt4Pane` |
| `0x8018CB00` | `208` | `pane` | `GetPaneRect__Q36nw4hbm3lyt4PaneCFRCQ36nw4hbm3lyt8DrawInfo` |
| `0x8018CBD0` | `12` | `pane` | `GetVtxColor__Q36nw4hbm3lyt4PaneCFUl` |
| `0x8018CBE0` | `4` | `pane` | `SetVtxColor__Q36nw4hbm3lyt4PaneFUlQ36nw4hbm2ut5Color` |
| `0x8018CBF0` | `32` | `pane` | `GetColorElement__Q36nw4hbm3lyt4PaneCFUl` |
| `0x8018CC10` | `4` | `pane` | `GetColorElement__Q36nw4hbm3lyt4PaneCFUl` |
| `0x8018CC20` | `32` | `pane` | `SetColorElement__Q36nw4hbm3lyt4PaneFUlUc` |
| `0x8018CC40` | `4` | `pane` | `SetColorElement__Q36nw4hbm3lyt4PaneFUlUc` |
| `0x8018CC50` | `8` | `pane` | `GetVtxColorElement__Q36nw4hbm3lyt4PaneCFUl` |
| `0x8018CC60` | `4` | `pane` | `SetVtxColorElement__Q36nw4hbm3lyt4PaneFUlUc` |
| `0x8018CC70` | `172` | `pane` | `FindPaneByName__Q36nw4hbm3lyt4PaneFPCcb` |
| `0x8018CD20` | `184` | `pane` | `FindMaterialByName__Q36nw4hbm3lyt4PaneFPCcb` |
| `0x8018CDE0` | `768` | `pane` | `CalculateMtx__Q36nw4hbm3lyt4PaneFRCQ36nw4hbm3lyt8DrawInfo` |
| `0x8018D0E0` | `132` | `pane` | `Draw__Q36nw4hbm3lyt4PaneFRCQ36nw4hbm3lyt8DrawInfo` |
| `0x8018D170` | `124` | `pane` | `DrawSelf__Q36nw4hbm3lyt4PaneFRCQ36nw4hbm3lyt8DrawInfo` |
| `0x8018D1F0` | `140` | `pane` | `Animate__Q36nw4hbm3lyt4PaneFUl` |
| `0x8018D280` | `180` | `pane` | `AnimateSelf__Q36nw4hbm3lyt4PaneFUl` |
| `0x8018D340` | `28` | `pane` | `BindAnimation__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt13AnimTransformb` |
| `0x8018D360` | `144` | `pane` | `UnbindAnimation__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt13AnimTransformb` |
| `0x8018D3F0` | `24` | `pane` | `UnbindAllAnimation__Q36nw4hbm3lyt4PaneFb` |
| `0x8018D410` | `160` | `pane` | `UnbindAnimationSelf__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt13AnimTransform` |
| `0x8018D4B0` | `52` | `pane` | `AddAnimationLink__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt13AnimationLink` |
| `0x8018D4F0` | `120` | `pane` | `FindAnimationLink__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt13AnimTransform` |
| `0x8018D570` | `184` | `pane` | `SetAnimationEnable__Q36nw4hbm3lyt4PaneFPQ36nw4hbm3lyt13AnimTransformbb` |
| `0x8018D630` | `264` | `pane` | `LoadMtx__Q36nw4hbm3lyt4PaneFRCQ36nw4hbm3lyt8DrawInfo` |
| `0x8018D740` | `224` | `pane` | `GetVtxPos__Q36nw4hbm3lyt4PaneCFv` |
| `0x8018D820` | `8` | `pane` | `GetMaterial__Q36nw4hbm3lyt4PaneCFv` |
| `0x8018D830` | `16` | `pane` | `__sinit_\lyt_pane_cpp` |
| `0x8018D840` | `296` | `picture` | `__ct__Q36nw4hbm3lyt7PictureFPCQ46nw4hbm3lyt3res7PictureRCQ36nw4hbm3lyt11ResBlockSet` |
| `0x8018D970` | `208` | `picture` | `__dt__Q36nw4hbm3lyt7PictureFv` |
| `0x8018DA40` | `76` | `picture` | `Append__Q36nw4hbm3lyt7PictureFP10TPLPalette` |
| `0x8018DA90` | `368` | `picture` | `Append__Q36nw4hbm3lyt7PictureFRC9_GXTexObj` |
| `0x8018DC00` | `20` | `picture` | `GetVtxColor__Q36nw4hbm3lyt7PictureCFUl` |

## Validation

Actual main `7f00715bab47b7344354e9ff1c64a421fc4f3329`, including external PR871, passes configure, plain Ninja and explicit frozen main.dol verification in this agent's own #790 worktree. The native audit reads actual compiled source objects, the final linked ELF, the Ninja link edge and an independently preserved frozen-equivalent native ELF. Every original range has exactly one containing source function, exact instruction bytes after relocation and its native final location. All non-relocation source words also compare exactly. The 114 retained relocation sites are checked through their exact final relocated native instructions. All three Ninja inputs select compiled source objects, with no corresponding fallback object on the link edge.

The existing complete unit objdiff diagnostics are preserved rather than rounded: Material text 99.99373%, rodata/data 100%; Pane text 99.9641%, data 98.98039%, other sections 100%; Picture text 100%, data 80%, other sections 100%. These diagnostics include metadata/target-symbol comparisons. They do not replace the direct source-stream, native final-byte and frozen executable checks, which all pass. This audit does not relabel or modify the accepted units to alter those scores.

The combined source delivery repeats this audit alongside the independent #790, #791 and #812 inventories after canonical main `5ffa817d`. Its fresh 515-target plain Ninja build, explicit frozen verifier and all four native inventory audits exit zero. All 40 original ranges, 38 containing functions and 114 relocation sites pass again against the actual final source-selected ELF and frozen-equivalent native reference. The complete main.dol remains byte-identical to the frozen original. Frozen scope excludes the boot executable, data contents and WAD packaging.

The later external FA/EF PR878/883 rebases are also preserved; the repeated fresh 515-target build, frozen verifier and original-range audit pass on main `6965907b` with the same 40 ranges, 38 containing functions and 114 relocation sites. No HBM implementation or byte credit changes during those guards.

After external PR886 landed, the repeated fresh 516-target build and every original-range/native/source-input audit also pass on main `cc8923bd`, preserving the new upstream HBMBase unit. This remains an evidence-only #296 acceptance with zero additional byte credit.

## Constructor investigation and reusable lesson

Before external PR871, the unchanged main `77bf6f31` baseline passed its complete 505-target build while Material remained NonMatching. Isolated compiler, setter, copy-helper, memcpy and const-local probes reproduced the other 22 Material functions but left the constructor eight bytes long. No diagnostic source or header edit was applied to the repository.

PR871 establishes the missing source distinction: `res::Material::tevKCols` is an array of `ut::Color`, rather than `GXColor`. Its original `ToU32` copy controls the relevant value operations and scheduling. Ordinary resource-constructor texcoord initialization reuses its existing array pointer; an unretained original default constructor preserves the weak Color-constructor emission order. Those accepted canonical changes supersede the earlier unresolved scheduling hypothesis. No additional compiler flag is established by this investigation.

The inspected CC0 Swiftshine/key reference is commit `7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2`. SHA-256: LICENSE `36ffd9dc085d529a7e60e1276d73ae5a030b020313e6c5408593a6ae2af39673`; material source `6bcb186bc988fa5aeb5a1f9520f5c0c4a9083829054d7eb49c0164bc85e17b6d`; layout types `e991c2e9363ac6ea9e2078ceb67a1399c907bda7087b65be7566c44db7f5cc3d`; Color header `4cb64de2524a6b2ef05c1c202c12095231324b31c21dbfd0c29ae9b5d1a77118`. Existing implementation provenance and the accepted fix remain in F-295-3/F-295-4.

## Traps

An original survey may divide a real source function into a body and return-instruction tail. Check containment and every original byte range instead of requiring one source symbol per surveyed entry or counting the tail twice.

An incorrect resource field type can look like a compiler scheduling mismatch while preserving its size. Recover the original value/helper interface before changing flags or adding qualifiers. Here the accepted canonical Color field resolves the constructor without the diagnostic workarounds.
