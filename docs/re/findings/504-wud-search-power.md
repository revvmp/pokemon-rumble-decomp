# WUD search and power callbacks

## F-504-7: Discovery state, reset command and power notifications

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. The reconstructed search callback `__wudSearchEventStackCallback` at `0x8030F6E0` has the original 0x208-byte length. Its spelling follows the pinned key reference with medium confidence. All non-relocation instructions are identical except seven folded data offsets. Six address byte-identical, NUL-terminated messages; the seventh addresses the exact 28-byte authentication-count reset command. The opcode and register operands of each differing instruction are identical to the target. Full WUD data and relocation acceptance remains pending.

Paired target/source HA/LO and SDA relocations establish the following state. The descriptive names do not claim recovered Nintendo spelling.

| Address | Name | Storage and role |
| --- | --- | --- |
| `0x8047CA70` | `sWudDiscoveryResponse` | Existing 0x108-byte `WUDDiscResp`: six-byte address, 64-byte name, service mask and existing opaque storage |
| `0x804B9B5E` | `sWudDiscoveryRssi` | Signed byte containing the inquiry result's RSSI |
| `0x804B9B5F` | `sWudDiscoveryResultCount` | Byte incremented for a discovery result and cleared when results are reset |
| `0x803DF2B8` | `_wudResetAuthCountCmd` | 28 initialized bytes, matching the reference-derived reset payload exactly |

The reset command begins with little-endian bytes `30 36 08 00` for controller address `0x00083630`, followed by 24 zero bytes. The source retains ordinary byte-array expressions for that protocol representation. Its size and every initializer byte match. The remaining coarse data record at `0x803DF194` ends at the command's start; its extent changes from 0x140 to 0x124, leaving the command's 0x1C bytes explicit. The original search callback passes length 28 and vendor opcode `0xFC4C`. Its call names outside `BTM_VendorSpecificCommand` at `0x8031D6DC` without changing that function's split, type or size.

Search events preserve the original timeout selection, copy the device address/name/services into the existing typed discovery record, count results, update the sync state and reset authentication/discovery state after cancellation. The two private functional reset helpers naturally inline and have no standalone compiled bodies. No placeholder calls or unimplemented helper bodies are added.

The power callback `__wudPowerMangeEventStackCallback` at `0x8030FF50` now has its original 0x12C-byte length. Preserve `Mange` exactly as established by the embedded message. It finds the paired or working device, reports an unknown address when neither exists, and sets connection status 8 for active or 9 for sniff notifications. All non-relocation words match except five folded message offsets; each addresses byte-identical text with identical instruction opcode and register operands. It remains NonMatching with the unit.

The older reference's `ASSERT(p_info != NULL)` annotation is not present in this target's power callback. No `ASSERT` macro exists in the current imported C headers. The earlier full-reference probe therefore emitted an undefined function symbol `ASSERT` and produced 0x13C bytes. Omitting that unavailable, target-absent debug annotation gives the exact 0x12C-byte body; it does not remove an original target check.

The current source contains 47 of 86 retained functions, totaling 7,756 function bytes. The 42-function raw audit still proves 4,848 bytes of exact non-relocation streams. The five other bodies have exact sizes and retain 32/4/8/7/5 instruction differences for security/registration/removal/search/power respectively. All 24 differences outside the security callback are proven folded addresses of identical strings or command bytes. The native HID descriptor and reset payload total 245 exact initialized bytes. Whole-unit diagnostics are text 30.88493%, data 5.473637%, BSS 40.43443%, sdata 0%, sbss 37.735847% and sdata2 100%.

## Validation and remaining work

The real `all_source` target compiles the actual source object. Direct comparisons verify both new function extents, all twelve differing data operands, the 28-byte reset command, the four new relocated bindings and natural inlining of the two private helpers. The existing 42-function, registration/removal and security diagnostics still pass. Full Ninja exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`.

WUD remains `NonMatching` and the final executable still selects its original object. No linked WUD source credit is added. Complete the 39 missing functions, natural data/BSS layout, all folded addresses and every relocation before source selection and full acceptance.

## Provenance

Functional bodies and reset bytes follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. Private helper names are corroborated by [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Original accesses establish addresses, types and behavior. Comments and unavailable debug annotations are removed; no decompiler output is included.

## Traps

- **An unavailable C assertion can become an implicit external call.** The full-reference probe compiled `ASSERT` as an undefined function and added sixteen power-callback bytes. Confirm the target's checks and the imported macro definitions before retaining another project's debug annotation.
- **Controller command byte order is independent of the CPU.** This reset command stores `0x00083630` as four little-endian bytes even on PowerPC. Its byte-array initializer matches; substituting a native big-endian word would change the protocol data.
