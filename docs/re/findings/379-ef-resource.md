# F-379-1: NW4R ef resource unit

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #379 opens with the tail of `ef_particlemanager.cpp` (`BeginDraw`, `EndDraw`, `Draw_ModifyColor`), which #743 split as part of #378. This change adds the next unit.

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x802347B4–0x80235320` | 12 functions from `Resource::GetInstance` (`0x802347C0`) to `__sinit_\ef_resource_cpp` (`0x802352D0`); the split starts with the 0xC bytes of padding after the particle-manager static initializer |
| `.ctors` | `0x80352A40–0x80352A44` | static initializer entry |
| `.bss` | `0x8042F2D8–0x8042F2F8` | `Resource::mResource` (0x20) |

The unit is `Matching`: objdiff reports 100% for all 12 functions, and the source-linked `main.dol` passes `tools/verify/verify.py`.

## Reference

The source follows `src/nw4r/ef/ef_resource.cpp` of CC0 `Etrainquandt17/Whippy` (commit `8ee40c841475db862231f37b8cb64196b613cc17`) unchanged. The target lacks `FindEmitter`, `_FindTexture`, `NumEmitter`, `_GetEmitterIndexOf`, `RemoveEffectProject` and `RemoveTextureProject` as out-of-line functions; `-ipa file` inlines the ones the unit calls and the linker removes the rest. The constructor and `Initialize` are inlined into the static initializer.

## Recovered layouts

- `TextureData` is 0x40 bytes. `breftRelocate` steps through texture records 64 bytes apart. The 2006 header ends at `tlut` (0x24); the reference adds 28 bytes after it, named `reserve` here (low confidence; nothing in this unit reads them).
- Animation-curve type 2 is relocated like type 5 (`AC_TYPE_CHILD`) in `RelocateCommand`. It is named `AC_TYPE_EMITTER_CHILD` (low confidence; the reference calls it `AC_TYPE_2`).
- `EmitterResource::GetPtclTrack` and `GetEmitTrack` index the table pointer directly. The register allocation in `RelocateCommand` depends on this form; `ef_emitter.cpp` matches with either.

New header `ef_res_texture.h` declares `TextureData` and `ResTexture`.

## Remaining work in #379

`ef_util.cpp` starts at `0x80235320` and ends at `0x80236298`. Its C functions (`UtlistToArray` to `MtxGetScale`) have reference bodies in Whippy, ogws, forecast and the Wii News Channel. `Normalize`, the three sine-cosine helpers (`0x80235CB0`, `0x80235D30`, `0x80235DB0`), `PSMTX34RotXYZRad`, both `MTX34ScaleNonZero`, `MTX34ScaleInv` and `MTXColLen` are paired-single assembly with no public reconstruction; `0x80235CB0` reads `math::detail::gSinCosTbl`. `ef_handle.cpp` follows at `0x802362A0`.

## Traps

- **A split can start in padding.** The particle-manager split ends at its last function (`0x802347B4`), so the resource split begins there. objdiff then lists a `pad_03_802347B4_text` symbol with no score. This is expected and does not affect the link.
