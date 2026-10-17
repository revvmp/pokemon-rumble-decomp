# WUD synchronization entry states

## F-504-8: Search preparation and connection attempts

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. Two additional retained synchronization routines are reconstructed with the existing typed SDK interfaces.

| Function | Address | Native bytes | Diagnostic |
| --- | --- | --- | --- |
| `__wudSyncPrepareSearch` | `0x8030A0D0` | `0x158` | Every non-relocation word exact |
| `__wudSyncTryConnect` | `0x8030A230` | `0x170` | Five folded literal-address differences; all referenced bytes identical |

Names follow the pinned key reference with medium original-spelling confidence. Search preparation disables visibility, stops when no sync loops remain or all four links/connections exist, wakes a sniffing device when fast checks require it, decrements a positive loop count and initializes the wait counter. The private indexed-device and connection-count helpers naturally inline. The release index guard uses an unsigned inclusive comparison to `WUD_MAX_DEV_ENTRY_FOR_STD - 1`, matching the target's compare-to-nine and branch-greater sequence.

The connection routine opens the working Wii Remote through `BTA_HhOpen`. When the linked-board flag permits an RVL-WBC device, it finds the registered Balance Board, rejects one already connected, and moves/removes an older entry whose address differs before opening the new working device. Both original messages, `Found the registered WBC in database` and `Removed WBC data`, are preserved with their newline terminators. The private board lookup helper naturally inlines. The outside call names `BTA_HhOpen` at `0x803195D4`, leaving its extent, type and split unchanged.

All five connection-routine differences are `addi` literal operands with identical opcode and register fields. Each source/target address pair identifies a byte-identical, NUL-terminated device-name or debug string. No other non-relocation instruction word differs. This is still narrower than unit acceptance.

## Natural BSS layout and security diagnostic

Adding the real early connection path makes the compiled source emit the following BSS prefix at the original offsets. The source declarations use the existing complete SDK types; no padding object or copied executable data is introduced to obtain this layout.

| Source object | Source BSS offset | Target address | Bytes |
| --- | --- | --- | --- |
| `_wcb` | `0x0` | `0x8047C320` | `0x750` |
| `sWudDiscoveryResponse` | `0x750` | `0x8047CA70` | `0x108` |
| `_work` | `0x858` | `0x8047CB78` | `0x60` |
| `_scArray` | `0x8B8` | `0x8047CBD8` | `0x461` |

This resolves all earlier BSS-offset and register-allocation differences in the security callback. Its remaining 22 non-relocation differences are now exclusively folded addresses of byte-identical messages, with identical instruction opcode/register fields. The earlier 32-difference diagnostic in F-504-4/F-504-7 is superseded by this result. This establishes only the listed BSS prefix; later globals and the complete unit still need reconstruction.

The current source contains 49 of 86 retained functions, totaling 8,468 function bytes. The raw audit covers 43 functions and 5,192 exact non-relocation bytes. Six other exact-size bodies retain 22/4/8/7/5/5 folded-address differences for security/registration/removal/search/power/connection attempts. Every one of these 51 differing operands resolves to byte-identical message, device-name or command data. The HID descriptor and reset command still contribute 245 exact initialized bytes. Whole-unit diagnostics are text 33.757233%, data 5.473637%, BSS 47.058823%, sdata 0%, sbss 37.735847% and sdata2 100%.

## Validation and remaining work

The actual `all_source` target compiles the source. Direct object comparisons verify 49 function addresses/extents, all 43 raw instruction streams, the 51 folded-address data pairs, both exact data initializers and the four BSS prefix extents. All three added private helper bodies are absent after natural inlining. Full Ninja exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`.

WUD remains `NonMatching` and the final link still selects its original object, adding zero linked WUD source credit. Complete the remaining 37 functions, naturally emitted data/BSS, folded addresses and all relocations before accepting source selection.

## Provenance

Functional bodies and helper behavior follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. The Balance Board path and private spellings are corroborated by [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Target accesses and messages verify the added behavior. Comments and annotations are removed; no decompiler output is included.

## Traps

- **Equivalent bounds need not select identical instructions.** The indexed helper's unsigned `< 10` comparison produced compare-to-ten/branch-greater-or-equal, differing in two words. The newer reference's unsigned `<= 9` form reproduces the observed target sequence. Preserve the proven inclusive release bound.
- **Partial source can distort real pool ordering.** Adding the legitimate early connection routine restored the original four-object BSS prefix and removed ten security-callback differences. Complete the actual source before trying to compensate for missing functions through fabricated padding or register constraints.
