# Sprint s1: near-miss sweep of UI units

No unit flipped. Gains and what is left:

| Function | Before | After | Change |
| --- | --- | --- | --- |
| `CUIManager::SetFocusedUI` | 92.61% | 99.05% | `BUI* pOldUI = pOld.get();` and `BUI* pNewUI = pNew.get();` declared before the `if (pOld)` / `if (pNew)` tests and used inside the blocks |
| `UpdateHomeButton` | 89.55% | 95.81% | the first loop walks `pError`, `pCon`, `pRead`, `pFiltered` with `++` in the for header (strength reduction as in the target); the pad-error test is written as `== -2 \|\| ... \|\| == 0` with the filtered-status branch first; `dx`/`dy` divided first and clamped afterwards |
| `CTitleSession::RefreshSlots` | 99.64% | 99.83% | `mSlotTexts[i].assign(text)` instead of a walking `pSlotText` pointer |
| `CPiiPersonalData::Import` | 95.45% | 97.08% | `mCreationTime` assigned before `mRandomSeed` |
| `CUIList::OnInput` | 99.50% | 99.59% | `pButtons[n] & buttons` operand order; the two `IsSelectable` early exits as separate `if (...) break;` statements |

## Findings

- **Hoisting `get()` into a local before the bool test changes CSE.** The target of `SetFocusedUI` keeps the raw pointer in a non-volatile register from the `neg/or/srwi` truth test through the loop; writing `BUI* p = ptr.get();` before `if (ptr)` reproduces that. Putting it inside the block does not.
- **A for loop over four parallel arrays needs explicit pointer increments to be strength-reduced.** Indexing `a[i]`, `b[i]`, ... emitted `mulli`/`slwi` per iteration; incrementing four pointers in the loop header gave the target's four `addi` steps.
- **A positive `||` condition with the rare case in `else` moves the rare block out of line.** `if (e != -2 && ... && e != 0) { rare } else { common }` placed the rare block inline; swapping the blocks matched the target layout.

## Open

- `CUIList::OnInput`: the target keeps `mSelectedIndex` (r6), `mItems.size()` (r5) and the page quotient (r4) in the reverse order of ours in the page-down branch, and builds `-1` as `li r3,0; subi r0,r3,1`. Tried pre-loading `selected`, hoisting `pages`, expression forms: no change.
- `CUIManager::DispatchFocusedInput`, `BUI::AddChild`, `BUI::RemoveChild`: the target reloads `rChild->` after every store (no CSE) while ours merges the loads. `operator->` through `get()` did not change it.
- `errmsg` `ShowSystemMessage` / `Present`: ours spills the `reverse_iterator` loop variable; by-value `operator!=` is needed (const references are worse). A `last` local, or an inline helper holding the loop, does not change it.
- `errmsg` `GetFontMap`: the target calls `allocator::destroy` for the font pair out of line while ours inlines it; moving the creation into an inline helper did not help.
- `DrawHomeButtonNotice`: the target stores the `GXColor` local at 0x8 and the by-value copy at 0xc; ours is reversed. Declaration position, braces and initialiser form do not change it.
- `CPostEffect::Show`: the target re-materialises the literal address twice (`addi r3,r28,lo` and `addi r6,r28,lo`); ours keeps one merged address. Direct initialisation of the string did not change it.
- `CUIMenu::UpdateSelection`: the target re-extends the `u8` pane position (`clrlwi`) before each switch compare; `if`/`else` forms are worse.
- `Import`: a full permutation of the last six assignments caps at 97.08%.

## Traps

- Several sources mix LF and CRLF line endings (`CTitleSession.cpp`, `homebutton.cpp`, `errmsg.cpp`). Multi-line text replacements must match both.
