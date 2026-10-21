# F-336-2: Erased shared-owner reference type

The canonical `shared_ptr<T>` formed `T&` in its dereference member declaration, rejecting `shared_ptr<void>` before that member was used. An internal reference-type trait keeps `T&` for ordinary types and provides `void` for the erased owner view. Storage, constructors, deleters, and owner operations remain unchanged.

This prerequisite was reviewed by the shared-memory owner. The native font candidate requires an erased owner view while retaining its `unsigned char` allocation and custom Free deleter. This header change selects no new source unit and accepts no reconstructed font bytes.

## Validation

Baseline main was `ca14c79b`. The existing #336 findings commits do not affect build inputs. `python configure.py` and `.venv/Scripts/ninja.exe` exited zero. Frozen-manifest verification exited zero before and after the change. Before the second build, the old `build/wpse-v1/main.dol` was deleted.

The complete current `build/wpse-v1/src` inventory contained 705 object files. SHA-256 comparisons before and after recompiling every consumer of the changed header found zero differences across all 705 objects. The header dependency rebuild compiled 17 consumers; the remaining objects were included in the comparison. The outside control script and inventories are `336-memory-control.py`, `336-reference/memory-control-before.json`, and `336-reference/memory-control-after.json`.

The separate native font experiment reproduced ten functions, 1,364 instruction bytes, and 41 independently resolved relocation targets. Whole-font data, exception-table, weak-order, source-selection, and frozen-DOL gates remain outstanding; this prerequisite does not satisfy them.

## Traps

Revalidate the complete current consumer inventory when a shared template changes. The prior inventory count is not a substitute for current object coverage.
