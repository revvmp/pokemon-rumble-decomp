# Issue 174: CPiiListFilterSort.cpp, 0x800E7A50 to 0x800EA634

Target `wpse-v1`. The block continues the unit of [F-173-1](173-cpiilistfiltersort.md); no new file boundary was needed, and the unit stays `NonMatching`.

## F-174-1: reconstructed functions

| Address | Function | Evidence |
| --- | --- | --- |
| `0x800E7A50` | `HasPrefixedItem(const vector<shared_ptr<CUIPiiListItem>>&)` | Loop over 8-byte elements (`begin` at 0, size at 4), reads `CUIPiiListItem` 8 then `CPiiPersonalData` 0x34 (`Prefix`), returns 1 on the first non-zero value |
| `0x800E7AA0` | `HasItemWithPrefix(const vector<...>&, int)` | Same loop, returns 1 on the first `Prefix` equal to the argument |
| `0x800E9300` | `IsNameBefore(const wchar_t* const&, const wchar_t* const&)` | Dereferences both arguments and returns `CompareCollation(a, b) < 0`; its address is the comparator passed to the sort at `0x800E9B70` |

All three score 100% in objdiff and are named in `symbols.txt` with the mangled name the source produces. The two item scans are called by the sort menu `0x800E7BA0` (disabling entries 3 and 4 when no item has a prefix or a matching owner) and by the prefix menu `0x800E9330` (entries 0 to 43, one per prefix). They have external linkage because the target emits them out of line; with internal linkage CodeWarrior drops them when nothing calls them.

`CPiiPersonalData` gains `GetPrefix` and `GetOriginalTrainerId`. The constructor of `CUIPiiListItem` is #217's.

## F-174-2: what remains in the block

| Address | Role | Blocker |
| --- | --- | --- |
| `0x800E7AF0` | True when the list holds items both with and without the owner's `OyaID` (compares word 0x10 of the `IGameSession` slot 25 object, `this + 0x28`) | `IGameSession` slot 25 undeclared (F-117-3) and the type of that object unknown |
| `0x800E7BA0`, `0x800E8020` | Sort menu and filter menu: build the `CUIMenu` item table with `GetMessageString`, call `0x8011D090` (the `CUIMenu` constructor, #217) and `0x801DD850` | `CUIMenu`, `CUIMenuItem` and the pause-style menu runner are undeclared |
| `0x800E8AB0`, `0x800E8CE0`, `0x800E8D90`, `0x800E8E60`, `0x800E8FC0`, `0x800E9070`, `0x800E91D0`, `0x800E9690`, `0x800E98B0`, `0x800E9960`, `0x800E9A20` | `function_imp` allocation, copy, clone and destroy for the two `bind(&CPiiListFilterSort::..., this, _1, ...)` callbacks (types in the `.data` RTTI names) | Emitted only by the menus above |
| `0x800E8000` | `function<void(const CPiiListFilterSort*)>::operator int bool_type::*`. A scratch use of the current `include/functional` produced the same four instructions (`lwz`, `neg`, `or`, `srwi`) | Inlined in a small caller; the target keeps it out of line only inside `0x800E9330` |
| `0x800E9330` | Prefix menu | As the menus |
| `0x800E9B70`, `0x800E9FF0`, `0x800EA470` | Introsort and insertion sort of 8-byte `(name, index)` records with `IsNameBefore` | Needs the record type from the menu |
| `0x800EA5F0`, `0x800EA610`, `0x800EA620` | `vector` constructor, `begin` and `end` out of line | Same record type |

## Traps

- **A file-local helper needs external linkage to survive.** Wrapping `HasPrefixedItem` in an anonymous namespace made the compiler drop it because the unit had no caller, so objdiff reported 0 functions. The target's helpers are out of line, so they are not `static`.
- **`unitdiff.py` needs `objdiff.json`.** In a fresh worktree run `python configure.py` once first.
- **`ninja` rewrites `splits.txt` stale.** After building, `git checkout config/wpse-v1/splits.txt` unless your issue changed it.
