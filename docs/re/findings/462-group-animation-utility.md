# F-462-6: Group animation-binding utility

`nw4r::lyt::BindAnimation(Group*, AnimTransform*, bool, bool)` at `0x802B1790` is the final function in #462. It is the only linked function of `nw4r/lyt/lyt_util.cpp`, with `.text` `0x802B1790–0x802B1800` and no owned data. The next function at `0x802B1800` is the SDK's PPCArch unit. This corrects the initial hypothesis that the routine was a Group member or part of `lyt_common.cpp`.

Source provenance: CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/lyt/lyt_util.cpp` and `include/nw4r/lyt/lyt_utils.h`. The target retains only the group's BindAnimation overload; unrelated reference utility functions and unfinished reference stubs are not imported. The prerequisite Pane and Group headers merged in #755 and are evidenced by F-462-4. Their source and splits remain outside this work.

The 112-byte implementation walks Group's pane list through `PaneList::Iterator`, loads each `PaneLink::mTarget`, and calls `Pane::BindAnimation` with the supplied transform, recursive flag, and disabled flag. Group's list begins at `0x0C`, its sentinel is at `0x10`, PaneLink's Pane pointer is at `0x08`, and the called Pane virtual is at `0x44`. Wii/1.1 and the existing `cflags_nw4r` produce the exact native instruction words with only actual ELF relocation words excluded in the diagnostic comparison. Objdiff reports 100% for the native function and `.text`.

## Complete issue inventory

| Source unit | Native functions | Native code bytes | Functions in #462 |
| --- | --- | --- | --- |
| `lyt_resourceAccessor.cpp` | 3 | 88 | 0; approved #461 prerequisite |
| `lyt_arcResourceAccessor.cpp` | 12 | 1,300 | 10, `0x802B08F0–0x802B0C30` |
| `lyt_common.cpp` | 13 | 2,832 | 13, `0x802B0C30–0x802B1790` |
| `lyt_util.cpp` | 1 | 112 | 1, `0x802B1790–0x802B1800` |
| Total | 29 | 4,332 | All 24 issue functions, 3,720 code bytes |

Five additional functions and 612 code bytes in #461 belong to the two approved shared resource-accessor units. Earlier animation code in #461 is not reconstructed by this work, so #461 remains unfinished.

## Acceptance evidence

The utility split with its original object passes configure, Ninja, and explicit frozen verification. After 100% objdiff, the source-selected candidate also passes all three commands with exit zero. The complete four-unit audit verifies 29 native functions and 4,332 code bytes at their original linked addresses and sizes, all owned sections at 100%, and actual Ninja link inputs selecting all four source objects without their fallback counterparts. The actual `build/462-utility-split/wpse-v1/main.dol` directly equals the immutable reference. No issue function remains in fallback, and no expected hash or manifest is changed.

## Traps

- **A custom output directory still uses the immutable default-path SHA1 input.** A fresh header worktree's custom build linked successfully, then Ninja failed because `config/wpse-v1/build.sha1` names absent `build/wpse-v1/main.dol`. Stop after that nonzero exit and run the ordinary default build, or ensure that explicit verification checks the actual custom output after a successful Ninja invocation. Never infer the custom artifact's identity from the default-path SHA1 target and never edit the frozen input. The header prerequisite was rerun successfully through the fresh default build before merging.
