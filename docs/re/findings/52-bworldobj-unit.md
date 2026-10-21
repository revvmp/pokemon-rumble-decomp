# F-52-1: BWorldObj.cpp and the following battle-royal unit

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. Issue #52 covers `0x8006A100`–`0x8006B464`. That range holds two source units. The first, `game/BWorldObj.cpp`, is now split and registered `NonMatching`. The second uses `CBattleRoyalBox` and `CBattleRoyalData`, but its file name is not yet evidenced. The full DOL still verifies.

## BWorldObj.cpp

| Section | Range |
| --- | --- |
| `extab` | `0x8000A6DC`–`0x8000A7F4` |
| `extabindex` | `0x80044D00`–`0x80044D48` |
| `.text` | `0x8006A100`–`0x8006A544` |
| `.data` | `0x8039A2B8`–`0x8039A300` |
| `.sdata2` | `0x804B9D28`–`0x804B9D30` |

- **Start.** `0x8006A100` is the `BWorldObj` constructor. It stores the `BWorldObj` vtable (`.data` `0x8039A2B8`, 0x48 bytes, RTTI `.sdata` `0x804B5A88`) and the two constants `0x804B9D28` and `0x804B9D2C`. The `BModelObj`, `BWorldCamera` and `BWorldLightSet` constructors call it, among others (`0x800528E0`, `0x80066460`, `0x80068610`, `0x800D4B50`, `0x801202D0`). The preceding `BWorldLightSet.cpp` ends at `0x8006A100` ([F-51-1](51-bworldlightset-unit.md)).
- **Contents.** The constructor; the deleting destructors at `0x8006A190` and `0x8006A200` (vtable slot 0, releasing the weak reference of `enable_shared_from_this<BWorldObj>` at offset 4 and the weak pointer at 0x1C); the weak-pointer assignment at `0x8006A290`; the shared-from-weak pair at `0x8006A2D0` and `0x8006A370`; three forwarders at `0x8006A480`, `0x8006A490` and `0x8006A4A0` to the member object at 0x24; and two flag operations at `0x8006A4B0` and `0x8006A4F0`, called from `BWorld.cpp`.
- **End.** The unit's only `.data` object is its vtable, ending at `0x8039A300`. Its only `.sdata2` constants are `0x804B9D28` and `0x804B9D2C`. The unit owns no `.sdata`: `BWorldObj`'s type info is the earlier copy at `0x804B5A88`.

## Following unit, name unresolved

| Section | Range |
| --- | --- |
| `extab` | `0x8000A7F4`–`0x8000AAA0` |
| `extabindex` | `0x80044D48`–`0x80044E08` |
| `.text` | `0x8006A550`–`0x8006B464` |
| `.data` | `0x8039A300`–`0x8039A4B8` |
| `.sdata` | `0x804B5BB0`–`0x804B5BD0` |
| `.sdata2` | `0x804B9D30`–`0x804B9D38` |

- Its data starts with `vector length error` and the exception specification list, used by the vector template at `0x8006A860`. Then come the `shared_ptr_deleter<CBattleRoyalBox, ...>` vtable (`0x8039A34C`, used by `0x8006AF10`), the `shared_ptr_deleter<CBattleRoyalData, ...>` vtable (`0x8039A3D0`, used by `0x8006A590` and `0x8006A640`), and the two `default_delete` type names, which end at `0x8039A4B6`. The next object, `battleroyal` at `0x8039A4B8`, is used by `0x8006B470` in #53's `CBattleRoyalDisplay.cpp`.
- `CBattleRoyalData` is a four-byte object holding one float. `0x8006A590` allocates one from a float and wraps it in a `shared_ptr`. `0x8006A550` takes a float and stores it when the stored value is below the constant at `0x804B9D34` or the new value is smaller than the stored value. It returns whether it stored, so it keeps a running minimum. It is called from `0x800D1CA0`.
- Its `.sdata2` constants `0x804B9D30` and `0x804B9D34` are used only by `0x8006A640` and `0x8006A550`. That is why the boundary falls between `0x8006A544` and `0x8006A550`, not at the `BWorldObj` data end alone.
- File-name order puts this unit after `BWorldObj.cpp` and before `CBattleRoyalDisplay.cpp`. No string or RTTI names the file, and `CBattleRoyal.cpp`, `CBattleRoyalBox.cpp` and `CBattleRoyalData.cpp` all fit that order. It is not split here, so no file name is invented.

## Blocker

Both units need the MSL C++ `tr1::shared_ptr`, `weak_ptr` and `enable_shared_from_this` templates (#53, #327). The second also needs `vector`. `BWorldObj.cpp` also needs the `ICalc` declaration (PR #906) and the class of its member at 0x24, whose constructor is `0x801119B0` and destructor `0x801119C0`. Source is deferred until those land.

## Method

Python reading the linked `main.elf` (byte-identical to the original DOL) for vtables, RTTI, `extabindex`, callers, and `lis`/`addi`, `r13` and `r2` references. No decompiler output was used.

## Traps

- **An issue range can hold an unnamed unit.** The block's data ownership shows a second unit with no class vtable and no file-name string. Do not stretch `BWorldObj.cpp` over it just because the issue range continues.


# F-52-2: Complete BWorldObj source and corrected register ownership

The first unit is complete source selected with Wii/1.1 and unchanged `cflags_game`. All eleven retained functions match their full 1,040 instruction bytes and all 24 resolved instruction targets. The six complete exception records match 280 bytes, including cleanup targets; the 72-byte vtable matches every slot, and both float constants match eight bytes. `unitdiff.py game/BWorldObj` reports 11/11 at 100%. Full `ninja` and frozen-manifest verification both exit 0. The linked DOL is also raw byte-identical to the immutable baseline at `92230e3414fa2e8433609353b4fc6e3ef14e1833`.

The complete BWorldObj remains 0x48 bytes. Its weak parent owner is at 0x1C/0x20 and is a weak pointer to BWorldObj, not the previous inferred BWorld. The by-value shared parent setter at 0x8006A290 is 60 bytes; the nonconst raw parent getter at 0x8006A2D0 is 160 bytes. A const getter changes nine native prologue words. The getter locks the weak owner through the retained 260-byte free Lock function and returns its raw pointer.

The member at 0x24/0x28 is the existing complete eight-byte CPpuRegister. Its constructor at 0x801119B0 and destructor at 0x801119C0 are external retained functions. The control vtable at 0x803ADE0C names `shared_ptr_deleter<vector<NPpu::Slice,allocator<NPpu::Slice>>,default_delete<vector<NPpu::Slice>>>` through RTTI at 0x804B7370. This supersedes the placeholder BWorldObjOwner storage. Canonical GetValue(unsigned int) const at 0x80111D20 returns two words, Set at 0x80111D80 receives a const value reference, and Swap at 0x80112590 mutates a nonnull CPpuRegister pointer by exchanging ownership through a local. The BWorldObj forwarders are eight bytes each.

The canonical NPpu::CValue retains its complete eight-byte tag/union representation. Native ScriptBinding_method_getRegister at 0x8014F4E0 stores Get's returned r3/r4 into an eight-byte local and passes that local directly to CEngine::Push at 0x801319F0. GetNewObjRegister at 0x8014F5D0 performs the same pairing. An independently compiled CValue return reproduces that ABI. Slice is the original embedded spelling, whereas CValue is inferred; this change introduces no alias, wrapper, storage change, or broad rename.

CPpuEngine has the same register at 0x88. Its typed getter still returns this+0x88 in eight bytes. The two ScriptCamera calls now name SwapRegister/GetRegister instead of the incorrect shared-owner interpretation, preserving their instruction bytes and actual targets. CPpuEngine's existing NonMatching constructor changes from 168 to the native 176 bytes; its existing NonMatching destructor changes from 2,436 to 2,412 bytes, closer to the native 2,424. Their register construction/destruction now call the native out-of-line functions. This is a lifetime correction, not a claim that the entire CPpuEngine destructor matches. Its other 43 retained source instruction bodies remain identical.

The BWorldObj vtable also establishes that 0x80069E60 is the common BWorldObj position setter. The preceding Matching BWorldLightSet source had named this identical 28-byte body as a derived override. Its declaration/body class is corrected to the base method, preserving all 27 retained functions / 6,740 instruction bytes, the vtable targets, and the frozen DOL. No preceding split or function extent changes.

## Consumer controls

An immutable full current baseline contains all 740 declared actual source objects at 92230e34, after explicit all_source, default link and verification. Against that baseline, 719 old objects are raw-identical. Twenty more preserve allocated section bytes, attributes, symbol identities and semantic relocations under the three explicitly documented API spelling corrections. BWorld's two byte-identical 96-byte weak pointer destructors exchange their symbol offsets and exception-index ordering; both bodies and their exception bytes remain identical. CPpuEngine is the single explicit native lifetime correction described above. No previously exact native consumer body regresses. The new BWorldObj object is the 741st actual source object; its selected input is the compiled source object, with no fallback object for this unit.

Independent #218 producer reconstruction also confirms the same 0x48 base: CWaza Setup with a by-value shared parent matches 236 native instruction bytes, its nonconst parent-consuming GetAttachmentPosition matches 464, and its constructor/Calc/reset/get-engine match 112/116/28/8 bytes. These are supplementary consumer evidence; #218 retains ownership of its source.

Names SetParent, GetParent, GetRegister, SetRegister, SwapRegister and CPpuRegister remain descriptive inferences from behavior and existing canonical declarations. No native mangled spelling for those game methods is asserted. The constructor's initial state is 4, not 1.

## Remaining issue work

The independent battle-royal unit after 0x8006A550 remains unaccepted. A typed outside producer has complete CBattleRoyalData size four and CBattleRoyalBox size 120, containing a 3-by-5 array of shared owners. Its first bounded probe still differs in the data factory, constructor and vector export context; no partial battle-royal source is added and #52 remains open.

## Traps

- Adding a vector definition transitively through CPpuRegister reordered unrelated weak helpers. A forward declaration with the complete allocator template argument and including CPpuRegister after BWorldObj's existing dependencies preserves old instruction bytes. Audit actual objects and relocations rather than inferring preservation from equal object sizes or objdiff scores.
- Native vtable targets can reveal a prior common base method named as a derived override. Correct both the selected source definition and its declaration; renaming only the fallback symbol leaves an undefined source-selected method.


The latest full control was refreshed at `4d997fb2` (full head recorded in the immutable control manifest): all 744 declared existing source objects were rebuilt, linked and frozen-verified. The candidate has 745 objects including BWorldObj. Of the existing objects, 717 are raw-identical and 26 preserve every allocated section byte, attributes and semantic targets with the explicit aliases above. New CGameMap's two weak and four shared destructors reorder symbol offsets and exception-index entries; every member of each group is independently asserted to have identical full instruction bytes and relative semantic targets. These are the same 96-byte weak or 132-byte shared destruction operations, not changed bodies. CPpuEngine remains the one explicit lifetime correction. Its constructor now independently matches all 176 native instruction bytes and all five targets. The just-landed canonical GetValue declaration is preserved exactly; the BWorldObj forwarder calls it without adding a competing getter API.
