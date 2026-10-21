# Issue 208: SelectLight and CSimpleGround

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #208 starts at `0x8010D7D0`, inside the `game/CShadowCamera.cpp` unit registered by [F-207-2](207-cshadowcamera.md#f-207-2-gamecshadowcameracpp-unit-boundaries).

## F-208-1: SelectLight and CSimpleGround

`0x8010D7D0`-`0x8010DDD8` are nine functions of the `CShadowCamera.cpp` unit. They are reconstructed in `src/game/CShadowCamera.cpp` (after `SetupShadowModel`, in address order) with the class declared in `include/game/CSimpleGround.h`.

| Address | Function | Match |
| --- | --- | --- |
| `0x8010D7D0` | `CShadowCamera::SelectLight()` | 100% |
| `0x8010D8E0` | `CSimpleGround::CSimpleGround(u32)` | 100% |
| `0x8010D940` | `CSimpleGround::~CSimpleGround()` | 100% |
| `0x8010D9E0` | `CSimpleGround::Initialize(bool)` (empty) | 100% |
| `0x8010D9F0` | `CSimpleGround::LoadScript(const char*, int, const shared_ptr<NPpu::CEnvironment>&)` | 100% |
| `0x8010DB40` | `CSimpleGround::LoadModel(const char*, bool)` | 99.6% |
| `0x8010DD20` | `CSimpleGround::Calc()` | 100% |
| `0x8010DD70` | `CSimpleGround::OnLeaveWorld()` | 100% |
| `0x8010DDC0` | `CSimpleGround::GetPpuEngine() const` (inline, emitted last) | 100% |
| `0x8010DDD0` | `@72@__dt__13CSimpleGroundFv` (BSoundSource-base destructor thunk) | 100% |

Behaviour:
- `SelectLight` walks the light sets of the world's `ScnRoot` light setting and takes the first light that is neither a spot nor a specular light (`IsDiffuseLight`). It stores its set and light indexes and sets the automatic-selection flag; with no world or no such light it stores 0 and 0.
- `CSimpleGround` is a `BModelObj` with type bit `1 << 14` (the `IsKindOf` test in `ScriptBinding_method_CSimpleGround_loadModel` at `0x801409A0`), a reserved word at `0xF8` (zeroed, never read) and the script engine `shared_ptr<CPpuEngine>` at `0xFC`, the same shape as `CItem`. Its vtable `0x803ADC90` overrides the destructor, `Calc`, `GetPpuEngine`, `OnLeaveWorld`, and adds one empty virtual after `BModelObj`'s last slot, named `Initialize(bool)` after `CItem`'s slot (descriptive).
- `LoadScript` creates the engine and loads a script for the object. `LoadModel` is called by the script binding with a path and `background = false`; it loads the file list, loads the model without a model name through the shared `CModel::LoadParams` constant `{nullptr, false, 0xD15, 1, nullptr}` at `0x80354E20`, terminates the fiber with 1 on failure, otherwise draws it in the translucent priority 1, installs it with `ChangeModel(model, 1)` and sets `gFrameTimingReset`.
- `LoadModel` differs from the original only in the names of two relocation targets (the `CModel` deleter vtable `0x8039BA78` and the parameter constant `0x80354E20`), which `symbols.txt` keeps under placeholder labels.

Symbols renamed in `symbols.txt`: the nine functions, `__vt__13CSimpleGround` (`0x803ADC90`) and `__RTTI__13CSimpleGround` (`0x804B7360`).

## Remaining work in #208

`0x8010DDE0`-`0x80110134` are the first part of the `MapPiiTable` unit that runs to `0x801119A4`; see F-208-2 for the split, the reconstructed functions and what is left.

## Method

GNU objdump 2.42 with `-M gekko`, call-site scans of `build/wpse-v1/main.elf`, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **Pass-through virtuals after `BModelObj` are visible only in the second vtable group.** `BModelObj`'s new virtuals sit after the `BSoundSource` destructor thunk at vtable offset `0x48`, so a class that adds one more virtual shows it as the entry after `AddDrawLayer` (`0x800570D0`), not in the first sixteen slots.

## F-208-2: the MapPiiTable unit

`0x8010DDE0`-`0x801119A4` is one translation unit registered as `game/MapPiiTable.cpp` (descriptive file name; the class is the shared table of [F-218-2](218-table-contract.md), used through `MapDesc::mPiiTable` and, under the alias `MapPiiTable`, by the session's move-description storage). F-209-4 left the split to #208.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x800290E8`-`0x800292DC` | Entry of the record for `0x8010DE20` to the entry of `0x801119C0` |
| `extabindex` | `0x8004BD44`-`0x8004BE28` | 19 records, `0x8010DE20` to `0x80111930`; the record after names `0x801119C0` (next unit) |
| `.text` | `0x8010DDE0`-`0x801119A4` | Starts at the constructor after `CSimpleGround`; ends at the end of the last destructor `0x80111930` (`0x74` bytes), not at the 16-byte padding |
| `.data` | `0x803ADD58`-`0x803ADDC0` | `"vector length error"`, the exception name cluster, `"tree::insert length error"`; `0x803ADDC0` starts the second cluster of the next unit |
| `.sdata` | `0x804B7368`-`0x804B7370` | The two `bool` constants returned by `set::insert` (`true` for a new node, `false` for an existing one) |

The split links and the DOL verifies unchanged.

### Layout and behaviour

`MapPiiTable` is 0x2C bytes: `vector<NPpu::CValue>` at 0 (the value table), `set<string>` at `0xC` and `set<wstring>` at `0x18` (string pools whose elements the values point into, so the pointers stay valid across inserts), and the row and column counts at `0x24` and `0x28`. The table has `rows + 1` rows of `columns` values (`GetIndex` is `(row + 1) * columns + column`).

| Address | Function | Match |
| --- | --- | --- |
| `0x8010DDE0` | `MapPiiTable()` | 100% |
| `0x8010DE20` | `~MapPiiTable()` (both tree `destroy` calls unrolled four levels) | 100% |
| `0x8010E6F0` | `Clear()`: swaps the table and both pools with empty locals, zeroes both counts | 100% |
| `0x8010F100` | `Initialize(int rowCount, int columnCount)`: stores the counts, `resize((rowCount + 1) * columnCount)`, clears both pools | 100% |
| `0x8010F150` | `vector<NPpu::CValue>::resize(unsigned long)` | 96.5% |
| `0x8010F6E0`, `0x8010FB30` | `set<string>::clear()`, `set<wstring>::clear()` | 100% |
| `0x8010FF80` | `SetValue(int row, int column, const CValue&, const CEngine*)`: a wide string is pooled (`InsertWString(engine->ToWString(value))`) and stored as a wide string pointer, a string likewise, anything else is copied | 100% |
| `0x80110070`-`0x80110120` | `CValue::IsWString`, `SetWStringPtr`, `IsString`, `SetStringPtr`, `operator=`, `GetValue(int)`, `GetCell(int, int)`, `GetIndex` | 100% |
| `0x80110140`, `0x80110530` | `InsertString(const char*)`, `InsertWString(const wchar_t*)`: find in the pool, insert when absent, return the stored data pointer (#209 range) | 92.8% |
| `0x80110990`, `0x80110F90` | `tree_imp<wstring>::insert_node_at`, `tree_imp<string>::insert_node_at` | 99.7% (data label names) |
| `0x80110BD0`, `0x801111D0` | `tree_imp::destroy` for the wide and narrow pools | 100% |
| `0x80111670`, `0x801117D0` | `tree_imp::find_leaf` for the wide and narrow pools | 100% |
| `0x80111590` | `allocator<string>::destroy` | 100% |

The four small destructors `0x80110920` (`vector<CValue>` through its base chain), `0x801115B0`, `0x80111610` (`alloc_ptr` of a set node) and `0x80111930` are weak instances referenced from exception cleanup tables; their sources are not written yet.

### Header work this needed

- `<string>`: `char_traits::compare` (`memcmp` / `wmemcmp`), `basic_string::compare(pos1, n1, const charT*, n2)` and `compare(const basic_string&)`, and `operator<`. The shape matters. The inlined search in `find_leaf` loads the pooled node's data and size once through `get_buffer`, takes the key size separately, clamps with `min(size - pos1, n1)` (`std::min` returns a reference to a stack copy, which is why the compare result is read back from the stack), calls the traits compare with `min(rlen, n2)` and, only when it returns 0, returns `-1`, `1` or `0` with `return` statements (an `if`/`else` assignment of the same three values became a branchless `subf`/`or`/`srwi` sequence). `get_buffer` now forms the short-string data pointer as `reinterpret_cast<const charT*>(&rep_) + 1`, so the compiler hoists it out of the search loop.
- `tree_imp`: iterator `operator*`, `==` and `!=`, `begin()`, `end()`, and `swap` (exchange size, root and begin, re-parent the roots on the end nodes, point `begin` at the end node when a tree is empty). `set` gained `swap`.
- The members `Initialize`, `Clear` and `SetValue` and the private `GetIndex`, `GetValue(int)`, `InsertString` and `InsertWString` are declared on `MapPiiTable` in `include/game/MapDesc.h`.

### Traps

- **Which inline functions become calls is decided per region.** `CValue::IsString`, `SetStringPtr` and the `MapPiiTable` accessors are called, not inlined, from `SetValue`, while the vector and tree templates inside `resize` and `Initialize` inline completely. Wrapping `Initialize` and `SetValue` in `#pragma dont_inline on` (and resetting before the accessors) reproduces both: `resize` and the two `set::clear` stay separate functions and the `CValue` members are emitted out of line.
- **A swap receiver decides the register allocation.** `mValues.swap(values)` matches `Clear()` exactly, `values.swap(mValues)` differs in register numbers only. The same holds for the two sets.
- **Temporaries versus named locals.** `std::vector<T>().swap(x)` compiles a null check on the temporary and a different frame; named locals in declaration order reproduce the original frame (the vector at `0x20`, the narrow set at `0x14`, the wide set at `8` from the stack pointer).
- **The pool search is `find` followed by `insert`.** It compares `node < key` with the node as the left operand and inserts only when the iterator equals `end()`; `insert` alone does not produce the second `find_leaf` call.

## F-208-3: NPpu::CValue has two plain words

`vector<NPpu::CValue>::resize` stayed at 93% because the original zero-initializes the temporary of `construct(p, T())` inside the loop (`li r0, 0; stw r0, 0x58(r1); stw r0, 0x5c(r1)` before the null check of the placement `new`), while the declaration produced an uninitialized temporary and loaded it. The cause was the class layout:

| `CValue` storage | `T()` zero-initialized | `s[i] = v` forwards the constants of `v` | out-of-line `operator=` |
| --- | --- | --- | --- |
| user `operator=`, anonymous `union` (old) | no | yes | explicit |
| implicit `operator=`, anonymous `union` | yes | no (a copy through the stack) | implicit |
| implicit `operator=`, two plain `int`s | yes | yes | implicit (`lwz`/`lwz`/`stw`/`stw`, 0x14 bytes, same as the explicit one) |

The original is the third row. `CValue` therefore keeps `int mType; int mInt;` and the float, string, wide string and reference views are accessors (`GetFloat`, `GetStringPtr`, `GetWStringPtr`, `GetPointer`, `SetPointer`); the 30 uses in `NPpuVM.cpp` were converted. `SetFloat` writes through a `reinterpret_cast` inside the class body because an out-of-line `SetFloatBits` call would appear under `#pragma dont_inline on` (`ScriptCamera::SetFloat` dropped to 60% until it was written in place). Per-function comparison of `report.json` against the previous build: no function loses a point, `resize` rises from 93.0% to 96.5% and `CEngine::BinaryOp` gains 0.04%.

What is left of `resize` is register numbering (the original keeps the size in `r6` and the capacity in `r7`, the source gives the opposite order), the frame offsets of the exception-state bytes, and which of the two address registers of the inlined `length_error` holds the literal.

### Traps

- **A union hides a plain struct.** A class whose members are copied through a union is not forwarded by the optimizer and is not memberwise; if value-initialization or constant forwarding differs between the target and the source, check the class for a union before touching the container.

## F-208-4: what is left of #208

Every function of `0x8010D7D0`-`0x80110134` is reconstructed. All are at 100% in objdiff except `CSimpleGround::LoadModel` (99.6%, two data relocation names) and `vector<NPpu::CValue>::resize` (96.5%; see F-208-3 for the remaining register-number and frame-offset differences). `InsertString` and `InsertWString` (`0x80110140`, `0x80110530`, #209) are at 92.8%: the frame holds the key and the `set::insert` result as separate pair copies in the original. The weak destructors `0x80110920`, `0x801115B0`, `0x80111610` and `0x80111930` (#209) are not written.


## F-208-6: primary vector interfaces and exact current producers

The authentic 2006 MSL headers are preserved with the archive SHA-256 and acquisition manifest in the external 198-reference/primary-header-research/2006-primary-manifest.json. They distinguish capacity accessor calls, a public reserve forwarding method, destructive move construction, append reallocation and direct member reads in resize. The bounded reconstruction uses those declaration contracts with independently inspected native producers; it does not import the complete original headers.

The public reserve method forwards to the existing backend. Reallocation uses a distinct move_construct_to_end API: the native pointer producer calls memcpy and then clears the source, proving destructive move semantics rather than ordinary copying. Class elements retain their existing allocator construction and forward range precondition. Append reallocation computes the missing capacity from current size, clears the moved source according to the existing traits and exchanges the actual capacity, pointer and size. Append reads capacity through cap() rather than a cached scalar. Resize reads its size member directly. Object layouts, compiler flags and existing trait policies are unchanged.

Two independent contexts establish the capacity correction. MapPiiTable value-vector resize at 8010F150/1416 changes from eleven register-role words to exact code and all 22 actual call targets. Its 148-byte exception record matches, including all eight cleanup references; the buffer lifetime identifies the 116-byte cleanup at80111930, whose code, call and eight-byte EH record also match. Balloon's outlined owner-vector append at 800CA210 changes 144 to native 164, with every instruction, six actual call targets and its eight-byte EH record exact. The independent Balloon-owner receipt is 152-reference/208-cap-independent-append-proof.json.

The combined vector contracts reproduce all twelve CPpuRegister functions, 3812 bytes, 118 applied relocations, eleven exception records 600 bytes and seventeen cleanup relocations; full 368-byte data and 16-byte small-data extents match in its owner's independent producer. See 209-reference/209-register-native-full-12. These are prerequisite producer proofs; source selection is owned by 209 and is not claimed here.

The same-base 759-source-object preflight identifies 51 consumers of vector. Only seven NonMatching objects have allocated semantic deltas. Previous exact streams are preserved after five independently proven interface-name corrections. Native ScriptCamera reallocate 196, its move forwarding 44 and destructive copy 136 match all instructions and sixteen applied targets; its EH records 24/8/8 match. Native SaveToSlot 1416 and three typed reserve bodies 516/572/492 match every instruction and applied relocation, with reserve target addresses 800B37F0,800B4000 and800B4960. SaveToSlot's 200-byte exception record is identical before and after but retains existing native stack/action differences; it is explicitly not an EH match. The preliminary score report is not an acceptance receipt. The self-contained native auditor and its input hashes are preserved in `208-combined-controls/208-shared-native-audit.py` and `shared-native-proof.json`. All other Matching code, data and destinations must remain exact in the official latest-main gate.

The five symbol corrections change only inferred interface names, preserving addresses, sizes and attributes: the three save-record reserve methods are public vector methods, and 80134E80/80134EB0 are the pointer move-construction methods. Actual typed callers, original declarations and the native source-clearing copy distinguish these identities; identical machine bodies alone are not used as type evidence.

The MapPiiTable class remains 44 bytes with its existing members. Original 208 owns 26 functions, including ten camera/ground functions through 8010DDD0's thunk under 207's first-file authority. This unit starts 8010DDE0 and includes thirteen continuation functions in 209, ending 801119A4. Its remaining string insertion temporary contexts and private helper bindings still require whole-unit source-selected acceptance. A full original-header reference reproduces the named game bodies exactly; it is only an outside source-context reference, not a published source dependency. No original 208 or 209 completion is claimed by this prerequisite.

### Traps

- A cached accessor result can erase original calls when a caller inhibits inlining. The native Balloon append has three cap() calls; replacing them with a scalar cache changes 164 bytes to 144. Pair the public native call context with the inlined context before accepting a library correction.
- A genuine move operation can have the same old inferred symbol as copying. Here the actual native memcpy followed by source clearing, the caller and the original declaration prove the move interface. Preserve all pointer destinations when correcting names.
- The remaining MapPiiTable insertion mismatch is not solved by changing stored comparator access, a named insertion-result pointer, generic pair/get_pair conversion, value-node iterator storage, friend comparison operators, named comparison lengths or out-of-class compare definitions. Those bounded declaration controls remained fifteen words or worsened to nineteen and are not published. The genuine complete original-header reference remains the useful discriminator; do not sweep register forms.

The exact-head full gate at `b70b05ae2f633c7d442e1ea486dd2a0901e2d311`, based on `e4d1adebb1d7d799e56b7cc4bf100a83c67c61b4`, rebuilt all 759 source objects and 711 selected objects. Every Matching allocated section and relocation remained identical. Six NonMatching objects had semantic deltas; all 558 previously exact streams retained their bytes and actual targets after the five proven interface renames. ScriptCamera was identical after those name corrections. Both complete incremental builds and frozen verification exited 0, with identical baseline and candidate DOL files. The independent seven-function / 3372-byte native audit passed, including ScriptCamera EH and baseline-identical SaveToSlot EH. The reproducible guard is `208-reference/208-land-vector.py --check`; its receipt is `208-combined-controls/official-proof.json`. The same guard runs inside the official merge loop after every rebase.
