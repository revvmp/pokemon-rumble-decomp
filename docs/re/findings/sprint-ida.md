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
8. Each class-template group visits its instances newest first. `CTemplClass_NewInstance` (`0x5F6BB0`) adds an instance to the front of the list at `+68` the first time the template-id is named. A typedef or a by-reference parameter type is enough; the class does not have to be completed.
9. A member template of a class template becomes its own function-template entry in the function-template list. The entries are added in declaration order when the class instance is instantiated, so the loop visits the last-declared member template first. The loop only generates instances that are already referenced. An instance that is first referenced from a template generated later in the same pass waits until the next walk.
10. A reference is registered only when the referencing function gets a call-graph node, not when it is defined. Something referenced only through an inline function that has no node yet stays unreferenced until that function gets one.

In practice, if the target emits an item right after group G, then either the item belongs to the class template declared just before G, or it is a member or function template emitted in the function-template loop.

## Fixes

- `find_leaf` in `include/MSL_CPP/detail/tree.h` is now a member template (`template <typename Key> find_leaf`). It was a non-template `value_type` overload. The target emits it in the function-template loop.
  - The nine `tree_imp` symbols were renamed to `find_leaf<T>__...`.
  - `l10n_files.cpp` is `Matching` as of commit `662db05d7`.
  - The `.text` order of `MapPiiTable` is now exact, but `Clear` still has a register swap.

## Fixes from the follow-up session

- **CSaveManager is `Matching`** (commit `d65ef4386`). The changes:
  - The deque iterators are standalone class templates declared before the classes that use them.
  - The three `__cdeque_pointer::insert` overloads are member templates, and `__deque_imp::at` is defined out of class.
  - `CSaveManager.cpp` includes `CFiber.h` before `CControllerComposition.h`.
  - In `include/functional`, `function_imp` is forward-declared before `mem_fn_0`. That makes the `bound_func` template newer, so the walk visits it before `function_imp`. `bound_func::operator()` is then not yet referenced when its group is walked. It is drained after the `__cdeque_buf::insert` instances, which gives the target's depth-first `call_bound` block.
  - The `symbols.txt` names in CSaveManager and CPlayerPii were renamed to the new mangled names.
- **Split-buffer deleters** (commit `872d8b421`). `__vec_constructor` no longer declares `swap(__vec_deleter<T, Allocator&>&)`. That declaration named the split-buffer base when the vector was completed. In CMsgBalloon, the target walks the split buffers and their `Allocator&` deleters in opposite orders. That rules out creating both when the vector is completed, and points to creating the deleter when the split buffer is instantiated. The change costs one or two misplaced functions in BWorld, CDlgCollection and CLayout, which are all still `NonMatching`.
- **`tree_imp` member templates** (same commit) are declared in the order `insert_key`, `find_or_insert`, `find`, `do_lower_bound`, `find_leaf`. As a result, `find` is emitted before `find_or_insert`, and `find_leaf` and `do_lower_bound` wait until the next walk.
- **`alloc_ptr::allocator()`** is declared after `release()` (same commit).
- **CMsgBalloon** declares `EntryList`, `TextureList` and `LayoutList` typedefs in that order (same commit). This creates the allocator instances in the target's age order. The source includes `MapDesc.h` before `CMsgBalloon.h`, so the `wstring` allocator is the oldest.
  - Misplaced functions went from 29 to 4. All 436 functions score 100%.
  - The four that remain:
    - the depth-first order of `__pointer_plus_bit` `->` and the pointer conversion under `increment`;
    - `wstring::operator=(const&)` is still defined one walk late;
    - `shared_ptr<CPiiTex>::reset` and its converting constructor come before `FormatMessage` in the target.
- **COyaInfo is `Matching`** (commit `16f89b7dd`). `COyaInfo.h` includes `<map>` before `<vector>`, so the tree template is older than the split buffer. The walk then reaches `tree_imp::copy` after `construct_at_front`.
- The game sweep's misplaced total fell by about 700. Every function that scored 100% still does, and `verify` passes.

- **CDlgPiiList `.data`** (commit `a5359bce2`). Each `shared_ptr_deleter` vtable is emitted when its class is instantiated, from the `shared_ptr(Y*)` member-template entry of `shared_ptr<T>`. The entries follow the order in which each `shared_ptr<T>` was completed, newest first, and `.data` lists the vtables in reverse of that instantiation order. The changes:
  - `~CCallbackListOwner` moves out of `BUI.h` into the new `BUICallbackList.h`, which `BUI.cpp`, `CUIList.cpp` and `CUIMenu.h` include. Its in-class body had completed `shared_ptr<function<void(BUI*)>>` as soon as `BUI.h` was read.
  - `Add` is defined after `Setup`.
  - The unit includes `CPii.h`, which completes `shared_ptr<CPpuEngine>` early.
  - One swap remains: `shared_ptr<CUIManager>` must be completed before the `mItems` vector. A probe global proves this, but no header in our tree completes it naturally.
  - The two misplaced `.text` functions are unchanged. The target emits the `const_iterator` `*` and `++` before `const_iterator(iterator)` and `begin` in OnAccept's inlined count loop. Writing the loop as a separate init before a `for`, or as a `do`/`while`, does not change this.

## Remaining

- **BWorld:** the target emits the non-template `map_compact_tree::find_leaf(const value_type&)` in the last walk, among the oldest class groups. It calls `insert_node_at` from the same inlined `insert_one`, but `insert_node_at` is emitted early. The unit also has eight functions below 100%.
- **CFileData and CLayout:** the open items are stack-slot and code-generation issues, not ordering (see `sprint-a03.md`).
- **CMsgBalloon (Matching):** `basic_string::operator=` in the class, the `weak_ptr<BWorldObj>` sizeof probe and the early `MessageFormat.h` include fix most of the order. In `increment`, `bool probe = false; if (probe) (void)x->parent_.operator->();` references `__rf` before the loop body without code: a literal `if (false)` is folded before references count, and a live call stays a call because the file is `dont_inline`. The file-wide `#pragma dont_inline on` also kept `map_base::find_or_insert` and `find_or_insert_impl` out of line as 4-byte stubs that objdiff and `ord.py` do not report; an inline explicit specialization of `map_base<int, bool>::operator[]` calling `tree_.find_or_insert` removes them without touching the shared header or CLayoutManager.
- **CDividedGround:** a non-inline function referenced before its definition is noded at the definition, not at the reference. An `inline` function defined before its first use is noded when the use is reached in the DFS. Making `FormatString(const char*, ...)` inline before `LoadPkc`, with `bool probe = false; if (probe) FormatString("");` before `std::string name;`, gives the target `LoadPkc, FormatString, string(), __as__, c_str` block (16 to 7 misplaced). Moving the duplicated attach block of `Load` and `OnEnterWorld` into an inline `AttachToWorld` brings it to 6. The rest are `BuildScene` references (`PushBack`, `OffsetToPtr`, `ScnMdlSimpleData()`, `vector` destructor). These probes are landed (still NonMatching). `LoadAttr` (99.27%) has a register swap (r3/r4) in the inlined `file && file->IsValid()` tests, where the bool result and the `mpData` load trade registers; try the RA rules in `CODEGEN_RULES.md` (an extra temporary or a split condition).
- **CBattleRoyalDisplay:** the target nodes `FormatString(const wchar_t*, ...)` between the walk-generated `move_ptr` destructor and `~CBattleRoyalDisplay`. As a plain `inline` function, or as a friend of a class template instantiated before `Calc`, it is noded in the DFS of `Calc`. If that class template is instantiated after `Calc` (before `Create` or at the end of the file), the friend is never defined and stays an undefined reference.
- **CNoticeDisplay and BWorld:** not attempted; three and five functions are below 100%, so ordering alone cannot flip them.
- **CPlayerPii:** the deque member order is not applied. `GetShadowName` (88.86%) blocks the unit.

## Traps

- A misplaced weak function usually means the template record was created too early or too late. Moving the definition does not help. Check where the class template, a nested class, or a forward declaration first appears.
- `cc.sh` and the sweep call `mwcceppc` without `sjiswrap`, so units with Japanese literals show `.data` and `.sdata` differences that the real build does not have. Compare `.text` only.
- Removing a declaration that names a template-id changes when that instance is created, and with it the walk order of every unit that uses the template. Sweep every game unit before landing a header change.
- A non-template member overload is instantiated in the class walk. A member template is instantiated later, in the function-template loop. Turning an overload into a member template moves it, but it changes the mangled name, so the `symbols.txt` entries must be renamed to match.
