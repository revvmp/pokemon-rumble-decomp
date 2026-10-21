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
