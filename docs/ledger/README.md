# Reconstruction ledger

The ledger records every reconstructed translation unit. It is split by area so parallel contributors edit different files; see [collaboration](../COLLABORATION.md).

| Area | Ledger |
| --- | --- |
| RVL_SDK | [sdk](sdk.md) |
| MSL_C, runtime, MetroTRK | [msl-runtime](msl-runtime.md) |
| NW4R | [nw4r](nw4r.md) |
| Home Button Menu | [hbm](hbm.md) |
| Other middleware | [middleware](middleware.md) |
| Game code | [game](game.md) |

For each unit record:
- address ranges per section
- source path
- library, compiler, and flags
- status
- match evidence
- remaining differences, with suspected causes and decomp.me scratch links
- provenance of any reused source

Non-matching notes belong here, never in source comments. Every unit not listed with `Matching` status is still an original fallback object and does not count as decompiled.
