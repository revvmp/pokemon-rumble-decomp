# CPpuEngine unit and class (#195)

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, analysed in the linked `build/wpse-v1/main.elf`, which verifies byte-identical. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). Issue #195 lists 23 functions in `.text` `0x800FE5F0`–`0x80100E74`. They are the first functions of one source unit that runs to `0x801035C8` and so spans #195 and #196.

## F-195-1: unit boundaries

The unit is registered as `game/CPpuEngine.cpp` (descriptive file name; no source-file string is embedded).

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80026DC4`–`0x80027C60` | Entries of the `extabindex` records below |
| `extabindex` | `0x8004B198`–`0x8004B39C` | First record names `0x800FE5F0`; the record at `0x8004B39C` names `0x801035D0`, the next unit |
| `.text` | `0x800FE5F0`–`0x801035C8` | Previous function `0x800FE5B0` is the `shared_ptr_deleter<CPostEffect>` destructor (slot 0 of the deleter vtable `0x803A95A4` in `CPostEffect`'s data). The static initializer `0x80103540` (`.ctors` `0x803529DC`) is followed by the two `CPpuEngine` thunks `0x801035B0` and `0x801035C0`; `0x801036D0` already calls the next unit's tree helpers |
| `.ctors` | `0x803529DC`–`0x803529E0` | Static initializer `0x80103540` |
| `.data` | `0x803A96D8`–`0x803A97D8` | First literal `"vector length error"` and the exception type-name cluster used by `0x800FFC50`, `0x80101CA0` and `0x80102400`; `"script.arc/"`, the `CPpuEngine` vtable `0x803A9730`, RTTI names and base list, and `"tree::insert length error"` (used by `0x80102400`, `0x80102A00`). `0x803A97D8` (`"NewStageCamera"`) belongs to the next unit. The preceding strings `"5:/post_effect.arc/"` and `"dummy_capture"` are used by `CPostEffect` functions |
| `.bss` | `0x803E6630`–`0x803E6648` | The two 12-byte destructor chain entries registered by the static initializer; `0x803E6660` is used by the next unit |
| `.sdata` | `0x804B7148`–`0x804B7168` | `"5:"` (`0x804B7148`) and the type info of `CPpuEngine`, `enable_shared_from_this<CPpuEngine>` and `NPpu::CEngine`; `0x804B7140` is `default_delete<CPostEffect>` type info |
| `.sbss` | `0x804B9048`–`0x804B9068` | Default environment `0x804B9048`, default stage target `0x804B9050`, script root `0x804B9058`, temporary-ID guard `0x804B9060`; `0x804B9068` is used by `0x80106660` |
| `.sdata2` | `0x804BA0D0`–`0x804BA0D8` | `0.0f` frame of `0x800FFC50`; `0x804BA0C8` belongs to `CPostEffect` and `0x804BA0D8` to `0x8010C490` |

`lbl_803E663C` was resized from 0x24 to its real 0xC bytes so that the `.bss` split can end at the last object. The split changes no output bytes; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-195-2: CPpuEngine layout and virtual functions

The class is 0xC4 bytes: `new(0xC4)` in `0x80102220` and in every caller of the constructor. The RTTI base list at `0x803A9760` names `NPpu::CEngine` at 0, `ICalc` at 0x64 and `std::tr1::enable_shared_from_this<CPpuEngine>` at 0x68. `NPpu::CEngine` is the class `include/game/NPpuVM.h` calls `NPpu::CVM` (F-231-2); the header keeps the `CVM` name until the owner of #281 to #283 renames it.

| Offset | Member | Type | Evidence |
| --- | --- | --- | --- |
| `0x00` | base | `NPpu::CVM` (`NPpu::CEngine`) | Constructor calls `__ct__Q24NPpu3CVMFv`; destructor ends with `__dt__Q24NPpu3CVMFv` |
| `0x64` | base vptr | `ICalc` | Constructor stores `0x803A9738` |
| `0x68` | base | `enable_shared_from_this<CPpuEngine>` | RTTI base list; destructor releases the weak count at 0x6C |
| `0x70` | `mFlags` | `u32` | `0x801006D0` sets bit 0 (called only by `ScriptBinding_LeaveInitPart`); `Calc` keeps stepping through waits until it is set |
| `0x74` | `mOwnerObj` | `BWorldObj*` | `0x80100AF0` stores its object argument; `GetWorld` reads `mWorld` (0x14), `0x80100DB0` compares `mId` (0x18); `Calc` removes it from its world when the script ends |
| `0x78` | `mCallbacks` | `std::map<int, std::list<int> >` | 0x10 bytes: size 0x78, root 0x7C, comparator 0x80 (never stored), begin pointer 0x84 = &root. Node values hold an `int` key and a list of 12-byte nodes with one word; `0x801012D0` (`ScriptBinding_InsertCallback`) inserts at the front (0), the back (-1) or before a matching value. Destructor `0x800FE710` |
| `0x88` | `mOwner` | `shared_ptr<BWorldObjOwner>` | Assigned to `BWorldObj` 0x24 through `0x8006A4A0` (F-231-2). Its constructor `0x801119B0` and destructor `0x801119C0` are called out of line |
| `0x90` | `mStageDescTarget` | `StageID` | Copied from the static default in the constructor; set by `ScriptBinding_SetStageDescTarget` (three popped bytes, the layout of `ScriptBinding_MakeStageID`) and `SetStageDescTargetAsCurrent` (the `IGameMap` slot 6 value) |
| `0x94` | `mFiles` | `std::set<std::tr1::shared_ptr<CFileData> >` | `0x800FFAE0` inserts each loaded file with `find_leaf` `0x80103460` and `insert_node_at` `0x80102400`, ordered by the control block; destructor `0x800FEAF0` |
| `0xA0` | `mPlacements` | `std::vector<nw4r::math::MTX34>` | `0x800FFC50` fills a 48-byte-element vector from `ResAnmChr::GetAnmResult` and `ChrAnmResult::GetMtx` for each node whose name starts with the given prefix, then swaps it in; destructor `0x800FF020` |
| `0xAC` | `mPPD` | `shared_ptr<CPPD>` | Set by `ScriptBinding_NewPPD` through `0x80100650` |
| `0xB4` | `mCachedPPDValue` | `int` | `0x80100F70` compares the popped value with it |
| `0xB8` | `mCachedPPD` | `weak_ptr<CPPD>` | `0x80100F70` stores the resolved PPD |
| `0xC0` | vptr | | Constructor stores `0x803A9730` |

The vtable at `0x803A9730` is CodeWarrior's split layout (F-40): the class header (RTTI, offset 0), the `ICalc` subobject table (RTTI, offset -0x64, the thunks `0x801035C0` and `0x801035B0`, then `ICalc::CalcMtx` `0x80063DC0`), and then the class's own slots, `~CPpuEngine` `0x800FF0A0` (vptr offset 0x1C) and `Calc` `0x80100C90` (0x20). `0x80102220` destroys through offset 0x1C and `Load`/`Execute` call `Calc` through 0x20, so the class declares exactly these two virtual functions. The data members are declared first, which places the vptr at 0xC0 (F-281 trap).

Statics: `sDefaultEnv` (`weak_ptr<NPpu::CEnvironment>`, `0x804B9048`, set by `0x800FFA80` from `0x800A7C9C` and `0x800A9900` and locked by `0x80100920` when no environment is passed), `sDefaultStageDescTarget` (`StageID`, `0x804B9050`, set by `0x800FFAD0`) and `sScriptRoot` (`shared_ptr<std::string>`, `0x804B9058`; `0x801006E0` returns its characters or `"5:"`). The byte `0x804B9060` guards a one-time `CVM::SetNextID(1000000000)` that `0x80100BA0` and `0x80102220` both inline.

Supporting evidence recorded for other headers:
- **`ICalc::CalcMtx` has an empty body.** The `ICalc` vtable `0x80399E38` holds 0 for `Calc` (pure) and `0x80063DC0` (`blr`) for `CalcMtx`, and about 40 vtables, including `CPpuEngine`'s, share that word. `include/game/ICalc.h` still declares `CalcMtx` pure, which makes the compiled `CPpuEngine` vtable hold 0 there.
- **The MSL tree keeps its comparator after the root.** The two trees of this class place a non-empty `map::value_compare` between the root and the begin pointer, while sets keep the 0xC-byte layout. This matches a `compressed_pair<root, value_compare>`. The shared `include/MSL_CPP/detail/tree.h` has no comparator member, so `include/map` uses its own tree with that layout until the shared tree is changed.

## F-195-3: the functions of #195

objdiff with `src/game/CPpuEngine.cpp`:

| Address | Function | Match | Notes |
| --- | --- | --- | --- |
| `0x800FE5F0` | `CPpuEngine::CPpuEngine()` | 75.1% | The original calls the out-of-line `mOwner` constructor `0x801119B0`; the repository `shared_ptr` constructor is inline |
| `0x800FE6A0` | `enable_shared_from_this<CPpuEngine>::~enable_shared_from_this()` | 100% | Exception-action destructor |
| `0x800FE710` | `map<int, list<int> >::~map()` | 97.0% | The original checks `this` once more (the MSL tree has one more layer) |
| `0x800FEAF0` | `set<shared_ptr<CFileData> >::~set()` | 100% | |
| `0x800FF020` | `vector<MTX34>::~vector()` | 94.7% | POD vector layering (F-60-3) |
| `0x800FF0A0` | `CPpuEngine::~CPpuEngine()` | 97.1% | Frame size, the vector and map layers above, out-of-line `mOwner` destructor |
| `0x800FFA20` | `GetWorld() const` | Not reconstructed | `mOwnerObj->mWorld`, else `IGameMap` slot 7 of the map getter `0x800AA2B0`; needs an `IGameMap` header |
| `0x800FFA70` | `GetEnv() const` | 100% | Returns `mEnv` (0x8); callers `AddPPDToPlayerPiiBox`, `AddPPDToTeamPiiBox` read a member at 0x14, past the 0x14-byte `CEnvironment`, so the environment is probably a `CPpuEnv` (medium) |
| `0x800FFA80` | `static SetDefaultEnvironment(const shared_ptr<NPpu::CEnvironment>&)` | 100% | |
| `0x800FFAC0` | `SetStageDescTarget(const StageID&)` | 100% | |
| `0x800FFAD0` | `static SetDefaultStageDescTarget(const StageID&)` | 100% | |
| `0x800FFAE0` | `LoadFile(const char*)` | Not reconstructed | Needs the loader `0x80066930` and a `CFileData` header (ready test: byte 0x28 clear and buffer 0x1C set) |
| `0x800FFC50` | `LoadPlacement(...)` | Not reconstructed | Called by `ScriptBinding_LoadEventPlacementData`; needs `CFileData` and the vector growth path |
| `0x80100650` | `SetPPD(shared_ptr<CPPD>)` | 100% | By value: `NewPPD` copies and releases the argument |
| `0x801006D0` | `LeaveInitPart()` | 100% | |
| `0x801006E0` | `static GetScriptRoot()` | Not reconstructed | Needs MSL `basic_string` (#327) |
| `0x80100720` | `LoadScriptFile(const char*, u32)` | Not reconstructed | Builds `"script.arc/" + name`; needs `basic_string` |
| `0x80100910` | Loader wrapper (flags `| 0x2000`, tail call `0x80128920`) | Not reconstructed | Weak header function; owner and name unknown |
| `0x80100920` | `Load(const char*, int, const shared_ptr<NPpu::CEnvironment>&)` | Not reconstructed | Locks `sDefaultEnv` when the environment is null, loads the code with `0x80103ED0` (next unit), copies the default stage target and calls `CVM::Init(code, env, 64, 32, 8)` |
| `0x80100AF0` | `Load(BWorldObj*, const char*, int, const shared_ptr<NPpu::CEnvironment>&)` | 100% | |
| `0x80100BA0` | `Execute(const char*, int, const shared_ptr<NPpu::CEnvironment>&)` | 100% | Mode 1 yields the fiber while the script waits |
| `0x80100C90` | `Calc()` | 100% | |
| `0x80100DB0` | `PopWorldObj()` | Not reconstructed | Pops an id; negative ids give null, `mOwnerObj` when the id matches, else `BWorld` lookup `0x8005F750`; needs `GetWorld` |

The source also produces #196's thunks `0x801035B0` and `0x801035C0` (100%) and the weak `set` and `pair` destroy helpers `0x801025E0` (99.9%, the remaining word is a relocation) and `0x80103360` (97.8%). The unit stays `NonMatching`.

Names: `CPpuEngine`, `NPpu::CEngine`, `ICalc` and `enable_shared_from_this<CPpuEngine>` are RTTI names. `LeaveInitPart`, the stage target and PPD names follow the script bindings that call them. All other method, member and static names are descriptive. `StageID` follows `ScriptBinding_MakeStageID`; `CPPD` is a descriptive class name for the object the PPD bindings use (low confidence). The `NPpu::CVM` members `Run` (`0x80170C70`), `SetThis` (`0x80170940`), `SetID` (`0x801738B0`) and `SetNextID` (`0x801738C0`) are descriptive names from their stores to `mState`, `mThis`, `mID` and `sNextID`; `STATE_NONE` and `STATE_WAIT` are low confidence.

## Method

GNU objdump 2.42 on `build/wpse-v1/main.elf`, Python scans of `extabindex`, `.ctors`, absolute and small-data references and the script binding table, and `tools/re/unitdiff.py`. No decompiler output was used.

## Traps

- **A by-value `shared_ptr` parameter is invisible in the callee.** `SetPPD` matched only with a by-value parameter: CodeWarrior's callers copy and destroy the argument, and the callee's load scheduling differs from the `const&` version.
- **`switch` on an enum compiles to signed range tests.** `Execute` needed `GetState()` to return the `CVM::State` enum: on `int` the 0–1 case became one `cmplwi`, while the original uses `cmpwi`/`blt`/`ble`. `Calc` switches on the `int` from `Run` and keeps `cmplwi`.
- **Field order inside a value build can be reversed.** `Load(BWorldObj*, ...)` stores the payload of the `CValue` before its type; assigning `mInt` before `mType` reproduces it.
- **The shared tree header cannot express a map.** `map` puts a non-empty comparator between the root and the begin pointer; see F-195-2.
