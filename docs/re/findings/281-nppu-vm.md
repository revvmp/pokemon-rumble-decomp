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

The script binding table's interpreter (`CVM`) is not an RTTI class; its name is descriptive.

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

## Traps

- **A `blr` after a tail call is not a function.** dtk split `0x801700E8` and `0x80170170` as four-byte functions; they belong to the preceding function.
- **Inline helpers must be marked inline.** `CCode::Clear` and `CEnvironment::AllocGlobals` were emitted out of line under `-ipa file` until they were declared `inline`; the original has no copies.
- **vptr position follows declaration order.** Declaring the virtual functions before the data members put the vptr at offset 0; the NPpu classes need the members first.
