# WUD firmware patch transport

## F-504-14: Six patch callbacks and controller protocol data

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. Six native functions add 1,664 reconstructed function bytes with Wii/1.0 and unchanged `cflags_sdk`.

| Function | Address | Extent | Non-relocation result |
| --- | --- | --- | --- |
| `__wudModuleRebootCallback` | `0x8030DBD0` | `0x30` | Exact |
| `__wudInstallPatchCallback` | `0x8030DC00` | `0xF0` | Two identical-message address differences |
| `__wudWritePatchCallback` | `0x8030DCF0` | `0x254` | Three identical-message address differences |
| `__wudRemovePatchCallback` | `0x8030DF50` | `0x1B4` | Three identical-message address differences |
| `__wudSuperPeekPokeCallback` | `0x8030E110` | `0x5C` | Exact |
| `__wudAppendRuntimePatch` | `0x8030E170` | `0xFC` | Three identical-message address differences |

All 84 call/data relocation entries agree by relocation type and instruction opcode/registers. Calls and callbacks agree by name; small-data accesses select the same named storage; the data base reproduces the native HID descriptor and controller-command prefix. Every differing non-relocation word addresses an identical retained message.

## Protocol and payload evidence

The code decodes a little-endian destination address and length from the patch record, conditionally submits the IPL-only super-peek/poke command, removes previous patches, writes bounded chunks with little-endian addresses and installs bounded groups of thirteen-byte commands. Completion callbacks continue the transfer or reboot the controller. The original download diagnostic prints the accumulated offset, not the size of the current chunk. All buffers are ordinary stack arrays, and the callbacks contain functional source control flow.

| Initialized object | Address | Size | Evidence |
| --- | --- | --- | --- |
| `_wudSuperPeekPokeCmd` | `0x803DF194` | 9 | Full byte comparison |
| `_wudPatchData` | `0x803DF1A0` | 188 | Relocation-bearing controller word; correction below |
| `_wudPatchInstallCmd` | `0x803DF25C` | 92 | Seven typed thirteen-byte commands plus count |

These arrays are the SDK's controller-firmware transfer data, consumed by the reconstructed vendor-command transport. They are not PowerPC function bodies or a substitute for reconstructing the WUD executable. Together with the existing descriptor and reset command, the native prefix contains 534 initialized bytes plus six alignment bytes, all exact.

Correction from [F-504-15](504-wud-full-initialization.md): the pinned reference and frozen DOL agree on bytes `80 01 01 38` at record offsets `0x5C` through `0x5F`. The split object contains zero placeholders and an inferred `R_PPC_ADDR32` relocation to `@etb_80010138`. The earlier conclusion that these zeros were a target payload variation was incorrect and is superseded. The source preserves the reference/frozen-DOL bytes. The record remains 188 bytes, with little-endian destination `0x00089970` and payload length `0xB4`; no manifest or expected hash changes.

Paired operands place `_wudPatchRemoveCmd` at `0x804B9B34`, `_wudInstallNum` at `0x804B9B4D`, `_wudPatchNum` at `0x804B9B4E`, `_wudPatchAddress` at `0x804B9B50`, `_wudPatchOffset` at `0x804B9B54` and `_wudPatchSize` at `0x804B9B58`. These original reference names replace coarse metadata names. Source storage uses the observed byte/word types; final whole small-data layout remains incomplete.

Restoring the data prefix also resolves earlier folded operands in connection attempts, standard pairing and the sync/delete/stack drivers. These five bodies now join the raw-exact audit, rather than remaining partial solely because of the missing initialized arrays.

## Validation and remaining work

Actual `all_source` compilation retains 77 native functions / 19,904 bytes at original extents. Sixty-six bodies / 13,940 bytes have exact non-relocation streams. Eleven partial bodies retain 69 addresses to identical data. All 340 baseline source objects remain byte-identical. Whole-unit diagnostics: text 79.406425%, data 11.850735%, BSS 100%, sdata 77.77778%, sbss 75.471695%, sdata2 100%.

Full Ninja exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`. WUD remains `NonMatching`, with its original object selected and zero linked WUD source credit. Nine native initialization/settings-recovery functions, complete initialized/small-data layout and final relocated-byte acceptance remain.

## Provenance

Functional patch transport and controller data follow target-corroborated [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. The typed patch list and retained diagnostics are corroborated by [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. The frozen DOL and split relocation explain the controller word corrected in F-504-15. Comments and annotations are removed. No decompiler text is included.

## Traps

- **Split-data zeros can be relocation placeholders.** At patch-record offset `0x5C`, the split object stores zeros with an inferred address relocation, while the frozen DOL contains `80 01 01 38`. Compare resolved data or the frozen executable before interpreting a raw split-object difference as a payload variation.
- **An undeclared utility can compile as an external call.** The reference's `MIN` macro is absent from this C include graph, so an unadapted probe emitted undefined `MIN` calls without a syntax error. Explicit ordinary conditional expressions reproduce the target operations; inspect undefined symbols as well as compiler status.
- **A diagnostic argument can affect code size.** Logging chunk length added a register move and made the write callback four bytes too long. The target logs the updated transfer offset; preserving that value restores the original extent and registers.
