# Issue 145: CMsgBalloon update virtuals and the vector iterator

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, objdump of the linked `build/wpse-v1/main.elf`, a full `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## F-145-1: the 22 functions of 0x800C7100-0x800C7EE4

Result: every function matches 100% in objdiff apart from relocation names of callees that other issues own or that are not yet named, and `Calc` and `UpdateBalloon`, whose only remaining rows are `.sdata`/`.sdata2`/`.data` literal names and one `mr r4, r31` that follows such a literal. Confidence high.

| Address | Function |
| --- | --- |
| `0x800C7100`, `0x800C7370` | `CMsgBalloon::CalcView` and `CalcMtx`: when `mCurrentEntry` exists and its stage value is not all ones, call the member of the same name on every `CPiiTex` in `mModelTextures` |
| `0x800C71A0`-`0x800C7300` | `vector<shared_ptr<CPiiTex> >::iterator`: default constructor, assignment, `vector::begin`, `vector::make_iterator`, `iterator(T* const&)`, `__vec_constructor::begin`, `shared_ptr::operator->`, `iterator::operator*`, `operator++`, `operator!=`, `operator==`, `base()` |
| `0x800C7310`, `0x800C7350` | `vector::end` and `__vec_constructor::end` |
| `0x800C7410` | `CMsgBalloon::Calc` |
| `0x800C7D40`, `0x800C7D70`, `0x800C7D80` | `list<Entry>::size() const`, `compressed_pair<unsigned long, allocator<Entry> >::first() const` and the storage `first() const` that it wraps |
| `0x800C7D90` | `CMsgBalloon::UpdateBalloon(BWorldCamera*, CLayout*, bool*)` (descriptive name) |
| `0x800C7EE0` | `nw4r::math::MTX44` default constructor |

`Calc` reads the current game map's world and camera, returns when `mEntries` is empty, advances the closing motion (`IsEnded(false)` then hides the layout), and, when shown and no other dialog (`GetCurrentDialog()`, slot 26 of `IGameMap`) is active, scores every entry for each of the four players. A player is skipped when its `CPlayerPii` is missing or has no HP (`CPiiPersonalData::GetHP`, offset 108). An entry is skipped when its `weak_ptr<BWorldObj>` locks to an invisible object (`BWorldObj::IsVisible`, then `GetStateFlags(1)`); a locked object adds its position to the entry's. The distance is measured from a point `0.33` units in front of the player, rotated by `0.71111 * rotation` through `SinCosFIdx`; the squared distance times `Entry::mDistanceScale` must be below `1.0f`. The winner either replaces the current entry (clearing bit 0 of the old entry's flags and copying the position to `mOffset`), or is kept. A balloon with a stage value shows the warp layout; one without shows `TEXT` and the `button` pane when the object's script engine has callback 1 (`CPpuEngine::HasCallback`). Pressing the player's controller trigger bit `0x20` sends message 1 with sender `pii->GetId()` and one integer argument 0 to the object's script engine (`CPpuEngine::SendMessage`, `0x80101CA0`).

### Order of statements that matters

- `mShowWait = 1` belongs inside the "new entry" block; `if (mHideWait != 1) mHideWait = 1` follows that block and runs on both paths (the `bne 0x800C7B6C` at `0x800C7898` jumps to it). Putting both in the block shares the constant register and scores 99.05%.
- The player test must be `pPii == nullptr || pPii->GetPersonalData()->GetHP() <= 0`. A bool flag, `!(a && b)` and `int` variants put the flag in a different register than the later `pNearest` and score 98.2% to 98.7%.
- `pBest = nullptr; VEC3 bestPosition; int bestIndex = -1;` in that order puts `li r29, -1` after the vector constructor call as in the target.
- The first motion test is `if (!IsEnded(false)) UpdateBalloon(...) else { SetVisible(false); reset }`; the reverse order swaps the two blocks.
- The warp title is passed as a nested call, `SetText("TITLE", GetMessageString(...))`; a local pointer lands in `r22` instead of `r31`.
- `pObject` for the `button` test needs its own scope so that its destructor runs before `SetPaneVisible`.
- `position.x * width / 2.0f` and `-position.y * height / 2.0f` give the target multiply operand order (`fmuls f0,f1,f0`); `* 0.5f` puts the constant first.

## F-145-2: the model textures are CPiiTex

`0x800C7100`, `0x800C7370` and `0x800C7410` call `CalcView__7CPiiTexFv` (`0x800F70F0`), `CalcMtx__7CPiiTexFv` (`0x800F70D0`) and `Calc__7CPiiTexFv` (`0x800F70B0`) on the element of the vector at `0x44`. The element type is therefore `shared_ptr<CPiiTex>`, not `shared_ptr<COffScreenTexture>` as the earlier inference from the vector constructor names had it. `CMsgBalloon.h` and the nine symbols of the constructor chain (`0x800C6A00`-`0x800C6BE0`, `0x800CA480`) are renamed; their code was already 100%.

## F-145-3: vector has a class iterator

The loops call out-of-line functions for the default constructor (`0x800C71A0`, empty), assignment (`0x800C71B0`, a store that returns `this`), `begin`, `end`, `!=` (which calls `==`), `*`, `++` and `base()`, and `begin`/`end` wrap the `__vec_constructor` pointer accessors (`0x800C7240`, `0x800C7350`) through `make_iterator` (`0x800C7200`) and an `iterator(T* const&)` constructor (`0x800C7230`). `include/vector` therefore declares `vector::iterator` as a nested class with those members; `const_iterator` stays `const T*`. `operator==` is `base() == rOther.base()`: the compiler evaluates the right operand first, which reproduces the target call order. Effects on other units, measured with `unitdiff` before and after:

- `BWorld.cpp` takes `&mObjEntries[0]` where it needed a pointer (`BWorld.h`, `BWorld.cpp`); `mObjEntries.begin()` as a pointer produced a stack temporary in `AddObject` (99.3% to 79.5%). `BWorld::~BWorld`, `RemoveAllEffects` and `Calc` improve (98.66, 94.93, 97.58 to 99.80, 98.92, 99.67).
- `SaveData.cpp` `SerializeVector` keeps a pointer loop (`rVector.begin().base()`) because the class iterator swaps two non-volatile registers and takes the six instances from 100% to 99.39%; the `DeserializeVector` and `DeserializeStruct` functions improve by one to four points with the class iterator.
- All other units, including the Matching ones, are unchanged; the full build links and `verify.py` passes.

## F-145-4: shared headers touched

- `compressed_pair::first() const` is an explicit function that calls the storage's `first() const` (`0x800C7D70` branches to `0x800C7D80`); the non-const `first()` and both `second()` keep `using Storage::...`, which matches the existing `cap()` and `alloc()` chains.
- `BWorldObj::IsVisible` calls `GetStateFlags(mask)`, as `0x800C7F90` calls `0x800C7FC0`.
- `CLayout` gains `GetViewRect() const` (the `Rect` at `0xC4`, `0x800C7EF0`), `UpdateAttachedLayouts()` and `DetachChildren()`; `BUI::RemoveChild`; `nw4r::lyt::Pane::SetTranslate`; `CPlayer::GetController`; `CPiiPersonalData::GetHP`; `CLayout::CMotionProgram::IsEnded(bool)`; `CPpuEngine::Message`, `HasCallback` and `SendMessage`.
- `CMsgBalloon::Entry` is completed to 0x2C bytes (F-145-1). The stage value is `u32`: comparing an `int` with `-1` compiles to `cmpwi`, while the target's `addis`/`cmplwi 0xFFFF` is the unsigned compare with `0xFFFFFFFF`.

## Remaining differences

- Relocation names of `GetViewRect`, `GetWidth`, `GetHeight`, `SetTranslate`, `GetHP`, the `list` iterator functions, `shared_ptr<BWorldObj>` conversions, `IsVisible`, `ClearFlags`, `SetupBattleWarp` (`0x800C8030`) and `SetupStageWarp` (`0x800C8760`) belong to the next blocks and keep their `fn_` names in `symbols.txt`.
- Emission order of the inline functions. The target places `size`/`first` after `Calc` and the rest (`GetViewRect` onward) after `UpdateBalloon`; the compiler here emits all of them after `Calc`. Order matters only for the final link of the whole file.
- The `.data` string pool: the target has two `"above_site"`/`"below_site"` pairs addressed as `0x803A5520`/`0x803A552C` (message balloon) and `0x803A5538`/`0x803A5544` (warp); the pooling order is for the file's last owner.

## Traps

- **A class iterator is invisible until `-inline` is off.** In a normal unit the iterator wrappers inline away and `vector::begin()` looks like a pointer; only under `#pragma dont_inline on` do `begin`, `end`, `!=` and `base` appear as functions. A pointer-typed `begin()` can still be 100% in one unit and wrong in another.
- **CodeWarrior compares an `int` and a `u32` with `-1` differently.** `x != -1` on `int` is `cmpwi r0, -1`; on `u32` it is `addis r0, r3, 1; cmplwi r0, 0xFFFF` and keeps the value in `r3`. The second form identifies unsigned sentinel fields.
- **A load-then-branch in a `&&` condition with a temporary allocates a flag register that can be shared.** The `li r25, 1 ... li r25, 0` flag in `Calc` shares its register with the later `pNearest` only for `a == nullptr || b <= 0`; other spellings of the same condition did not.
- **Constants merge across statements in a block.** `mShowWait = 1` and `mHideWait = 1` in one block use one `li`; the target's two `li r0, 1` mean two blocks.
- **A shared header edit moves `unitdiff` scores in unrelated units.** Compare per-function percentages of every unit that includes `vector` or `compressed_pair` (`report.json` of a baseline and of the new build) before landing; the aggregate matched-function count alone hid the `SaveData` loss.
