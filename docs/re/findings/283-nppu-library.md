# Issue 283: NPpu script VM, third block

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`. Compiler and flags are the game settings of [F-735-1](735-game-compiler.md). The block `0x801738B0`–`0x80175804` covers the end of `game/NPpuVM.cpp` (split, object line and header from #281, [F-281-1](281-nppu-vm.md#f-281-1-the-first-nppu-unit-ends-at-0x80174d64)) and the start of a second file.

## F-283-1: the second NPpu file starts at 0x80174D70

The second file starts with the native `GetThrownValue` at `0x80174D70` and ends with the last NPpu function `0x80176C60` (0x494 bytes); RevoEX `NETVersion.c` follows at `0x801770F4`. #283 owns its first function, so it owns the split. It is registered as `game/NPpuLibrary.cpp` (descriptive; no file name string is embedded). The rest of the file is #284.

| Section | Range | Evidence |
| --- | --- | --- |
| `extab` | `0x80034EBC`–`0x8003501C` | entries of `0x80174D70` to `0x80176C60` |
| `extabindex` | `0x8004FF14`–`0x8005004C` | first record `0x80174D70`, last `0x80176C60`; the next record indexes `0x8019DC50` |
| `.text` | `0x80174D70`–`0x801770F4` | previous unit ends `0x80174D64` |
| `.data` | `0x803B8440`–`0x803B8730` | the wide `"hjlLqtz"` and `"%cCsSdiouxXfeEgGnp"` and the jump table `0x803B8478` used by `0x80175090`, the narrow conversion string and jump table `0x803B85DC` used by `0x801753B0`; `0x803B8730` (`"HUSIGIDANE"`) is referenced only from the `.rodata` name table |
| `.sdata` | `0x804B7B68`–`0x804B7B78` | `"hjlLqtz"` (`0x801753B0`) and the 8-byte zero object used by `Sprintf` and `String`; `0x804B7B78` (`"RIZAADO"`) belongs to another file |

No other data section is referenced from `0x80174D70`–`0x801770F4`. Splitting changes no output bytes; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-283-2: identities of the 24 functions

| Address | Function | Evidence |
| --- | --- | --- |
| `0x801738B0`, `0x801738C0` | `CEngine::SetID`, `SetNextID` | F-195-3 |
| `0x801738D0`, `0x801739E0` | `CEngine::ToString`, `ToWString(const CValue&) const` | String-table index (no bounds check), pointer, `"%d"`/`"%g"` into a 32-character static buffer (`.bss` `0x803E9910`, `0x803E9930`) |
| `0x80173AF0`, `0x80173E70` | `CEngine::ToStdString`, `ToStdWString(const CValue&) const` | Build a `std::string`/`std::wstring` from the value, converting the other width through `0x80174C50`/`0x80174B30` |
| `0x801741F0` | `CEngine::GetThrown(CValue&) const` | Copies member 0x50; called by `GetThrownValue` |
| `0x80174210`, `0x801743B0` | `CEngine::AllocString`, `AllocWString` | Allocate a 0x10-byte `{string, mark}` object, stamp it with the mark `0x804B90E0`, count allocations in `0x804B90E4`, insert `{c_str, object}` into `sStrings`/`sWStrings` and return `c_str` |
| `0x80174550` | collector | When the allocation count exceeds the argument it bumps the mark, marks every string referenced by type 3 or 7 values on each engine's stack and in the environment globals, then frees unmarked entries |
| `0x80174AD0` | `find_leaf` of the string sets | Weak template instance; both sets share it, so both have the same element type |
| `0x80174B30`, `0x80174C50` | `ToUnicode(const std::string&)`, `ToSjis(const std::wstring&)` | `ENCConvertStringSjisToUnicode`/`UnicodeToSjis` twice, sized by the first call into an `alloca` buffer |
| `0x80174D70`–`0x80175010` | natives `GetThrownValue`, `Interrupt`, `Yield`, `Exit`, `GetThisObject`, `ShowPPU`, `SetDebugLevel`, `SetInitialDebugLevel` | F-29 binding table; return 0 (continue), 1 (`Yield`) or 3 (`Exit`); the debug-level and `ShowPPU` bodies are empty in this build |
| `0x80175090`, `0x801753B0` | `Format(CEngine*, int, const wchar_t*)`, `Format(CEngine*, int, const char*)` | printf-style formatting of script arguments into 1024-character buffers; `S` forces a wide string argument, `n` is rejected |
| `0x801756C0` | native `Printf` | Argument count from the code stream, formats and discards the result (output removed in this build) |

The natives are C++ functions whose mangled names keep the F-29 `ScriptBinding_` prefix (`ScriptBinding_Printf__FPQ24NPpu7CEngine`); the prefix and their linkage are descriptive, low confidence. The method names are descriptive, high confidence for behaviour.

The string-table lookups use MSL `basic_string`. Its observed layout is 12 bytes: bit 31 of the first word selects the long form, which holds the capacity in the other 31 bits, the size at 4 and the data at 8; the short form keeps a 7-bit size in the first byte and the characters from offset 1 (`char`) or 2 (`wchar_t`). The new `include/string` reproduces this layout with the out-of-line `reserve` (`0x8006D8B0`, `0x80082020`) and `replace(pos, n1, first, last, forward_iterator_tag)` (`0x80067F70`, `0x80082260`) instances, which are named in `symbols.txt` with the mangled names it produces (template names medium confidence; behaviour high: length error above `0x7FFFFFFE`, out-of-range position check).

## F-283-3: reconstruction state

| Function | Match |
| --- | --- |
| `SetID`, `SetNextID`, `GetThrown`, `ToUnicode`, `ToSjis`, eight natives, `Printf` | 100% |
| `ToString`, `ToWString` | 99.85% (literal relocation names only) |
| `ToStdString`, `ToStdWString` | 96.1% |
| `Format` wide, narrow | 95.9%, 95.5% |
| `AllocString`, `AllocWString` | 68.5%, 65.6% |
| collector `0x80174550`, `find_leaf` `0x80174AD0` | Not reconstructed |

Remaining differences:
- **`ToStdString`, `ToStdWString`.** The original keeps the address of the converted temporary in a register; the source recomputes it from the stack pointer.
- **`Format`.** Register allocation of the scanning pointers. The argument counter `1 - argCount` counting up to 0 is required for the `subfic` forms.
- **`AllocString`, `AllocWString`.** `basic_string::operator=` computes the target size before the source range with a single long-form test, and the original stores the `pair<iterator, bool>` returned by `insert` (as in `Init`, F-281-2).
- **Collector and `find_leaf`.** The string sets are 12-byte trees of `{key, object}` pairs ordered by key, and `insert_node_at` `0x800B10A0` is shared with a map in an earlier file (`0x800AE0D0`). Iterating them needs iterator increment and dereference, and `find_leaf` needs a definition, neither of which the shared `MSL_CPP/detail/tree.h` provides; that header is not changed here.

Both units stay `NonMatching`; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0 with the original objects.

## Method

`tools/re/unitdiff.py`, GNU objdump 2.42 on `build/wpse-v1/main.elf`, and the generated assembly. No decompiler output was used.

## Traps

- **Explicit tag temporaries change scheduling.** Passing `random_access_iterator_tag()` to a `forward_iterator_tag` parameter moved the tag address load; passing `forward_iterator_tag()` matches the conversion functions.
- **Bool-returning bitfield tests add `cntlzw`.** `!is_long()` through a `bool` helper inside a conditional expression produced `cntlzw`/`srwi.`; testing the bitfield against 0 directly gives the original `srwi.`/`bne`.
- **Declaring locals before a call fixes prologue order.** In `Format`, the pointers and argument counter are initialised before `strncpy` in the original; declaring them after the copy cost about 2%.
