# F-60-1: CDividedGround.cpp unit boundaries

Target: `wpse-v1`. Issues #60 to #65 survey the CDividedGround block as six address ranges, `0x80071220`–`0x800776A8`. This finding establishes that the six ranges are one translation unit, `game/CDividedGround.cpp`, and gives its exact section ranges. The split is in `config/wpse-v1/splits.txt`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with it.

## Section ranges

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x8000BB5C`–`0x8000CD9C` | Lowest and highest exception records named by this unit's index entries; the next index entry (`0x800776B0`) points to `0x8000CD9C` |
| `extabindex` | `0x80045264`–`0x800455AC` | 70 entries, every one for a function in `.text` below; the neighbours index `0x80070D80` and `0x800776B0` |
| `.text` | `0x80071220`–`0x800776A8` | First function is the constructor; last is the `BSoundSource` destructor thunk at `0x800776A0` |
| `.rodata` | `0x80352AC0`–`0x80352B00` | Three tables referenced only from this unit; `0x80352B00` is first used by `0x800776B0` onward |
| `.data` | `0x8039B7B8`–`0x8039BFE8` | From the `.pkc` format strings to the end of the `exception` name; `0x8039BFE8` is the `CDlgCollection` deleter vtable |
| `.sdata` | `0x804B5E68`–`0x804B5F30` | `0x804B5F30` is used by `0x80079AF0` in the next unit |
| `.sdata2` | `0x804B9DB0`–`0x804B9DC8` | The preceding unit's last constant is `0x804B9DAC`; the next unit's first is `0x804B9DC8` |

The unit has no `.ctors` entry, `.bss` or `.sbss` of its own. The one-byte flag at `0x804B9110` that it writes is shared with many units and stays outside the split.

## Why one unit

- **Start.** The function before `0x80071220` is the third virtual of `CDisplayPhaseCtrl`, whose vtable, name and base list end at `0x8039B7B8` in `.data`. `0x80071220` stores the `CDividedGround` vtables (`0x8039BB68` and its `BSoundSource` part at `+0x48`) after calling the base constructor at `0x800528E0`, so it is the constructor and the first function of a new unit. This confirms the survey's start address.
- **One class.** `.data` holds one cluster of out-of-line literals (`.pkc` names, `stage.brres` paths, `vector length error`, `rep_EX%02d`), then the deleter vtables and type names for `shared_ptr` of `nw4r::g3d::ScnGroupEx`, `ScnMdlSimpleData`, `CBSPTree`, `CModel` and `CPpuEngine`, the `CDividedGround` vtable, RTTI name and base lists, and one cluster of template literals (`tree::insert length error`, `basic_string` errors, `default_delete` names). All deleter functions named by those vtables lie in `0x800769A0`–`0x800776A0`. No `.ctors` entry or second class name interrupts the range.
- **End.** `0x800776B0` starts `CDlgCollection`, whose deleter vtable follows at `0x8039BFE8`.

## Class layout evidence

The base lists give `CDividedGround : BModelObj`, with `BModelObj : BWorldObj` at 0, `BSoundSource` at `0x48` and `BKDTreeLink` at `0x64`; `BWorldObj : ICalc` at 0 and `std::tr1::enable_shared_from_this<BWorldObj>` at 4. The `CDividedGround` vtable has 16 primary slots and a 12-slot `BSoundSource` table. The constructor initializes members from `0xF8` to `0x16C`, including two arrays of two eight-byte objects built with `__construct_array` at `0x120` and `0x130`, and four self-linked list heads at `0x10C`, `0x14C`, `0x158` and `0x164`.

# F-60-2: Prerequisites for reconstructing the unit

The unit is 0x6488 bytes of code in 136 functions. Every function depends on declarations that do not yet exist in the repository and that belong to other issues, so no source is committed yet and the original object stays linked.

| Prerequisite | Needed by | Owner |
| --- | --- | --- |
| `BModelObj`, `BWorldObj`, `BSoundSource`, `BKDTreeLink`, `IKDTreeLinkRemoved` declarations with exact layout to `0xF8` and vtable order | Constructor (`0x800528E0` base call, flag argument ORed with `0x8000`), destructor (`0x80052F60`), the 28 vtable slots, the `BSoundSource` thunk | #39 to #52 |
| Metrowerks `std::list`, `std::vector`, `std::map` tree and `std::basic_string` storage | The four list heads, the vector at `0x114`, the list destructors at `0x80071310`–`0x80071830`, the `tree::insert` and `basic_string` error paths | MSL C++ headers, in progress under #327 (compressed pair storage, PR #919) |
| `CModel`, `CBSPTree`, `CPpuEngine`, `ScnMdlSimpleData`, `nw4r::g3d::ScnGroupEx` | The `shared_ptr` deleters at `0x800769A0`–`0x800776A0` | Owning units of those classes |

Deleter evidence, from the five `shared_ptr_deleter` vtables (slots: destructor, `get_deleter`, `dispose`):

| Type | Deleter | `dispose` |
| --- | --- | --- |
| `nw4r::g3d::ScnGroupEx` | `void (*)(nw4r::g3d::G3dObj *)` | Calls the stored function pointer (`+0x10`) on the stored object (`+0xC`); the existing `shared_ptr_deleter.h` storage only accepts class deleters, so a function-pointer deleter needs the compressed pair storage |
| `ScnMdlSimpleData` | `default_delete` | Inline destructor: a `vector` of `shared_ptr` at `+8` (pointer, size, capacity; elements destroyed from the end) and a `shared_ptr` at 0, then `operator delete` |
| `CBSPTree` | `default_delete` | Non-virtual deleting destructor `0x801E6710` |
| `CModel` | `default_delete` | Virtual destructor in slot 0 of a vtable at offset 0 |
| `CPpuEngine` | `default_delete` | Virtual destructor in the fourth slot of a vtable at offset `0xC0` |

## Traps

None found while establishing the split.

# F-60-3: CDividedGround layout and the first block's sources

Issue #60 owns the first function of `game/CDividedGround.cpp`, so it adds the object line (`NonMatching`), `include/game/CDividedGround.h`, a new MSL `include/list`, and `src/game/CDividedGround.cpp` with the constructor and destructor. The original object stays linked; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## Member layout

Offsets come from the constructor (`0x80071220`), the destructor (`0x80071830`), and the destructor's exception actions, which name the out-of-line destructor of each member type.

| Offset | Member | Type | Evidence |
| --- | --- | --- | --- |
| `0xF8` | `mPpuEngine` | `shared_ptr<CPpuEngine>` | `0x80071B80` stores a new 0xC4-byte object (`enable_shared_from_this` at 0x68) with the `CPpuEngine` deleter vtable `0x8039BAEC`; cleanup action `0x80076F80` |
| `0x100` | `mStageID` | pointer to an unrecovered record | Stored by `0x80072340` from its first argument; its word holds signed bytes that index the tables at `0x8035B9D0` and `0x8035B9F8` |
| `0x104` | `mStageNo` | `int` | Stored by `0x80072340`; formatted with `%02d` into the `.pkc` names |
| `0x108` | `mFiles` | `list<shared_ptr<CFileData>>` | `0x80074100` reads each element through the CFileData data getter `0x80072800` (F-326-5) and `ResFile::GetResAnmChr`; destructor `0x80071410` |
| `0x114` | `mFileNames` | `vector<const char*>` | `0x80072AE0` appends four-byte name pointers from null-terminated arrays, then a null; destructor `0x800714F0` |
| `0x120` | `mBSPFiles[2]` | `shared_ptr<CFileData>` | Assigned from the file loader `0x80066930`; array destructor `0x800652D0` |
| `0x130` | `mBSPTrees[2]` | `shared_ptr<CBSPTree>` | Passed to the `CBSPTree` method `0x801E6760`; array destructor `0x80076EF0` |
| `0x140` | `mScnGroup` | `shared_ptr<nw4r::g3d::ScnGroup>` | Each new `ScnGroupEx` is inserted into it; destructor `0x800656C0`, shared with `BWorld.cpp` |
| `0x148` | `mScnGroups` | `list<shared_ptr<nw4r::g3d::ScnGroupEx>>` | Elements made with the `DestroyG3dObj` deleter; destructor `0x80071590` |
| `0x154` | `mModels` | `list<shared_ptr<CModel>>` | Elements are new 0xDC-byte models used with `CModel::Play`; destructor `0x80071670` |
| `0x160` | `mSimpleModels` | `list<shared_ptr<ScnMdlSimpleData>>` | Elements are new 0x14-byte `ScnMdlSimpleData`; destructor `0x80071750` |

The class is 0x16C bytes. The constructor passes `type | 0x8000` to `BModelObj`, so `CDividedGround::TYPE` is `0x8000`. The vtable (`0x8039BB68`) overrides slots 0 (`~CDividedGround`), 1 (`Calc`), 9 (`GetOwner`), 13 (`OnEnterWorld`), 14 (`OnLeaveWorld`) and 15 (`UpdateVisible`), and adds no virtual functions. Member names, `StageID` and `BSP_COUNT` are descriptive.

## MSL list

The four list destructors and `0x80071310` show the layout of the MSL `list`: the size first, then the sentinel node with the previous pointer at 0 and the next pointer at 4; a node stores its value at 8. The inlined `list` destructor checks `this` three times, so `include/list` layers `list` over `__list_imp` over `__list_deleter`. `0x80071310` checks `this` once and is the `__list_deleter` destructor; only the exception actions of `0x80072FF0` (#61) name it. Clearing unlinks the whole range first and then destroys and frees each node.

Three source shapes are needed for the code:
- The sentinel is initialized through a pointer (`end_node()->init()`), which stores `next` and then `prev` through that register.
- The value is reached through a node accessor (`value()`), so its address is computed into `r3` before the `shared_ptr` destructor's `this` check.
- `clear()` declares the first node before the end pointer, which assigns `r29` and `r30` as in the original.

## Block status

objdiff, functions of #60:

| Address | Function | Match |
| --- | --- | --- |
| `0x80071220` | `CDividedGround::CDividedGround(u32)` | 100% |
| `0x80071310` | `__list_deleter<shared_ptr<CFileData>>::~__list_deleter()` | 100% (emitted, not referenced yet) |
| `0x80071410` | `list<shared_ptr<CFileData>>::~list()` | 100% |
| `0x80071570`, `0x80071580` | `shared_ptr<CFileData>()`, `shared_ptr<CBSPTree>()` | 100% |
| `0x80071590`, `0x80071670`, `0x80071750` | `list` destructors for `ScnGroupEx`, `CModel` and `ScnMdlSimpleData` | 100% |
| `0x800714F0` | `vector<const char*>::~vector()` | 91.6% |
| `0x80071830` | `CDividedGround::~CDividedGround()` | 97.3% |
| `0x800713F0`, `0x80071400` | `list<shared_ptr<CFileData>>::begin()`, `end()` | Not emitted; only `0x80072AE0` and `0x80074100` (#61) call them out of line |
| `0x80071B80` | Loads the `.pkc` package into `mPpuEngine` | Not reconstructed |
| `0x800721A0` | `printf`-style formatter returning a `basic_string` | Not reconstructed |
| `0x80072260` | Twelve-byte zeroing constructor, called from `0x80073660` | Not reconstructed |

The outside destructors named for the link (`0x800652D0`, `0x800656C0`, `0x80076EF0`, `0x80076F80`) and the `BSoundSource` thunk `0x800776A0` also compare at 100%.

Remaining differences:
- **POD vector destructor.** The original checks `this` four times and computes `size -= size` with a load and `subf`. The shared `include/vector` checks three times and folds the size to zero. The `BWorld` vectors it matches all hold class types, so the POD layering is still unknown.
- **Inlined lists in the destructor.** The destructor inlines the same vector code. In the second, third and fourth inlined lists, the original swaps the registers for the first node and the end pointer. The first inlined list matches.
- **Strings.** `0x80071B80`, `0x800721A0` and `0x80072260` need a matching MSL `basic_string`, which the repository does not have yet (#327).

## Traps

- **An out-of-line weak function can depend on a later block.** `0x800713F0`, `0x80071400` and `0x80071310` are emitted only because functions in #61 call them or name them in exception actions. With only the first block's source they compile inline or are dead-stripped, so they cannot be checked until those callers exist.
- **Copies of one inline function can get different registers.** The four inlined list destructors in `~CDividedGround` come from the same template, but the original gives the first copy different registers from the other three. Matching the out-of-line destructor does not mean every inlined copy matches.

# F-60-4: POD vector destructor, FormatString and the .pkc loader

Target: `wpse-v1`. `~CDividedGround` and `vector<const char*>::~vector` now match 100%, `FormatString(const char*, ...)` `0x800721A0` matches 100%, and the `.pkc` loader `0x80071B80` matches 99.97%. The object stays `NonMatching` because the rest of the unit (#61 to #65) is unreconstructed. `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

| Address | Function | Match |
| --- | --- | --- |
| `0x800714F0` | `vector<const char*>::~vector()` | 100% |
| `0x80071830` | `CDividedGround::~CDividedGround()` | 100% |
| `0x80071B80` | `CDividedGround::LoadPkc(int, const shared_ptr<NPpu::CEnvironment>&, const char*)` | 99.97% |
| `0x800721A0` | `FormatString(const char*, ...)` | 100% |

## POD vector clearing

The POD `erase_at_end` takes its count by reference (`size_type&`), and `clear()` passes a local copy of the size. The original then loads the size once and computes `size -= size` as `subf r0, r5, r5` (standalone) or with the load placed before the exception-state store (inlined in `~CDividedGround`). Passing `size_` directly folds the subtraction to zero, and a `const size_type&` loads twice.

## FormatString

`0x800721A0` zeroes the 12-byte return string, builds a `va_list` and calls `0x801F5A50` with the string, the format and the `va_list`, so that function is `VFormatString(std::string&, const char*, va_list)` (descriptive, medium; F-347-1 identifies it as `vsnprintf` into a `basic_string`). `FormatString` is defined in `CDividedGround.cpp` and declared in `include/game/StringFormat.h`.

## The .pkc loader

`0x80071B80` is called only from `CGameMap` `RunAction` (`0x800A98E0`) with mode 1, an environment pointer and no prefix, so it is public. It resets `mPpuEngine` to a new `CPpuEngine`, replaces a null prefix with the empty string, and tries three names in order, each loaded with `CPpuEngine::LoadScriptFile(const char*, u32)` (`0x80100720`, which has no `this` and is therefore static): `%s%s_%s%02d.pkc`, `%s%s.pkc` and `%s%s_%02d.pkc`, built from the prefix, a world name, a rank letter and the stage number. The first two are tried through `CPpuEngine::Load(BWorldObj*, const char*, int, const shared_ptr<CEnvironment>&)`; the third is loaded without a check. `LoadPkc` is a descriptive name.

The world and rank names come from two `.rodata` tables at `0x8035B9F8` (`gStageWorlds`, eight 12-byte entries: index, name such as `terminal` or `battle`, message ID) and `0x8035B9D0` (`gStageRanks`, five 8-byte entries: `D`, `C`, `B`, `A`, `S` and a message ID). These are in another unit; the names and the `StageRankInfo` and `StageWorldInfo` records in `StageID.h` are descriptive.

`StageID` is four signed 8-bit bitfields, not a `s8[4]`: `ScriptBinding_MakeStageID` builds it with `rlwimi` into one word and `LoadPkc` reads it with `lwz`, `rlwinm ..., 0, 8` and `srawi 24`, which a `s8` array does not produce (verified with a scratch compile). Byte 1 indexes the ranks and byte 2 the worlds; the members are named `mKind`, `mRank`, `mWorld` and `mReserved` (descriptive; `mKind` and `mReserved` are unproven). Every `Matching` unit that uses `StageID` and `vector` still verifies.

Source shapes the match needs:
- The stage number is read once into a local and reused by the first and third format.
- Each name is assigned with `name.assign(FormatString(...))`, which inlines the `basic_string::assign(const basic_string&)` already in `include/string`; `operator=` does not match.
- The result of `LoadScriptFile` is kept in a `shared_ptr` that is assigned for the second attempt, which produces the copy and release in the original.
- The null prefix is replaced with `pPrefix ? pPrefix : ""`; an `if` produces the wrong branch layout.

The two `.data` addends that differ (`0x02F0`, `0x04D4`) are relocations to the format strings inside this unit's `.data`, which does not match until the whole unit is reconstructed.

## Traps

- **Signed-byte struct fields read as a word are bitfields.** Table lookups through `lwz`, `rlwinm 0, 8` and `srawi 24` mean `signed x : 8` fields; `s8` members give `lbz` and `extsb`.
- **A reference parameter can reproduce `x -= x`.** `size_ -= n` with `n` a reference to a local copy of `size_` gives the original load-once `subf`; a by-value or `const` reference parameter does not.

# F-60-5: Leftovers closed by the second block

Target: `wpse-v1`. The three functions F-60-3 and F-60-4 left open are emitted once #61's `LoadFileLists` ([F-61-2](61-cdividedground-second-block.md#f-61-2-reconstruction-of-the-second-block)) exists, and all compare 100%: `list<shared_ptr<CFileData>>::begin` `0x800713F0` and `end` `0x80071400` (called out of line by `0x80072AE0`), and the twelve-byte `basic_string::basic_string()` `0x80072260` (`init()`, emitted out of line only for #62's `0x80074100`; checked with a scratch caller under `#pragma dont_inline on`). With them every function of `0x80071220`–`0x80072280` compares 100%, except `LoadPkc` at 99.97% (two `.data` relocation addends that depend on the whole unit being reconstructed, F-60-4).

## Traps

- **An out-of-line weak function needs a caller that does not inline it.** `begin()` and `end()` stay inline in a small caller; the larger `LoadFileLists` calls them, so they appear. A scratch caller built under `#pragma dont_inline on` reproduces the effect for any helper.
