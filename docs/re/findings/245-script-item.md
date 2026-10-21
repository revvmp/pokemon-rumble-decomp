# Script item binding unit (#245)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Issue #245 lists 17 functions in `.text` `0x80140C40`-`0x80142070`. They are one source unit, `game/ScriptItem.cpp` (descriptive name; the survey label was `bind_2`), that starts exactly at the issue's first address and ends at `0x801420E4`, just after the last function of the issue.

## F-245-1: unit boundaries

| Section | Range | Evidence |
| --- | --- | --- |
| `.text` | `0x80140C40`-`0x801420E4` | The previous unit ends with the `shared_ptr_deleter<CSimpleGround>` group `0x80140A70`-`0x80140C40` (`get_deleter`, `dispose`, two destructors, `default_delete::operator()`), emitted after `method_CSimpleGround_loadModel`. This unit's own deleter group (`CFacility`, `CWarpZone`) is `0x80141DF0`-`0x80142070`, and the last function, `0x80142070` (`~__vec_split_buffer<UsedPassword>`), is referenced only from the `extab` cleanup of `SetPasswordUsed` (`0x8002F2BC`, `0x8002F2D8`). `0x801420F0` (`CatGets`) starts the next unit |
| `extab` | `0x8002EF78`-`0x8002F32C` | Entries of the 21 `extabindex` records whose functions lie in the range; the next record (`0x801420F0`) points to `0x8002F32C` |
| `extabindex` | `0x8004DE74`-`0x8004DF34` | First record names `0x80140C40`; the record before it names `0x80140C00` |
| `.data` | `0x803B67A8`-`0x803B6980` | Starts with `"vector length error"` and the exception type names used by the inlined `vector` growth of `SetPasswordUsed`, then the deleter vtables `0x803B67F4` (`CFacility`) and `0x803B686C` (`CWarpZone`), the `UsedPassword` vtable `0x803B68E8` with its name and base list, and the `default_delete` type names. The previous unit's `.data` ends with `default_delete<CSimpleGround>` at `0x803B67A8`; `0x803B6980` starts a 0x3C0 object of the next unit |
| `.sdata` | `0x804B79E0`-`0x804B7A10` | `"%08d"`, the type-info records of both deleters, `UsedPassword`, and both `default_delete` types; `0x804B79D8` is the previous unit's `default_delete<CSimpleGround>` |

The unit has no `.ctors` entry, `.sdata2`, `.bss` or `.sbss`. Split and `configure.py` registration verify with the original object (`NonMatching`). The CItem deleter group that `NewItem` needs (vtable `0x803A01CC`) is emitted in the `CItem` unit, and the linker keeps one copy of each weak function. The source therefore defines many weak functions (`~shared_ptr<CItem>`, `VectorStructs` accessors, `std::exception` destructors) that the original object of this unit does not contain; objdiff lists them as source-only.

## F-245-2: the functions of #245

| Address | Binding (original spelling) | Evidence |
| --- | --- | --- |
| `0x80140C40` | none: file-local `AddItem(NPpu::CEngine*, shared_ptr<CItem>)` | Reads the argument count, an optional parent (`CPpuEngine::PopWorldObj`) and an optional script name, swaps in the engine's register (`BWorldObj::SwapRegister`, `0x8006A4A0`), calls virtual slot 30 (`CItem::Initialize(false)`), links the parent (`BWorldObj::SetParent`, `0x8006A290`), adds the item to the world and loads the script (`CItem::LoadScript`). Pushes the object id or 0. Called by `NewItem`, `NewWarpZone` and `NewFacility` |
| `0x80140EC0`, `0x80141050`, `0x801411E0` | `NewItem`, `NewWarpZone`, `NewFacility` | `new CItem(CItem::TYPE)` (0x104 bytes), `new CWarpZone(1 << 12)` and `new CFacility(CFacility::TYPE)` (both 0x110 bytes); each returns `AddItem`'s result |
| `0x80141370` | `method_CItem_loadModel` | Pops the name, `PopWorldObj`, `DynamicCast<CItem>`, `CItem::LoadModel(name, false)`, pushes 0 |
| `0x80141440` | `method_loadModel` | `CPpuEngine::PeekWorldObjType(1)` (`0x80101240`, descriptive: `GetArg(1)` as an object id, then `BWorld::GetObject` and the type word) tests for `CItem::TYPE`; the `CItem` path is `method_CItem_loadModel` inlined, otherwise it calls `method_CSimpleGround_loadModel` |
| `0x80141530` | `IsPasswordUsed` | `wide_string_to_decimal` of the popped wide string, then a scan of `SlotData::mUsedPassword` of `GetSaveData(-1)`; pushes 0 or 1 |
| `0x80141620` | `SetPasswordUsed` | A local `UsedPassword` (vptr and one `int`), then `push_back` into the same vector; pushes 0 |
| `0x80141BC0` | `IsSamePassword` | Pops five `u32` and a number, formats the number with `"%08d"`, SHA-1 hashes the first eight characters, hashes the digest again and compares the result with the five words |
| `0x80141DF0`-`0x80142070` | weak | `shared_ptr_deleter` `get_deleter`, `dispose` and destructor for `CFacility` and `CWarpZone`, their `move_ptr` destructors, and `~__vec_split_buffer<UsedPassword>` |

`CWarpZone` has no header yet; `include/game/CWarpZone.h` declares what the unit needs: derived from `CItem`, `TYPE = 1 << 12`, a constructor taking the type (`0x80120000`, which ORs 0x1000 into it and zeroes three words at 0x104) and a virtual destructor. The three members are named `mDestination` descriptively (low confidence).

## F-245-3: objdiff status

Every function that exists in the original object is at 100% except `SetPasswordUsed`, which is at 96.04% with the same size (1428 bytes) and instruction sequence; only stack slot numbers of one-byte tag temporaries (the `int2type` arguments of the inlined `vector` growth) and their store order differ. This is the same residue that [F-127](127-cgamesession-2.md) records for `CGameSession`'s `push_back` and `reserve`. The unit stays `NonMatching`.

- **`vector<T>::push_back` for non-POD `T`.** The original growth path constructs the new element with a counted loop (`construct_at_end(1, x)`, n = 1, not folded) and clears the moved-from size (`clear_moved()`) before the swap. `include/vector` now has the non-POD `construct_at_end(n, x)` overload and `push_back_realloc` uses it. The same original shape is in `CGameSession`'s four `push_back` instances, which rise from 92.8-93.7% to 95.8-96.8%. A function-by-function comparison of `report.json` before and after shows no other change.
- **`UsedPassword::Data::mPassword` is `int`.** `IsPasswordUsed` compares the decimal (a `long`) with the stored word using `cmpw`; with `u32` the compiler emits `cmplw`.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf` and the split object, a Python scan of `extabindex` records and of absolute and small-data references of the range, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **A script-binding file can begin with an unnamed helper.** `0x80140C40` has no entry in the binding table; it is called by the three `New*` bindings and is the unit's first function. It follows the previous unit's deleter group, which is the file-end marker.
- **A by-value `shared_ptr` parameter hides the real ABI shape.** The caller copies the argument into a temporary and the callee copies it again into a local after the argument pops. The local copy must be declared after the pops; declared first it moves the copy to the start of the function.
- **Register order follows the declaration order of the engine pointer.** `AddItem` matched only after the parameter became `NPpu::CEngine*` with a `CPpuEngine*` local; the same code with a `CPpuEngine*` parameter allocates the saved registers in the opposite order.
- **Include order can break the nw4r headers.** `<game/BWorld.h>` before `<game/CFacility.h>` pulls in `RoundUp` from the MEM headers and breaks `ut_algorithm.h`; include `CFacility.h` (or `CItem.h`) first.
