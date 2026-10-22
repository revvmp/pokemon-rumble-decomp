# Sprint a18: UI framework, CLayout, CUIList, CUIMenu, homebutton, text layout

## Units added or extended

| Unit | Range | Status |
| --- | --- | --- |
| `game/BUI.cpp` | `0x801C8700-0x801C9680` | NonMatching, 17 of 20 functions at 100% |
| `game/CUIManager.cpp` | `0x801C9680-0x801C9EE0` | NonMatching, SetFocusedUI and DispatchFocusedInput near 100% |
| `game/BUILayout.cpp` | `0x801C9EE0-0x801CA130` | Matching |
| `game/CArchiveFontWithBuf.cpp` | `0x801CA130-0x801CA444` | Matching |
| `game/CUIMenu.cpp` | `0x801DC6C0-0x801DF650` | NonMatching, CUIMenuItem and CUIMenu methods |
| `game/errmsg.cpp` | `0x801DF650-0x801E2110` | NonMatching, PaneRectangle helpers only |
| `game/homebutton.cpp` | `0x801E2110-0x801E5D98` | NonMatching, 44 of 51 functions at 100% |
| `game/TextLayout.cpp` | `0x801E5DA0-0x801E66FC` | NonMatching, WrapText at about 98% |

## Traps

- **`#pragma dont_inline on` also stops the callee inlines.** A function that must stay out of line while its own calls still inline needs `#pragma auto_inline off` around its definition. `homebutton.cpp` InitializeSoundData matched only with `auto_inline off`; `dont_inline on` expanded every shared_ptr helper into a call.
- **An exception-table SPECIFICATION action means the function itself is `throw()`.** The extab comment `Type: SPECIFICATION` with empty types, a frame pointer (`mr r31, r1`) and a trailing `bl __unexpected` is reproduced by declaring the function `throw()` (CLayout::LoadLayout).
- **Renaming a local weak helper to a name that exists at another address breaks the DOL.** dtk emits references by symbol name, so an object that defines `N` locally and also calls the external `N` binds the call to its own copy. Keep the original unique `fn_`/`dtor_` name for the duplicate address, or the verification fails at the first such call.
- **Weak RTTI and vtable data must be named at the address that the original link kept.** Linking a freshly compiled unit that references `__RTTI__Q23std11logic_error` kept a second copy alive until `lbl_804B5AB0` was renamed to that symbol. Rename the lbl to the real RTTI name before marking a unit Matching.
- **Leaf functions with a stack frame can differ only by `stwu` position.** `CLayout::NormalizeViewPosition` and `NErrMsg::GetPaneRectangle` produce identical instructions except that the compiler sinks `stwu r1` below the first loads; the target keeps it first. No source variation tried (declaration order, struct-return form, VEC2 versus _VEC2) changed it.
- **Scratchpad file names collide across agents.** The shared scratchpad is one directory; prefix helper scripts with the agent ID.
- **objdiff matches data and constant symbols by name.** Local constants compile as `@NNNN` and string literals as `@STRING@fn`; rename the target `lbl_` symbols to the compiled names after each source edit that changes numbering, or the function shows as a mismatch while the instructions are identical. A pooled constant shared by two functions that the compiler numbers differently cannot match both.
- **Return type of 8-byte math structs.** `CLayout::GetViewScale` and `NormalizeViewPosition` return `nw4r::math::_VEC2` (a POD returned in r3/r4), not `VEC2`, which has constructors and is returned through memory.
- **BUI callback owner is one class.** `BUI::CCallbackListOwner` is shared by BUI, CUIList and CUIMenuCallbacks; the target has a single `Add` and a single destructor for it.

## Evidence notes

- CLayout flag bits: `1` visible, `2` loaded (a Layout exists), `8` matrix dirty. `UpdateAttachedLayouts` marks the dirty bit recursively through `CAttach::mChildren`, and `CalculateMtx` clears it.
- `CAttach` is `{CLayout* parent; Pane* pane; set<CLayout*> children;}`, 20 bytes.
- The text-pane suffix table at `0x80367038` is `_nw _n _ne _w _e _sw _s _se _c ""` followed by null; `SetText` writes the string to every existing outline and shadow pane built from it.
- `UIMenu`/`CUIMenu` reference sources came from `337-menu-reference/CUIMenu.cpp` and matched at 99% to 100% after the vtable and constant symbols were named.

## Traps (CLayout motion, attach and loader work)

- **Naming a local weak helper after a template instance another unit also defines breaks the link.** Renaming `fn_801D29C0` to `find_leaf<PCv>` (set of void pointers) collided with CGameSession.o: "multiply-defined ... Previously defined in CGameSession.o". Keep the unique `fn_` name for such instances; unique-to-CLayout names (CAttach, PaneMP, CMotionProgram deleters, the pane and animation map helpers) linked fine.
- **A shared_ptr returned from a helper wants a lifetime-extended reference, not a named local.** `CreateMotion`, `QueueMotion` and `AppendMotion` match only with `const shared_ptr<CMotionProgram>& rProgram = BuildMotion(...)`; a plain local copies through registers differently.
- **Declaring a container iterator at the top of the function changes register allocation.** `ApplyMotionFrames` and `Calc` swapped `r28`/`r31` until `PaneProgramMap::iterator it;` was declared before the `if` and assigned in the `for`.
- **`vector::reserve(1)` before `push_back` is source, not inlining luck.** `LoadArchive` and `LoadFromMemory` gained exactly one 312-byte inline `reallocate` chunk; adding `mArchives.reserve(1);` made both 100%.
- **`CLayout::DetachChildren` was misnamed.** The function at `0x801D57E0` detaches the layout from its parent (now `Detach`); the function at `0x801D59E0` detaches every attached child layout (`DetachAttachedLayouts`).
- **Functions that contain an inlined `std::list::clear` or erase keep four copies of each inlined set iterator.** StopMotion, AdvancePaneMotions and ReplaceMotions show four stores of `begin()` and three of `end()` per inlined `UpdateAttachedLayouts` level in the target; the same inline in `CalculateMtx` and `ApplyMotionFrames` has one. No source form tried (inline keyword, declaration order, pragmas for propagation, scheduling, peephole) reproduces it.
- **The target keeps the map lookup key in memory in StopMotion and FindPaneMP (`stw r4, 8(r31)` at entry).** Our `map::find` does not; this and the set insert tail (`pair<iterator,bool>` dead stores to `0xc`/`0x10`/`0x18`) cap Attach, AttachLayout, Detach, StopMotion and FindPaneMP at 90 to 99 percent.
- **Identical eight-byte accessors fold at link time.** `__ml__shared_ptr<FileList>`, `get__shared_ptr<Font>` and similar share one address, so a call can show a different name in the target than in the rebuilt object; those rows cannot reach 100 percent by source changes.
