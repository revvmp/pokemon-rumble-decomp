# CLayout.cpp unit and its first block (#329)

Part of #329. Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`. Issue #329 lists 27 functions in `.text` `0x801CA880`-`0x801CCA74`; the complete unit continues through #335 (static initializer `0x801D99E0`). Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). The earlier prerequisites [F-329-1](329-clayout-map.md), [F-329-2](329-clayout-contract.md) and [F-329-3](329-clayout-attachment.md) already fixed the class layout; this file adds the split, the first block and the structures the block proved.

## F-329-4: unit boundaries of `game/CLayout.cpp`

The split is in `config/wpse-v1/splits.txt` and the object is registered `NonMatching` in `configure.py`. With the original object linked, `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 before any source was written.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x8003C83C`-`0x8003E964` | `EncodingConverter` ends at `0x8003C83C`; `CResFont` starts at `0x8003E964` |
| `extabindex` | `0x80051414`-`0x80051990` | 117 twelve-byte entries between the same two neighbours |
| `.text` | `0x801CA880`-`0x801D9A08` | First function builds the `"CLayout.cpp: in "` message; last is the static initializer `0x801D99E0` (`0x28` bytes). `CResFont::Load` starts at `0x801D9A10`; the `8` bytes between are padding left to the next unit, as in [F-495-1](495-arc-unit.md) |
| `.ctors` | `0x80352A24`-`0x80352A28` | The table entry at `0x80352A24` is `0x801D99E0` |
| `.rodata` | `0x80367038`-`0x80367068` | `0x80367038` is read only by `SetText` and `0x801D60B0`; `0x80367068` is the first `CUIList` table. The `0x60` bytes before it belong to earlier units and are left unowned as before |
| `.data` | `0x803C0530`-`0x803C0FB0` | From the `get_what` string to the `exception` type-name string; `0x803C0FB0` is the `CRomFont` vtable |
| `.sdata` | `0x804B8348`-`0x804B8430` | From `"arc"` to the `0x804B8428` shared-pointer type info; `0x804B8430` is `__RTTI__8CRomFont` |
| `.sbss` | `0x804B9180`-`0x804B9188` | The initialize-failure handler read by `Load`; `0x804B9188` is first read at `0x801DE060` |
| `.sdata2` | `0x804BA490`-`0x804BA4D0` | Constants of the constructor, `GetFrame` and `Draw`; `0x804BA4D0` is first read at `0x801DB540` |

No `.bss` belongs to the unit. The unit has `.ctors` for exactly one object, the failure handler.

## F-329-5: proven structures of the first block

- **`CInitializeException`** derives from `std::runtime_error` (vtable `0x803C0B28`, size `0x30`): `int mError` `0x0C`, `list<string> mArchives` `0x10`, `string mLayoutName` `0x1C`, `shared_ptr<FontMap> mFonts` `0x28`. The constructor at `0x801CAD80` takes the null-terminated archive array; `Load` constructs the object through an inlined overload that takes one archive path. Both build the `what` text with two anonymous-namespace functions named `get_what`: the message is `"CLayout.cpp: in "` followed by `__FUNCTION__`, which is why `get_what` is stored twice and unpooled at `0x803C0530` and `0x803C0550`.
- **`Load`** (`0x801CC350`) forwards to the private archive worker `0x801CD300` and, on a non-zero result, builds the exception. If the `.sbss` `0x804B9180` `shared_ptr<function<void (const CInitializeException&)>>` is set it calls the handler and then `OSPanic("CLayout.cpp", 337, what())`; otherwise it throws the exception. The wrapper for the array overload at `0x801CCBD0` is owned by #330.
- **`CMotionProgram`**: state `0` is not started, `1` and `2` run, `3` ends. `0x801CB0E0` returns the target's frame in states `1` and `2`, `0.0f` in state `0` and `mLastFrame` otherwise; `0x801CB140` is `Wait` (until state `3`); `0x801CB180` waits until the state leaves `0`; `0x801CC220` stores `3` (`Finish`).
- **`PaneMP`** (size `0x24`, allocated by `0x801D2180`): `Pane*` `0x00`, frame `f32` `0x04`, `list<shared_ptr<CMotionProgram>>` `0x08`, a flag byte `0x14` that `StopMotion` clears, and a 12-byte set of panes `0x18` built from pane parents by `0x801D2180`. Names of the flag and the set are descriptive; confidence low.
- **View scale.** `0x801D07E0` (`SetViewScale`) writes the `DrawInfo` location-adjust scale at `0x9C` and rebuilds the view rectangle from the `VEC2` at `0xD4`, and `GetViewScale` (`0x801D0880`) returns the `DrawInfo` scale. The `0xD4` member is therefore not the value those accessors name; it is renamed `mPixelScale` (the video-mode pixel aspect scale built from `GetPixelAspectRatio`, a PAL factor `0.83623695f` and the 16:9 factor `0.75f`). The two lazily filled scales are plain `_VEC2 = {-1.0f, -1.0f}` objects so they land in `.sdata` at `0x804B8358` and `0x804B8360`; a `VEC2` with a constructor produces a dynamic initializer and `.sbss` instead.
- **Destruction.** `~CLayout` finishes every motion of every pane program, calls `0x801D59E0` (named `DetachAttachedLayouts`, it walks the attachment's child set at `+0x08` and clears each child's attachment) and then `DetachChildren` (`0x801D57E0`, which clears this layout's own attachment and removes it from its parent's set). The behaviour of `0x801D57E0` suggests it detaches the layout from its parent, so its existing name may be reversed; callers in `CMsgBalloon` are consistent with either reading, and the name is left to the owner of #331.

## F-329-6: reconstruction result

`src/game/CLayout.cpp` holds the first block. After the final build, 20 of the 27 listed functions compare 100% with objdiff (3,732 of 8,544 bytes), the rest at 80-99%:

| Function | Address | Result |
| --- | --- | --- |
| both `get_what`, `GetFrame`, `Wait`, `WaitStart`, `Finish` | `0x801CA880`, `0x801CA960`, `0x801CB0E0`, `0x801CB140`, `0x801CB180`, `0x801CC220` | 100% |
| vector, string-map and pane-map destructors | `0x801CB540`, `0x801CB610`, `0x801CBAC0` | 100% |
| list, map and shared-pointer iterator helpers | `0x801CC230`-`0x801CC2C0`, `0x801CC320`, `0x801CC330` | 100% |
| `~CLayout` | `0x801CC070` | 99.9%; only the vtable symbol name differs |
| `MakeArchiveList` (single, array) | `0x801CAA40`, `0x801CABD0` | 95.8%, 94.3%; the load of the first element is scheduled before the list is initialized |
| `CInitializeException` constructor | `0x801CAD80` | 92.5%; stack slots of the iterator copies are four bytes apart |
| `CLayout::CLayout` | `0x801CB1C0` | 95.2%; temporary slots of the `VEC2` copies are eight bytes apart |
| `Load` | `0x801CC350` | 94.6% |
| `SetViewScale` (owned by #331, defined here because the constructor inlines it) | `0x801D07E0` | 100% |
| list-of-`CFileData` constructor | `0x801CB520` | Not emitted: only `Load` of the array overload (#330) calls it out of line |

Remaining differences that are symbol names only (`lbl_803C0AF4` against `__vt__7CLayout`) disappear when the data symbols are named by the owners of the data. The unit stays `NonMatching` until #330-#335 are in.

## Shared header changes

- `include/map`: the compact tree selection gains two empty wrapper classes (`map_compact_red_black_tree`, `map_compact_tree_adapter`) and the compact tree `iterator` gains `operator*`, `operator++` (successor search), `operator!=` and `end()`. With `map_base` already supplying one level, the two wrappers reproduce the five `beq` null checks that open every `map` destructor in the target. Rebuilding every game unit changes only `BWorld` and `CPpuEngine` (their `map` destructors move to 100% on the earlier measurement); no function in any other unit moved.
- `include/list`: the copy constructor delegates to `__list_imp(first, last, allocator)`, which is how the target copies (two nested pairs of iterator temporaries). No other unit changed.

## Traps

- **Out-of-line template helpers appear only when something forces them.** The `begin`, `operator*`, `operator++`, `end` and `operator!=` helpers and the member destructors are separate functions in the target because `~CLayout` is compiled without inlining; `#pragma dont_inline on` around that one destructor reproduces it at 99.9%, whereas without it every helper inlines (`0xC4C` bytes). The pragma is a reproduction, not proof of the original spelling.
- **`__FUNCTION__` reproduces unpooled duplicate literals.** Two identical `"get_what"` strings in separate functions are not merged; a plain literal is pooled.
- **Weak extra functions are harmless.** The compiled object also carries weak inline instances (`~map_compact_tree`, wrapper destructors) that the linker dropped from the target; `unitdiff` lists them as "only in source" without penalty.
- **Evaluate the rectangle edges in the target order.** `right` and `top` first, then `left = -right` and `bottom = -top`, reaches 100% for `SetViewScale`; computing and negating each pair in turn does not.
- **A name can describe the opposite of the code.** `DetachChildren` at `0x801D57E0` removes the layout from its parent; check the callee before trusting a name from a call site.
