# Issue 287: HBM homebutton core, block 1

## F-287-1: HBM compiler, flags and first unit boundaries

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The block `0x80178530`–`0x80179124` starts the Home Button Menu library (`homebuttonLib`). RTTI name strings in `.data` (`homebutton::FrameController` at `0x803B9FF0`, `homebutton::GroupAnmController` at `0x803BA028`, then `homebutton::gui::PaneComponent`, `PaneManager`, `Manager`, `Component`, `Interface`) give the classes and the file order `HBMFrameController.cpp`, `HBMAnmController.cpp`, `HBMGUIManager.cpp`.

Unit boundaries, from the vtable at `0x803BA018` (`GroupAnmController`: destructor `0x80178700`, `calc` `0x80178590`) and the RTTI records:

| File | `.text` | `.rodata` | `.data` |
| --- | --- | --- | --- |
| `HBMFrameController.cpp` | `0x80178530`–`0x801786DC` | `0x80366A40`–`0x80366A48` (`1.0f`) | `0x803B9FF0`–`0x803BA018` (class name, `__RTTI__Q210homebutton15FrameController`) |
| `HBMAnmController.cpp` | `0x801786E0`–`0x801787F0` | none | `0x803BA018`–`0x803BA060` (vtable, class name, base list, RTTI) |
| `HBMGUIManager.cpp` | starts at `0x801787F0` (`drawLine_`) | starts at `0x80366A48` | starts at `0x803BA060` (`PaneComponent` vtable) |

`FrameController` has no vtable in the DOL: no `FrameController` object is constructed out of line, so only its RTTI survives. The weak copies of its vtable, destructor and RTTI that each translation unit emits are dead-stripped at link.

Flags: `-lang=c++ -ipa file -fp_contract off -func_align 16 -sdata 0 -sdata2 0 -RTTI on` over the project base flags. Small-data is off: `1.0f` is loaded with `lis`/`lfs` from `.rodata`, not through `r2`. RTTI is on: every class has an RTTI record in `.data`.

Compiler: the six functions compile byte-identically with Wii/1.0, 1.0RC1, 1.0a, 1.1 and 1.3, so the code does not discriminate. The HBM build banner `0x4201_142` (F-0007) is Wii/1.0a (`Ver 4.2, build 142` in the compiler bundle's `info.txt`), so the library uses Wii/1.0a. Confidence: high for the flags, medium for the exact compiler until a discriminating function is matched.

With these settings both units match: objdiff 3.8.2 reports all six functions at 100%, and `tools/verify/verify.py verify` passes with both units `Matching`. The `.data` section scores below 100% in objdiff only because of the weak `FrameController` records described above.

Names: `init`, `initFrame`, `calc`, the `GroupAnmController` constructor, destructor and `do_calc` follow the public HBM reconstructions (CC0 xenoblade `ea25691b`, `libs/RVL_SDK/src/revolution/hbm`), whose function order and member offsets match the target exactly. Game code at `0x801B1580`, called from every deleting destructor in this block, is the global `operator delete` (`__dl__FPv`).

## F-287-2: Home Button Menu layout library version

The `nw4hbm` headers were imported from xenoblade (CC0, `ea25691b`), stripped of comments, and changed where this target differs:

- `nw4hbm::lyt::Pane` follows the older layout. The `Bounding` vtable at `0x803BB630` is `0x64` bytes, so the last two virtual functions are `GetMaterial` at `0x5C` and `LoadMtx` at `0x60`; xenoblade's `GetMaterialNum` and indexed `GetMaterial` do not exist here. The recursive visibility test at `0x80179D50` reads `mpParent` at `0xC` and the flag byte at `0xCF`, which matches the member order `mName[16]` (`0xB4`), `mUserData[8]` (`0xC4`), `mBasePosition`, `mAlpha`, `mGlbAlpha`, `mFlag` (`0xCF`), `mbUserAllocated` (`0xD0`) also used by the CC0 TP-derived dusklight source (`34a144a4`, `libs/revolution/src/homebuttonLib/nw4hbm/lyt/pane.h`).
- Text, font, window, picture, material, layout and resource-accessor headers were left out until a unit that uses them validates them against this version.
- `std::` maths wrappers whose C library declarations do not exist in this repository were left out of `math_arithmetic.h` and `math_triangular.h`.

Validated against this target so far: `Pane` vtable through `SetAnimationEnable` (`0x58`), `Group::mPaneLinkList` at `0xC`, `PaneLink::mTarget` at `0x8`, `AnimTransform` frame at `0x10`, and the `ut::LinkList` iterator. Other declarations are reference-derived and unverified.

The shared SDK headers gained real SDK declarations used by these headers: `OSu16tof32`, `OSs16tof32`, `OSf32tos16` inline fast casts in `revolution/os/OSFastCast.h`, `PSMTXConcatArray`, `PSMTXInvXpose`, `PSVECSquareDistance` and `C_MTXLookAt` in `revolution/mtx.h`, and a C++ `nullptr` definition in `revolution/types.h`.

## F-287-3: HBMGUIManager.cpp is one unit from 0x801787F0 to 0x8017A008

`homebutton::gui` (`HBMGUIManager.cpp`) owns `.text` `0x801787F0`–`0x8017A008` (56 functions), `.rodata` `0x80366A48`–`0x80366A58` (`0.0f`, the red `GXColor`, `0.5f`, and `drawLine_`'s unused local `cubeScale` `1.0f`), `.data` `0x803BA060`–`0x803BA2D0` (vtables and RTTI of `PaneComponent`, `PaneManager`, `Manager`, `Component`, and RTTI of `Interface`) and `.bss` `0x803E99E0`–`0x803E99E4` (`PaneManager::suIDCounter`). The unit runs through all of #288's block and the first 13 functions of #289's block (`is_visible` at `0x80179D50` to `PaneManager::setDrawInfo` at `0x8017A000`). `0x8017A010` starts the next unit, which uses WPAD and the controller array at `0x803E9B70` (`HBMController.cpp`).

Class layouts, from the vtables at `0x803BA060`, `0x803BA108`, `0x803BA1A8` and `0x803BA218` and the inlined `PaneComponent` constructor in `walkInChildren`:

- `Interface` (vtable only): `create`, `init`, `calc`, `draw(Mtx&)`, `draw()`, destructor.
- `Component` (0x2C bytes): `mabPointed[8]` `0x4`, `mDragStartPos` `0xC`, `mbDragging` `0x18`, `muDraggingButton` `0x1C`, `muID` `0x20`, `mbTriggerTarger` `0x24`, `mpManager` `0x28`; virtual functions from `getID` (`0x20`) to the pure `contain` (`0x5C`). The constructor does not initialise `mDragStartPos`.
- `PaneComponent` (0x30 bytes) adds `mpPane` `0x2C`, `setPane` (`0x60`) and `getPane` (`0x64`).
- `Manager` (0x18 bytes): `mpEventHandler` `0x4`, `mIDToComponent` (`ut::List`) `0x8`, `mpAllocator` `0x14`. Unlike the TP reference, `delComponent` is virtual (slot `0x24`), between `addComponent` and `getComponent`.
- `PaneManager`: `mPaneToComponent` `0x18`, `mpDrawInfo` `0x24`. `addLayoutScene`, `delLayoutScene` and `walkInChildrenDel` are virtual (slots `0x44`, `0x48`, `0x60`). The trailing `muNumPoint` is reference-derived and not observed.

The source follows the CC0 TP (`zeldaret/tp`, `libs/revolution/src/homebuttonLib/HBMGUIManager.cpp`) and xenoblade reconstructions, which agree with this target in function order. Two target-specific details were needed: `GXSetTevOp` uses `GX_PASSCLR`, and `setAllBoundingBoxComponentTriggerTarget` matches only with `#pragma opt_propagation off`, as in the TP source (16 bytes longer without it). With Wii/1.0a and `cflags_hbm` all 56 functions are 100% in objdiff and the full DOL verifies with the unit `Matching`.

`.data` scores 66% in objdiff because the object also carries the weak `Interface` vtable and destructor, which no linked code references and the linker strips; the linked DOL is byte-identical.

Outside names used by this unit, from call-site argument use and the reference source: `nw4hbm::ut::List_Append` (`0x80193900`), `List_Remove` (`0x80193970`), `List_GetNext` (`0x801939E0`), `List_GetNth` (`0x80193A00`), `nw4hbm::lyt::Pane::GetPaneRect` (`0x8018CB00`), the `typeInfo` statics of `Bounding` (`0x803E9BC0`), `Pane` (`0x803E9BF8`), `Picture` (`0x803E9C00`) and `Window` (`0x803E9C10`), `MEMAllocFromAllocator` (`0x802E1B90`), `MEMFreeToAllocator` (`0x802E1BA0`), `PSMTXInverse` (`0x802C8D50`), `PSMTXTrans` (`0x802C90B0`), `GXClearVtxDesc` (`0x802CBCD0`), `GXSetVtxDesc` (`0x802CB6C0`), `GXSetVtxAttrFmt` (`0x802CBD10`), `GXSetNumTexGens` (`0x802CC600`), `GXLoadPosMtxImm` (`0x802D0BB0`), and the game's global `operator new` (`__nw__FUl`, `0x801B1450`).

`nw4hbm` headers added for this unit: `lyt_layout.h` (validated: `mpRootPane` at `0x10`), `lyt_picture.h`, `lyt_bounding.h`, `lyt_window.h`, `lyt_material.h`, `lyt_texMap.h` and `lyt_drawInfo.h`, from xenoblade. `Window` was changed to this target's vtable, which has six virtual functions after `LoadMtx` (`0x64`–`0x78`): xenoblade's `GetMaterialNum` and indexed `GetMaterial` were removed and its `FindAnimationLinkSelf` is the `FindAnimationLink` override. Only the RTTI of `Picture`, `Window` and `Bounding` is exercised here; their other members are unverified.

## Traps

- **The xenoblade `nw4hbm` headers describe a newer layout library.** Trigger: its `Pane` declares `GetMaterialNum` and pads to the flag byte. This target's `Pane` vtable is `0x64` bytes and its members follow the older order. Check each imported class against target vtables and accesses before relying on it. Evidence: F-287-2.
- **HBM code does not discriminate the compiler.** Trigger: FrameController and GroupAnmController compile identically under every Wii compiler. The banner gives Wii/1.0a; re-check when a larger HBM function matches. Evidence: F-287-1.
- **CodeWarrior keeps an unused local `static const`.** `drawLine_`'s `cubeScale` is never read but occupies `0x80366A54` in `.rodata`; removing it shortens the unit's `.rodata` by four bytes. Evidence: F-287-3.
- **SDK header order matters for `nw4hbm`.** `revolution/mem/heapCommon.h` defines a `RoundUp` macro that breaks `nw4hbm/ut/ut_algorithm.h`, and `revolution/kpad.h` needs `revolution/mem.h` first for `OSThreadQueue`. Include `nw4hbm` headers before SDK headers, and `mem.h` before `kpad.h`. Evidence: F-287-3.
