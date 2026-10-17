# Issue 293: HBM nw4hbm::lyt, block 1

## F-293-1: lyt_animation.cpp and lyt_arcResourceAccessor.cpp

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The HBM layout library starts at `0x80186340`, right after `HBMBase.cpp`. Its first two translation units are:

| File | `.text` | `.rodata` | `.data` |
| --- | --- | --- | --- |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_animation.cpp` | `0x80186340`–`0x80186EE8` (11 functions) | `0x80366E38`–`0x80366E58` | `0x803BB4E0`–`0x803BB560` |
| `RVL_SDK/hbm/nw4hbm/lyt/lyt_arcResourceAccessor.cpp` | `0x80186EF0`–`0x80187520` (12 functions) | none | `0x803BB560`–`0x803BB630` |

The eight bytes after `FindAnimationLink` are padding before the 16-byte-aligned `FindNameResource`. `lyt_arcResourceAccessor.cpp` runs five functions into #294's block. Those are the `MultiArcResourceAccessor` constructor, destructor, `Attach`, `GetResource` and `GetFont`, which end at `0x80187520`. The `Bounding` constructor at `0x80187520` starts the next unit.

Names and order follow CC0 xenoblade (`config/jp/symbols.txt`), whose `lyt_animation.cpp` and `lyt_arcResourceAccessor.cpp` have the same functions and sizes. The exception is the NAND build's `ArcResourceLink::Set` (`0x80187220`) and the `MultiArcResourceAccessor` members, which come from CC0 Kirby's Epic Yarn (`Swiftshine/key` `7f2a9ab9`, `src/hbm/nw4hbm/lyt`). The code follows key's sources, and every function matches.

Evidence for this library version:

- The `AnimTransformBasic` vtable (`0x803BB4E0`) has six slots: destructor, one `SetResource`, two `Bind` and two `Animate`. The newer `SetResource(..., u16)` overload in the imported header does not exist here and was removed.
- `AnimTransform` has no out-of-line constructor, destructor or vtable in the DOL, only its RTTI. Its constructor and destructor are inline, and its weak vtable is stripped at link.
- `Material` follows the older layout that TP's `nw4hbm` uses: `mName[20]` at `0x4`, `mAnimList` at `0x18`, `mTevCols` at `0x24`, `mTevKCols` at `0x3C`, `mGXMemCap` at `0x4C`, `mGXMemNum` at `0x50`, `mbUserAllocated` at `0x54` and `mpGXMem` at `0x58`. Its vtable has no `AnimResource` overloads. `Bind(Material*)` reads the name at `0x4`, and `Animate(u32, Material*)` reads the capacity and count fields at `0x4C` and `0x50`.
- The `AnimationLink` constructor clears `mbDisable` once, then sets only the transform and index (`SetAnimTransform`). The header's three-field `Set` was replaced.
- `ArcResourceAccessor` is 0xB0 bytes: `mArcHandle` at `0x4`, `mArcBuf` at `0x20`, the font list at `0x24` and `mResRootDir[128]` at `0x30`. `MultiArcResourceAccessor` is 0x1C bytes, with its archive list at `0x4` and its font list at `0x10`. `ArcResourceLink` is 0xA4 bytes: node, `ARCHandle` at `0x8` and `mResRootDir` at `0x24`. The font lookup is inlined into both `GetFont` functions, so key's separate `detail::FindFont` is an inline here.
- The `ArcResourceAccessor` destructor is weak. The linked copy is the one at `0x8017CA20` in `HBMBase.cpp`, referenced from the vtable at `0x803BB5B8`.

Headers: `lyt_resourceAccessor.h` and `lyt_arcResourceAccessor.h` are new. `lyt_animation.h` gained `res::StepKey`, `res::HermiteKey` and `res::AnimationTarget`. `OSFastCast.h` gained the SDK `OSf32tou8` (GQR2).

On the branch head, `configure.py`, `ninja` and `tools/verify/verify.py verify` exit 0 with both units `Matching`. objdiff reports all 23 functions and every `.text` and `.rodata` section at 100%. `lyt_arcResourceAccessor.cpp` `.data` is also at 100%. `lyt_animation.cpp` `.data` scores 37% only because the object carries the weak `AnimTransform` vtable, which the linker strips. The linked DOL is byte-identical.

Outside names used for linking, from the CC0 references and their call patterns:

- `ResourceAccessor` destructor (`0x8018DDA0`), constructor (`0x8018DDE0`) and `GetFont` (`0x8018DDF0`);
- `ut::detail::LinkListImpl` destructor (`0x80193780`), `Clear` (`0x80193860`) and `Insert` (`0x801938B0`);
- `Material::GetTexSRTAry` (`0x8018B200`), `GetIndTexSRTAry` (`0x8018B240`), `SetTextureNoWrap` (`0x8018B490`), `SetColorElement` (`0x8018B580`) and `AddAnimationLink` (`0x8018C6A0`);
- `Pane::AddAnimationLink` (`0x8018D4B0`) and `detail::EqualsMaterialName` (`0x801876D0`);
- the weak `ArcResourceAccessor` destructor (`0x8017CA20`) and MSL `strncpy` (`0x80345D20`).

## Traps

- **The imported `nw4hbm` headers are partly the newer library.** `Material` and the `AnimTransform` vtable had to be rolled back to the older layout. Match each class against the target's vtable and member accesses before reusing it. TP's `nw4hbm` headers describe this older `Material`. Evidence: F-293-1.
- **Placement new.** There is no `<new>` header. HBM sources define the inline placement `operator new` locally, as `HBMGUIManager.cpp` does.
