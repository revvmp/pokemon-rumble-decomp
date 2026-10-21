# F-115-1: Original scope and enemy unit boundaries

Target `wpse-v1`, immutable original executable and frozen acceptance manifest. The original issue owns 26 functions and 4,932 instruction bytes from `0x800A3B10` through `0x800A4EE8`. Its three evidenced classes are `CEnemyInfo`, `CEnemyPii` and `CFacility`; the following issue begins with the distinct `CGameEnding` unit at `0x800A4EF0`. The original inventory remains the completion denominator.

The first bounded unit is `CEnemyPii`, whose complete class contract and the required HP/marker/layout interfaces were established by F-54-3 through F-54-5. The enemy adds no instance fields to the `0x14C`-byte `CPii`. Its retained functions are constructor `0x800A47E0` (168 bytes), destructor `0x800A4890` (88), initialization `0x800A48F0` (748), script-loader wrapper `0x800A4BE0` (24), and the secondary-base destructor thunk `0x800A4C00` (8): five functions and 1,036 instruction bytes.

| Section | Complete enemy extent |
| --- | --- |
| Text | `0x800A47E0`–`0x800A4C08` |
| Exception tables | `0x800159FC`–`0x80015BDC` |
| Exception index | `0x80047BA4`–`0x80047BC8` |
| Data | `0x803A16B0`–`0x803A1760` |

The exception index has exactly the constructor, destructor and initializer entries. The data comprises `minimap_enemy`, `CEnemyPii.pkc`, and the native 144-byte vtable. The class RTTI, shared-pointer helpers and base metadata already have earlier definitions in the boss unit; no independent enemy small-data, constructor-table or BSS extent is inferred.

The original-only split checkpoint completes `configure.py --map`, default Ninja after deleting the prior DOL, and frozen-manifest verification with exit 0. The link still selects the original enemy object. This establishes the split without adding source-completion credit.

The 24-byte wrapper is nonvirtual. Its sole caller is the script helper at `0x80153680`, which supplies mode zero and a `shared_ptr<NPpu::CEnvironment>` created by the existing canonical constructor at `0x80131D80`. The wrapper inserts `CEnemyPii.pkc` and tail-calls `0x800DCDF0`. That generic `CPii` routine constructs the canonical `CPpuEngine` and forwards owner, filename, integer mode and environment reference to the existing `CPpuEngine::Load` interface at `0x80100AF0`. This supplies a concrete typed script-loader contract; descriptive method spelling remains inferred.

## F-115-2: Typed script loading prerequisite

The additive declarations name the generic loader `CPii::LoadScript(const char*, int, const std::tr1::shared_ptr<NPpu::CEnvironment>&)` and its filename-supplying enemy overload. Both return `bool`, following the canonical engine loader. The method spelling is descriptive and inferred; the argument order, environment type, nonvirtual dispatch and return forwarding are established by the native caller chain. Neither declaration adds fields or changes a virtual slot.

An outside ordinary C++ enemy implementation compiled with the unchanged canonical game flags resolves all five retained instruction streams (168, 88, 748, 24 and 8 bytes) and the complete 144-byte vtable byte-for-byte against the frozen-verified executable. The proof is retained in `C:/MassStorage/GitHub/115-reference/115-enemy-contract-proof.json`. All 18 currently source-selected game consumers have identical allocated sections and normalized relocation contracts when compiled with the previous declarations versus the additive headers. The pinned comparison base is `c7c1d0daf7f9825bd98f00bd9f6c1c83a39aa026`; the receipt is `115-prereq-current-controls/proof.json`. This prerequisite retains the original-only split and claims no source completion.

## F-115-3: Complete enemy source acceptance

The canonical source reconstructs the constructor, destructor, HP-bar and minimap initialization, and typed script-loader wrapper. The compiler emits the secondary-base destructor thunk. All five retained functions are 100% in objdiff: 1,036 of 1,036 instruction bytes. The earlier canonical boss unit supplies shared inline/deleter definitions, so raw source-object metadata includes legitimate duplicate definitions discarded at link. No duplicated helper or metadata is counted as new source coverage.

With the enemy selected as `Matching`, Ninja exits 0 and the full frozen DOL verifies. The final ELF preserves each original function address, size and exact native bytes, and the map assigns all five to `CEnemyPii.o`. The complete retained exception tables (480 bytes), exception index (36), data (176), and 1,064-byte allocated text extent match the frozen original. The link graph contains the source object and excludes the original enemy fallback. The independent receipt is `C:/MassStorage/GitHub/115-reference/115-enemy-source-audit.json`. This completes five of the issue's original 26 functions; CEnemyInfo and CFacility remain.

## Traps

The 24-byte filename wrapper is not another virtual slot: the independently proved boss vtable already inherits the complete enemy virtual contract without that entry. Use its actual callers and generic loader target to establish the nonvirtual member interface.
