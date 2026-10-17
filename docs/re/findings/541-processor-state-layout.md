# MetroTRK processor-state declaration prerequisite

## F-541-6: Canonical complete state types

This header-only prerequisite extends `include/MetroTRK/ppc/Generic/targimpl.h` for the coordinated #541, #542, #543 and #545 reconstructions. The #545 owner approved publishing the complete processor-state declaration here; the #542 owner supplied the original declaration subset and independent size/native-access evidence. Existing APIs remain unchanged. There is no implementation, split, build-object selection or source-progress credit.

The types adapt the comment-free declaration subset of [Petari e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/libs/MetroTRK/include/ppc/Generic/targimpl.h), CC0-1.0 with no file-level exception found. Original type and register-member names are retained. The fifteen exception-vector IDs used by the target's ISR offset table are independently checked against the sixty native table bytes at `0x803E62E0`. The original `PPC_FloatingPointUnavaiable` spelling is preserved.

| Type | Native-compatible size | Evidenced fields and offsets |
| --- | --- | --- |
| `Default_PPC` | `0x94` | GPR array `0`; PC `0x80`; LR `0x84`; CR `0x88`; CTR `0x8C`; XER `0x90` |
| `Float_PPC` | `0x110` | 32 eight-byte FPR slots, followed by eight-byte FPSCR and FPECR slots |
| `Extended1_PPC_6xx_7xx` | `0x184` | Native DBAT3U access is at complete-state offset `0x238` |
| `Extended2_PPC_6xx_7xx` | `0x100` | 32 pairs of four-byte paired-single register words |
| `ProcessorState_PPC_6xx_7xx` / `ProcessorState_PPC` | `0x430` | Default `0`; Float `0x98`; Extended1 `0x1A8`; Extended2 `0x32C`; saved transport return address `0x42C` |
| `TRKState` | `0xA4` | saved MSR `0x8C`; stopped flag `0x98`; input flag `0x9C`; pending pointer `0xA0` |

The complete state extents agree with native `gTRKCPUState` at `0x804B55E0` and `gTRKState` at `0x804B5538`. Startup's save/restore assembly establishes the default-register offsets; address translation independently loads DBAT3U at `0x238`; target initialization stores stopped/MSR at `0x98`/`0x8C`. The processor assembly being reconstructed by #543 and #545 supplies independent register-bank accesses. Names of unused legacy register slots follow the SDK reference; no claim is made that every later Broadway SPR captured into those slots retains the older processor's register meaning.

The declarations for `TRKSaveExtended1Block(void)` and `__TRK_get_MSR(void)` describe the original assembly interfaces exercised by startup. The existing canonical APIs and all field widths remain intact. Header layout is checked by compiling the actual header with both pinned Wii/1.1 C and C++ modes and inspecting the resulting size/offset data, rather than assuming the host ABI. Implementation acceptance still requires complete native instruction/relocation and frozen-DOL verification in each source delivery.

## Traps

- A zero-filled BSS comparison cannot establish structure layout. Check native field accesses, complete object extents and target-compiler C/C++ offsets independently.
- Retain one canonical complete state declaration. Consumers should replace opaque local aliases with this shared interface; do not create parallel processor-state structures.
