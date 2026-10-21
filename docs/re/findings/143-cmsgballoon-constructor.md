# Issue 143: CMsgBalloon constructor and member container constructors

Target `wpse-v1`, checked with `tools/re/unitdiff.py src/game/CMsgBalloon.cpp`, objdump of the linked `build/wpse-v1/main.elf`, a full `ninja` and `tools/verify/verify.py`. No decompiler output was used.

## F-143-1: the 23 functions of 0x800C6690-0x800C6BE4

Result: all 23 functions match 100% in objdiff. Confidence high.

| Address | Function |
| --- | --- |
| `0x800C6690`, `0x800C66E0` | `move_ptr<CMsgBalloon>::release`, compressed-pair storage `first()` (already emitted by `Create`) |
| `0x800C66F0` | `CMsgBalloon::CMsgBalloon()`, an empty body: `IMsgBalloon`, `enable_shared_from_this`, `shared_ptr<CUIManager>`, three `shared_ptr<BUILayout>`, the two vectors, `list<Entry>`, `VEC3` and `shared_ptr<CMotionProgram>` members are constructed in declaration order |
| `0x800C6780`, `0x800C67C0` | `IMsgBalloon` constructor and deleting destructor |
| `0x800C6820`, `0x800C6850`, `0x800C6860` | `enable_shared_from_this<CMsgBalloon>` constructor, its `weak_ptr` constructor and deleting destructor |
| `0x800C68C0` | `shared_ptr<CUIManager>()` |
| `0x800C68D0`-`0x800C69B0` | Constructor chain `vector`, `__vec_constructor`, `__vec_deleter`, `compressed_pair`, `CompressedPairStorage` for `shared_ptr<BUILayout>` |
| `0x800C69F0`, `0x800C6BE0` | `allocator<T>::allocator()` for `shared_ptr<BUILayout>` and `shared_ptr<COffScreenTexture>` |
| `0x800C6A00`, `0x800C6A60` | `__vec_constructor` and `vector` destructors for `shared_ptr<COffScreenTexture>` |
| `0x800C6AC0`-`0x800C6BA0` | The same constructor chain for `shared_ptr<COffScreenTexture>` |

## F-143-2: the MSL allocator has a user-provided default constructor

The `CompressedPairStorage<unsigned long, allocator<T>, 2>` constructor calls an empty function for its empty base (`0x800C69F0`, `0x800C6BE0`). Giving `std::allocator` an empty `allocator() {}` reproduces the call and the original frame; with the implicit constructor the storage constructor was 11.88%. The full build still links and verifies.

## F-143-3: CMsgBalloon::Entry

`list<Entry>` instantiates `node::data_`, so the constructor needs a complete `Entry`. The entry constructor `0x800C9B30` and destructor `0x800C9B80` give a `weak_ptr<BWorldObj>` at 0, a `basic_string<wchar_t>` at 8 and a `VEC3` at 0x14 (initialized from zero floats). Names are descriptive, medium confidence.

## Remaining differences

The list constructor `0x800C6CB0` (owned by #144) differs only by the relocation name of `__list_imp` `0x800C6CE0`, and the `__vec_deleter<shared_ptr<COffScreenTexture>>` destructor `0x800CA480` by unreconstructed neighbours.

## Traps

- **An empty callee in a constructor chain is a user-provided constructor.** A 4-byte `blr` called from a storage constructor means the empty base has a non-trivial default constructor, not that the constructor is inlined.
