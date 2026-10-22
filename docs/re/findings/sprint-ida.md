# Sprint IDA: weak-function emission order in mwcceppc Wii/1.1

This note describes how the CodeWarrior Wii/1.1 C++ front end orders the weak template and inline functions it emits under `-ipa file`. The evidence comes from Hex-Rays on `mwcceppc.exe`. Frida traces of real unit compiles confirmed each step. It extends [CODEGEN_RULES](../CODEGEN_RULES.md#emission-order-of-weak-functions-and-data).

## Compiler functions

| Address | Name | Role |
| --- | --- | --- |
| `0x47D430` | `CMid_Finish` | End of translation unit |
| `0x47D6E0` | global cleanup loop | Repeats the drain, implicit-member and template-walk steps until nothing changes |
| `0x47D870` | `CMid_GlobalDeclarationActions` | Drains the action list |
| `0x47EF90` | action pop | Pops the most recent action (LIFO) |
| `0x47E590` | IPA codegen loop | Walks the call-graph node list in creation order |
| `0x47E760` | `CMid_GenerateCGNode` | Generates one node |
| `0x47F100` | `CMid_ObjectAddrRef` | Creates nodes for referenced inline functions, depth first |
| `0x47F460` / `0x47F4A0` / `0x47F570` | `CMid_Add{Friend,Inline,Default}FunctionAction` | Push actions |
| `0x47FC50` | `CMid_DefineFunction` | Function definition; can create a node |
| `0x480450` | reference scanner | Queues the undefined callees of a just-defined function |
| `0x4805C0` | template member queue | Queues a class-template member |
| `0x5C5EB0` | `CCallGraph_NewNode` | Appends a node to the call-graph list |
| `0x5C6190` | `CTempl_Instantiate` | Template walk |
| `0x5C62F0` / `0x5C64F0` / `0x5C6590` / `0x5C6980` | `CTempl_GenClassInstance` / `GenMemberInstance` / `InstantiateMember` / `GenFuncInstance` | Instantiation |
| `0x5F54C0` | `CTemplClass_DefineNestedClass` | Gives every nested class its own template record |
| `0x5F5420` | `CTemplClass_MakeRealTemplate` | Copies nested class templates at instantiation |
| `0x7754B8` / `0x776478` | `ctempl_ctemplates` / `ctempl_ftemplates` | Class and function template lists |

The traces read these object offsets:

| Offset | Field |
| --- | --- |
| `+12` | name (the string is at `+10` of the hash node) |
| `+16` | type |
| `+20` | qualifiers |
| `+24` | flags |
| `+28` | call-graph node |
| `+64` | stored function data |

## Algorithm

1. Output order is the order in which call-graph nodes are created. Nothing sorts or reorders the list afterwards.
2. A node is created in one of two places:
   - In `CMid_DefineFunction`, if the function cannot be inlined, if it is inline and already referenced (flag 2 at `+24`), or if it is auto-inline (`0x800`).
   - In `CMid_ObjectAddrRef`, when the first reference reaches an inline function that already has stored function data. The new node then references its own callees, so nodes are created in a depth-first pre-order walk.
3. Every definition queues its still-undefined callees on one LIFO action list. An action can be an in-class body, a class-template member, a function-template instance, an implicit member, or a friend. A callee that is already queued is not queued again.
4. The action list is drained only in `CMid_GlobalDeclarationActions`. At end of file, `0x47D6E0` repeats three steps until nothing new appears:
   - drain the action list;
   - finalize implicit members;
   - run `CTempl_Instantiate`.
5. `CTempl_Instantiate` walks the class-template list newest first:
   - A class template enters the list at its first declaration. A forward declaration counts.
   - For each template, the walk visits its instances, then its partial-specialization instances, then each referenced but undefined member in scope order. In-class and out-of-class members are treated the same.
   - After all class templates, the walk processes the function-template list. Member templates are in that list.
6. Every nested class gets its own template record, which enters the list at the nested declaration. The walk therefore reaches a nested class before the class that encloses it.
7. A friend function defined inside a class template is instantiated through the action list. It is emitted during a drain, not during the walk.

In practice, if the target emits an item right after group G, then either the item belongs to the class template declared just before G, or it is a member or function template emitted in the function-template loop.

## Fixes

- `find_leaf` in `include/MSL_CPP/detail/tree.h` is now a member template (`template <typename Key> find_leaf`). It was a non-template `value_type` overload. The target emits it in the function-template loop.
  - The nine `tree_imp` symbols were renamed to `find_leaf<T>__...`.
  - `l10n_files.cpp` is `Matching` as of commit `662db05d7`.
  - The `.text` order of `MapPiiTable` is now exact, but `Clear` still has a register swap.

## Remaining

- **COyaInfo:** two functions are misplaced, `tree_imp::copy` and `copy_children`.
  - The target requests the map copy chain in the second walk, from the `split_buffer` front construction.
  - Ours requests it in the first walk, through `construct(p, T())` in `construct_at_end(n)`.
- **BWorld:** `map_compact_tree::find_leaf` is still misplaced. Its key and value overloads have different bodies, so they cannot be merged.
- **CSaveManager:** an uncommitted restructure of the deque brings it down to 14 misplaced functions out of 243. It was not landed because it needs symbol renames.
  - The restructure has three parts:
    - The deque and pointer-deque iterators become standalone class templates, declared before the classes that use them.
    - The three `__cdeque_pointer::insert` overloads become member templates.
    - `__deque_imp::at` moves out of the class.
  - It also requires including `CFiber.h` before `CControllerComposition.h`.
  - The remaining gap is in the tr1 bind machinery in `include/functional`:
    - `bound_func::operator()` is walked too early.
    - `call_bound` and `get_bound` are emitted before the two `__cdeque_buf::insert` instances.
- **CMsgBalloon:** the interleave of `__vec_deleter` and `__vec_constructor` has not been analysed with this model.
- **CPlayerPii:** the deque member order has been derived but not applied. `GetShadowName` (88.86%) blocks the unit anyway.

## Traps

- A misplaced weak function usually means the template record was created too early or too late. Moving the definition does not help. Check where the class template, a nested class, or a forward declaration first appears.
- A non-template member overload is instantiated in the class walk. A member template is instantiated later, in the function-template loop. Turning an overload into a member template moves it, but it changes the mangled name, so the `symbols.txt` entries must be renamed to match.
