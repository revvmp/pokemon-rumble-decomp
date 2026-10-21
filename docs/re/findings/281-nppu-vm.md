# Issue 281: NPpu script VM, first block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md).

## F-281-1: The first NPpu unit ends at 0x80174D64

The NPpu region of F-0012 (`0x8016FC10`–`0x801770F4`) holds two source files. The first, registered as `game/NPpuVM.cpp` (descriptive name; the original file name is unknown), owns:

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80034D94`–`0x80034EBC` | entries of `0x8016FC30` to `0x80174C50` |
| `extabindex` | `0x8004FDC4`–`0x8004FF14` | first entry `0x8016FC30`; the next entry indexes `0x80174D70` |
| `.text` | `0x8016FC10`–`0x80174D64` | previous unit ends `0x8016FC08`; last function `0x80174C50` (0x114 bytes) |
| `.data` | `0x803B8298`–`0x803B8440` | previous unit ends with its exception type-name cluster and the `exception` string at `0x803B828C`; jump tables of `0x80172170` and `0x80172980`, then the `NPpu::CEnvironment` vtable `0x803B8418` and the `NPpu::CCode` vtable `0x803B842C` |
| `.bss` | `0x803E9910`–`0x803E9980` | two formatting buffers used only by `0x801738D0`, `0x801739E0`, `0x80173AF0`, `0x80173E70`; NCD starts `0x803E9980` |
| `.sdata` | `0x804B7B40`–`0x804B7B68` | format strings, the instance counter `0x804B7B58`, empty strings and `true`/`false` constants of this unit |
| `.sbss` | `0x804B90E0`–`0x804B90F8` | the counters `0x804B90E0`/`0x804B90E4` and the three lazily created sets; NCD starts `0x804B90F8` |
| `.sdata2` | `0x804BA2F8`–`0x804BA320` | float constants of the value conversions; RevoEX SHA-1 starts `0x804BA320` |

The second file starts with `ScriptBinding_GetThrownValue` at `0x80174D70`: its data (`.data` `0x803B8440`, the `printf` flag strings used by `0x80175090`, and `.sdata` `0x804B7B68`) follows the vtables, which CodeWarrior emits after the last function's data of a unit. Every function from `0x8016FC10` to `0x80174C50` is called only from game code or from this range, and no function after `0x80174D70` references the first unit's data. The unit therefore runs through all of #281 and #282 and the first part of #283; the split, object line and header belong to #281. Confidence is medium for the exact text end (the helpers `0x80174AD0`, `0x80174B30` and `0x80174C50` are called only by first-unit functions); it is high for the data boundaries. Splitting it changes no output bytes; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

The RTTI objects of `NPpu::CEnvironment` and `NPpu::CCode` (`0x804B72F0`, `0x804B72F8`, names at `0x803AD04C` and `0x803AD060`) are weak copies kept by the earlier `CPpuEnv`/`CPpuCode` file, whose classes derive from these bases.

The script binding table's interpreter was first named `CVM`; [F-282-1](282-nppu-engine.md#f-282-1-the-interpreter-class-is-nppucengine) identifies it as the RTTI class `NPpu::CEngine`.

## F-281-2: Classes and the first block's sources

`include/game/NPpuVM.h` declares four classes. Layouts come from the constructors, the loader and `CVM::Init`; see [symbols](../SYMBOLS.md#game-nppu-vm-281).

- `NPpu::CValue` (8 bytes): a type tag and a 32-bit payload. Types 0 (int), 1 (float), 2 and 3 (string by code index or pointer), 4 (reference to another value; increments step by 8 bytes), 5 (int-like, pushed from a 24-bit opcode operand; named `TYPE_FUNCTION`, low confidence), 6 and 7 (wide string by index or pointer).
- `NPpu::CCode` (0x24 bytes, vptr at 0x20): the loaded script image. The loader accepts magic `0x201A706B` and `0x706B1A20` and rejects their byte swaps in three separate branches. It builds tables of the wide and narrow strings that follow the code.
- `NPpu::CEnvironment` (0x14 bytes, vptr at 0x10): a global value array (0x400 entries from the caller) and the native binding table of F-29-1. `GetNative` accepts indices up to and including the count with an unsigned compare.
- `NPpu::CVM` (0x64 bytes, no vtable): ID from a counter starting at 1, code and environment references, one allocation split into handler, base, return, frame and stack arrays, and the `this`, thrown and interrupt values. All instances are registered in a global `set<CVM*>`.

Both virtual classes declare their data members before their virtual functions, which puts the vptr after the members. `AddRef` and `Release` are empty inline virtuals; CodeWarrior emits each one directly after the first function that calls it virtually (`~CVM` emits both `Release` at `0x801706A0`/`0x801706B0`, `Init` both `AddRef` at `0x80170920`/`0x80170930`). `0x80170930` is listed in #282 but is produced by this source.

The blocks at `0x801700E8` and `0x80170170` are not functions: they are the `blr` CodeWarrior leaves after a tail call, so `ParseInt` and `ParseWInt` are 0x7C and 0x84 bytes.

objdiff for the 24 functions of #281 (plus `0x80170930`):

| Address | Function | Match |
| --- | --- | --- |
| `0x8016FC10`, `0x8016FC30` | `CCode::CCode()`, `~CCode()` | 100% |
| `0x8016FCD0` | `CCode::Load` | 85.0% |
| `0x8016FEB0`, `0x8016FF10`, `0x8016FF30`, `0x8016FFF0`, `0x80170060` | `CValue::ToInt`, `IsFloat`, `IsNull`, `ToFloat`, `GetReference` | 100% |
| `0x8016FF60` | `CValue::ToBool` | 99.7% |
| `0x80170070`, `0x801700F0` | `CVM::ParseInt`, `ParseWInt` | 98.9% |
| `0x80170180`, `0x80170280` | `CValue::Increment`, `Decrement` | 96.5% |
| `0x80170270` | `CValue::SetReference` | 100% |
| `0x80170370`, `0x80170420`, `0x80170490` | `CEnvironment` constructor, destructor, `GetNative` | 100% |
| `0x801704C0` | `CVM::CVM()` | 100% |
| `0x80170510` | `CVM::~CVM()` | 86.5% |
| `0x801706C0` | `CVM::Init` | 95.3% |
| `0x801706A0`, `0x801706B0`, `0x80170920`, `0x80170930` | `Release` and `AddRef` | 100% |

Remaining differences:
- **Load.** The original loads all four header fields before storing them; the source alternates loads and stores.
- **ToBool.** `fcmpu` operand order of the float test.
- **ParseInt, ParseWInt.** Two registers swapped in the inlined string-table lookup.
- **Increment, Decrement.** The source reloads the type before the `switch`.
- **~CVM.** Register allocation of the inlined `set::find` and `erase`; the calls to the outside tree helpers (`0x8007B770`) keep their placeholder names.
- **Init.** The original stores the `pair<iterator, bool>` returned by `set::insert` to the stack, loading the bool from the `true`/`false` constants at `0x804B7B64`/`0x804B7B65`; the source drops it. The tree helpers `0x800BC3D0` and `0x800BB3C0` are shared by several files; their element type and so their mangled names are unknown, so they keep placeholder names.

The unit stays `NonMatching`: the rest of the file (#282, part of #283) is not reconstructed.

## F-281-3: Value conversion and mutation lifetimes

`CValue::ToBool` (`0x8016FF60`, 140 bytes), `Increment` (`0x80170180`, 240 bytes), and `Decrement` (`0x80170280`, 240 bytes) now reproduce every native instruction. The float boolean conversion names the loaded `f32` before comparing it with `0.0f`; implicit conversion reverses the `fcmpu` operands. Both mutations bind a const reference to the current value for the type, conversion, and null tests. The reference preserves the initial type load through the switch and removes the previous extra reload and reference-type comparison. The reference spelling is inferred source structure, not an original recovered name.

Independent comparison against the frozen DOL checks all 620 instruction bytes and pairs the three `R_PPC_EMB_SDA21` sites with their exact original zero and one float constants. No header, layout, compiler setting, or unit selection changes. The unit remains `NonMatching`, now 48/63 functions at 100%; the loader, engine lifetime/initialization, interpreter operations, and collector remain unfinished. This is no linked-source acceptance or issue closure.

Raw controls are outside the repository at `C:/MassStorage/GitHub/281-native-values`: `values-paired.json` contains the native instruction and relocation comparison; the final verification round records the current-main baseline source, proposed source, whole source inventory, unchanged function comparison, and linked artifact hashes. Full configure, Ninja, and frozen-manifest verification are required again on the exact merge head.

Rejected controls: a copied value adds stack storage; a separate integer type snapshot adds another initial load; early return and direct if/else forms do not reproduce the native switch. Header-field snapshots improve the loader's instruction comparison but do not solve its alias-sensitive scheduling and are not retained.

## F-281-4: Pointer-set declaration prerequisite

The primary MSL tree header at [11c112f](https://github.com/KooShnoo/pokemon-rumble/blob/11c112f7548b26ab2cecfcf52e201b8d3b76120f/libs/PowerPC_EABI_Support/MSL/MSL_C%2B%2B/tree) describes the compiler's pointer-container optimization: default pointer comparisons use a `const void*` backing tree and typed iterator wrappers. Its base iterator has an explicit copy constructor; its `insert_one` returns an iterator/bool pair. The existing header had omitted the copy constructor and gave `insert_one` a void return despite providing the correct search and insertion operations.

The bounded prerequisite restores those two declarations and adds the default-pointer specialization of `std::set`. The generic set and its existing `insert` path remain intact. The pointer path uses the already separate `insert_one` operation and retains the intermediate backing-tree pair when converting its iterator. `CEngine::Init` now reproduces all 608 native instruction bytes and 20 independently paired relocation sites. The two outside tree callee types are inferred from this primary interface and the native caller; their native targets are `0x800BC3D0` and `0x800BB3C0`. Their original mangled spellings are not claimed recovered, and no outside symbol is renamed by this prerequisite. The source-visible set remains 12 bytes, as proven by the unchanged native allocations and following member accesses.

The engine destructor improves to 90.21% but remains unfinished. Eighty other common emitted VM functions retain their instruction bytes and relocation targets; four string conversion functions' local format/empty-string constants move within the unaccepted source object's small-data section but preserve their exact values. Four pointer-tree destructor helpers change template identity from the typed pointer to the evidenced `const void*` backing type. This is a declaration prerequisite, not complete VM unit acceptance; the object remains `NonMatching` and its original fallback is selected.

The fresh baseline at `31674ae11216b7f0cc45a4279845c4b16fe2afce` and proposed build each run actual configure, Ninja, `all_source`, and frozen verification with exit 0. All 714 declared sources compile. Every allocated section, size, alignment, and normalized relocation identity in all 713 other source objects remains exact; 701 are also whole-file identical, while twelve differ only in compiler metadata/local names. Every allocated linked ELF section, address, and extent is exact and the complete DOL remains identical to the frozen original. The ELF whole-file hash changes in metadata. The final exact-head landing repeats these controls after rebasing. Raw controls are `C:/MassStorage/GitHub/281-header-controls`, with `existing-consumer-sections.json`, `vm-consumers.json`, and `281-pointer-set-probe/canonical-init-paired.json`.

An initial global reversal of the generic insertion flag declarations regressed the previously exact `BModelObj::AddDrawLayer`. It is rejected. Correcting the already separate `insert_one` return interface preserves that 144-byte consumer at 100% while producing the native VM initialization shape. No existing generic insertion order is changed.

## Traps

### Bounded follow-up controls after F-281-3

The canonical unit compiled with the established Wii/1.3 alternative has the same remaining function scores as Wii/1.1. This provides no reason to change its compiler. Outside controls are `C:/MassStorage/GitHub/281-compiler-control.py` and `281-compiler-control/NPpuVM.json`.

The 2006 MSL tree reading aid is pinned to [KooShnoo/pokemon-rumble tree at 11c112f](https://github.com/KooShnoo/pokemon-rumble/blob/11c112f7548b26ab2cecfcf52e201b8d3b76120f/libs/PowerPC_EABI_Support/MSL/MSL_C%2B%2B/tree). It supplies an actual pointer-container optimization design: typed wrappers over a `const void*` tree, with an intermediate insertion pair. A bounded outside reconstruction of that design makes the missing true/false pair stores appear in `Init`, but retains additional copies and scores only 88.49%; the destructor improves from 86.54% to 90.21%. Neither header is adopted. An explicit iterator copy constructor and by-reference increment alone leave both functions unchanged. Deliberate `#error` inclusion control confirms the overlay header is really used. Inputs and results remain outside at `281-pointer-set-probe*` and `281-tree-reference-probe*`.

Two other hypotheses are rejected without canonical changes. A const-reference alias inside `ConvertToNumber` worsens the three operator bodies, and returning a converted value by value worsens them further. A four-field loader aggregate reaches only 90.62% and does not justify changing the class declaration. Moving the ordinary inline `Clear` definition after its callers leaves the loader unchanged; reading the magic before clearing worsens it. The outside `281-loader-aggregate-probe*` and `281-clear-placement-probe*` retain these controls. No compiler options, synthetic calls, volatile fields, or offset-only views are introduced.

- **A reference can preserve an existing type load.** In these two mutation functions, reading through a const reference to `*this` preserves the native initial tag through the switch. A value copy and an independent integer snapshot do not; this is a bounded compiler observation, not a general reference rule. See F-281-3.

- **A `blr` after a tail call is not a function.** dtk split `0x801700E8` and `0x80170170` as four-byte functions; they belong to the preceding function.
- **Inline helpers must be marked inline.** `CCode::Clear` and `CEnvironment::AllocGlobals` were emitted out of line under `-ipa file` until they were declared `inline`; the original has no copies.
- **vptr position follows declaration order.** Declaring the virtual functions before the data members put the vptr at offset 0; the NPpu classes need the members first.
