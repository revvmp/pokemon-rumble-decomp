# F-272-1: Personal-data binding interfaces

Original #272 is 25 numeric script bindings / 5324 instruction bytes at 0x8015A0E0–0x8015B624. All25 ordinary outside source bodies match native instructions, every applied relocation target, and1208 exception bytes. This interface prerequisite does not select the binding unit or claim issue completion.

## Complete pointee identity

The canonical CPiiPersonalData is the complete0x80-byte personal-data object established by F-173-3 and the existing native CEnemyInfo consumers. Its constructor is0x800EC3D0 and its original initialization filename is CPiiPersonalData_init.pkc. The bindings' shared owner from0x80100F70 reads that object's MaxHP at0x54, sex at0x14, level at0x20, bonus stats at0x24–0x30 and derived stats at0x58–0x68. Its WazaID methods dispatch the two IDs at0x18/0x1C. It is not a separate opaque PPD object.

F-195-1 explicitly called CPPD a low-confidence descriptive placeholder. CPpuEngine::SetPPD, mPPD and mCachedPPD now use the existing complete CPiiPersonalData type. Storage, virtual slots and behavior remain unchanged. The existing SetPPD source signature and native symbol at0x80100650 change to the correct pointee identity; the actual120-byte body and40EH bytes remain strictly native exact. No compatibility class, reinterpret cast or duplicate view is introduced.

The descriptive CPpuEngine::PopPersonalData member at0x80100F70 returns shared_ptr<CPiiPersonalData>. Its native caller supplies the return destination and engine pointer, then uses a strong owner and matching cleanup. The native producer checks cached IDs, locks the weak cache or resolves a personal-data ID and updates that cache. This prerequisite declares its interface; it neither reconstructs nor counts that #196 body.

## Numeric interface evidence

| Property | Existing member / offset | Getter implementation | Native setter |
| --- | --- | --- | --- |
| MaxHP | mMaxHP /0x54 | Inline field read | 0x800EC930 |
| BonusMaxHP | mBonusMaxHP /0x24 | Inline field read | 0x800EC970 |
| Sex | mSex /0x14 | Inline field read | None added |
| Level | mLevel /0x20 | Inline field read | 0x800EC980 |
| CappedLevel | mCappedLevel /0x68 | Inline field read | 0x800EC990 |
| AP,DP,SPD | mAP/mDP/mSPD /0x58,0x5C,0x60 | Inline field reads | 0x800EC9A0,0x800EC9C0,0x800EC9E0 |
| BonusAP,BonusDP,BonusSPD | mBonusAP/mBonusDP/mBonusSPD /0x28,0x2C,0x30 | Inline field reads | 0x800EC9B0,0x800EC9D0,0x800EC9F0 |
| BP | mBP /0x64 | Inline field read | 0x800ECA00 |
| WazaID | mMoveIds /0x18,0x1C | Out-of-line0x800EC7E0 | 0x800EC7D0 |

All new property method names and the PopPersonalData spelling are descriptive, following original binding names. This adds public methods to the already complete canonical class without changing its fields or their types. Setter declarations retain native work outside this issue; no guessed implementation is supplied. All formal types are32-bit int, and getters which only inspect existing fields are const.

Native behavior includes three details that the outside source preserves: MaxHP and BonusMaxHP setters push the submitted value on a valid owner and zero on a missing owner; GetSex yields2 when no owner exists; WazaID script selector32 chooses slot0 and every other value chooses slot1. The precise original spelling of that selector constant is unknown.

## Current controls

At base20cf0729b1, default configure/Ninja/frozen verification and all_source pass. The same-base candidate rebuilt all761 actual source objects:755 whole files are identical, five change only compiler numeric metadata, and CPpuEngine changes only three semantic identities: SetPPD120 bytes, weak_ptr destructor96 bytes and shared_ptr destructor132 bytes. Their allocated bytes, offsets, sizes, symbol attributes and raw relocation structure are unchanged. The comparison applies only those three explicit type-name aliases and numeric-label normalization, with no broad target/address normalization. Whole ELF allocated bytes and the frozen DOL are identical; ELF's only semantic symbol change is SetPPD's corrected name at the same address and size. No previously exact function loses its bytes or targets.

The actual canonical-header source probe matches all25/5324 code bytes and1208EH bytes, including every real call and cleanup target. The actual existing CPpuEngine.o also matches SetPPD120/40EH against native. Reproducible outside commands are272-reference/canonical-command.json,272-native.py,272-native-contract.py and272-contract-control.py. Receipts are272-reference/native-proof.json,contract-native-proof.json and272-controls/<base>/{raw-header-report,header-report}.json. The raw report explicitly preserves the three type-name differences rather than silently discarding them. Official local merge verification repeats these controls on each rebased head.

## Traps

- CPPD was a historical descriptive placeholder, not evidence for a distinct type. The native property readers and existing complete object establish the real pointee. Correct both cached ownership fields and the existing by-value SetPPD source contract together; do not introduce casts between duplicate declarations.
- Opcode-only comparisons missed first-draft script return behavior. The MaxHP setters must push their input, and the missing-owner sex result is2. These were corrected from actual native stores before the strict25-function audit.
- The survey's bind_7 label is an estimated grouping, not a recovered original filename. The next source acceptance must document its bounded numeric binding ranges and verify that it owns no hidden data or static state before selecting the source.


# F-272-2: Complete numeric binding source acceptance

`src/game/ScriptPiiStats.cpp` reconstructs every original #272 function:25 functions and5324 instruction bytes. This is a descriptive self-contained numeric binding module; no original filename marker was recovered. The survey bind_7 label is not asserted as an original file boundary. The exact bounded ranges are extab0x80031E68–0x80032320 (1208 bytes), extabindex0x8004F620–0x8004F74C (25 records/300 bytes), and text0x8015A0E0–0x8015B624 (5444 bytes including120 bytes of native function alignment).

All25 native functions reference only their engine/personal-data arguments and existing external functions. There are no owned SDA constants, RTTI, data objects, static state, zero storage or initializer records. The preceding #271 HP/damage/heal bindings and following #273 prefix/property bindings use the same public pointee but no hidden static ownership crosses these ranges. They remain outside this source selection and are not counted. The only extra weak function the compiler emits is the132-byte shared_ptr<CPiiPersonalData> destructor, whose existing earlier native definition at0x8006C5A0 supplies exception cleanup; its coalesced copy and40EH bytes do not extend the owned ranges.

## Original inventory

| Address | Bytes | Source function |
| --- | --- | --- |
| 0x8015a0e0 | 176 | `method_getMaxHP__FPQ24NPpu7CEngine` |
| 0x8015a190 | 264 | `method_setMaxHP__FPQ24NPpu7CEngine` |
| 0x8015a2a0 | 176 | `method_getBonusMaxHP__FPQ24NPpu7CEngine` |
| 0x8015a350 | 264 | `method_setBonusMaxHP__FPQ24NPpu7CEngine` |
| 0x8015a460 | 316 | `method_setWazaID__FPQ24NPpu7CEngine` |
| 0x8015a5a0 | 316 | `method_getWazaID__FPQ24NPpu7CEngine` |
| 0x8015a6e0 | 176 | `method_getSex__FPQ24NPpu7CEngine` |
| 0x8015a790 | 176 | `method_getLevel__FPQ24NPpu7CEngine` |
| 0x8015a840 | 228 | `method_setLevel__FPQ24NPpu7CEngine` |
| 0x8015a930 | 176 | `method_getCappedLevel__FPQ24NPpu7CEngine` |
| 0x8015a9e0 | 228 | `method_setCappedLevel__FPQ24NPpu7CEngine` |
| 0x8015aad0 | 176 | `method_getAP__FPQ24NPpu7CEngine` |
| 0x8015ab80 | 228 | `method_setAP__FPQ24NPpu7CEngine` |
| 0x8015ac70 | 176 | `method_getDP__FPQ24NPpu7CEngine` |
| 0x8015ad20 | 228 | `method_setDP__FPQ24NPpu7CEngine` |
| 0x8015ae10 | 176 | `method_getSPD__FPQ24NPpu7CEngine` |
| 0x8015aec0 | 228 | `method_setSPD__FPQ24NPpu7CEngine` |
| 0x8015afb0 | 176 | `method_getBonusAP__FPQ24NPpu7CEngine` |
| 0x8015b060 | 228 | `method_setBonusAP__FPQ24NPpu7CEngine` |
| 0x8015b150 | 176 | `method_getBonusDP__FPQ24NPpu7CEngine` |
| 0x8015b200 | 228 | `method_setBonusDP__FPQ24NPpu7CEngine` |
| 0x8015b2f0 | 176 | `method_getBonusSPD__FPQ24NPpu7CEngine` |
| 0x8015b3a0 | 228 | `method_setBonusSPD__FPQ24NPpu7CEngine` |
| 0x8015b490 | 176 | `method_getBP__FPQ24NPpu7CEngine` |
| 0x8015b540 | 228 | `method_setBP__FPQ24NPpu7CEngine` |

## Source and native behavior

The source uses the merged F-272-1 public contracts and ordinary shared ownership. Getters return their property, or the native missing-owner default. Setters pop their numeric argument, obtain the strong personal-data owner, call the existing setter only when valid, and return the script result. WazaID dispatches selector32 to slot0 and every other selector to slot1, exactly as the two native branches do. Numeric selector spelling remains descriptive/unknown; no unchecked index or guessed third slot is introduced.

No copied executable bytes, explicit template instantiations, assembly, opaque private layout view, changed compiler flags or forced register operations are present. Source names preserve the original binding table spellings. All class storage and member interfaces come from the canonical complete headers.

## Verification

The initial NonMatching split build and the subsequent Matching build both pass default Ninja's frozen DOL check. Unitdiff reports25/25 functions,5324/5324 code bytes at100%. The strict audit on the actual registered `build/wpse-v1/src/game/ScriptPiiStats.o` applies every relocation operand and proves all5324 code bytes, all actual targets and1208EH bytes/cleanup targets exact. A fresh mapped source link proves each retained function's native address, size and bytes; the Ninja input query selects only the source object, excluding the original object. All three retained allocated ranges, including every exception index and alignment byte, match frozen native bytes.

The complete default build, all_source and explicit frozen verifier pass. The source PR's official land wrapper repeats fresh same-base all-source controls, the full native/exception audit, and fresh source-only MAP acceptance after every rebase. Its immutable archive retains the actual source object, main.elf, main.dol, raw MAP, build.ninja and linker input query for independent review, in addition to JSON proof. The exact verified merge head is recorded by the official local land loop; no GitHub CI is used.

The interface prerequisite PR1210 merged as2c76819025, with exact verifiedhead3d807e4a692c57a648867a0a7221991a727299c3. It preserved all762 existing consumers at that head after only the three explicitly proved pointee-name aliases and numeric metadata. This source change adds one actual source object and uses those canonical interfaces without further header changes.
