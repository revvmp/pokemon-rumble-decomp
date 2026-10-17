# F-455-2: complete surveyed issue inventory

Issue #455 lists 36 functions and 6,992 instruction bytes from `0x802A2F80` through `0x802A4BB4`. Its first 12 functions are the wide debug-print template tail delivered by [PR #868](https://github.com/revvmp/pokemon-rumble-decomp/pull/868), merged as `11ce6ba8b3d11fe3df4a27524c6de6b21cd32ae0`. The remaining 24 functions were previously reconstructed in the five units documented in [F-455-1](455-nw4r-math-lyt-init.md). All six owning units are Matching and source-linked.

The debug-print file starts in #453. Its full-unit split, layout, chosen-name confidence, and independent reconstruction provenance are documented in [F-453-2](453-debug-text-queue.md). No new split, duplicate implementation, or additional source credit is introduced by this completion audit. Existing hand-written paired-single assembly in the math units remains accounted as such under F-455-1; this finding does not reclassify it as C/C++ reconstruction.

## Acceptance audit

The audit reads the live issue address/size table and the real default-Ninja report. It checks 36 distinct listed functions totaling 6,992 bytes, exact native sizes, 100% per-function comparison, source-linked owning units, and complete section comparison. Configure, default Ninja, and the frozen artifact verifier independently validate the complete executable. Exact committed-head commands and exit codes are recorded in the PR.

| Native address | Size | Current symbol | Owning unit |
| --- | --- | --- | --- |
| `0x802A2F80` | `0x98` | `Clear__Q34nw4r2db15DbgPrintBase<w>Fv` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3020` | `0x18C` | `Draw__Q34nw4r2db15DbgPrintBase<w>Fv` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A31B0` | `0xC0` | `Draw__Q34nw4r2db15DbgPrintBase<w>Fiiii` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3270` | `0x54` | `__ct__Q34nw4r2db15DbgPrintBase<w>FQ34nw4r2ut5ColorPCQ34nw4r2ut4Font` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A32D0` | `0x48` | `__dt__Q34nw4r2db15DbgPrintBase<w>Fv` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3320` | `0xC` | `GetFirst__Q34nw4r2db15DbgPrintBase<w>Fv` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3330` | `0x8` | `GetNext__Q34nw4r2db15DbgPrintBase<w>FPQ44nw4r2db15DbgPrintBase<w>5Entry` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3340` | `0x3C` | `Remove__Q34nw4r2db15DbgPrintBase<w>FPQ44nw4r2db15DbgPrintBase<w>5Entry` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3380` | `0x28` | `CreateHeap__Q34nw4r2db15DbgPrintBase<w>FPvUl` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A33B0` | `0x60` | `DestroyHeap__Q34nw4r2db15DbgPrintBase<w>Fv` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3410` | `0x44` | `__nw__Q34nw4r2db15DbgPrintBase<w>FUl` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3460` | `0xC` | `__dl__Q34nw4r2db15DbgPrintBase<w>FPv` | `main/nw4r/db/db_DbgPrintBase` |
| `0x802A3470` | `0x24` | `FrSqrt__Q24nw4r4mathFf` | `main/nw4r/math/math_arithmetic` |
| `0x802A34A0` | `0x58` | `CntBit1__Q24nw4r4mathFUl` | `main/nw4r/math/math_arithmetic` |
| `0x802A3500` | `0x70` | `SinFIdx__Q24nw4r4mathFf` | `main/nw4r/math/math_triangular` |
| `0x802A3570` | `0x60` | `CosFIdx__Q24nw4r4mathFf` | `main/nw4r/math/math_triangular` |
| `0x802A35D0` | `0x70` | `SinCosFIdx__Q24nw4r4mathFPfPff` | `main/nw4r/math/math_triangular` |
| `0x802A3640` | `0x1B8` | `Atan2FIdx__Q24nw4r4mathFff` | `main/nw4r/math/math_triangular` |
| `0x802A3800` | `0x34` | `MTX34ToMTX33__Q24nw4r4mathFPQ34nw4r4math5MTX33PCQ34nw4r4math5MTX34` | `main/nw4r/math/math_types` |
| `0x802A3840` | `0x20` | `MTX34Zero__Q24nw4r4mathFPQ34nw4r4math5MTX34` | `main/nw4r/math/math_types` |
| `0x802A3860` | `0x54` | `MTX34Scale__Q24nw4r4mathFPQ34nw4r4math5MTX34PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | `main/nw4r/math/math_types` |
| `0x802A38C0` | `0x6C` | `MTX34Trans__Q24nw4r4mathFPQ34nw4r4math5MTX34PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | `main/nw4r/math/math_types` |
| `0x802A3930` | `0x38` | `MTX34RotAxisFIdx__Q24nw4r4mathFPQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3f` | `main/nw4r/math/math_types` |
| `0x802A3970` | `0x168` | `MTX34RotXYZFIdx__Q24nw4r4mathFPQ34nw4r4math5MTX34fff` | `main/nw4r/math/math_types` |
| `0x802A3AE0` | `0x90` | `VEC3TransformNormal__Q24nw4r4mathFPQ34nw4r4math4VEC3PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | `main/nw4r/math/math_types` |
| `0x802A3B70` | `0x34` | `MTX44Identity__Q24nw4r4mathFPQ34nw4r4math5MTX44` | `main/nw4r/math/math_types` |
| `0x802A3BB0` | `0x44` | `MTX44Copy__Q24nw4r4mathFPQ34nw4r4math5MTX44PCQ34nw4r4math5MTX44` | `main/nw4r/math/math_types` |
| `0x802A3C00` | `0xF4` | `VEC3TransformCoord__Q24nw4r4mathFPQ34nw4r4math4VEC3PCQ34nw4r4math5MTX44PCQ34nw4r4math4VEC3` | `main/nw4r/math/math_types` |
| `0x802A3D00` | `0xBC` | `Set__Q34nw4r4math4AABBFPCQ34nw4r4math4VEC3Ui` | `main/nw4r/math/math_geometry` |
| `0x802A3DC0` | `0x1D8` | `Set__Q34nw4r4math4AABBFPCQ34nw4r4math4AABBPCQ34nw4r4math5MTX34` | `main/nw4r/math/math_geometry` |
| `0x802A3FA0` | `0xB4` | `Set__Q34nw4r4math7FRUSTUMFffffRCQ34nw4r4math5MTX34` | `main/nw4r/math/math_geometry` |
| `0x802A4060` | `0x6D8` | `Set__Q34nw4r4math7FRUSTUMFffffffRCQ34nw4r4math5MTX34` | `main/nw4r/math/math_geometry` |
| `0x802A4740` | `0x100` | `IntersectionLine3Plane__Q24nw4r4mathFPCQ34nw4r4math5LINE3PCQ34nw4r4math5PLANEPfPQ34nw4r4math4VEC3` | `main/nw4r/math/math_geometry` |
| `0x802A4840` | `0x164` | `IntersectionRay3Plane__Q24nw4r4mathFPCQ34nw4r4math4RAY3PCQ34nw4r4math5PLANEPfPQ34nw4r4math4VEC3` | `main/nw4r/math/math_geometry` |
| `0x802A49B0` | `0x1AC` | `IntersectAABB_Ex__Q34nw4r4math7FRUSTUMCFPCQ34nw4r4math4AABB` | `main/nw4r/math/math_geometry` |
| `0x802A4B60` | `0x54` | `LytInit__Q24nw4r3lytFv` | `main/nw4r/lyt/lyt_init` |

## Traps

The previous remaining-work note described this prefix as unidentified. The complete native template starts earlier, in #453, and spans three survey issues. Audit every listed function across all owning units before closing a mixed survey block. No additional reverse-engineering behavior or tooling trap was discovered beyond F-453-2 and F-455-1.
