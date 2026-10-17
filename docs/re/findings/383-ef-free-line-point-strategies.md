# F-383-1: NW4R ef free, line and point draw strategies

Target: `wpse-v1`, immutable `orig/wpse-v1/main.dol`. #383 opens with the end of the directional strategy file (`0x802408C0–0x80241320`): `CalcRotate`, an out-of-line `MTX34` constructor, `CalcLocalTransform`, two unnamed functions, `DrawDirectionalStrategy::GetCalcAheadFunc` (`0x80241210`) and the directional destructor (`0x802412E0`). That file starts in #382 and has no public source. This change splits the next three units.

| Unit | Section | Range | Status |
| --- | --- | --- | --- |
| `nw4r/ef/ef_drawfreestrategy.cpp` | `.text` | `0x80241320–0x80241D50` | NonMatching, 3/4 functions at 100% |
| | `.rodata` | `0x8036C630–0x8036C690` | the two quad vertex tables `p` and `px` |
| | `.data` / `.sdata` / `.sdata2` | `0x803C56C0–0x803C56D8` / `0x804B8940–0x804B8948` / `0x804BA9E0–0x804BAA08` | vtable / `free_tex0_u8` / constants |
| `nw4r/ef/ef_drawlinestrategy.cpp` | `.text` | `0x80241D50–0x80242380` | Matching, 3/3 |
| | `.data` / `.sdata` / `.sdata2` | `0x803C56D8–0x803C56F0` / `0x804B8948–0x804B8950` / `0x804BAA08–0x804BAA18` | vtable / `Min` literal / constants |
| `nw4r/ef/ef_drawpointstrategy.cpp` | `.text` | `0x80242380–0x80242820` | Matching, 3/3 |
| | `.data` / `.sdata` / `.sdata2` | `0x803C56F0–0x803C5708` / `0x804B8950–0x804B8958` / `0x804BAA18–0x804BAA24` | vtable / `Min` literal / constants |

`lbl_804BAA20` is now 4 bytes so the point `.sdata2` split ends at its last constant. The stripe strategy starts at `0x80242820` with its constructor.

## Reference

The sources follow Whippy `src/nw4r/ef/ef_drawfreestrategy.cpp`, `ef_drawlinestrategy.cpp` and `ef_drawpointstrategy.cpp`. Each unit holds the constructor, `Draw` with `InitGraphics` inlined, and the weak destructor. Findings:

- The line width and point size use `std::min(42.5f, sx)`, which takes `const T&`. Binding the literal to a reference places 42.5f in `.sdata` (`0x804B8948`, `0x804B8950`) and loads it through its address. `ef_util.h` gains a reference-based `Min` to reproduce this; `ut::Min` takes values and loads the constant from `.sdata2` instead.
- `DrawInfo::GetViewMtxCooked` is now in `ef_drawinfo.h`, from Whippy; all three `Draw` functions inline it. `GXTransform.h` gains `GX_PROJECTION_SZ` (7).
- `DrawQuad` (`0x80241360`), which Whippy only declares, tests its texture flag as `texCoord & 1` (a `clrlwi.` by 31 bits) and writes positions through a file-local `GXPosition(const math::VEC3&)`, like the line and point units. The parameter is `int`.

## Remaining difference

`DrawFreeStrategy::Draw` scores 96.7%. The target compiles the switches of the inlined `GetViewMtxCooked` and `Particle::Draw_GetSizeX`/`Y` as binary searches and sets the texture flag through a branch. The same inlines compile as compare chains, in this unit and in the matching line and point units. Whippy records the same problem in a note on this function and leaves the unit `NonMatching`. Compiler versions Wii/1.0 to 1.7 and `-O4,s` do not change it.

## Traps

- **A reference-taking `min` moves its literal to `.sdata`.** If a float constant that should be in `.sdata2` is loaded with `addi` and `lfs 0(r3)` from `.sdata`, the original passed it by reference, as in `std::min`. Use a `const T&` helper, not a by-value one.
