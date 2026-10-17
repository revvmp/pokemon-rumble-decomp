# Issue 793: lighting and vertex blending

## F-793-1: Existing lighting interfaces and corrected source boundaries

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The claim lists 19 functions and 4,572 code bytes. Its reference-span filename is an estimate. Eighteen functions belong to `g3d_light.cpp`, beginning at `0x80270C00` and ending at `0x80271668`. The preceding function at `0x80270B90` calls `GXSetFogRangeAdj` and `GXSetFog`, so it belongs to the fog unit. The final claimed function, `0x80271670` through `0x80271E0C`, is `nw4r::g3d::CalcVtx` from `g3d_calcvtx.cpp`: it iterates model vertex-position entries, tests shape-animation bindings, obtains `ShpAnmResult`, and blends positions, normals and colors. Both source units' first retained functions are owned by this issue.

The pinned CC0 reference is zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `src/nw4r/g3d/g3d_light.cpp`, `src/nw4r/g3d/g3d_calcvtx.cpp` and corresponding headers. File-specific notices were inspected; the root `LICENSE` dedicates the reference under CC0. Compiler and flags follow F-25-1: Wii/1.1 and the existing `cflags_nw4r`, with no new compiler options. Scratch compilation reproduces all 19 function sizes and every non-relocation instruction. This is diagnostic evidence only; full source selection, paired relocations, object sections and frozen DOL verification remain required for acceptance.

The small header prerequisite adds declarations only. `LightSet::GetLightObj(unsigned long)` at `0x80271530` validates the setting and light-set pointers, checks index below eight, sign-extends the selected byte, rejects negative indices and returns the light-object array entry at a 68-byte stride. Its reconstructed signature and body reproduce the full 88-byte non-relocation instruction stream. Existing `LightObj`, `LightSetting` and `LightSet` layouts are unchanged.

Four SDK declarations are already evidenced by their source-built definitions in `src/RVL_SDK/gx/GXLight.c` and native symbols: `GXInitLightAttnA`, `GXInitLightAttnK`, `GXGetLightPos` and `GXGetLightDir`. They extend the canonical `include/revolution/gx/GXLighting.h`; no duplicate SDK header tree is introduced.

Initial unmodified checkout `8b4844566aa48e167c755dc9607b597fe49958e3` passed `python configure.py`, plain `.venv/Scripts/ninja` and the explicit frozen verifier, each exit 0. Header-baseline and final exact-head evidence will be added after execution. No reconstructed-code credit is claimed by the header prerequisite.

## Traps

- A recut reference span can include a different trailing source unit. The claimed 1,948-byte tail uses shape animation and vertex arrays, not lighting. Validate calls and compare the actual reference source before assigning a unit name or extending its split.
