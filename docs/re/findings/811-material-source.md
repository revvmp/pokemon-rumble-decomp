# F-811-2: Complete native Material translation unit

## Scope and ownership

The complete unit is reconstructed as ordinary C++ Material member functions,
its constructors and virtual methods, packed GX state initialization, resource
loading, matrix calculation, and animation-list operations. The complete
Material 0x5C header and related resource types were merged in prerequisite
#855. There are no competing layouts, byte-array instruction bodies, original
fallback inputs for this unit, placeholder bodies, or compiler-flag changes.

The complete original #811 inventory below contains 75 functions / 10,412
instruction bytes. Two preceding functions belong to retained owner #808:
CalcTextureMtx at 802ABD50 /264 and SetIndTexMtx at 802ABE60 /512. The same
contributor owns both claims and the complete native Material split. The
whole-file proof covers 77 functions /11,188 instruction bytes. #808 stays
incomplete; Layout and TextBox are separate coordinated deliveries.

## Native unit and source proof

| Section | Native range | Bytes |
| --- | --- | --- |
| .text | 802ABD50–802AEB30 | 11,744 including observed function alignment |
| .data | 803C71A8–803C7218 | 112 |
| .sdata2 | 804BB1C0–804BB1E0 | 32 |
| .sbss2 | 804BBC10–804BBC18 | 8 |

The two SetupGX selector tables are 32 bytes each, followed by the 48-byte
Material vtable. The constant pool contains .5, 256/360, zero, one, two and
the unsigned conversion double. The eight-byte zero-initialized signed color
is DefaultBlackColor. Its SDA2 reference proves .sbss2 ownership; omitting
it changes the small-data layout. All retained sections compare 100% in the
actual Ninja object, not merely an external diagnostic object.

Actual source-selected linking reproduces every native function address and
size, and the entire frozen main.dol. The independent native relocation audit
pairs all 221 text relocations with their original target addresses and all
10 vtable relocations, then compares all 152 allocated non-text bytes. There
are no inferred or unresolved relocation names. The link input includes the
compiled src/nw4r/lyt/lyt_material.o and excludes its original obj fallback.

The initial real-entry-point validation ran configure and default Ninja for
the unchanged baseline (0), split-only Ninja and explicit verifier (0), and
NonMatching source compilation plus fallback link and verifier (0). After
reconstruction, the actual source-selected Ninja main.dol target and explicit
frozen verification both exited 0. External 811-full-audit.py and
811-paired-native.py both exited 0. Final clean latest-main default-Ninja,
committed-head checks and exact guarded merge are recorded in the pull request.

## Provenance and outside signatures

The source adapts CC0 zeldaret/ss commit
57a709b2128eb163a18e2969d561c9abb7d1fcee, checked against CC0 xbret/xenoblade
ea25691b2029394ab777bda7060139e517a13cba. Original names and types are retained.
The real default constructor is independently present in CC0 Petari commit
e0ab141914921726a595573dc48d3f13d0922efe, src/nw4r/lyt/lyt_material.cpp:
it calls Init and clears the name. Its unretained body earns no byte credit.
Reference-only comments and the later revision's artificial float-ordering
helper and empty constructor are omitted. White color initialization and
comparison use ordinary component helpers without an extra retained constant.

Only names change outside the owned source range: TexMap::Get(GXTexObj*) at
802AEB30, derived math::VEC2 assignment at 800969A0 /48 and base math::_VEC2
assignment at 800969D0 /20. Their existing sizes, types and splits are retained.
A separate probe using unchanged canonical vector headers and the evidenced
game compiler context reproduces both assignment bodies. The derived native
call at 800969B4 independently resolves to the base function at 800969D0.
No vector overload or artificial signature change was introduced.

## Traps

- All 77 objdiff instruction streams can match while the DOL fails because
  implicit assignment functions appear in the wrong physical order. Removing
  original inline Material state setters postpones those definitions. The
  ordinary original APIs restored in #855 reproduce the complete native order;
  do not add dummy emitters, fake calls or an empty constructor.
- A scratch deferred-inlining flag did not repair this ordering. Existing
  cflags_nw4r flags remain unchanged. The resource constructor's bounded
  dont_inline pragma is reference-derived and its entire body/relocations
  are checked; the rest of the source keeps ordinary compilation.
- An implicit VEC2 assignment can look like the 20-byte base routine in a
  library-only probe while the real game-context wrapper is 48 bytes. Prove
  both native bodies and the wrapper-to-base call before changing a shared
  math declaration. Here the canonical declaration was already correct.
- DefaultBlackColor belongs in .sbss2, not .sdata2. Its actual r2-relative
  reference establishes the boundary; the next eight-byte zero object stays
  with its original owner.

## Complete original #811 inventory

Every row below passes native body, size, final address and source-selection
checks, with all original issue addresses covered exactly once.

| Address | Code bytes | Original name |
| --- | --- | --- |
| `802AC060` | 24 | `__ct__Q34nw4r2ut38LinkList<Q34nw4r3lyt13AnimationLink,0>Fv` |
| `802AC080` | 1636 | `__ct__Q34nw4r3lyt8MaterialFPCQ44nw4r3lyt3res8MaterialRCQ34nw4r3lyt11ResBlockSet` |
| `802AC6F0` | 36 | `__as__11_GXColorS10FRC11_GXColorS10` |
| `802AC720` | 12 | `__as__Q34nw4r2ut5ColorFRC8_GXColor` |
| `802AC730` | 12 | `GetTexMapNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC740` | 12 | `GetTexSRTNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC750` | 12 | `GetTexCoordGenNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC760` | 12 | `GetChanCtrlNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC770` | 12 | `GetMatColNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC780` | 12 | `HasTevSwapTable__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC790` | 12 | `HasAlphaCompare__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC7A0` | 12 | `HasBlendMode__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC7B0` | 12 | `GetIndTexSRTNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC7C0` | 12 | `GetIndTexStageNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC7D0` | 12 | `GetTevStageNum__Q44nw4r3lyt3res19MaterialResourceNumCFv` |
| `802AC7E0` | 20 | `SetWrapMode__Q34nw4r3lyt6TexMapF14_GXTexWrapMode14_GXTexWrapMode` |
| `802AC800` | 12 | `GetWarpModeS__Q44nw4r3lyt3res6TexMapCFv` |
| `802AC810` | 12 | `GetWarpModeT__Q44nw4r3lyt3res6TexMapCFv` |
| `802AC820` | 20 | `SetFilter__Q34nw4r3lyt6TexMapF12_GXTexFilter12_GXTexFilter` |
| `802AC840` | 20 | `GetMinFilter__Q44nw4r3lyt3res6TexMapCFv` |
| `802AC860` | 20 | `GetMagFilter__Q44nw4r3lyt3res6TexMapCFv` |
| `802AC880` | 36 | `__as__Q34nw4r3lyt11TexCoordGenFRCQ34nw4r3lyt11TexCoordGen` |
| `802AC8B0` | 36 | `__as__Q34nw4r3lyt8ChanCtrlFRCQ34nw4r3lyt8ChanCtrl` |
| `802AC8E0` | 36 | `__as__Q34nw4r2ut5ColorFRCQ34nw4r2ut5Color` |
| `802AC910` | 12 | `__as__Q34nw4r3lyt11TevSwapModeFRCQ34nw4r3lyt11TevSwapMode` |
| `802AC920` | 44 | `__as__Q34nw4r3lyt6TexSRTFRCQ34nw4r3lyt6TexSRT` |
| `802AC950` | 36 | `__as__Q34nw4r3lyt13IndirectStageFRCQ34nw4r3lyt13IndirectStage` |
| `802AC980` | 100 | `__as__Q34nw4r3lyt8TevStageFRCQ34nw4r3lyt8TevStage` |
| `802AC9F0` | 36 | `__as__Q34nw4r3lyt12AlphaCompareFRCQ34nw4r3lyt12AlphaCompare` |
| `802ACA20` | 36 | `__as__Q34nw4r3lyt9BlendModeFRCQ34nw4r3lyt9BlendMode` |
| `802ACA50` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt9BlendMode>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt9BlendMode` |
| `802ACA60` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt12AlphaCompare>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt12AlphaCompare` |
| `802ACA70` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt8TevStage>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt8TevStage` |
| `802ACA80` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt13IndirectStage>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt13IndirectStage` |
| `802ACA90` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt11TevSwapMode>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt11TevSwapMode` |
| `802ACAA0` | 8 | `ConvertOffsToPtr<Q34nw4r2ut5Color>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r2ut5Color` |
| `802ACAB0` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt8ChanCtrl>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt8ChanCtrl` |
| `802ACAC0` | 8 | `ConvertOffsToPtr<c>__Q34nw4r3lyt6detailFPCvUi_PCc` |
| `802ACAD0` | 8 | `ConvertOffsToPtr<Q44nw4r3lyt3res7Texture>__Q34nw4r3lyt6detailFPCvUi_PCQ44nw4r3lyt3res7Texture` |
| `802ACAE0` | 16 | `Min<Uc>__Q34nw4r2ut26@unnamed@lyt_material_cpp@FUcUc_Uc` |
| `802ACAF0` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt11TexCoordGen>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt11TexCoordGen` |
| `802ACB00` | 8 | `ConvertOffsToPtr<Q34nw4r3lyt6TexSRT>__Q34nw4r3lyt6detailFPCvUi_PCQ34nw4r3lyt6TexSRT` |
| `802ACB10` | 8 | `ConvertOffsToPtr<Q44nw4r3lyt3res6TexMap>__Q34nw4r3lyt6detailFPCvUi_PCQ44nw4r3lyt3res6TexMap` |
| `802ACB20` | 112 | `Init__Q34nw4r3lyt8MaterialFv` |
| `802ACB90` | 172 | `__dt__Q34nw4r3lyt8MaterialFv` |
| `802ACC40` | 1676 | `ReserveGXMem__Q34nw4r3lyt8MaterialFUcUcUcUcbUcUcbbbb` |
| `802AD2D0` | 8 | `GetTexMapAry__Q34nw4r3lyt8MaterialCFv` |
| `802AD2E0` | 8 | `GetTexMapAry__Q34nw4r3lyt8MaterialFv` |
| `802AD2F0` | 24 | `GetTexSRTAry__Q34nw4r3lyt8MaterialFv` |
| `802AD310` | 36 | `GetTexCoordGenAry__Q34nw4r3lyt8MaterialFv` |
| `802AD340` | 44 | `GetChanCtrlAry__Q34nw4r3lyt8MaterialFv` |
| `802AD370` | 52 | `GetMatColAry__Q34nw4r3lyt8MaterialFv` |
| `802AD3B0` | 60 | `GetTevSwapAry__Q34nw4r3lyt8MaterialFv` |
| `802AD3F0` | 68 | `GetAlphaComparePtr__Q34nw4r3lyt8MaterialFv` |
| `802AD440` | 76 | `GetBlendModePtr__Q34nw4r3lyt8MaterialFv` |
| `802AD490` | 84 | `GetIndirectStageAry__Q34nw4r3lyt8MaterialFv` |
| `802AD4F0` | 92 | `GetIndTexSRTAry__Q34nw4r3lyt8MaterialFv` |
| `802AD550` | 100 | `GetTevStageAry__Q34nw4r3lyt8MaterialFv` |
| `802AD5C0` | 60 | `SetName__Q34nw4r3lyt8MaterialFPCc` |
| `802AD600` | 144 | `SetTextureNum__Q34nw4r3lyt8MaterialFUc` |
| `802AD690` | 136 | `SetTexCoordGenNum__Q34nw4r3lyt8MaterialFUc` |
| `802AD720` | 292 | `SetTevStageNum__Q34nw4r3lyt8MaterialFUc` |
| `802AD850` | 196 | `SetIndStageNum__Q34nw4r3lyt8MaterialFUc` |
| `802AD920` | 440 | `SetColorElement__Q34nw4r3lyt8MaterialFUls` |
| `802ADAE0` | 3592 | `SetupGX__Q34nw4r3lyt8MaterialFbUc` |
| `802AE8F0` | 32 | `BindAnimation__Q34nw4r3lyt8MaterialFPQ34nw4r3lyt13AnimTransform` |
| `802AE910` | 8 | `UnbindAnimation__Q34nw4r3lyt8MaterialFPQ34nw4r3lyt13AnimTransform` |
| `802AE920` | 20 | `UnbindAllAnimation__Q34nw4r3lyt8MaterialFv` |
| `802AE940` | 120 | `Animate__Q34nw4r3lyt8MaterialFv` |
| `802AE9C0` | 52 | `AddAnimationLink__Q34nw4r3lyt8MaterialFPQ34nw4r3lyt13AnimationLink` |
| `802AEA00` | 8 | `FindAnimationLink__Q34nw4r3lyt8MaterialFPQ34nw4r3lyt13AnimTransform` |
| `802AEA10` | 8 | `FindAnimationLink__Q34nw4r3lyt8MaterialFRCQ34nw4r3lyt12AnimResource` |
| `802AEA20` | 76 | `SetAnimationEnable__Q34nw4r3lyt8MaterialFPQ34nw4r3lyt13AnimTransformb` |
| `802AEA70` | 76 | `SetAnimationEnable__Q34nw4r3lyt8MaterialFRCQ34nw4r3lyt12AnimResourceb` |
| `802AEAC0` | 112 | `GetTextureSize__Q34nw4r3lyt6detailFPQ34nw4r3lyt8MaterialUc` |

## Final current-main combined acceptance

PR884's clean current-main source head 058728c7 is based on actual merged
PR879 at 813a7c74. A fresh output directory, configure, default 548-target
Ninja, explicit all_source (541 source objects), and separate frozen
verification all pass. The actual native/source audit and actual-object
paired-target audit repeat successfully, including all initialized data,
final native positions and source-only link inputs. The independent
884-final-inventory-audit.py checks all original #808, #811 and #450 rows
against the final linked executable and frozen native code with no gaps
or duplicate credit. F-808-5 records the complete combined acceptance and
canonical inherited contributions. The existing PairFont diagnostic
limitation recorded in F-450-3 remains explicit.
