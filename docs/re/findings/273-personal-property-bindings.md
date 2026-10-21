# F-273-1: personal-data property binding contracts

Target `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue273 owns24 original script bindings at8015B630–8015C9F4,4892 instruction bytes. The original names come from `docs/re/script-bindings.tsv`. The complete0x80-byte `CPiiPersonalData` layout already exists; F-173-3 identifies the fields and flags using these same original bindings. No class size, inheritance, storage, vtable or existing API is changed.

## Public dependency

The actual ordinary producer uses `CPpuEngine::PopPersonalData`, strong ownership, existing public VM operations and the existing `CPpuRegister` API. Its24 functions reproduce every original4892 instruction byte after applying actual relocations, plus1136 exception bytes and all cleanup targets. The matching register operations prove a real `CPpuRegister*` getter at offset0x4C; no private storage view or copied register representation is introduced.

| Public interface | Native evidence |
| --- | --- |
| GetPrice/SetPrice(int) | BindingB630 reads0x78; B6E0 calls800ECAE0, an8-byte store to0x78 |
| GetGroupNo/SetGroupNo(int) | B7D0 reads0x7C; B880 calls800ECA10,8-byte store |
| SetPrefix(int) | BA20 calls800ECA20; existing GetPrefix reads0x34 |
| IsBoss/SetBoss(bool) | BC00 extracts flag31; BB10 converts integer tobool and calls800ECA50 |
| IsBossHideHP/SetBossHideHP(bool) | BDC0 extracts flag30; BCC0 sets or clears exactly that bit |
| IsSelling/IsNewlyDisclosed | BE80/BF40 extract flag20/19 |
| GetRegister | C000/C120 add0x4C and call existing GetValue80111D20/Set80111D80 |
| SetOriginalTrainerId(u32) | C2E0 calls800EC650; getter reads0x48 |
| GetRandomSeed/SetRandomSeed(u32) | C3D0 reads0x38; C480 calls800EC6C0 |
| IsRareColor/SetRareColorRnd/SetNormalColorRnd | C570/C620/C6D0 call800EC700/740/750 respectively |
| IsInWiiRemote | C940 extracts flag17 |

Method spellings are descriptive translations of original script names and established members; original C++ mangled names were absent. Integer storage types follow the complete existing class: Price/GroupNo/Prefix areint, original-trainer ID/random seed areu32. The bool argument to SetBoss is proved by the caller's conversion. Source integration will name the nine evidenced outside methods while preserving every address, size, attribute and split; this dependency changes no native symbol binding.

## Current consumers

On same-base4108e2fa3f1ba8b3b5ff36fd859f3214f951ed7d, all763 actual source objects preserve all allocated section bytes, section attributes, complete relocation structure, and named symbol attributes.757 whole files are identical; six change only compiler numeric labels. No semantic alias is accepted. The prerequisite's official land wrapper repeats this same-base comparison, complete ELF allocated-byte equality, frozen DOL and actual24-function producer proof against canonical headers after every rebase.

## Traps

`CEngine::Pop` takes a real Slice reference; the register setter declares a Slice then calls Pop(value), matching the native two-word copy. A nonexistent by-value Pop overload is unnecessary and was not added. The register getter pushes the actual returned Slice; it does not convert it to an integer. This context reproduces both original register bindings on the first corrected API compile.

These are dependency and outside-producer proofs. Issue273 remains open until its complete actual source is registered, selected by the linker, and accepted against the frozen DOL with fresh MAP evidence.

# F-273-2: complete property binding source acceptance

`game/ScriptPiiProperties.cpp` reconstructs every original function of273 with canonical complete game types. The module filename is descriptive; no original filename marker was recovered. The survey's bind_7 start is an estimate. This self-contained property/register group has no private static state, shared data ownership or initializer tying it to preceding272 or following274. Those neighboring ranges remain outside this split. The accepted preceding272 group and this group use the same already-established public owner contract; no original first-owner class declaration is duplicated.

| Native address | Original script name | Code bytes |
| --- | --- | --- |
| 8015B630 | method_getPrice | 176 |
| 8015B6E0 | method_setPrice | 228 |
| 8015B7D0 | method_getGroupNo | 176 |
| 8015B880 | method_setGroupNo | 228 |
| 8015B970 | method_getPrefix | 176 |
| 8015BA20 | method_setPrefix | 228 |
| 8015BB10 | method_setBossFlag | 236 |
| 8015BC00 | method_getBossFlag | 180 |
| 8015BCC0 | method_setBossHideHP | 256 |
| 8015BDC0 | method_getBossHideHP | 180 |
| 8015BE80 | method_isSelling | 180 |
| 8015BF40 | method_isNewlyDisclosed | 180 |
| 8015C000 | method_ppdGetRegister | 276 |
| 8015C120 | method_ppdSetRegister | 268 |
| 8015C230 | method_getOyaID | 176 |
| 8015C2E0 | method_setOyaID | 228 |
| 8015C3D0 | method_getRnd | 176 |
| 8015C480 | method_setRnd | 228 |
| 8015C570 | method_isRareColor | 172 |
| 8015C620 | method_setRareColorRnd | 164 |
| 8015C6D0 | method_setNormalColorRnd | 164 |
| 8015C780 | method_setIsSpecHidden | 256 |
| 8015C880 | method_isSpecHidden | 180 |
| 8015C940 | method_isInWiiRemote | 180 |

The retained sections are extab80032320–80032790 (1136bytes), extabindex8004F74C–8004F86C (288bytes) and text8015B630–8015C9F4 (5060bytes including168 alignment bytes). There is no owned data, SDA, RTTI, BSS or initializer. The compiler's extra weak strong-owner destructor coalesces to the existing canonical native implementation; it supplies no original fallback for any of the24 owned functions.

Null owners produce0 for scalar/flag getters and a null Slice for the register getter. Setters always return the null script value and mutate only when the owner exists. The register setter pops the actual Slice before the numeric index; the getter pushes the returned Slice without integer conversion. Native bool conversion is preserved for the three flag setters. All source methods are ordinary public C++ operations, with no guessed storage view, flags, assembly, explicit template instantiation, fabricated byte arrays or instruction substitutions.

The initial NonMatching split and actual Matching link both pass Ninja's frozen DOL check. Unitdiff reports24/24 and4892/4892 code bytes at100%. A strict audit of the actual registered source object applies all relocation operands and proves4892 instruction bytes,1136 exception bytes and all actual callee/cleanup targets. A fresh mapped link proves unique source-object selection, each native address/size/body and all three complete retained allocated ranges, including exception indices and natural padding. all_source and the explicit frozen verifier exit0.

The header prerequisite1215 merged5c78dff7ff, exact verifiedbba039c4e6eaeec385a617ac079753e8712ffacd. It preserves all763 current source consumers and complete allocated ELF/DOL bytes. The source PR's official latest-main land loop repeats same-base consumer controls, actual native proof and fresh source-only MAP acceptance, retaining raw object/ELF/DOL/MAP/build.ninja and linker-query receipts for independent review.
