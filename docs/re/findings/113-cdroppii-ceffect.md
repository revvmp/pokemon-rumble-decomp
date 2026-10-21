# Issue 113: CDropPii.cpp and CEffect.cpp

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf` and the dtk split assembly. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). No decompiler output was used.

## F-113-1: unit boundaries

The block `0x8009FD40`–`0x800A1D34` holds the whole of `CDropPii.cpp` and the first 21 functions of `CEffect.cpp`. The file names follow the RTTI names `CDropPii`, `CEffect` and `@unnamed@CEffect_cpp@::EmitterWithParam`. `CEffect.cpp` continues through #114 (`0x800A1D40`–`0x800A3B08`); #113 owns its split because its first function is here.

| Unit | Section | Range | Evidence |
| --- | --- | --- | --- |
| `CDropPii.cpp` | `extab` | `0x80014E20`–`0x80015000` | Records of `0x8009FD40`, `0x8009FDF0`, `0x8009FE50`; `0x80015000` belongs to `0x800A0150` |
| | `extabindex` | `0x8004794C`–`0x80047970` | `0x80047940` names `0x8009FCF0` of `CDlgPiiSelector.cpp` |
| | `.text` | `0x8009FD40`–`0x800A0148` | Constructor, deleting destructor, the override `0x8009FE50` (`"minimap_drop"`) and the secondary-base thunk `0x800A0140` |
| | `.data` | `0x803A1020`–`0x803A1108` | `"minimap_drop"`, the `CDropPii` vtable `0x803A1030` (primary and `BSoundSource` parts), type name and base list ending at `0x803A1108`; `0x803A1010` (`exception`) ends `CDlgPiiSelector.cpp` |
| | `.sdata` | `0x804B6598`–`0x804B65A0` | `CDropPii` type info |
| `CEffect.cpp` | `extab` | `0x80015000`–`0x80015608` | First record `0x800A0150`; `0x80015608` belongs to `0x800A3B10` |
| | `extabindex` | `0x80047970`–`0x80047B2C` | `0x80047B2C` names `0x800A3B10` |
| | `.text` | `0x800A0150`–`0x800A3B08` | `0x800A3AE0` is the static initializer listed in `.ctors`; `0x800A3B10` uses `CEnemyInfo` data |
| | `.ctors` | `0x803529CC`–`0x803529D0` | Entry `0x800A3AE0` |
| | `.rodata` | `0x80352B18`–`0x80352B90` | Read by `0x800A1D40`; `0x80352B90` is read by `0x800AC4B0` |
| | `.data` | `0x803A1108`–`0x803A14E8` | `shared_ptr_deleter<vector<bool>>` vtable, the `MemoryManager`, four `MemoryManagerTmp`, `EmitterWithParam` and `CEffect` vtables, type names and base lists, ending with the `default_delete<vector<bool>>` name; `0x803A14E8` is read by `0x800A3B10` |
| | `.bss` | `0x803E65F0`–`0x803E6600` | Destructor chain registered by `0x800A3AE0` |
| | `.sdata` | `0x804B65A0`–`0x804B6628` | Type infos, `"main"`, `"sub"`; `0x804B6628` (`COLOR0`) belongs to `CEnemyInfo` |
| | `.sbss` | `0x804B8FE8`–`0x804B8FF8` | The `shared_ptr<vector<bool>>` of used effect groups and its use count |
| | `.sdata2` | `0x804B9E20`–`0x804B9E58` | Read by `CEffect.cpp` only; `0x804B9E58` is read by `0x800A3E20` |

With both splits and `NonMatching` object lines, `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-113-2: classes

`CEffect` (0x8C bytes) derives from `ICalc` and `enable_shared_from_this<CEffect>` (offset 4). Its vtable `0x803A13C4` holds the destructor `0x800A1AE0`, `Calc` `0x800A1D30` (empty) and `CalcMtx` `0x800A1D40`.

| Offset | Member | Evidence |
| --- | --- | --- |
| 0xC | Flags: 1 finalized, 2 local matrix, 4 follow rotation, 8 follow object alive, 0x10 target alive | `Create` overloads, `Finalize`, `CalcMtx` |
| 0x10 | `BWorld*` | `Create` |
| 0x14 | Handle read by the script bindings after `BWorld` adds the effect | `ScriptBinding_NewEffectWithPosDir` |
| 0x18 | `Param` (0x14 bytes): `EffectInfo*`, `bool` enable, then `u16` emit span, `u16` life, `f32` radius, `f32` height, overlaid by `f32` scale at 0x8 | `Create` switch and `CalcMtx` |
| 0x2C | `nw4r::ef::Handle<Effect>` | `HandleBase` calls |
| 0x34 | Local `MTX34`, a Y rotation built by `SinCosFIdx` | `Create` overloads |
| 0x64 | Position | `Create(…, const VEC3&, f32)` |
| 0x70, 0x84 | `weak_ptr<BWorldObj>` follow and target objects | Destructor extab |
| 0x78 | Target node position | `CalcMtx` |

`EffectInfo` entries (0x18 bytes, table `0x803550E0`) hold an index, the effect name, two node names (`"ROOT"`, `"HEAD"`), a type that selects the `Param` override (1 stop emission tracks, 2 radius, 3 extend emission, 4 and 5 particle life through a `PrevEmission` callback, 6 scale) and a flag that selects the capture effect group.

`nw4r::ef::MemoryManager` (0x3C bytes, vtable `0x803A11C0`) is defined by the game in this file and implements the 24 `MemoryManagerBase` slots over a buffer from `GameMemory::Allocate`: pool counts and `MemoryManagerTmp<T>*` pools at 0x4–0x20, heap start and end at 0x24 and 0x28, used and free block lists at 0x2C and 0x30, and the first block at 0x38. Heap blocks have a 0x20-byte header: list links at 0 and 4, neighbour links at 8 and 0xC, size at 0x10 and a used flag at 0x14. `MemoryManagerTmp<T>` (0x24 bytes) holds the object array, free and active `ut::List`s, the object size and its vptr at 0x20. The emitter pool uses `@unnamed@CEffect_cpp@::EmitterWithParam` (0x168 bytes), an `Emitter` with a `CEffect::Param` copy at 0x154.

`CDropPii` derives from `CPii` (constructor `0x800DCA10`, destructor `0x800DCBE0`) with type bit 0x400.

## F-113-3: reconstruction

`src/game/CEffect.cpp` reconstructs 19 of the block's 25 functions, 14 of them at 100%:

| Address | Function | objdiff |
| --- | --- | --- |
| `0x800A0150` | `InitializeEffect(u32, u32, u32, u32, u32)` (called by `main` with 500, 1000, 1000, 10000, 32) | 99.62% |
| `0x800A0720` | `MemoryManagerBase::~MemoryManagerBase` | 100% |
| `0x800A0760` | `MemoryManager::AllocHeap` | 100% |
| `0x800A08B0`, `0x800A0900` | `EmitterWithParam` constructor and destructor | 100% |
| `0x800A0960` | `FinalizeEffect()` (empty, called at the end of `main`) | 100% |
| `0x800A0CC0` | `CEffect::CEffect` | 100% |
| `0x800A0D40` | `enable_shared_from_this<CEffect>` destructor | 100% |
| `0x800A0DB0` | `CEffect::Create(BWorld*, const Param&)` | 100% |
| `0x800A12F0`, `0x800A1350` | `PrevEmission` callbacks | 100% |
| `0x800A13B0`, `0x800A14E0`, `0x800A1670`, `0x800A1810` | `Create` with position, random, given and following rotation | 99.75–99.85% |
| `0x800A19B0` | `Create` with follow and target objects | 100% |
| `0x800A1AE0` | `CEffect::~CEffect` | 100% |
| `0x800A1C40` | `CEffect::Finalize(BOOL)` (`ScriptBinding_method_effFinalize` and `effFinalizeAll`) | 100% |
| `0x800A1D30` | `CEffect::Calc` | 100% |

The four rotation `Create` overloads differ only in the relocation names of three `.sdata2` constants: the literal pool order is set by `CalcMtx` and later functions of #114. `InitializeEffect` differs only in the registers of the pool loops. `AllocEffectGroupID` `0x800A0970` and `FreeEffectGroupID` `0x800A0BE0` use MSL `vector<bool>`, which the repository lacks, and stay unreconstructed. `CDropPii.cpp` is split only: its constructor and override need a complete `CPii` declaration (constructor taking a `shared_ptr` by value, the four virtuals `CPii` adds) and the classes constructed by `0x8009FE50` (0x13C bytes, `0x800BE9E0`; 0xF0 bytes, `0x800C4810`).

The reconstruction adds placement `operator new[]` and `operator delete[]` to `<new>`, the null-manager branch of `EffectSystem::SetMemoryManager` (zeroing `mMaxGroupID` and `mActivityList`, as at `0x800A06E4`), `BWorld::GetEffectGroupID` `0x80060C10`, `GetCaptureEffectGroupID` `0x80060C60` and `RemoveOldestEffect(bool)` `0x80061750`, `Lock(weak_ptr<BWorldObj>)` `0x8006A370` and `RandReal(f32, f32)` `0x8019E010` (behind `ScriptBinding_RandReal`).

## F-113-4: CDropPii source acceptance

`src/game/CDropPii.cpp` reconstructs all four functions of the unit, each at 100% in objdiff, and the unit is linked as `Matching`:

| Address | Function |
| --- | --- |
| `0x8009FD40` | `CDropPii::CDropPii(shared_ptr<CPiiPersonalData>, u32)`, passing `type | 0x400` to `CPii` |
| `0x8009FDF0` | `CDropPii::~CDropPii` |
| `0x8009FE50` | `CDropPii::Initialize(bool)`: creates a `CHPBar(1)` initialized with -1, and a `CMiniMapMarker` initialized with kind 0 and `"minimap_drop"`, then calls `CPii::Initialize` |
| `0x800A0140` | `@72@CDropPii::~CDropPii`, the compiler-generated thunk for the `BSoundSource` base |

The unit has the same shape as `CEnemyPii.cpp` ([F-115-3](115-enemy-info-pii-facility.md#f-115-3-complete-enemy-source-acceptance)) without the script loader: the marker kind is 0 where `CEnemyPii` passes 1. The vtable `0x803A1030` is named `__vt__8CDropPii`. With the object `Matching`, `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

`CEffect.cpp` stays `NonMatching`. A pool-creation helper template for the four `MemoryManager` pools lowered `InitializeEffect` from 99.62% to 94.12%, so the explicit pool blocks stay. `AllocEffectGroupID` and `FreeEffectGroupID` still need MSL `vector<bool>`, a shared-header addition outside this unit, and 37 functions of the file belong to #114.

## Traps

- **A heap header can be padded by alignment.** The block header uses 0x15 bytes but every offset uses 0x20; `__attribute__((aligned(32)))` on the struct reproduces `sizeof`.
- **Pointer arithmetic form changes scheduling.** `reinterpret_cast<u8*>(pBlock + 1) + size` produced the original `stwu`; `(u8*)pBlock + size + sizeof(HeapBlock)` and `(HeapBlock*)((u8*)pBlock + size) + 1` did not.
- **Constructor body order beats the initializer list.** The original `MemoryManager` stores counts, then null pools, then the heap end before its start; assignments in the body in that order matched, the initializer list did not.
- **A signed compare against 1 means `BOOL`.** `Finalize` compares its flag with `cmpwi 1`; `bool` gives `cmplwi` and a plain test gives `cmpwi 0`.
- **A float-wise copy of a `VEC3` comes from the `_VEC3` constructor.** The implicit copy moves words; the original loads and stores floats.
