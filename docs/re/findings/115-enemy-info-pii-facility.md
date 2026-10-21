# F-115-1: Original scope and enemy unit boundaries

Target `wpse-v1`, immutable original executable and frozen acceptance manifest. The original issue owns 26 functions and 4,932 instruction bytes from `0x800A3B10` through `0x800A4EE8`. Its three evidenced classes are `CEnemyInfo`, `CEnemyPii` and `CFacility`; the following issue begins with the distinct `CGameEnding` unit at `0x800A4EF0`. The original inventory remains the completion denominator.

The first bounded unit is `CEnemyPii`, whose complete class contract and the required HP/marker/layout interfaces were established by F-54-3 through F-54-5. The enemy adds no instance fields to the `0x14C`-byte `CPii`. Its retained functions are constructor `0x800A47E0` (168 bytes), destructor `0x800A4890` (88), initialization `0x800A48F0` (748), script-loader wrapper `0x800A4BE0` (24), and the secondary-base destructor thunk `0x800A4C00` (8): five functions and 1,036 instruction bytes.

| Section | Complete enemy extent |
| --- | --- |
| Text | `0x800A47E0`â€“`0x800A4C08` |
| Exception tables | `0x800159FC`â€“`0x80015BDC` |
| Exception index | `0x80047BA4`â€“`0x80047BC8` |
| Data | `0x803A16B0`â€“`0x803A1760` |

The exception index has exactly the constructor, destructor and initializer entries. The data comprises `minimap_enemy`, `CEnemyPii.pkc`, and the native 144-byte vtable. The class RTTI, shared-pointer helpers and base metadata already have earlier definitions in the boss unit; no independent enemy small-data, constructor-table or BSS extent is inferred.

The original-only split checkpoint completes `configure.py --map`, default Ninja after deleting the prior DOL, and frozen-manifest verification with exit 0. The link still selects the original enemy object. This establishes the split without adding source-completion credit.

The 24-byte wrapper is nonvirtual. Its sole caller is the script helper at `0x80153680`, which supplies mode zero and a `shared_ptr<NPpu::CEnvironment>` created by the existing canonical constructor at `0x80131D80`. The wrapper inserts `CEnemyPii.pkc` and tail-calls `0x800DCDF0`. That generic `CPii` routine constructs the canonical `CPpuEngine` and forwards owner, filename, integer mode and environment reference to the existing `CPpuEngine::Load` interface at `0x80100AF0`. This supplies a concrete typed script-loader contract; descriptive method spelling remains inferred.

## F-115-2: Typed script loading prerequisite

The additive declarations name the generic loader `CPii::LoadScript(const char*, int, const std::tr1::shared_ptr<NPpu::CEnvironment>&)` and its filename-supplying enemy overload. Both return `bool`, following the canonical engine loader. The method spelling is descriptive and inferred; the argument order, environment type, nonvirtual dispatch and return forwarding are established by the native caller chain. Neither declaration adds fields or changes a virtual slot.

An outside ordinary C++ enemy implementation compiled with the unchanged canonical game flags resolves all five retained instruction streams (168, 88, 748, 24 and 8 bytes) and the complete 144-byte vtable byte-for-byte against the frozen-verified executable. The proof is retained in `C:/MassStorage/GitHub/115-reference/115-enemy-contract-proof.json`. All 18 currently source-selected game consumers have identical allocated sections and normalized relocation contracts when compiled with the previous declarations versus the additive headers. The pinned comparison base is `c7c1d0daf7f9825bd98f00bd9f6c1c83a39aa026`; the receipt is `115-prereq-current-controls/proof.json`. This prerequisite retains the original-only split and claims no source completion.

## F-115-3: Complete enemy source acceptance

The canonical source reconstructs the constructor, destructor, HP-bar and minimap initialization, and typed script-loader wrapper. The compiler emits the secondary-base destructor thunk. All five retained functions are 100% in objdiff: 1,036 of 1,036 instruction bytes. The earlier canonical boss unit supplies shared inline/deleter definitions, so raw source-object metadata includes legitimate duplicate definitions discarded at link. No duplicated helper or metadata is counted as new source coverage.

With the enemy selected as `Matching`, Ninja exits 0 and the full frozen DOL verifies. The final ELF preserves each original function address, size and exact native bytes, and the map assigns all five to `CEnemyPii.o`. The complete retained exception tables (480 bytes), exception index (36), data (176), and 1,064-byte allocated text extent match the frozen original. The link graph contains the source object and excludes the original enemy fallback. The independent receipt is `C:/MassStorage/GitHub/115-reference/115-enemy-source-audit.json`. This completes five of the issue's original 26 functions; CEnemyInfo and CFacility remain.

## F-115-4: Complete CItem base contract and Facility consumer

Facility RTTI names its immediate base `CItem`, whose first retained function is `0x800C0C60`, owned by unassigned issue #139. This is a bounded header prerequisite; the outside source and split remain with #139. Its constructor calls `BModelObj` with type bit 11, then clears a word at `0xF8` and the shared engine pair at `0xFC`/`0x100`, establishing complete size `0x104`. The word is named `mFlags` descriptively; no interpreted flag bits are asserted. The engine is the same canonical `CPpuEngine` used by Pii. Its destructor releases the engine then destroys `BModelObj`.

The complete native virtual table identifies overridden destructor, `Calc`, `GetPpuEngine() const`, `OnLeaveWorld`, and the appended `Initialize(bool)` slot. Native nonvirtual routines load a script through the canonical PPU engine interface and a model from `item.arc/`; the descriptive names are `LoadScript` and `LoadModel`, with parameters established by their callers. RTTI class spelling is original; method and member names not already inherited remain inferred. No existing virtual slot or base declaration changes.

An ordinary outside Facility consumer derives a `0x110`-byte class with its own word at `0x104` and shared minimap marker at `0x108`/`0x10C`. Native type bit 13, marker kind 4 and `minimap_facility` reproduce all seven retained functions (696 bytes) and the complete 132-byte vtable byte-for-byte. These include the inline CItem engine accessor and compiler secondary-base destructor thunk. The proof is `C:/MassStorage/GitHub/115-reference/115-facility-contract-proof.json`; no source-completion credit is claimed by the header prerequisite.

## F-115-5: Complete Facility source acceptance

The canonical Facility source and full class declaration reproduce all seven retained functions, 696 of 696 instruction bytes at 100% in objdiff. A fresh Matching link and frozen verification pass. The final ELF retains the exact original addresses, sizes and native instruction bytes; exact map columns select `CFacility.o`, and the source graph excludes its original fallback. All retained exception tables (196 bytes), exception index (36), data (280), small data (24) and allocated text (728) match the original. The native receipt is `C:/MassStorage/GitHub/115-reference/115-facility-source-audit.json`.

The complete data boundary is `0x803A1878`, including the final terminator of the CItem RTTI base list. Ending at the earlier survey estimate `0x803A1874` bisects the original symbol. This complete native source link proves the actual extent without adding padding bytes. Together with CEnemyPii this reconstructs 12 of the original 26 functions and 1,732 of 4,932 instruction bytes. The 14-function CEnemyInfo unit remains.

## Traps

The 24-byte filename wrapper is not another virtual slot: the independently proved boss vtable already inherits the complete enemy virtual contract without that entry. Use its actual callers and generic loader target to establish the nonvirtual member interface.

A prior neighboring finding described the Facility base-list end as `0x803A1874`; the actual complete symbol ends at `0x803A1878`. Confirm the complete RTTI terminator and source-selected native data extent before defining the split.
