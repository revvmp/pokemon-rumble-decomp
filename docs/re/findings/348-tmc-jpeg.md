# F-348-1: TMC JPEG library identity and compiler

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #348.

The block `0x801F6060`–`0x801F8DC4` is the first half of the **TMC JPEG** middleware. Its banner, `<< RVL_MWM - TMC_JPEG090323 \trelease build: Mar 23 2009 16:43:33 (0x4302_145) >>`, is the first `.rodata` object of the library at `0x803677E8`. The 8-byte `.sdata` slot at `0x804B8850` holds the pointer that `0x801F618C` passes to `OSRegisterVersion`. The issue calls the library a decoder, but the code is a baseline JPEG **encoder**. It converts GX `RGB565` (format 4) and `RGBA8` (format 6) textures to YCbCr with 4:1:1, 4:2:2 or 4:2:0 sampling. It runs a scaled integer forward DCT, quantizes with reciprocal tables and Huffman-codes each block. It writes SOI, DQT, SOF0, DHT, SOS, optional RSTn and EOI markers.

Compiler: **Wii/1.0** with `cflags_base` plus `-func_align 4 -use_lmw_stmw on -str reuse,readonly`, so `-O4,p` and `-inline auto`. The configuration is `cflags_tmcjpeg` in `configure.py`. The evidence:

- Functions are packed on 4-byte boundaries, so the RVL_SDK default of 16 does not apply.
- Without `-str readonly`, the banner moves to `.data`.
- `-O4,s` saves four registers with `stmw`, but the target saves up to four with separate `stw`.
- Wii/1.0, Wii/1.0a, Wii/1.1 and Wii/1.3 give identical code for the matched units. The GameCube compilers (GC/3.0) do not reproduce the target.

The release tag `0x4302_145` is also carried by CNT and FA, which build with Wii/1.0.

Validation: `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. See F-348-2 for the per-unit state.

# F-348-2: Unit boundaries, names and remaining work

| Unit | `.text` | Data | Status |
| --- | --- | --- | --- |
| `TMC_JPEG/tmcjpeg.c` | `0x801F6060`–`0x801F62C4` | `.rodata` `0x803677E8`–`0x80367839` (banner); `.sdata` `0x804B8850`–`0x804B8854` (`__TMCJPEGVersion`) | Matching, 2/2 |
| `TMC_JPEG/tmcjpeg_block.c` | `0x801F62C4`–`0x801F6C5C` | `.rodata` `0x80367840`–`0x80367862` (bit-mask initializer of `TMCJPEGEncodeBlock`) | Matching, 6/6 |
| `TMC_JPEG/tmcjpeg_encoder.c` | `0x801F6C5C`–`0x801F8968` | none | NonMatching, 13/15 |
| `TMC_JPEG/tmcjpeg_huffman.c` | `0x801F8968`–`0x801F8DC4` | `.rodata` `0x80367868`–`0x80367A18` (standard Annex K Huffman tables) | Matching, 2/2 |

Boundary evidence:

- With `-str readonly`, MWCC places a unit's string pool after its other `.rodata`. The banner is the first `.rodata` object, so the unit that registers the version holds no other `.rodata`. That unit ends after the two public entry points.
- The next `.rodata` object, the `TMCJPEGEncodeBlock` mask table, is 8-byte aligned at `0x80367840`. It starts a new object.
- `TMCJPEGSetEncodeEdgeMCU` (`0x801F661C`) is called out of line by `TMCJPEGInitEncoder`, even though `-inline auto` inlines every other small static helper. The two must therefore be in different units.
- `TMCJPEGWriteRestartMarker` (`0x801F7C20`) is inlined into `TMCJPEGEncodeMCUs` but called out of line from `TMCJPEGEncodeMCUsWithEdges`. It therefore shares a unit with the former but not the latter.
- The Huffman tables start a new 8-byte aligned `.rodata` object at `0x80367868`.
- `0x801F8DC4` (quantization tables) starts #349, which also owns the stream writer, the colour converters and the `.rodata` from `0x80367A18`.

The file names are descriptive. The executable carries no TMC JPEG symbols, so every function, type and field name is descriptive and of medium confidence. Exceptions are the JPEG-standard terms (zigzag, DC/AC tables, restart interval), the banner pointer `__TMCJPEGVersion` (named by the RVL_SDK `__<lib>Version` convention, medium) and the `TMCJPEG` prefix, which comes from the banner. Layouts are proven by field accesses:

- `TMCJPEGContext` (0x164C bytes, the work buffer):
  - code and size tables from `0x58`;
  - `lumHuff`/`chrHuff` at `0x114C`/`0x1164`;
  - quantization and reciprocal tables from `0x117C`;
  - `zigzag` at `0x157C`;
  - `TMCJPEGState` at `0x1580`, with three 0x34-byte planes from `0x158C` and a `TMCJPEGComponent` at plane `+0xC`;
  - encoder pointer at `0x1628`, converter callbacks at `0x162C`/`0x1630`;
  - `TMCJPEGStream` at `0x1634`.
- `TMCJPEGEncoder` is 0x6FC bytes.
- `TMCJPEGParam` is 0xA0 bytes.

Outside callees named for the link are owned by #349: `TMCJPEGBuildQuantTables` (`0x801F8DC4`), `TMCJPEGStreamInit` (`0x801F9134`), `TMCJPEGStreamGetSize` (`0x801F9158`), `TMCJPEGStreamPutByte` (`0x801F9174`), `TMCJPEGStreamPutWord` (`0x801F91C0`), `TMCJPEGStreamPutBytes` (`0x801F924C`), `TMCJPEGStreamPutMarker` (`0x801F92B8`), `TMCJPEGStreamPutEOI` (`0x801F93C0`), `TMCJPEGSelectPutBits` (`0x801F9488`), `TMCJPEGStreamPutBits` (`0x801F95B4`), `TMCJPEGSetupRGBA8` (`0x801F9764`) and `TMCJPEGSetupRGB565` (`0x801FA424`).

Remaining work in `tmcjpeg_encoder.c`: `TMCJPEGForwardDCT` (`0x801F855C`) and `TMCJPEGForwardDCTSigned` (`0x801F8754`) have the target's size and instruction selection. Each still differs in 13 instructions of its second (column) pass, where the scheduler orders the even-part sums differently because two temporaries take other registers. Searching the declaration order through several thousand hill-climbing steps, with function-scope or block-scope declarations, and reordering the even-part statements did not remove the difference. objdiff reports 98.7% and 95.8% for the build. The other 13 encoder functions are identical, so the unit stays `NonMatching` and links from the original bytes.

## Traps

- **Declaration order picks callee-saved registers in this library.** Trigger: correct instruction streams that differ only in `r27`–`r31` assignments. Reordering local declarations fixed `TMCJPEGEncodeMCUsWithEdges`, `TMCJPEGEncodeMCUs`, `TMCJPEGEncodeBlock` and `TMCJPEGBuildHuffTables`. Search the order mechanically instead of guessing. Evidence: F-348-2.
- **A `const` parameter changes load scheduling across an inlined callee.** Trigger: `TMCJPEGInitEncoder` scheduled `SetParam`'s loads differently while `SetParam` took `const TMCJPEGParam*`. A non-const parameter reproduces the target. `SetParam` reloads `param->format` only when the store goes through a small inline helper taking `const TMCJPEGParam*`. Evidence: F-348-2.
- **A byte-typed temporary can block common-subexpression reuse.** Trigger: `TMCJPEGBuildHuffTables` reloaded `vals[k]` for the size store. The target indexes both tables from one `int` temporary; a `u8` temporary takes a different register. Evidence: F-348-2.
- **A split must end on an aligned boundary even when the object is shorter.** Trigger: ending the Huffman `.rodata` split at the object end `0x80367A16` made dtk reject the following automatic split as misaligned. Ending it at `0x80367A18` links and verifies. Evidence: F-348-2.
