# Issue 478: RVL_SDK GXTexture and GXBump

## F-478-1: Unit boundaries and source

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The sources are compared with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/gx/GXTexture.c` and `GXBump.c`, using dtk 1.8.4 split assembly and objdiff.

| File | `.text` | Data |
| --- | --- | --- |
| `GXTexture.c` | `0x802CE630`-`0x802CF730` (31 functions) | `.data` `0x803D6468`-`0x803D668C`: jump tables for `GXGetTexBufferSize`, `__GetImageTileCount` and `GXInitTexObj`. `.sdata` `0x804B8B38`-`0x804B8B80`: nine 8-byte ID and filter tables. `.sdata2` `0x804BB348`-`0x804BB380` |
| `GXBump.c` | `0x802CF730`-`0x802CFB84` (9 functions) | `.sdata2` `0x804BB380`-`0x804BB384` (`1024.0f`) |

The data owners come from which functions reference each address. `GXTev.c` data starts at `0x803D6690`, and `GXPixel.c` constants at `0x804BB388`.

Differences from Petari:

- `GXInitTexObjFilter` at `0x802CEBC0` (`0x28` bytes) is not in Petari. It sits between `GXInitTexObjTlut` and `GXInitTexObjUserData`, and writes the `mode0` magnification filter (`mag_filt == GX_LINEAR`) and the minification filter through `GX2HWFiltConv`. It was written from the target.
- `GXInitTexPreLoadRegion`, `GXSetTexCoordScaleManually` and `GXSetTevIndWarp` have no code in the target and are not reconstructed.
- The Wii/1.0 `cflags_sdk` (`-inline auto,level=3`) inline `GXSetTevIndirect` into `GXSetTevDirect`, and the target calls it. Declaring `GXSetTevIndirect` `NO_INLINE` gives 100%.

Compiled with Wii/1.0 and `cflags_sdk`, all 40 functions and every data section score 100% in objdiff. With both units `Matching`, `ninja` exits 0 and `tools/verify/verify.py verify` passes.

## Traps

- **`-inline auto,level=3` inlines larger SDK callees than the original build did.** Trigger: `GXSetTevDirect` scored 18% because `GXSetTevIndirect` was inlined into it. Mark the callee `NO_INLINE` when the target keeps the call. Evidence: F-478-1.
