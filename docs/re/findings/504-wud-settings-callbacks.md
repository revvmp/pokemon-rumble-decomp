# WUD settings persistence and NAND completions

## F-504-10: Settings save, Wii Fit callbacks and sync completion

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. Six more retained routines reproduce every non-relocation instruction word at their original lengths using the unchanged Wii/1.0 SDK flags.

| Function | Address | Bytes |
| --- | --- | --- |
| `__wudSyncStoredDevInfoToNand` | `0x8030A720` | `0x134` |
| `__wudOpenWiiFitCallback` | `0x8030A860` | `0x7C` |
| `__wudSeekWiiFitCallback` | `0x8030A8E0` | `0x84` |
| `__wudUpdateWiiFitCallback` | `0x8030A970` | `0x7C` |
| `__wudCloseWiiFitCallback` | `0x8030A9F0` | `0x54` |
| `__wudSyncDone` | `0x8030AA50` | `0x168` |

Private spellings follow the pinned key reference with medium original-spelling confidence. All bodies use existing typed SDK APIs and structures. No shared header changes are needed.

## Settings save and regional path

The save routine waits while `SCCheckStatus` is busy, clears only the ten standard 70-byte settings records, stores the current standard-device count and copies each listed device's six-byte address and 64-byte profile. It preserves the remaining six settings slots, including the Wii Fit entry. A failed `SCSetBtDeviceInfoArray` returns to state 22; success normally selects settings flush state 25.

For a linked RVL-WBC working device in product game region 0, success selects Wii Fit open state 100. The existing reconstructed producer `src/RVL_SDK/sc/scapi_prdinfo.c` maps game-region value 0 to `JP`, corroborating the newer reference's Japan constant. The private descriptive `WUD_WII_FIT_REGION_JAPAN` preserves that value without changing the shared SC interface. The status helper `__wudIsBusyScCmd` naturally inlines and has no standalone body.

## Asynchronous completion states

The callbacks use the canonical `s32 result, NANDCommandBlock*` signature. Each changes the sync or delete state only when that state is nonzero. All four clear the byte lock and preserve the original `NANDOpen.`, `NANDSeek.`, `NANDWrite.` or `NANDClose.` result message.

| Completion | Successful result | Next state on success | Sync fallback | Delete fallback |
| --- | --- | --- | --- | --- |
| Open | `NAND_RESULT_OK` / 0 | 101, seek | 25 | 5 |
| Seek | `0x4AF18` | 102, update | 25 | 5 |
| Write | 132 bytes | 103, close | 25 | 5 |
| Close | Result is logged without a success predicate | 25 for nonzero sync; 5 for nonzero delete | Same | Same |

The observed write length is 128 checksum-input bytes plus the four-byte checksum, consistent with F-504-9's pairing record. Paired SDA operands place descriptive byte `sWudNandLocked` at `0x804B9B4C`. This pass proves the callbacks' lock clearing; the operation initiators and complete asynchronous driver remain to be reconstructed.

Sync completion first requests sniff mode for a still-active device when fast checks require it, returning state 14 until that work is complete. It then cancels the alarm, restores connectable visibility unless synchronization was aborted, chooses the standard/simple completion callback and passes result 1 with the synced-device count. The original `Pairing Done` message is retained before returning state 0.

## Validation and remaining work

The actual `all_source` target compiles 56 retained bodies totaling 10,488 function bytes. Direct extent/metadata comparison covers all 56. The raw audit covers 49 functions / 6,324 bytes of exact non-relocation streams. The seven other bodies retain the same 55 folded addresses to byte-identical data. Paired comparison of the six new functions checks 80 call/data relocations: external call names, literal bytes, BSS operands and named small-data storage agree. This does not establish the unfinished source object's final small-data layout or linked function acceptance.

The HID descriptor/reset payload still contribute 245 exact initialized bytes. Whole-unit diagnostics are text 41.813168%, data 5.473637%, BSS 66.66667%, sdata 0%, sbss 47.16981% and sdata2 100%. Full Ninja exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`.

WUD remains `NonMatching`; its original object is still selected for the final link and gains zero linked WUD source-byte credit. Complete the remaining 30 functions, operation initiators, entire data layout, folded addresses and all relocated bytes before acceptance.

## Provenance

The functional persistence/completion bodies and retained messages follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. Wii Fit state transitions, private spellings and regional behavior follow the target and [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. The source uses ordinary control flow instead of the reference's release-only return-variable macro. Comments and annotations are removed; no decompiler output is included.

## Traps

- **Settings save includes a regional continuation.** The older reference stops at settings flush. This target enters the Wii Fit path only for a linked board in Japan game region 0; preserve that producer-verified condition.
- **A successful async result need not be zero.** The seek callback expects returned position `0x4AF18`, and the write callback expects 132 bytes. Preserve these predicates and the separate sync/delete fallback states rather than replacing them with a generic zero-success check.
