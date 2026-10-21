# Issue 148: CMsgBalloon map access layers and the vector resize chain

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, a full `ninja`, `tools/verify/verify.py` and a per-function comparison of `build/wpse-v1/report.json` against a fresh baseline of `origin/main`. No decompiler output was used.

## F-148-1: the 25 functions of 0x800C9430-0x800C9764

Result: all 25 functions compare 100% in objdiff once the callees of the block are renamed from `fn_` placeholders (all 25 sit in `symbols.txt` under their mangled names). `SetupStageModels` rose from 99.37% to 100.00% because its calls now resolve to named symbols. Confidence high for structure, medium for the names of the `tree` wrappers. The object stays NonMatching; the unreconstructed neighbours (`find`, `find_or_insert` and `begin` of `tree_imp`, the vector `append` and `erase_at_end` bodies, the `tree_imp` destructor) belong to #149 onwards.

| Address | Function |
| --- | --- |
| `0x800C9430` | `tree_imp::iterator::operator==` |
| `0x800C9450` | `map_base::find(const key_type&)` |
| `0x800C9490`, `0x800C94C0` | `map_base::iterator(const tree_type::iterator&)` and the `tree_imp::iterator` copy constructor |
| `0x800C94D0`, `0x800C9520`, `0x800C9760` | `red_black_tree::find<Key>`, `end` and `begin`, one-instruction tail calls into `tree_imp` |
| `0x800C94E0`, `0x800C9720` | `map_base::end()` and `begin()` |
| `0x800C9530`, `0x800C9570` | `tree_imp::end()` and `iterator(tree_node_base*)` |
| `0x800C9580`, `0x800C95B0` | `map_base::operator[]` and `red_black_tree::find_or_insert<Key>` |
| `0x800C95C0` | `MapPiiTable::GetRowCount() const` |
| `0x800C95D0`, `0x800C95E0`, `0x800C9610`, `0x800C9620` | `map_base::size()`, `tree_imp::size()`, the private `tree_imp::count()` and the `CompressedPairStorage<unsigned long, Allocator, 2>::first() const` it calls |
| `0x800C9630`, `0x800C9640`, `0x800C9670`, `0x800C96A0` | `vector::resize`, `__vec_constructor::resize`, `append(n)` and `erase_at_end(n)` for `shared_ptr<CPiiTex>` |
| `0x800C96D0`, `0x800C9700`, `0x800C9710` | `map_base::iterator()`, `tree_imp::iterator()` and `tree_imp::iterator::operator=` |

Callees named so the block links to them (their bodies are #149 onwards): `0x800CA660` `tree_imp` destructor, `0x800CB270` `tree_imp::find<Key>`, `0x800CB340` `tree_imp::find_or_insert<Key>`, `0x800C9770` `tree_imp::begin`, `0x800CA210` `__vec_constructor::append(n, int2type)` and `0x800CA2C0` `__vec_deleter::erase_at_end(n, int2type)`.

## F-148-2: what the layers show

- `red_black_tree` carries `begin`, `end`, `find<Key>` and `find_or_insert<Key>` as pure forwarders to `tree_imp`; the other members stay unwrapped (`size` goes straight to `tree_imp`). With `dont_inline on` each forwarder is a 4-byte `b` and the hidden return slot passes through unchanged.
- `map_base::operator[]` is `tree_.find_or_insert(rKey).second`. The target does not go through `insert`: `find_or_insert` finds the leaf by key, compares the predecessor against the key and inserts `value_type(key, mapped_type())` only when absent. `tree_imp` now holds a `find_or_insert` and a key-typed `find_leaf` in that shape; the bodies are #149 onwards.
- `tree_imp::size()` is `bl count(); lwz r3, 0(r3)`: a private `const size_type& count() const` returning `size_.first()` is the extra tail-call layer at `0x800C9610`.
- `it = unlocked.begin()` calls the `tree_imp::iterator` assignment directly, so `map_base::iterator` has no user-declared `operator=`; it is the implicit member-wise one.
- `vector::resize` forwards to `__vec_constructor::resize`, which reads `size_`, then calls `append(n - size)` or `erase_at_end(size - n)`; both are one-line wrappers that add the `int2type` argument (a one-byte temporary written with `stb` before the call).

## Traps

- **A one-argument wrapper changes inlined code elsewhere.** Adding the `vector::resize` chain (three tiny forwarders) lowered five `SaveData::DeserializeVector` functions by 0.02% and raised `InitializeWorkBuffer` slightly. Reading `size_` into a local first (`size_type size = this->size_;`) recovered `InitializeWorkBuffer` and `SetCamera`; the remaining `SaveData` change is a stack-slot shift of the `int2type` temporary. Compare per-function scores of every unit that resizes a vector before landing.
- **A tail call to an unnamed callee scores 95% until the callee is renamed.** The iterator comparison and the `tree_imp` destructor call were only relocation-name differences; naming the callee in `symbols.txt` is what turns them into 100%.
- **`map::operator[]` is not `insert(...).first->second` here.** The block shows `bl; addi r3, r3, 4` on a pointer to the pair; a find-or-insert on the tree that returns the value reference produces it.
