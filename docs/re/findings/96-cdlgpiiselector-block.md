# CDlgPiiSelector CDataPerPlayer helper block (#96)

Target `wpse-v1`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #96 lists 25 functions in `.text` `0x800962F0`-`0x80096954`. All belong to `game/CDlgPiiSelector.cpp` ([F-91-1](91-cdlgpiiselector.md), start moved by [F-91-4](91-pii-item-vector-block.md#f-91-4-the-block-starts-cdlgpiiselectorcpp-split-moved-to-0x80091360)), compiled without inlining ([F-92-1](92-cdlgpiiselector-construction.md#f-92-1-the-file-is-compiled-without-inlining)). No function of the block is emitted by the committed source, because every one is reached only from `CDataPerPlayer` code that is not reconstructed ([F-96-3](#f-96-3-why-nothing-is-emitted)). What this issue adds is the identity of every function, scratch-verified source for 13 of the 25 at 100% and one more (`SetFlag`) at 82.9%, the `CPiiPersonalData` accessors in `include/game/CPiiPersonalData.h`, and symbol names.

Slot numbers count from the first function word of a vtable, so slot `n` is at offset `8 + 4n`.

## F-96-1: identities of the 25 functions

| Address | Size | Function | Evidence |
| --- | --- | --- | --- |
| `0x800962F0` | 0x14 | `CPiiPersonalData::IsFainted() const`, `mHP <= 0` | `lwz 0x6C(r3)` is `mHP`; `cntlzw` then `rlwnm r3,1,r0,31,31` yields 1 for zero and for negative values, so the test is signed `<= 0`, not `== 0`. Callers: `0x800955C0` (after `CPii::GetPersonalData`, before clearing a text), `0x80096E20`, `0x80097CD0`, `0x8009A0E0`. Descriptive name, medium confidence |
| `0x80096310` | 0x7C | `std::wstring::clear()` | Tests the long flag (`0x80085AB0`), then stores a zero `wchar_t` (`sth`) at the character data and zero at the size, through the short representation (`0x80087A50` data, `0x80087C70` size) or the long one (`0x80087A90` data pointer, `0x80087AC0` size). Callers: `0x800955C0` and `0x800C84A0` (`CMsgBalloon`). `include/string` has no `clear` and gives `is_long` and the fields direct access, so this body needs the original out-of-line representation accessors; not added |
| `0x80096390` | 0x7C | Argument-tuple constructor of 20 bytes, `{VEC3, CDataPerPlayer*, bool}` | Called twice by `0x800955C0` with a copy of a `VEC3` at +0x14C or +0x158 of a UI object, `this`, and a small-data `bool`; the result (20 bytes) is copied and handed to `0x8009C620`, then connected to the callback lists at +0x174 and +0x178 of the `CUIPiiList` (`0x80078FB0`). It looks like the storage of a `std::tr1::bind` expression. The `bool` is never stored: `0x80096500` copies only the pointer word and `0x80096480` copies a word and a byte from the temporary, so the byte is uninitialized in the original. Not reproduced |
| `0x80096410` | 0x48 | Constructor of the 16-byte tail pair | `0x80096460` copies three words (the `VEC3`), `0x80096480` copies a word and a byte at +0x0C |
| `0x80096460`, `0x800964A0`, `0x800964E0` | 0x1C, 0x3C, 0x1C | 12-byte POD copy and assignment helpers | `0x800964A0` copies through a stack temporary into `0x800964E0`; all move words, not floats |
| `0x80096480`, `0x80096500` | 0x14, 0xC | Copy of a `{word, byte}` pair and of a single word | See `0x80096390` |
| `0x80096510` | 0x240 | `CDataPerPlayer::OnSelectionChanged()` | See [F-96-2](#f-96-2-the-selection-callback-scratch-verified-at-100). Not virtual: its address sits in a pointer-to-member triple `{0, -1, fn}` at `.data` `0x8039F61C`, next to the `CDataPerPlayer` string passed to the `CFiber` |
| `0x80096750` | 0x8 | `CPiiPersonalData::IsSpecHidden() const` | Tail call to `HasFlag(0x40000)` |
| `0x80096760` | 0x18 | `CPiiPersonalData::HasFlag(u32) const` | `(mFlags & mask) != 0` on the word at +8 |
| `0x80096780` | 0xC | `CPiiPersonalData::SetNewlyDisclosed(bool)` | Tail call to `SetFlag(0x80000, b)` |
| `0x80096790` | 0x30 | `CPiiPersonalData::SetFlag(u32, bool)` | Sets or clears the mask in +8 |
| `0x800967C0` | 0xC | `CPiiPersonalData::SetSpecHidden(bool)` | Tail call to `SetFlag(0x40000, b)` |
| `0x800967D0` | 0x8 | `CPiiPersonalData::IsFavorite() const` | Tail call to `HasFlag(2)`; flag 2 is the favourite flag of [F-173-3](173-cpiilistfiltersort.md#f-173-3-layouts-recovered-for-the-dependencies) |
| `0x800967E0`, `0x80096800` | 0x8 | `CUIList` accessors of the callback owners at +0x174 and +0x178 | `addi r3,r3,0x174` and `0x178`. A scratch `offsetof` check on `include/game/CUIList.h` gives `mSelectionCallbacks` at 0x174 and `mAcceptCallbacks` at 0x178, `sizeof(CUIList)` 0x180. Callers: `0x800955C0` and `0x801DE060`. Names not chosen; `CUIList.h` belongs to #337 |
| `0x800967F0` | 0xC | `CDataPerPlayer` setter that stores 3 in the word at +0xC | Second pointer-to-member triple in `.data`; the word at +0xC is zero in the constructor. No caller |
| `0x80096810` | 0x4 | `nw4r::math::VEC3::VEC3()` | Empty |
| `0x80096820`, `0x80096850` | 0x30, 0x1C | `VEC3::operator=` and `_VEC3::operator=` | Three `lfs`/`stfs` pairs |
| `0x80096870` | 0x54 | `Transform(const MTX34&, const VEC3&)` | Inline from `include/game/MathUtil.h`; also used by the camera adjuster units |
| `0x800968D0` | 0x74 | `nw4r::math::VEC3Transform(VEC3*, const MTX34*, const VEC3*)` | Inline from `include/nw4r/math/math_types.h`; calls `PSMTXMultVec`. The symbol stays `fn_800968D0`: `0x8023F120` (`ef_drawdirectionalstrategy`) already carries the mangled `VEC3Transform` name, and a second global of that name fails the link (see Traps) |
| `0x80096950` | 0x4 | `MTX34::operator MtxRefConst() const` | Already named |

The outside callees of `OnSelectionChanged` are named in `symbols.txt` from RTTI and mangling: `shared_ptr<CUIPiiList>::operator->` `0x80093F40`, `shared_ptr<IUIListItem>::operator bool` `0x80093F50`, `static_pointer_cast<CUIPiiListItem, IUIListItem>` `0x8008FE10`, `shared_ptr<CUIPiiListItem>::operator->` `0x8008BA60`, `CUIPiiListItem::GetPPD() const` `0x8008BA50` (returns the `shared_ptr<CPiiPersonalData>` at +8 by reference), the two `shared_ptr` destructors `0x8008F4B0` and `0x8007C440`, and `CPiiPersonalData::SetFavorite(bool)` `0x800ECA80`. `CUIPiiList` is the 0x180-byte class with vtable `0x8039E2A8`, named in RTTI (F-82).

## F-96-2: the selection callback, scratch-verified at 100%

`0x80096510` was reconstructed in a scratch copy of the unit, with temporary stand-ins for `CUIPiiListItem` (`GetPPD() const`), `CUIPiiList` (empty `CUIList` subclass) and a `CDataPerPlayer` padded to the real offsets, and with a temporary call from `CalcMtx` so that the anonymous-namespace member is emitted. After the callee renames above, `tools/re/unitdiff.py` reports 576 bytes at 100%. The scratch was discarded because `CUIPiiListItem` has no header yet (#217). The verified form:

```
void CDataPerPlayer::OnSelectionChanged() {
    if (!mSilent) {
        PlaySound(0x54);
    }
    std::tr1::shared_ptr<IUIListItem> selected = mpList->GetSelectedItem();
    if (selected) {
        std::tr1::shared_ptr<CUIPiiListItem> item = std::tr1::static_pointer_cast<CUIPiiListItem>(selected);
        if (item->GetPPD()->IsSpecHidden()) {
            item->GetPPD()->SetNewlyDisclosed(true);
            item->GetPPD()->SetSpecHidden(false);
            mpList->UpdateSelection();
        }
        IController* pController = IGameSession::GetInstance()->GetPlayer(mPlayer)->GetController();
        if (pController != nullptr && (pController->GetHold() & 0x800) && !(pController->GetTrig() & 0x800)) {
            if (item->GetPPD()->IsFavorite()) {
                if (!mFavorite) {
                    item->GetPPD()->SetFavorite(false);
                    PlaySound(0x5A);
                }
            } else if (mFavorite) {
                item->GetPPD()->SetFavorite(true);
                PlaySound(9);
            }
        }
        CPlayer* pPlayer = IGameSession::GetInstance()->GetPlayer(mPlayer);
        std::tr1::shared_ptr<CPiiPersonalData> pii = item->GetPPD();
        mpDlg->SetEntrancePPD(pPlayer->GetController(), pii);
    }
}
```

Members it fixes in `CDataPerPlayer` (0x450 bytes, vtable at 0x448): `IDlgPiiSelectorInternal*` at 0 (slot 11 is `SetEntrancePPD`), the player index at 8, a `bool` at 0x10 (favourite drag mode: the callback makes the selected Pii's favourite state follow it), `bool` at 0x51 (silent: no cursor sound), `shared_ptr<CUIPiiList>` at 0xA4. `UpdateSelection` is `CUIList` slot 13 (offset `0x3C`), the button mask 0x800 is Wii Remote A, and `GetHold` and `GetTrig` are `IUIInput` slots 3 and 4. The copy `pii` is a real local: passing `item->GetPPD()` directly omits the `shared_ptr` copy constructor call and its destructor.

Two layout facts that held in the scratch: the virtual destructor must be declared after the data members, because CodeWarrior places the vtable pointer where the first virtual function is declared (the `CFiber` at 0xD8 does the same, which is why the vtable pointer sits at 0x448); and `static_pointer_cast` on `shared_ptr<IUIListItem>` produces the `0xF8`-byte call at `0x8008FE10`.

## F-96-3: why nothing is emitted

The 25 functions have these only callers, none reconstructed:

- `0x800955C0` (#95, the 0xB74-byte `CDataPerPlayer` setup): `IsFainted`, `wstring::clear`, the tuple group, the `CUIList` accessors, `VEC3`, `operator=` and `Transform`.
- `0x80096E20`, `0x80097CD0`, `0x8009A0E0` (#97 to #110): `IsFainted`, `IsFavorite`.
- `OnSelectionChanged` and `0x800967F0` have no caller; they are named only by the pointer-to-member constants consumed in `0x800955C0`.
- `Transform`, `VEC3::operator=` and `VEC3Transform` are also used by `0x80098480` and later units, whose first copies the linker keeps from this unit.

`0x800955C0` and the constructor `0x800952B0` need `CDataPerPlayer` members whose types are not recovered (the 0x14-byte member at 0x14, the `shared_ptr` members at 0x54, 0x5C and 0x64, `CUIPiiSpec`, the 0x13C-byte class at 0xAC), `CUIPiiListItem` (#217), `CUIManager` (#327, #328) and `IGameSession` slot 40 (#126), so no function can be reached without inventing a call site.

Committed in this pull request: the `CPiiPersonalData` accessors and flag enum, evidenced by F-173-3, and the symbol names. Scratch results with the accessors called from a temporary function: `IsFainted`, `IsFavorite`, `IsSpecHidden`, `SetSpecHidden`, `SetNewlyDisclosed`, `HasFlag`, `VEC3::VEC3`, `VEC3::operator=`, `_VEC3::operator=`, `Transform` and `VEC3Transform` (under its mangled name, in a scratch rename that was not kept) at 100%; `SetFlag` at 82.9%.

## F-96-4: SetFlag branch shape

The target `SetFlag` (and a second copy at `0x80098780` for a flags word at +0xC, a neighbouring class) compiles `if (enable) set else clear` as `cmpwi r5,0; beq +8; b set; b clear; set: ... blr; clear: ... blr`. Plain `if/else`, an early return, `?:`, a `switch`, `goto`, `enable == true`, `enable == false`, reversed tests, `mFlags = mFlags | flag` and separate inline helpers all give 82.9% or less (the same instructions without the two extra `b`). The cause is not found. The header keeps the plain `if/else`, so `SetFlag` is the one function of this block that does not reach 100%.

## Traps

- **A signed compare compiles to `cntlzw` plus `rlwnm`.** `cntlzw r0,x; li r3,1; rlwnm r3,r3,r0,31,31` is `x <= 0`, not `x == 0`, which is `cntlzw` then `srwi r3,r0,5`. A first guess of `mHP == 0` scored 68%.
- **`unitdiff.py` only pairs symbols by name.** Target functions still named `fn_XXXXXXXX` appear as missing in the target and as "only in source" for the scratch code, so a correct body scores nothing. Rename the target symbol to the mangled name of the intended member first, and rename the unnamed callees as well, or relocation names lower an exact body to about 99%.
- **Git Bash `sed -i` removed the carriage returns again** from `src/game/CDlgPiiSelector.cpp` in a scratch edit. The scratch copy was restored from a backup before committing; edit with the editor tool or Python with `newline=''`.
- **An unreferenced anonymous-namespace member is not emitted**, so a scratch needs a temporary caller in an emitted function ([F-93-4](93-cdlgpiiselector-calc-block.md#f-93-4-calcview-calcmtx-and-their-helpers)); this one used a call from `CalcMtx`, discarded afterwards.
- **Two weak instances of one inline function cannot both take its mangled name.** Renaming `0x800968D0` to `VEC3Transform__Q24nw4r4mathFPQ34nw4r4math4VEC3PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` failed the link with `multiply-defined ... in ef_drawdirectionalstrategy.o, previously defined in CDlgPiiSelector.o`, because `0x8023F120` already has that name and the two copies differ (0x74 and 0x3C bytes, compiled with different inlining). A global `sed` over `symbols.txt` also renamed that other symbol; rename by anchored address, and check `grep` for the old name at other addresses first.
- **A `shared_ptr` argument may be a local copy.** Passing `item->GetPPD()` straight to a `const shared_ptr&` parameter omitted the copy constructor and destructor the original has; a named local reproduces them.
- **`dtk` rewrote `config/wpse-v1/splits.txt` during the scratch builds** (it dropped the `CUIList.cpp` block after a rebase). Revert unrelated rewrites before committing.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, a `bl` scan of its full disassembly for callers, the generated assembly under `build/wpse-v1/asm/game/CDlgPiiSelector.s`, and Wii/1.1 with `cflags_game` through `ninja` and `tools/re/unitdiff.py`. IDA was unavailable (connection refused); no decompiler output was used.
