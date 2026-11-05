# Game BSS data comparison audit

Final integration: the [complete 2026-11-05 acceptance](matched-data-completion-20261105.md) reaches 100% Matched Data and passes two complete clean builds. Intermediate remaining-work statements below describe their recorded checkpoints.

The 2026-11-05 report at 1,573,084 / 1,788,360 matched data bytes (87.96238%) still withheld BSS or SBSS section credit from twenty game units. An isolated metadata-only packet makes every affected section compare at 100% against unchanged source objects. No source, split extent, or data denominator changes are required. Full shared-build and frozen-executable verification remain the coordinator's acceptance gate.

## Evidence method

The audit first reads original split and compiled ELF symbol tables. Original BSS contents are not evidence for an identity: every object is uninitialized. Instead, it pairs references at corresponding instruction positions in the same named functions and verifies matching relocation types and effective symbol addresses. All 124 participating functions have identical instruction streams after masking only their architecture-defined relocation fields. Scalar and array sizes are checked against the actual compiled definitions and relevant source declarations.

The exact proposal is recorded in `tools/local/game-bss-20261105/corrections.json`; `relocation-evidence.json` records function names, instruction offsets, relocation addends, source sizes, and proposed original offsets. The packet contains 27 corrected inferred sizes, two additional local symbol boundaries, and sixteen explicit diagnostic mappings. Scratch config/symbol/split copies are processed by `dtk dol split --no-update`; real objdiff then compares each isolated unit with the unchanged production source object. Splitting and every comparison exit zero. `results.json` records all section results. These private files are reproducible diagnostic receipts, not production build dependencies.

## CRemoteMemory

The original inferred `sRemoteMemories` size was 32 bytes. Its actual declaration is `CRemoteMemory* sRemoteMemories[4]`, and its compiled storage is sixteen bytes. The next RemoteDataBlock array starts at offset 0x20 because of its real alignment. Six direct matched-function relocation pairs confirm the named array; further references use the compiler's section-base symbol. Correcting only this inferred size leaves the whole 9,928-byte section intact.

The anonymous twelve-byte object at 0x803E88A0 is the compiler's `@3560` destructor-registration node. Two matching relocation operands in the static initializer identify the same storage. An explicit mapping resolves its comparison identity. Both changes together raise the entire unchanged CRemoteMemory BSS section from 99.717970% to 100%.

## Corrected inferred sizes

Every row preserves the existing start address and split extent. These are symbol-boundary corrections, not declarations of new reconstructed storage.

| Unit | Symbol/address | Inferred bytes | Actual bytes |
| --- | --- | ---: | ---: |
| CDlgPiiList | spDefaultMotionPane__25@unnamed@CDlgPiiList_cpp@ at 0x804B8FD8 | 8 | 4 |
| CEffect | sNumUsedEffectGroupID__21@unnamed@CEffect_cpp@ at 0x804B8FF0 | 8 | 4 |
| CEffect | lbl_803E65F0 at 0x803E65F0 | 16 | 12 |
| CGameEnding | lbl_804B8FF8 at 0x804B8FF8 | 8 | 4 |
| CGameSession | lbl_804B9000 at 0x804B9000 | 8 | 4 |
| CGloballyReusableTexBuf | lbl_803E6600 at 0x803E6600 | 16 | 12 |
| CHPBar | lbl_803E6610 at 0x803E6610 | 16 | 12 |
| CIconCollection | lbl_803E6620 at 0x803E6620 | 16 | 12 |
| CPpuEngine | sDefaultStageDescTarget__10CPpuEngine at 0x804B9050 | 8 | 4 |
| CPpuEngine | sTemporaryIDReady at 0x804B9060 | 8 | 1 |
| CRemoteMemory | sRemoteMemories__27@unnamed@CRemoteMemory_cpp@ at 0x803E6660 | 32 | 16 |
| CSaveManager | @GUARD@max_size__Q23std139__deque_imp<Q326@unnamed@CSaveManager_cpp@12CSaveManager8WorkType,Q23std64allocator<Q326@unnamed@CSaveManager_cpp@12CSaveManager8WorkType>>CFv@n at 0x804B9078 | 8 | 1 |
| SaveData | sVectorTypes__12UsedPassword at 0x804B90C8 | 8 | 4 |
| SaveData | lbl_803E9900 at 0x803E9900 | 16 | 12 |
| NPpuVM | sInstances__Q24NPpu7CEngine at 0x804B90F0 | 8 | 4 |
| NPpuVM | @LOCAL@ToWString__Q24NPpu7CEngineCFRCQ24NPpu5Slice@sBuffer at 0x803E9930 | 80 | 64 |
| CFiber | sCurrent__6CFiber at 0x804B9120 | 8 | 4 |
| pad | lbl_803EA890 at 0x803EA890 | 16 | 12 |
| resetmgr | sDisableCount__Q29NResetMgr22@unnamed@resetmgr_cpp@ at 0x804B9150 | 8 | 4 |
| resetmgr | lbl_803EA8B0 at 0x803EA8B0 | 16 | 12 |
| errmsg | lbl_803EA900 at 0x803EA900 | 16 | 12 |
| homebutton | sOpenBlocked__24@unnamed@homebutton_cpp@ at 0x804B91D0 | 8 | 4 |
| sound | lbl_803EAF88 at 0x803EAF88 | 16 | 12 |
| sound | sBgmDisabled__19@unnamed@sound_cpp@ at 0x804B91F0 | 8 | 1 |
| sound | sBgmIndex__19@unnamed@sound_cpp@ at 0x804B91E0 | 8 | 4 |
| Message | lbl_803EAF98 at 0x803EAF98 | 16 | 12 |
| l10n_files | lbl_803E90D8 at 0x803E90D8 | 36 | 12 |

The source declarations distinguish four-byte pointers and integers, one-byte boolean/guard storage, and true arrays. In particular NPpuVM's wide conversion buffer is `wchar_t[32]`, totaling 64 bytes for this target, not the inferred eighty bytes. CSaveManager's deque initialization guard is one byte; its surrounding discarded source statics explain its different pre-link offset. The matching identity and actual size allow objdiff to compare it without removing those legitimate definitions.

## Destructor-registration identities

Anonymous twelve-byte BSS nodes are compiler-generated storage for the runtime's three-pointer DestructorChain: next, destructor, and object. Matched static-initializer relocations establish each identity; equal zero contents alone are not used. The confirmed diagnostic mappings are:

| Unit | Original symbol | Compiler symbol |
| --- | --- | --- |
| CEffect | lbl_803E65F0 | @13343 |
| CGameEnding | lbl_804B8FF8 | spDefaultMotionPane__25@unnamed@CGameEnding_cpp@ |
| CGameSession | lbl_804B9000 | sGameSession__26@unnamed@CGameSession_cpp@ |
| CGloballyReusableTexBuf | lbl_803E6600 | @1285 |
| CHPBar | lbl_803E6610 | @14607 |
| CIconCollection | lbl_803E6620 | @5293 |
| CRemoteMemory | lbl_803E88A0 | @3560 |
| SaveData | lbl_803E9900 | @3404 |
| pad | lbl_803EA890 | @3003 |
| resetmgr | lbl_803EA8B0 | @946 |
| errmsg | lbl_803EA900 | @7474 |
| sound | lbl_803EAF88 | @6325 |
| Message | lbl_803EAF98 | @1439 |
| l10n_files | lbl_803E90D8 | @1818 |
| l10n_files | sFileSetsDestructorRegistration__24@unnamed@l10n_files_cpp@ | @1819 |
| l10n_files | sArchivesDestructorRegistration__24@unnamed@l10n_files_cpp@ | @1820 |

The l10n_files range at 0x803E90D8 is three consecutive registration nodes, not a single 36-byte object. Its exact static initializer loads that base and passes offsets 0, 12, and 24 to `__register_global_object` for sPaths, sFileSets, and sArchives, respectively. The runtime stores exactly three pointers in each node. The compiler emits `@1818`, `@1819`, and `@1820` at those offsets. The packet shrinks the existing first symbol to twelve bytes and adds local boundaries at 0x803E90E4 and 0x803E90F0, with descriptive FileSets/Archives registration names. All 36 bytes remain counted, and the BSS section scores 100%.

The CGameEnding and CGameSession mappings identify actual four-byte pointer statics through matching load/store relocations. They are distinct from generated destructor nodes.

## Section-credit result

| Unit | Previously withheld BSS/SBSS bytes | Isolated corrected score |
| --- | ---: | --- |
| CDlgPiiList | 8 | 100% |
| CEffect | 32 | 100% |
| CGameEnding | 8 | 100% |
| CGameSession | 8 | 100% |
| CGloballyReusableTexBuf | 16 | 100% |
| CHPBar | 16 | 100% |
| CIconCollection | 16 | 100% |
| CPpuEngine | 32 | 100% |
| CRemoteMemory | 9928 | 100% |
| CSaveManager | 16 | 100% |
| l10n_files | 36 | 100% |
| SaveData | 48 | 100% |
| NPpuVM | 136 | 100% |
| CFiber | 8 | 100% |
| pad | 32 | 100% |
| resetmgr | 40 | 100% |
| errmsg | 16 | 100% |
| homebutton | 72 | 100% |
| sound | 40 | 100% |
| Message | 16 | 100% |
| Total | 10524 | 100% in the affected sections |

These byte counts are whole-section credit. They do not count new source work: the source and native layout already existed, while inferred sizes and incomplete symbol identities obscured the object comparison. No actual executable byte or original hash was changed.

## Traps

- Equal zero contents cannot identify BSS objects. Use matched instruction references, relocation addends, declarations, and the compiler's symbol extents. A repeated zero range can contain pointers, guard bytes, registration records, or native alignment.
- One inferred BSS symbol can cover several real records. l10n_files' constructor proves three independent twelve-byte destructor registrations through its 0/12/24 arguments. Correct the boundaries rather than mapping the whole region to one compiler symbol.
- An oversized named object can absorb another object's alignment. CRemoteMemory's four pointers occupy sixteen bytes even though the next buffer begins thirty-two bytes later. Correcting the symbol extent restores raw data scoring without changing source or excluding padding from the denominator.
