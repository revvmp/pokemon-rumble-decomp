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
