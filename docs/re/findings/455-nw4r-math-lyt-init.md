# F-455-1: NW4R math units and lyt_init

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Issue #455 owns `.text` `0x802A2F80–0x802A4BB4`. The range from `0x802A3470` holds four NW4R `math` units and `lyt_init.cpp`; all five are `Matching`. The preceding twelve functions are now reconstructed in the debug-print template unit delivered by [F-453-2](453-debug-text-queue.md). [F-455-2](455-complete-inventory-audit.md) audits every issue function across all six Matching units.

## Units and boundaries

| Unit | Section ranges | Functions |
| --- | --- | --- |
| `nw4r/math/math_arithmetic.cpp` | `.text` `0x802A3470–0x802A34F8`; `.sdata2` `0x804BB110–0x804BB118` | `FrSqrt`, `CntBit1` |
| `nw4r/math/math_triangular.cpp` | `.text` `0x802A3500–0x802A37F8`; `.rodata` `0x803705F8–0x80371608`; `.data` `0x803C6D20–0x803C6E28`; `.sdata2` `0x804BB118–0x804BB134` | `SinFIdx`, `CosFIdx`, `SinCosFIdx`, `Atan2FIdx`; `detail::gSinCosTbl` (257 x 16 bytes), anonymous `sArcTanTbl` (33 x 8 bytes) |
| `nw4r/math/math_types.cpp` | `.text` `0x802A3800–0x802A3CF4`; `.sdata2` `0x804BB138–0x804BB150` | `MTX34ToMTX33`, `MTX34Zero`, `MTX34Scale`, `MTX34Trans`, `MTX34RotAxisFIdx`, `MTX34RotXYZFIdx`, `VEC3TransformNormal`, `MTX44Identity`, `MTX44Copy`, `VEC3TransformCoord(VEC3*, const MTX44*, const VEC3*)` |
| `nw4r/math/math_geometry.cpp` | `.text` `0x802A3D00–0x802A4B5C`; `.sdata` `0x804B8A08–0x804B8A0C`; `.sdata2` `0x804BB150–0x804BB160` | `AABB::Set` (two overloads), `FRUSTUM::Set` (two overloads), `IntersectionLine3Plane`, `IntersectionRay3Plane`, `FRUSTUM::IntersectAABB_Ex` |
| `nw4r/lyt/lyt_init.cpp` | `.text` `0x802A4B60–0x802A4BB4`; `.data` `0x803C6E28–0x803C6E70`; `.sdata` `0x804B8A10–0x804B8A14` | `LytInit` |

objdiff reports 100% for all five units, and the source-linked `main.dol` passes `tools/verify/verify.py`. They use NW4R's compiler and flags (F-25-1: Wii/1.1, `cflags_nw4r`). The `lyt_init.cpp` `.data` range ends at `0x803C6E70` rather than at the end of the version string (`0x803C6E6E`), because dtk rejects an automatic split that starts at a 2-byte-aligned address.

## Sources and evidence

- `math_arithmetic`, `math_triangular`, `math_types`, and `math_geometry` start from the CC0 xbret/xenoblade sources (commit `ea25691b2029394ab777bda7060139e517a13cba`). Their comments were removed, and functions absent from the target were dropped where the literal pool shows the original file did not contain them.
- The names `IntersectionLine3Plane` and `IntersectionRay3Plane` and their sizes (0x100 and 0x164) come from the `SUPE01` symbol map in `riptl/wii-symbols`. The `LINE3`/`RAY3` layout (point at 0x0, direction at 0xC) follows the loads in the target code. The tolerance is a writable `.sdata` float (`0.0001f` at `0x804B8A08`), not a `.sdata2` literal.
- `0x802A3C00` is `VEC3TransformCoord` with an `MTX44`. The transformed vector is computed before `w`, and the code multiplies by `1.0f / w`; computing `w` first changes the schedule.
- `0x802A3970` `MTX34RotXYZFIdx` is a hand-written paired-single routine. It is not the C version in the references (0x168 bytes against 0x14C). It reads a `{65536.0f, 65536.0f}` pair from `.sdata2` with `psq_lx` and indexes `detail::gSinCosTbl` directly, so the table is a global `detail` object, as in Xenoblade. It quantizes through GQR3, which `LytInit` and the SDK set up. In CodeWarrior inline assembly, the small-data address is written as `la r0, sFIdxRange(r2)`, and the qualified table name needs a `using` declaration.
- `LytInit` registers `NW4R_LYT_Version_` (`"<< NW4R    - LYT \tfinal   build: Sep 24 2009 16:54:15 (0x4302_151) >>"`) and then inlines `OSInitFastCast`, as in Petari.
- `CntBit1` matches only when the running count is held in a separate local from the argument.
- In `IntersectionLine3Plane`, the plane distance must be written as `VEC3Dot(&n, &P) + d`. The header's `PLANE::Test` (`d + VEC3Dot(...)`) gives the same instructions with different float register allocation.

## Literal pool order

Matching instructions were not enough: two units first linked with their `.sdata2` constants out of order.

- `math_types`: the target pool is `0.0f, 1.0f, π/128, {65536, 65536}`. `1.0f` is used before `MTX34RotAxisFIdx` only if `MTX33Identity` is compiled first. The unit therefore keeps the reference's `VEC3Maximize`, `VEC3Minimize`, `MTX33Identity`, and `MTX34InvTranspose`, which the linker discards because nothing calls them.
- `math_triangular`: the target pool is `65536, 0, 32, 64, -64, 128, -128`, so the arctangent helper's `32.0f` is allocated before `Atan2FIdx`'s constants, and no `1.0f` exists. `AtanFIdx` is therefore absent. The helper must be an out-of-line definition placed between `SinCosFIdx` and `Atan2FIdx`. An anonymous-namespace or `static` helper allocates its literal only when inlined, which puts `32.0f` last. The source keeps it as `detail::AtanFIdx_`; the linker discards the out-of-line copy. Its linkage and name are a hypothesis (confidence low).
- `math_geometry`: the target pool is `0.5f, 0.0f, π/128, 256/360`, while `FRUSTUM::Set(fovy, ...)` alone allocates `TanDeg`'s constants first. Xenoblade forces the order with a dummy literal macro. Here an unlinked `SPHERE::Set(const VEC3*, unsigned int)` is placed before the frustum functions instead; later NW4R headers declare it (TwilitRealm/dusklight `nw4hbm/math/geometry.h`, CC0). Its body (box center times `0.5f`, a maximum squared distance starting at `0.0f`) is a reconstruction that reproduces the pool. The linker discards it together with `PLANE::Set` and `IntersectionAABB`, which are inlined at every use.

## Completed remaining prefix

The former unresolved range `0x802A2F80`-`0x802A346C` is the final twelve methods of `DbgPrintBase<wchar_t>`, whose native source file begins at `0x802A1950` in #453. [PR #868](https://github.com/revvmp/pokemon-rumble-decomp/pull/868) reconstructs that entire file, including these methods, and links it as Matching. Direct call evidence identifies `TextWriterBase<wchar_t>`, superseding the earlier char hypothesis and the proposed #454 file start. No issue functions remain as original-object fallbacks. The full inventory and exact executable acceptance are recorded in [F-455-2](455-complete-inventory-audit.md).

## Traps

- Paired-single NW4R functions can be hand-written assembly in this revision even when every public reference has C. Compare function sizes before tuning C source, and write the routine as a CodeWarrior `asm` function with `nofralloc`. Use `la rX, sym(r2)` for an SDA21 address and a `using` declaration for namespaced symbols; `@sda21` and qualified names are rejected.
- With `-ipa file`, an inline-only helper's literals are allocated where it is inlined. An out-of-line definition allocates them where it is defined. When a pool order is wrong, check whether the original file had an out-of-line definition or an unlinked function at that position.
- dtk sizes `.sdata`/`.sdata2` objects up to the next symbol. Shrink the symbol size before ending a split at the object's last byte. Otherwise dtk fails with "ends within symbol".
