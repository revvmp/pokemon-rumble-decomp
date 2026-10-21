# F-47-1: BWorld calc, view and draw functions, tree templates and deleters

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Question from issue #47: what are the 24 functions in `0x80062980`–`0x80064BA4`, and how much of them can be reconstructed from source inside `src/game/BWorld.cpp` (unit boundaries are F-46-1).

## Result

The block is the tail of `BWorld.cpp`'s virtual interface plus the template instances emitted after it. `src/game/BWorld.cpp` now holds `CalcMtx`, `CalcView`, `Draw`, the private `DrawShadow` and the two file-local GX helpers; `include/MSL_CPP/detail/tree.h` and `include/map` now define `find_leaf` and `insert_node_at` for the two red-black tree instances. All names below are in `config/wpse-v1/symbols.txt`.

| Address | Identity | State (`tools/re/unitdiff.py`) |
| --- | --- | --- |
| `0x80062980` | `BWorld::CalcMtx` (vtable slot `0x10`) | 99.31%, 2 instruction rows differ |
| `0x80062C20` | `FrameStepScope::~FrameStepScope`, weak | 100% |
| `0x80062C70` | `BWorldObj::CalcMtx`, weak | 100% |
| `0x80062C80` | `BWorld::CalcView` (slot `0x14`) | 100% |
| `0x80062D20` | `BWorld::Draw` (slot `0x18`) | instruction-exact, only `.sdata2` constant names differ |
| `0x800633D0` | `DrawFrame(int, int)`, file-local | instruction-exact apart from constant names |
| `0x800636B0` | `DrawTexture(int, int, const GXTexObj*)`, file-local | instruction-exact apart from constant names |
| `0x800639F0` | `BWorld::DrawShadow(GXColor, GXPixelFmt)` | 98.31%, 14 rows (stack slot order of colour temporaries) |
| `0x80063DC0`, `0x80063DD0` | `ICalcDraw::CalcMtx`, `ICalcDraw::CalcView`, weak | 100% |
| `0x80063DE0` | `map_compact_tree<pair<const ulong, CEffect*>>::insert_node_at` | 99.67%, 3 rows |
| `0x80063FB0` | `map_compact_tree::destroy` | 100% |
| `0x80064240` | `tree_imp<shared_ptr<BWorldObj>>::insert_node_at` | 99.68%, 3 rows |
| `0x80064420` | `tree_imp<shared_ptr<BWorldObj>>::destroy` | 100% |
| `0x80064840` | `__vec_deleter<void*>` destructor | 100% |
| `0x800648B0` | `__vec_deleter<BWorld::ObjEntry>` destructor | 100% |
| `0x80064980`, `0x800649D0` | `shared_ptr_deleter<CEffect>` `get_deleter`, `dispose` | 100% |
| `0x80064A70`, `0x80064AC0` | `shared_ptr_deleter<CKDTree<BModelObj>>` `get_deleter`, `dispose` | 100% |
| `0x80064B40`, `0x80064B90` | `shared_ptr_deleter<ScnRoot, void (*)(G3dObj*)>` `get_deleter`, `dispose` | 100% |
| `0x80064A00`, `0x80064A50` | `shared_ptr_deleter<AnmScnRes, void (*)(G3dObj*)>` `get_deleter`, `dispose` | not emitted: only the scene-animation loader at `0x8005EB70` (#45) instantiates `shared_ptr<AnmScnRes>` |

"Only constant names differ" means every differing objdiff row compares an `lbl_` or `.sdata2` target symbol with the compiler's `@N` local. Those names cannot match before the whole unit is complete, because they depend on the order of every constant in the unit.

Supporting renames in `symbols.txt` (callees and data these functions reference): `tree_balance_after_insert` (`0x80059AF0`), `DrawOutline` (`0x8005E560`, called from `Draw`), `CaptureTexture::CopyFromEFB` (`0x801C4C20`, `0x801C4D70`), both `weak_ptr::lock` instances (`0x80064EB0`, `0x80064FE0`), `allocator::destroy` (`0x80065D00`), both `find_leaf` instances, the eight `__vt__` objects at `0x80399A14`–`0x80399E04` and the four `default_delete` RTTI objects `0x804B5B50`–`0x804B5B68`.

## F-47-2: structure recovered from the code

- **`BWorld` virtual slots.** The vtable at `0x80399DA0` is `~BWorld` `0x8005C9F0`, `Calc` `0x80062200`, `CalcMtx` `0x80062980`, `CalcView` `0x80062C80`, `Draw` `0x80062D20`.
- **Frame step.** `gFrameStep` (`.sbss` `0x804B910C`) is saved and replaced by `mPendingFrames` for the duration of `CalcMtx` and `CalcView`, and restored by an inline guard whose out-of-line destructor is `0x80062C20`. The guard is `FrameStepScope` in `include/game/FrameStep.h`; its constructor must only save the old value, because moving the assignment into the constructor changes the load order in `CalcView`. `CalcMtx` also sets `FrameCtrl::smBaseUpdateRate` to `60.0f * gFrameStep / 60.0f` (the multiply and divide are both in the code).
- **Scene root.** `CalcMtx` and each pass of `Draw` empty `mScnRoot` through a helper (inlining it fixed a register assignment that no declaration order could), then add `mModelGroup` and `mOutlineGroup`. `ScnRoot` has no public accessor for its scene animation, so `ScnRoot::GetAnmScn` was added; the game updates its frame without calling `ScnRoot::UpdateFrame`, which has no symbol in the executable.
- **Draw types.** `Draw` tests `mDrawType` in `1`, `3`, `4` with a `switch` whose `default` comes first: the range test for 3 and 4 is emitted before the test for 1, which no `||` chain reproduces.
- **Effects.** The `if (mEffectData)` test must go through a small inline `HasEffectData()` to reproduce the second load of `mEffectData` that the original performs; the same change made `RemoveAllEffects` instruction-exact.
- **Light projection.** For frustum and perspective cameras `Draw` builds the projection texture matrix with `C_MTXLightFrustum` and `C_MTXLightPerspective` and the constants `0.5f`, `-0.5f`, `0.5f`, `0.5f`, reading the `CameraData` fields into locals in reverse parameter order to get the original load order, then copies the matrix into `nw4r::ef::DrawInfo`.
- **Shadow maps.** `DrawShadow` renders the shadow camera into `CShadowCamera`'s three `CaptureTexture` levels. Each level is drawn from the previous one with `DrawTexture`, and the last gets `DrawFrame`. Clear colours go through an inline `SetCopyClear(GXColor, GXPixelFmt)` that passes `0xFFFFFF` as depth, and flat-colour GX state goes through an inline `SetupFlatColor(nw4r::ut::Color)`; both take their colour by value, which is why the original stores each colour twice. The two GX helpers take `int` sizes, not `u16`: the callers zero-extend and the helpers convert with the signed `xoris` sequence.
- **Tree templates.** The red-black tree keeps its node allocator at offset 4, in a `compressed_pair` with the root pointer, because `insert_node_at` passes `this + 4` to an `alloc_ptr<node, node_allocator&>`. Node construction places `parent_` with `new (&pNode->parent_) unsigned long(...)`, which produces the null check and exception marker in the original. `length_error("tree::insert length error")` is thrown from a static inline helper, not inline: the helper form reproduces the frame and flag bytes. `std::pair` has a user-provided copy constructor, which adds an exception marker between the two member copies.
- **`CKDTree`.** Its `shared_ptr` is held inside a nested struct; with a bare member the inlined destructor has one null check fewer than the original at `0x80064AC0`.
- **Vector iterators.** `vector<T>::iterator` is a class for non-POD `T` (a pointer for POD `T`). With a plain pointer the loops over `vector<BWorld::ObjEntry>` lack the stack copies of the begin and end iterators that the original stores in `__dt__6BWorldFv`, `Calc` and `CalcMtx`; with the class they match. The class converts to `T*` so existing pointer users still compile, and `BWorld` takes the entry base with `&mObjEntries[0]`.

## F-47-3: remaining differences

- `CalcMtx` and `Calc`: the first skip-state bit is emitted as `li r0, 0` then `ori r30, r0, 2` (zero materialised in `r0`), where the source form tried here (`states |= 2` in an inline function, `states = states | 2`, a conditional expression, an inline `AddState`, `int`/`u8`/`u16` types) gives `ori r30, r30, 2` or folds to `li r30, 2`. Cause unknown.
- `DrawShadow`: the same instructions, but the stack slots of the colour temporaries are ordered differently (the original places the two zero-colour temporaries below the two GX channel-colour copies). Using `GXColor()`, named locals, `nw4r::ut::Color(0)` temporaries or a different parameter order for the inline helper did not change it.
- `insert_node_at` (both): the thrown `length_error` temporary is addressed from the stack pointer in the original and from a copy of its address in the rebuild (three rows).
- `0x80064A00`, `0x80064A50`: need the `AnmScnRes` loader (#45).

## Traps

- **A changed `vector::iterator` moved other units.** Making the iterator a class for non-POD `T` lowered six `SerializeVector` instances in `SaveData.cpp` from 100% to 99.39% (register swap) until their loops took `T*` from `begin()`. The same change raised `BModelObj` and `BWorld`. Check `build/wpse-v1/report.json` for every unit after touching `include/vector`, `include/utility`, `include/map` or `include/MSL_CPP/detail/tree.h`, and compare against a build of the same base, not against a newer seed (CEffect looked 57 points worse only because the seed was newer).
- **Boolean helpers can change CSE.** Wrapping the shared-pointer test in an inline member (`HasEffectData`) reproduced a reload the original has; a boolean-returning inline for the draw-type test instead introduced `bool` materialisation. Try the helper form for tests of members, and the in-place `switch` for enumerations.
- **A range test followed by a single test is a `switch`.** A compare order of `3 || 4` then `1` is the signature of a `switch`, not of an `||` chain in any order.
- **`sed -i` rewrites line endings.** Editing `BWorld.cpp` with `sed -i` converted it to LF; restore CRLF before committing.
