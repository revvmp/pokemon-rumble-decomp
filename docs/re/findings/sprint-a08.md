# Sprint a08

## Traps

- **Inlining inside a large constructor is all-or-nothing.** `IGameResult::Create` (0x800AAA40, 3132 bytes) needs the `CGameResult` constructor and the layout/world/model loading inlined into the factory. Folding the loader into the in-class constructor reproduces the inline expansion (68.8 percent), but the three member-init `shared_ptr(Y*)` constructors then stay out of line, while the original inlines exactly those three and none of the later ones. Deleting any single statement of the loader flips every `shared_ptr` constructor to inline, so the compiler is applying a size threshold; `inline_depth`, `inline_max_size`, `-inline auto,deferred` and an in-class `shared_ptr(Y*)` definition did not change it. Splitting the body out of the constructor makes the constructor itself stay out of line. Unresolved.
- **A destructor keeps its vtable store if the body has an inlined call.** `CGameMap::~CGameMap` matched after adding an empty non-virtual `Finalize()` call to the body (same finding as F-137-3).
- **Unnamed weak functions in a unit are named by their vtables and extab.** `fn_`/`dtor_` entries in `CGameResult` were `shared_ptr_deleter`, `move_ptr` and `__vec_deleter` instances; the data-section vtable and the extab destructor names identify them.
