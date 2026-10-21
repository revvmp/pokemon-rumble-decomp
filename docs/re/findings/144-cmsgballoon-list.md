# Issue 144: CMsgBalloon list constructor chain and destructors

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, objdump of the linked `build/wpse-v1/main.elf`, a full `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## F-144-1: the 25 functions of 0x800C6BF0-0x800C70F4

Result: all 25 functions match 100% in objdiff once their callees carry the compiler names. Confidence high.

| Address | Function |
| --- | --- |
| `0x800C6BF0`, `0x800C6C50` | `__vec_constructor` and `vector` destructors for `shared_ptr<BUILayout>` (the `COffScreenTexture` pair is `0x800C6A00`, `0x800C6A60`) |
| `0x800C6CB0`, `0x800C6CE0` | `list<Entry>` and `__list_imp<Entry>` constructors |
| `0x800C6D10` | `__list_deleter<Entry>` constructor: `size_(0)`, `tail_(alloc())`, then the sentinel links itself through `end_node()` |
| `0x800C6D80`, `0x800C6D90` | `end_node() const` and `CompressedPairStorage<allocator<node>, node_base, 1>::second() const` |
| `0x800C6DA0` | `allocator<node>` converting constructor from `allocator<Entry>` |
| `0x800C6DB0`, `0x800C6DE0`, `0x800C6E20` | `compressed_pair<unsigned long, allocator<Entry> >(unsigned long)`, its storage constructor, `allocator<Entry>()` |
| `0x800C6E30`, `0x800C6E40` | `__list_deleter::alloc()` and `CompressedPairStorage<unsigned long, allocator<Entry>, 2>::second()` |
| `0x800C6E50`, `0x800C6E90`, `0x800C6EC0` | `compressed_pair<allocator<node>, node_base>(allocator<node>)`, its storage constructor and `allocator<node>()` |
| `0x800C6ED0` | `__list_deleter<Entry>` destructor, `erase(begin(), end())` |
| `0x800C6F50`, `0x800C6F90` | `begin()` and `iterator(node*)` |
| `0x800C6FA0`, `0x800C6FE0` | `end()` and `iterator(node_base*)` |
| `0x800C6FF0`, `0x800C7050` | `__list_imp` and `list` destructors |
| `0x800C70B0` | `shared_ptr<CMsgBalloon>::operator*` (already matched) |
| `0x800C70C0` | `CMsgBalloon::Draw`, `mUIManager->Draw()` |

## F-144-2: include/list is iterator based

The previous `__list_deleter` was node-pointer based. The target is not: its destructor calls out-of-line `begin()` and `end()` functions that return iterators and passes their addresses to `erase(iterator, iterator)` (`0x800CA010`, owned by #145). `erase` itself compares with `operator==`, calls `unlink_nodes(first, last->prev_)` (`0x800CA130`), then loops destroying through `alloc().destroy(&*first)` and `tail_.first().deallocate`. `include/list` now has `iterator` nested in `__list_deleter` (user copy constructor, `node*` and `node_base*` constructors, `get()`), `begin()`, `end()`, `alloc()`, the iterator `erase` and a static `unlink_nodes`. The sentinel link stores are written directly in the constructor instead of a `node_base::init()`, because `init` is not a separate function anywhere in the target. `allocator` gains a converting constructor, and `CompressedPairStorage<First, Second, 1>` an `explicit` constructor from `First`. `erase` matches 100% in `CMsgBalloon.cpp` apart from its callee names. The Matching `BWorldLightSet.cpp` and the other list users keep their objdiff results.

## Traps

- **A node-pointer `erase` can match inlined and still be wrong out of line.** The first iterator rewrite left `BWorldLightSet` destructors at 99% because the inlined register assignment depends on the real `erase` shape. Only the faithful iterator `erase` restored 100% in both the inlined and the out-of-line context.
- **A trivial-looking callee that tail-calls another is an accessor.** `0x800C6E30` (`b 0x800C6E40`) is `alloc()` wrapping the pair's `second()`, not a second function with the same job; it exposed the `tail_(alloc())` initializer.
