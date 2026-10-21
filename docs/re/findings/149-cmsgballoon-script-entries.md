# Issue 149: CMsgBalloon script entry points, list insertion and tree iterator

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, a full `ninja`, `tools/verify/verify.py` and a per-function comparison of `build/wpse-v1/report.json` against a fresh build of the same `origin/main`. No decompiler output was used.

## F-149-1: the 26 functions of 0x800C9770-0x800CA004

Result: all 26 functions measure 100% in objdiff (`report.json` `fuzzy_match_percent`); `RegisterWarpzone` differs only in the relocation name of its empty wide literal. A comparison against the baseline shows no other function below its previous score. Confidence high for structure, medium for the parameter names.

| Address | Function |
| --- | --- |
| `0x800C9770` | `tree_imp::begin` (already in `tree.h`) |
| `0x800C97B0`, `0x800C97C0` | `map_base::iterator::operator*` and `tree_imp::iterator::operator*` |
| `0x800C97D0`, `0x800C9800` | `map_base::iterator::operator++` and `tree_imp::iterator::operator++` |
| `0x800C9830`, `0x800C9840` | `map_base::iterator::operator!=` and `tree_imp::iterator::operator!=` (a direct node comparison) |
| `0x800C9860` | `shared_ptr<CPiiTex>::shared_ptr()` |
| `0x800C9870`-`0x800C98A0` | `vector::operator[]` and `__vec_constructor::operator[]` for `shared_ptr<CPiiTex>` and `shared_ptr<BUILayout>` |
| `0x800C98D0` | `CMsgBalloon::AdjustY(BWorldObj*, const VEC3&)`, vtable slot 12: assigns the entry position of the entry whose locked object is the argument |
| `0x800C99D0` | `CMsgBalloon::RegisterWarpzone(BWorldObj*, f32 radius, const StageID&)`, slot 10 |
| `0x800C9B30`, `0x800C9B80` | `CMsgBalloon::Entry::Entry()` (position zeroed by `VEC3(0, 0, 0)`) and its destructor |
| `0x800C9BF0`, `0x800C9C50`, `0x800C9F20` | `list<Entry>::push_back`, `generic_iterator::operator--` and `__list_deleter::insert` |
| `0x800C9C60` | `CMsgBalloon::Add(const wchar_t*, f32 z, f32 x)`, slot 8 |
| `0x800C9D80` | `CMsgBalloon::Register(BWorldObj*, const wchar_t*, f32 radius)`, slot 9 |
| `0x800C9EE0`, `0x800C9F10` | `CMsgBalloon::IsShown(int)`, slot 11, and `Close()`, slot 6 (returns true) |

The signatures declared by #142 (`Add(int, f32, f32)` and similar) were placeholders; the bodies show a text pointer, an object pointer and a radius, so `IMsgBalloon` now declares the evidenced parameter lists. `Add` stores its second float at `+0x1C` and its third at `+0x14`; the entry scale is `1.0f / (radius * radius)` for `Register` and `RegisterWarpzone` and `1.0f` for `Add`. Every virtual first builds an unused `Entry` local, pushes a default `Entry()` temporary and then writes the new last element through `--end()`; that is the observed code order, kept as written. The invalid stage is a four-byte `StageID` of all ones read from `.sdata2` before the float constants, written in the source as a file-local `static const StageID`.

## F-149-2: library corrections the block exposed

- `0x800C6F50` and `0x800C6FA0` are `list<Entry>::begin` and `end`, not `__list_deleter::begin` and `end`. Renaming them turned the callee differences in `Calc`, `AdjustY` and the new virtuals into matches. `list::begin` is `iterator(end_node()->next_)` and needs a `generic_iterator(node*)` constructor next to the `node_base*` one (`0x800C6F90`, `0x800C6FE0`).
- `__list_deleter::insert` is `link_nodes(position.base(), pNode, pNode)` with `node_alloc()`, `alloc()` and `size_ref()` wrappers; `0x800CA110`, `0x800CA120`, `0x800CA150`, `0x800CBCE0` and the `alloc_ptr` helpers `0x800CAF20`, `0x800CAF90`, `0x800CB030`, `0x800CB070` are named for their callers here.
- `tree_imp::iterator::operator++` calls a private `increment()` (`0x800CA760`): a `tree_leftmost` descent (`0x800CBB90`) or a climb through the parent link. The target's parent helpers (`0x800CA7E0`, `0x800CA7F0`) take the address of the `parent_` field, so `parent_` is a typed field in the original; the reconstruction keeps the `unsigned long` field and scores 80% on `increment`, a function that belongs to #150.
- The sentinel `0.71111f` in `Calc` is `0.71111112f` (`0x3F360B61`); the `.sdata2` words now match.

## Traps

- **A wrong-looking name can hide a missing wrapper.** `0x800C6F50` was named `__list_deleter::begin` from a reference map and scored 99.38% with a `tail()`/`end_node()` mismatch; it was `list::begin` all along. When a callee shows a small structural mismatch in every caller, test the neighbouring wrapper name before editing the inner function.
- **Two symbols with one mangled name fail the link.** Giving both `0x800C6F90` and its four-byte forwarder `0x800CBCF0` the same constructor name produced a multiply-defined error. Leave the forwarder as `fn_` until its own source is known.
- **An unused local can be real code.** Each virtual constructs an `Entry` it never reads; CodeWarrior keeps non-trivial constructors and destructors, so the unused object is part of the match.
