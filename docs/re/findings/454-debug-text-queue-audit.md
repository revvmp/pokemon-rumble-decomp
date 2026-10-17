# F-454-1: complete surveyed issue inventory

Issue #454 lists 27 functions and 2,464 instruction bytes from `0x802A2540` through `0x802A2F74`. Every listed address now belongs to the single Matching `nw4r/db/db_DbgPrintBase.cpp` unit delivered by [PR #868](https://github.com/revvmp/pokemon-rumble-decomp/pull/868), merged as `11ce6ba8b3d11fe3df4a27524c6de6b21cd32ae0`. The native file begins in #453; its first-function ownership correctly kept the source, header, and split there. No second split or duplicate implementation is added for this survey block.

[F-453-2](453-debug-text-queue.md) records layout, chosen-name confidence, source provenance, and native behavior. The existing NW4R ledger row already accounts for the whole 62-function unit; the 27 functions audited here are a subset, not additional source credit.

## Acceptance audit

The audit reads the live issue's address/size table and the real report produced by the default Ninja workflow. It requires exactly 27 distinct listed functions totaling 2,464 bytes, exact native sizes, 100% per-function comparison, a fully source-linked owning unit, and 100% for every owned section. All conditions pass. Independently running configure, default Ninja, and the frozen artifact verifier validates the complete executable; exact committed-head commands and exit codes are recorded in the PR.

| Native address | Size | Current descriptive symbol |
| --- | --- | --- |
| `0x802A2540` | `0x48` | `__dt__Q34nw4r2db15DbgPrintBase<c>Fv` |
| `0x802A2590` | `0xC` | `GetFirst__Q34nw4r2db15DbgPrintBase<c>Fv` |
| `0x802A25A0` | `0x8` | `GetNext__Q34nw4r2db15DbgPrintBase<c>FPQ44nw4r2db15DbgPrintBase<c>5Entry` |
| `0x802A25B0` | `0x3C` | `Remove__Q34nw4r2db15DbgPrintBase<c>FPQ44nw4r2db15DbgPrintBase<c>5Entry` |
| `0x802A25F0` | `0x28` | `CreateHeap__Q34nw4r2db15DbgPrintBase<c>FPvUl` |
| `0x802A2620` | `0x60` | `DestroyHeap__Q34nw4r2db15DbgPrintBase<c>Fv` |
| `0x802A2680` | `0x44` | `__nw__Q34nw4r2db15DbgPrintBase<c>FUl` |
| `0x802A26D0` | `0xC` | `__dl__Q34nw4r2db15DbgPrintBase<c>FPv` |
| `0x802A26E0` | `0x8` | `GetInstance__Q34nw4r2db15DbgPrintBase<w>Fv` |
| `0x802A26F0` | `0x90` | `Create__Q34nw4r2db15DbgPrintBase<w>FPvUlQ34nw4r2ut5Color` |
| `0x802A2780` | `0x98` | `Create__Q34nw4r2db15DbgPrintBase<w>FPvUlPCQ34nw4r2ut4FontQ34nw4r2ut5Color` |
| `0x802A2820` | `0x8` | `SetFont__Q34nw4r2db15DbgPrintBase<w>FPCQ34nw4r2ut4Font` |
| `0x802A2830` | `0x8` | `GetFont__Q34nw4r2db15DbgPrintBase<w>CFv` |
| `0x802A2840` | `0x24` | `SetColor__Q34nw4r2db15DbgPrintBase<w>F8_GXColor` |
| `0x802A2870` | `0xC` | `GetColor__Q34nw4r2db15DbgPrintBase<w>CFv` |
| `0x802A2880` | `0x8` | `SetFontSize__Q34nw4r2db15DbgPrintBase<w>Ff` |
| `0x802A2890` | `0x8` | `GetFontSize__Q34nw4r2db15DbgPrintBase<w>CFv` |
| `0x802A28A0` | `0x8` | `SetVisible__Q34nw4r2db15DbgPrintBase<w>Fb` |
| `0x802A28B0` | `0x8` | `IsVisible__Q34nw4r2db15DbgPrintBase<w>CFv` |
| `0x802A28C0` | `0x110` | `Printf__Q34nw4r2db15DbgPrintBase<w>FiiPCwe` |
| `0x802A29D0` | `0x120` | `Printf__Q34nw4r2db15DbgPrintBase<w>FiiiPCwe` |
| `0x802A2AF0` | `0xC8` | `VPrintf__Q34nw4r2db15DbgPrintBase<w>FiiiPCwP16__va_list_struct` |
| `0x802A2BC0` | `0xAC` | `Append__Q34nw4r2db15DbgPrintBase<w>FiiiPCwi` |
| `0x802A2C70` | `0x110` | `PrintfCopy__Q34nw4r2db15DbgPrintBase<w>FiiPCwe` |
| `0x802A2D80` | `0x120` | `PrintfCopy__Q34nw4r2db15DbgPrintBase<w>FiiiPCwe` |
| `0x802A2EA0` | `0xC8` | `VPrintfCopy__Q34nw4r2db15DbgPrintBase<w>FiiiPCwP16__va_list_struct` |
| `0x802A2F70` | `0x4` | `AppendCopy__Q34nw4r2db15DbgPrintBase<w>FiiiPCwi` |

## Traps

A survey issue boundary is not a source-file boundary. This issue begins inside the narrow debug-print instantiation and ends inside the wide instantiation. Auditing the complete listed inventory avoids both duplicate splits and leaving already-reconstructed functions open as apparent fallback work. No additional reverse-engineering behavior or tooling trap was discovered beyond F-453-2.
