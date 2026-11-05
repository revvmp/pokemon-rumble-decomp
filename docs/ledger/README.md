# Reconstruction ledger

## Current acceptance checkpoint

The final 2026-11-04 build source-selects all 897 units: 13,462 CPU functions, 3,078,460 CPU code bytes and 1,788,360 data bytes, with repeated clean frozen main.dol verification. All identified embedded programs now generate from symbolic source; D-0017 discloses the two owner-approved language exceptions. This measured checkpoint supersedes older per-issue NonMatching status rows, which retain investigation history. No original split fallback remains. See [final acceptance](../re/findings/main-source-completion-20261104.md).

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

Non-matching notes belong here, never in source comments. Current source selection is established by the verified report and final-link inventory above; older issue rows preserve their original investigation state and must not override the measured accepted checkpoint.
