# F-61-1: Inventory of the second CDividedGround block

Target: `wpse-v1`. Issue #61 covers `0x80072280`–`0x800740F4` of `game/CDividedGround.cpp` ([F-60-1](60-cdividedground-unit.md)): 28 functions, 7,632 code bytes. The split and object line already exist; this finding identifies each function from its instructions, relocations and callers, so the remaining work can be planned.

Callers were found in the split objects with `powerpc-eabi-objdump -dr` and `readelf -rW`, and in the generated assembly under `build/wpse-v1/asm/`. Offsets in the original object are the address minus `0x80071220`.

## Functions

| Address | Size | Identification | Evidence | Needs |
| --- | --- | --- | --- | --- |
| `0x80072280` | `0xA0` | `basic_string` assignment from another string | Copies the 12-byte representation when both strings are short; otherwise calls `0x80067F70` with position 0, the current size and the source range | MSL `basic_string` |
| `0x80072320` | `0x1C` | `basic_string` data pointer | Short strings (bit 31 of word 0 clear) store data at `+1`, long strings at `+8` | MSL `basic_string` |
| `0x80072340` | `0x4B0` | Ground loading entry point | Called from `0x800A8BA4`; calls `0x80072AE0`, then `0x80073660` with a null name; formats `ground_` and `%02d`; makes a 0xDC-byte model and looks up `ResMdl` in each file of `mFiles` | MSL `basic_string`, `CModel` |
| `0x800727F0` | `0x8` | One-word setter | Called from `0x80074204` (#62) and from the units at `0x801C75E0` and `0x801E24C0` | Its #62 caller |
| `0x80072800` | `0x8` | `CFileData` data getter (`+0x1C`) | F-326-5; called from `0x800741F8` (#62) and four later units (`0x8012C660`, `0x801C75E0`, `0x801CE500`, `0x801E24C0`) | Its #62 caller |
| `0x80072810`, `0x80072820` | `0x8` each | Getters of words `+8` and `0` | Called from #62 (`0x800746D8`, `0x80074868`); `0x80072810` also from `CDlgPiiSelector.cpp` and a script binding | Their #62 callers |
| `0x80072830` | `0xD8` | `operator+(const basic_string&, const basic_string&)` | Zeroes the result, reserves the sum of both sizes through `0x8006D8B0`, appends both ranges through `0x80067F70` | MSL `basic_string` |
| `0x80072910` | `0xC8` | `operator+(const char*, const basic_string&)` | As above with `strlen` for the left operand | MSL `basic_string` |
| `0x800729E0` | `0x8C` | `shared_ptr<T>::shared_ptr(T*)` | Allocates a 16-byte deleter with vtable `0x8039BA78` and both counts 1 | The caller that emits it |
| `0x80072A70`, `0x80072A80`, `0x80072A90`, `0x80072AA0` | `0xC`–`0x1C` | `list` iterator `operator->` (node `+8`), dereference of word 0, `operator++` (next at `+4`) and `operator!=` | Called from #62 (`0x800741F0`–`0x80074258`) | Their #62 callers |
| `0x80072AC0`, `0x80072AD0` | `0x10`, `0x8` | `vector<const char*>` `operator[]` and data pointer | Called from #62 (`0x8007449C`–`0x80074868`); `0x80072AD0` also from `CDlgPiiSelector.cpp` and a script binding | Their #62 callers |
| `0x80072AE0` | `0x2F0` | Builds `mFileNames` and `mFiles` | Indexes the table at `0x8035B9F8` with the stage ID, joins `stage/`, `/`, `%02d`, `.arc` and `stage.brres` paths, reserves `mFileNames` (`0x80072FF0`), appends each name (`0x800731B0`) and a null, and inserts each loaded list into `mFiles` (`0x80072DE0`); calls `list::begin` `0x800713F0` and `end` `0x80071400` out of line | MSL `basic_string` |
| `0x80072DD0` | `0x8` | File-loader wrapper | ORs the flags in `r6` with `0x2000` and tail-calls `0x80127CF0`, like `0x80066930` (F-326) | Loader declaration |
| `0x80072DE0` | `0x1DC` | `list<shared_ptr<CFileData>>::insert(iterator, const_iterator, const_iterator)` | Builds a temporary list of copied nodes, splices it before the position, then destroys what is left | `include/list` insert |
| `0x80072FC0`, `0x80072FD0` | `0x10`, `0x14` | `shared_ptr` default constructor and non-null test | Two zeroed words; `(p != 0)` as `srwi` of `neg|p` | The caller that emits them |
| `0x80072FF0` | `0x1B4` | `vector<const char*>::reserve` | Throws `length_error("vector length error")` above `0x3FFFFFFF`; moves with `memcpy` and clears with `memset` | POD layering of `include/vector` |
| `0x800731B0` | `0x47C` | `vector<const char*>::push_back` | Same growth policy as the class-type vector (`3/5` below a third of the maximum, `1/2` below two thirds) | POD layering of `include/vector` |
| `0x80073630` | `0x8` | `shared_ptr` dereference returning a list | Its result is passed as `this` to `list::begin` and `end` | The caller that emits it |
| `0x80073640` | `0x14` | `CDividedGround::LoadAttr(const char*, u32)` | Forwards `mStageID` and `mStageNo`; called only from `ScriptBinding_LoadGroundAttr` (`0x8013CD18`) with flags 0 | Done |
| `0x80073660` | `0x904` | `CDividedGround::LoadAttr(const StageID*, int, const char*, u32)` | For each of the two BSP slots, loads a file with the loader `0x80066930` (passing the flags), makes a `CBSPTree` and stores both in `mBSPFiles` and `mBSPTrees`; appends the optional name when it is non-empty | MSL `basic_string` |
| `0x80073F70` | `0x84` | `CDividedGround::FindIndexSuffix(const char*)` | Returns the `_` before a trailing run of digits, or the end of the string; called from `0x80074300` (#62) | Done |
| `0x80074000` | `0xF4` | Predicate on a `basic_string` | True when the length is at least 8, the name ends with `_fog`, and it starts with `layer_` or `pale_`; called from `0x80074424` (#62) | MSL `basic_string` |

`0x80073640` is not virtual and has a single caller, the script binding named `LoadGroundAttr` (#29), so it is a public member. `LoadAttr` and `FindIndexSuffix` are descriptive names. The type of the last parameter follows the open loader parameter in F-326 and is unresolved.

## Status

objdiff, after this issue's source:

| Address | Function | Match |
| --- | --- | --- |
| `0x80073640` | `CDividedGround::LoadAttr(const char*, u32)` | 100% |
| `0x80073F70` | `CDividedGround::FindIndexSuffix(const char*)` | 100% |
| Others | | Not reconstructed |

The unit is 13/136 functions at 100%. The original object stays linked; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

The digit test in `FindIndexSuffix` must be written `'0' <= *p && *p <= '9'`. Written `*p >= '0' && *p <= '9'`, the compiler folds both comparisons into one unsigned `subi`/`clrlwi`/`cmplwi` test (91.5%); the original keeps two signed comparisons.

## What remains

- **MSL `basic_string`.** Eight of the remaining functions use it directly, and the iterator, vector and `shared_ptr` helpers are emitted only through those callers or #62's. The layout used here is 12 bytes: bit 31 of word 0 marks a long string with the size at `+4` and data at `+8`; a short string keeps its size in the low seven bits of byte 0 and its data at `+1`. Its out-of-line members live in earlier units (`0x80067F00` destructor, `0x80067F70` range replace, `0x8006D8B0` reserve, `0x8006DCA0` `operator+(const basic_string&, const char*)`). F-60-2 places the MSL C++ headers with #327, which is claimed.
- **POD `vector`.** `reserve` and `push_back` for `const char*` need the POD layering that F-60-3 left open.
- **#62 callers.** Ten small helpers are called in this unit only from #62's functions. They are weak copies that later units also call, so the first copy is emitted here only once those #62 callers exist.

## Traps

- **An equivalent comparison order can change code.** `*p >= '0' && *p <= '9'` and `'0' <= *p && *p <= '9'` compile differently under `cflags_game`: the first becomes a single unsigned range test. When a range check shows two signed compares in the target, try the constant on the left.
- **Most of this block cannot be compiled before MSL `basic_string`.** Only two non-template members are independent of it; the small helpers are weak and depend on string-using callers here or in #62.
