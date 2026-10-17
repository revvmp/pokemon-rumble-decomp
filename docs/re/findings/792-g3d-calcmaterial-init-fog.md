# #792: NW4R g3d CalcMaterial, Init and Fog

## F-792-1: three units match; ScnGroupEx remains

For `wpse-v1` / `main.dol`, three of the issue's units are reconstructed and linked as `Matching`:

| Unit | Range | Functions |
| --- | --- | --- |
| `nw4r/g3d/g3d_calcmaterial.cpp` | `.text` `0x80270750–0x80270984` | 1/1 |
| `nw4r/g3d/g3d_init.cpp` | `.text` `0x80270990–0x80270A88`; `.data` `0x803C6150–0x803C6196`; `.sdata` `0x804B89C8–0x804B89CC` | 2/2 |
| `nw4r/g3d/g3d_fog.cpp` | `.text` `0x80270A90–0x80270C00`; `.sdata2` `0x804BAD40–0x804BAD44` | 5/5 |

Sources come from CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee`, stripped of comments. Differences from ss:

- The version string in `.data` is `NW4R:G3D - Sep 24 2009 16:53:04 (0x4302_151)`, so `g3d_init.cpp` uses that build date and version.
- `G3dReset` switches on `VI_NTSC`, `VI_PAL`, `VI_EURGB60` and `VI_MPAL` from the shared Petari VI headers.
- `Fog::SetGP` calls `GXSetFog` with the SDK argument order (type, startz, endz, nearz, farz, color).
- Fast-cast initialisation needed `OSSetGQR6` and `OSSetGQR7` (`mtspr 0x396` and `0x397`) and the `OS_GQR_TYPE_*` values in `OSFastCast.h`, plus `GX_FOG_ADJ_TABLE_SZ` and `GXInitFogAdjTable` in the GX headers.

ss's `g3d_scnproc.cpp` and `g3d_tmem.cpp` are absent from this executable.

The remaining range in this issue, `.text` `0x8026F880–0x8027074C`, belongs to `ScnGroupEx`: `0x8026F890` references the type name `ScnGroupEx` at `0x8036C9C8`. ss, Petari, Whippy, kdc and xenoblade do not implement `g3d_scngroupex.cpp`, so it is left blocked with no reference.

Evidence: `python configure.py` exit 0; `.venv/Scripts/ninja` exit 0 after deleting `build/wpse-v1/main.dol`; objdiff 100% for all three units; `tools/verify/verify.py verify` PASS for `main.dol`.

## Traps

- **ss's `GXSetFog` argument order differs from the SDK header.** Compiling ss's call against the shared Petari `GXPixel.h` passes the floats in the wrong registers. Use the SDK order (type, startz, endz, nearz, farz, color).
- **Not every ss g3d unit is in this game.** `ScnProc` and `g3d_tmem` have no code or type names in `main.dol`. Before splitting a reference unit, check for its type names or vtables.
