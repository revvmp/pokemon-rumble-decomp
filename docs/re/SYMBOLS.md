# Symbols and interfaces

No functions, data symbols, or ABI definitions have been established.

For each recovered symbol record target, artifact, section, runtime address, file offset, extent, original label if present, chosen name, kind, confidence, evidence ID, and source destination. Distinguish known bounds from inferred ones.

For each shared type or interface record size, alignment, field offsets, parameter and return conventions, signedness, ownership evidence, and unresolved questions. Keep target-specific address mappings separate from shared declarations. Rename speculative labels only when evidence supports the change and preserve traceability.

## RVL_SDK PPCArch

| Address | Size | dtk label | Chosen name | Kind | Confidence | Evidence | Source |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `0x802B1830` | `0x8` | `fn_802B1830` | `PPCMthid0` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |
| `0x802B1870` | `0x8` | `fn_802B1870` | `PPCSync` | function | high | F-463-1 | `src/RVL_SDK/base/PPCArch.c` |

## RVL_SDK GXMisc and GXGeometry

Names, ranges and evidence for 25 renamed functions and four statics are in [F-476-1](findings/476-gxmisc-gxgeometry.md). Statics, all local to `GXMisc.c`: `FinishQueue` `.sbss` `0x804B96E0` (OSThreadQueue, 8 bytes), `DrawDone` `0x804B96E8` (volatile GXBool), `DrawDoneCB` `0x804B96EC`, `TokenCB` `0x804B96F0`.

`GXData` (`include/revolution/gx/GXTypes.h`, reached through the `.sdata2` pointer `__GXData`) is 0x600 bytes, 4-byte aligned. Offsets confirmed by target accesses: `vNumNot` 0x0, `bpSentNot` 0x2, `vNum` 0x4, `vLim` 0x6, `lpSize` 0x7C, `ambColor` 0xA8, `matColor` 0xB0, `chanCtrl` 0xB8, `texGenCtrl` 0xC8, `dualTexGenCtrl` 0xE8, `suTs0` 0x108, `peCtrl` 0x22C, `genMode` 0x254, `dlSaveContext` 0x5F9, `abtWaitPECopy` 0x5FA, `dirtyState` 0x5FC. Other fields follow Petari's layout; confidence medium until their units are matched. `XF_REG_*` register names in `GXRegs.h` are descriptive choices, not recovered names.
