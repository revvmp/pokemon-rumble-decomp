# Issue 475: vector, GXInit, GXFifo and GXAttr units

Target `wpse-v1`, frozen `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Runtime `.text` addresses map to DOL file offsets by subtracting `0x8004FD40`.

## F-475-2: unit boundaries

The 35 issue functions belong to four translation units, all compiled with Wii/1.0, `cflags_sdk`, default 16-byte function alignment.

| Unit | Sections | Functions |
| --- | --- | --- |
| `RVL_SDK/mtx/vec.c` | `.text` `0x802C9910`-`0x802C9B48` (`align:16`); `.sdata2` `0x804BB2C8`-`0x804BB2D4` | 7 |
| `RVL_SDK/gx/GXInit.c` | `.text` `0x802C9B50`-`0x802CACE0` (`align:16`); `.data` `0x803D5EA0`-`0x803D60E0`; `.bss` `0x80453B80`-`0x80454200`; `.sdata` `0x804B8B20`-`0x804B8B24`; `.sbss` `0x804B9698`-`0x804B96C0`; `.sdata2` `0x804BB2D8`-`0x804BB300` | 6 |
| `RVL_SDK/gx/GXFifo.c` | unchanged from F-475-1 | 9 |
| `RVL_SDK/gx/GXAttr.c` | `.text` `0x802CB6C0`-`0x802CC624`; `.data` `0x803D6110`-`0x803D6300`; `.sdata` `0x804B8B28`-`0x804B8B34` | 13 |

The vector unit holds `PSVECAdd`, `PSVECNormalize`, `PSVECMag`, `PSVECDotProduct`, `PSVECCrossProduct`, `C_VECHalfAngle` and `PSVECSquareDistance`, in that order; the reference's subtract, scale, C magnitude and distance routines are dead-stripped. Its constants are 0.5, 3.0 and 0.0. `C_VECHalfAngle` is identified by its negate, normalize, add, dot-product and conditional-normalize sequence and its three 12-byte stack vectors.

GXInit begins at `0x802C9B50` with `__GXDefaultTexRegionCallback`, followed by `__GXDefaultTlutRegionCallback`, `__GXShutdown`, `__GXInitRevisionBits`, `GXInit` and `__GXInitGX`; `EnableWriteGatherPipe` is inlined. The version string reads `<< RVL_SDK - GX \trelease build: Feb 27 2009 10:04:13 (0x4302_145) >>`. Its data is the version string (0x45 bytes), the 32-byte aligned `DefaultTexData` at `0x803D5F00`, `GXDefaultVATList` (13 entries of 16 bytes), `GXDefaultProjData`, `GXTexRegionAddrTable` and `GXShutdownFuncInfo`. The `.bss` holds the 128-byte `FifoObj` and the 0x600-byte `gxData`. Small data is `__piReg`, `__cpReg`, `__peReg`, `__memReg`, then `peCount`, the eight-byte `time`, `calledOnce` and `shutdownFuncRegistered`. Small data 2 is `__GXData` and the 2^24, 0, clear-colour, white, 1.0, 0.1 and 2^52 conversion constants.

GXAttr holds `GXSetVtxDesc`, `__GXSetVCD`, `__GXCalculateVLim`, `GXGetVtxDesc`, `GXClearVtxDesc`, `GXSetVtxAttrFmt`, `GXSetVtxAttrFmtv`, `__GXSetVAT`, `GXGetVtxAttrFmt`, `GXSetArray`, `GXInvalidateVtxCache`, `GXSetTexCoordGen2` and `GXSetNumTexGens`. Its six switch tables fill `.data`, and the three four-byte `__GXCalculateVLim` component tables fill `.sdata`. The render-mode objects at `0x803D6300` onwards follow GXAttr and belong to a later unit.

## F-475-3: complete reconstruction

All four units are `Matching` and source-linked: 35/35 functions, 11,348 code bytes, and every data section compare at 100% in objdiff 3.8.2, and the full DOL verifies against the frozen manifest. GXFifo now uses the shared `revolution/gx` headers and defines its own storage; the narrowed `include/RVL_SDK/gx/GXFifo.h` from F-475-1 is removed. The six vector routines other than `C_VECHalfAngle` are paired-single inline assembly (304 of 520 bytes); the GX units are C.

Provenance: `vec.c`, `GXInit.c`, `GXFifo.c` and most of `GXAttr.c` follow Petari (CC0) at `e0ab141914921726a595573dc48d3f13d0922efe`. `GXGetVtxDesc`, `GXGetVtxAttrFmt` and the file-scope ordering of the shutdown statics follow zeldaret/tp (CC0) `libs/revolution/src/gx/` at `c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0`, which carries the same `0x4302_145` GX build. `C_VECHalfAngle` and `PSVECSquareDistance` were written from the target instructions.

The shutdown counters are file-scope statics declared in reverse of their small-data order, which reproduces the target layout. The four render-mode objects referenced by `__GXInitGX` are named `GXNtsc480IntDf` (`0x803D6300`), `GXMpal480IntDf` (`0x803D6378`), `GXPal528IntDf` (`0x803D63B4`) and `GXEurgb60Hz480IntDf` (`0x803D63F0`) from the `VIGetTvFormat` switch. Nine callees in neighbouring units are named from their reference call sites, with sizes and splits unchanged: `GXGetTexObjFmt` (`0x802CECB0`), `GXGetTexObjMipMap` (`0x802CECE0`), `GXLoadTexObj` (`0x802CEF50`), `GXInvalidateTexAll` (`0x802CF150`), `GXSetTexRegionCallback` (`0x802CF1A0`), `GXSetTlutRegionCallback` (`0x802CF1C0`), `GXSetIndTexCoordScale` (`0x802CF8E0`), `GXSetNumIndStages` (`0x802CFAB0`) and `GXSetTevDirect` (`0x802CFAD0`).

## Traps

### Linking GXFifo from source removes its trailing `.bss` padding

The source GXFifo object ends its `.bss` after the 36-byte `CPUFifo`, at `0x80454248`. The following fallback object then started there instead of at `0x80454280`, `.bss` shrank by 0x38, and every later data section moved; verification failed at DOL header byte `0x7E`, the `.sdata` address. Giving `__DVDTicketViewBuffer` `align:64` in `symbols.txt` restores the original start. If a unit's `.bss` ends before an aligned buffer, check the next object's alignment.

### A missing header changes register allocation

Without `revolution/gx/GXGet.h`, `GXGetTexObjMipMap` is implicitly declared as returning `int`. The compiler then masks the result once and keeps it in a separate register, while the target tests the low byte with `clrlwi.` at each use. Including the header that declares the `GXBool` return reproduces the target. CodeWarrior accepts the implicit declaration without an error, so check includes when a single conversion is hoisted.

## Validation

`python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit 0, with `build/wpse-v1/main.dol` deleted before each build. The split-only build verified before any source was linked.
