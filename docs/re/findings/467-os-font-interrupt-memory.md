# Issue 467: RVL_SDK OSFont, OSInterrupt, OSLink, OSMessage, OSMemory

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Evidence comes from the dtk 1.8.4 split assembly under `build/wpse-v1/asm/`, objdiff-cli 3.8.2 reports in `build/wpse-v1/report.json`, GNU binutils 2.42-2 disassembly of the split and compiled objects, and Wii/1.0 `mwcceppc.exe` with `cflags_sdk`. Reference: Petari `e0ab141914921726a595573dc48d3f13d0922efe`, CC0, `src/RVL_SDK/os/OSFont.c`, `OSInterrupt.c`, `OSLink.c`, `OSMessage.c`, `OSMemory.c` and the matching headers under `libs/RVL_SDK/include/revolution/os/`. None of these files carries a file-level notice.

## F-467-1: Unit boundaries

| Unit | `.text` | Data | Evidence |
| --- | --- | --- | --- |
| `OSFont.c` | `0x802B7900`–`0x802B8C90` | `.data` `0x803C7F58`–`0x803C8A68`; `.sdata` `0x804B8A88`–`0x804B8A90`; `.sbss` `0x804B94B8`–`0x804B94C8`; `.sdata2` `0x804BB270`–`0x804BB278` | `GetFontCode` starts after `OSFatal.c`'s last routine at `0x802B7310`, whose string `0x803C7F48` precedes the font tables. The two tables are `HankakuToCode` (0x180) and `Zenkaku2Code` (0x98A plus padding). `FontEncode` is the 0xFFFF halfword. The four `.sbss` words are, in address order, `ParseString`, `FixedPitch`, `FontDataSjis`, `FontDataAnsi`. The `.sdata2` pair is ReadFont's `imageT` initializer. |
| `OSInterrupt.c` | `0x802B8C90`–`0x802B9460` | `.data` `0x803C8A68`–`0x803C8A98`; `.sbss` `0x804B94C8`–`0x804B94E0` | `InterruptPrioTable` (12 words) and `__OSLastInterruptSrr0`, `__OSLastInterrupt`, `__OSLastInterruptTime`, `InterruptHandlerTable` |
| `OSLink.c` | `0x802B9460`–`0x802B9480` | none | `__OSModuleInit` only |
| `OSMessage.c` | `0x802B9480`–`0x802B9760` | none | Four message-queue routines |
| `OSMemory.c` | `0x802B9760`–`0x802B9F00` | `.data` `0x803C8A98`–`0x803C8AA8`; `.sbss` `0x804B94E0`–`0x804B94E8` | `ShutdownFunctionInfo` and `__OSInitMemoryProtection`'s `initialized`. `0x802B9F00` begins the mutex unit (it calls `OSInitThreadQueue` and touches thread offsets 0x2F0–0x2F8). `@2083` at `0x803C8AA8` belongs to code at `0x802BA8A0`. |

The issue's estimated block ends at `0x802B98A0`, but `OSMemory.c` continues to `0x802B9F00`, covering nine functions the issue table assigns to #468. Because this issue owns the file's first function, it owns the split. A partial `OSMemory.c` does not work: with `__OSInitMemoryProtection` absent, the static `ShutdownFunctionInfo`, `OnShutdown`, `MEMIntrruptHandler`, and the configuration routines are never referenced, so the compiler emits none of them. The whole unit was therefore reconstructed here. Confidence high.

The split-only build and the frozen-manifest verifier passed before any source was registered.

## F-467-2: Names

Recovered from Petari and identical instruction sequences: `GetFontCode`, `Decode`, `OSGetFontEncode`, `ReadFont`, `OSLoadFont`, `ParseStringS`, `ParseStringW`, `OSGetFontTexel`, `__OSGetInterruptHandler`, `__OSDispatchInterrupt`, `OSInitMessageQueue`, `OSSendMessage`, `OSReceiveMessage`, `OSJamMessage`, `OnShutdown`, `ConfigMEM1_24MB`, `ConfigMEM2_52MB`, `ConfigMEM2_56MB`, `ConfigMEM2_64MB`, `ConfigMEM2_112MB`, `ConfigMEM2_128MB`, `RealMode`, and the data names `HankakuToCode`, `Zenkaku2Code`, `InterruptPrioTable`, `FontEncode`, `ParseString`, `FixedPitch`, `FontDataSjis`, `FontDataAnsi`. Confidence high.

Five OSFont routines are newer than Petari's revision. `ExpandFontSheet` (`0x802B8640`), `OSInitFont` (`0x802B88B0`), `OSGetFontTexture` (`0x802B8A90`), and `OSGetFontWidth` (`0x802B8BE0`) are the public RVL SDK names for, respectively, expanding a 2bpp sheet to I4 or IA4 and flushing it with `DCStoreRange`, decoding ROM fonts into a caller buffer, returning a glyph's sheet image and cell position, and returning a glyph's width. The names come from SDK conventions, not from strings in this executable. Confidence medium.

`0x802B9DE0` is called by `0x802B63F0` (OSExec) with `0xBA2CF`. It switches to real mode to run `ConfigMEM1_24MB`, then loops forever unless its argument equals `0xBA2CF`. `BATConfig` takes the same key from `__OSInitMemoryProtection` and ends with the same check. No original name was found, so the routine is named descriptively `__OSRestoreMEM1Config`, and the key is `OS_MEMORY_CONFIG_KEY`. Confidence low. Rename it when evidence appears.

This revision's `BATConfig` omits Petari's ES1.0 and 48 MB MEM1 paths. `OSGetPhysicalMem1Size` has no code in the original; it is kept in the source and is dead-stripped at link, like `PPCMfwpar` in F-463-1.

## F-467-3: Codegen observations

- `GetFontCode`'s ANSI branch matches only when written as `code > 0x20 && code < 0x100`. Then Wii/1.0 emits the u16 range check `addis`/`addi -33`/`clrlwi`/`cmplwi 222`. `code > 0x20 && code <= 0xFF`, which is Petari's form, emits two compares.
- `OSGetFontTexture` matches with `fontCode - sheet * CharsInSheet(font)`. Writing out `sheet * sheetColumn * sheetRow` swaps r5 and r6.
- `OSGetFontEncode` is inlined into every caller, which `-inline auto,level=3` allows.

## F-467-4: Split alignment of a successor object

The first link attempt that used `OSLink.c` as a source object shifted `.ctors` by 8 bytes. The compiled object's `.text` is 0x18 bytes with 16-byte section alignment. The dtk-split `OSMessage.o` that follows has only 4-byte `.text` alignment, so it was placed at `0x802B9478`. Adding `align:16` to the `OSMessage.c` `.text` split restores the original layout, because the original object was compiled at Wii/1.0's default 16-byte alignment. The same effect blocks `OSMemory.c`: its object ends at `0x802B9EFC`. The auto-split successor at `0x802B9F00`, a unit owned by #468, links at `0x802B9EFC` unless it gets its own split with `align:16`.

## F-467-5: Match and link status

| Unit | objdiff | Linked from source | Blocker |
| --- | --- | --- | --- |
| `OSInterrupt.c` | 13/13 functions, data 100% | yes, verifier passes | none |
| `OSLink.c` | 1/1, 100% | yes, verifier passes | none |
| `OSMessage.c` | 4/4, 100% | yes, verifier passes | none |
| `OSFont.c` | 13/13 functions, data 100% | yes, verifier passes | none |
| `OSMemory.c` | 15/15 functions, data 100% | yes, verifier passes (F-467-6) | none |

The first source-link attempts of `OSFont.c` and `OSMessage.c` failed with undefined external names. The collaboration rules allow naming outside symbols with evidence, so seven outside labels were renamed in `symbols.txt`; their splits, sizes, and types stay with their owning issues. `0x802B40A0` (#465) is `DCStoreRange`: a `dcbst` loop over 32-byte lines with no trailing `sync`, which ExpandFontSheet tail-calls with the sheet and its size. `0x802BB460` (#469) is `__OSReadROM`: it calls `DCInvalidateRange`, `EXILock(0, 1, 0)` and `EXISelect(0, 1, 3)`, as in Petari's OSRtc.c, and ReadFont retries it in 256-byte chunks. `0x802BCF60` (#469) is `OSWakeupThread`: it disables interrupts and walks the queue passed by every OSMessage routine, matching the reference call sites. `0x802BD730`, `0x802BD850`, `0x802BD8C0` and `0x802BD940` (#470) are `OSUTF8to32`, `OSUTF16to32`, `OSUTF32toANSI` and `OSUTF32toSJIS`, from ParseStringW's call order and arguments. The last one rejects values of 0x10000 and above and looks up a halfword table. Confidence high. With these names in place, both units link from source and the verifier passes.

## F-467-6: OSMemory.c links from source

#619 split `OSMutex.c` at `0x802B9F00` with `align:16` (F-468-1), which removes the F-467-4 blocker. With `OSMemory.c` registered as `Matching` and no change to its source, splits, or symbols, `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. objdiff reports 15/15 functions, 1,852/1,852 code bytes and 24/24 data bytes for the unit. No outside symbol needed a new name. Confidence high.

## Traps

- **A source object can shift the next split object.** If a compiled unit's code does not end on a 16-byte boundary, as with `OSLink.c` (0x18 bytes) or `OSMemory.c` (ending `0x802B9EFC`), a following dtk-split object with 4-byte `.text` alignment moves up and the whole DOL shifts. Add `align:16` to the successor's `.text` split, as done for `OSMessage.c`. The original Wii/1.0 SDK objects did have 16-byte alignment. See F-467-4.
- **A partial SDK source can compile to nothing.** If a unit's root routine is left out (here `__OSInitMemoryProtection`), its static data and static callbacks are no longer referenced, CodeWarrior emits none of them, and objdiff reports no functions. Reconstruct everything reachable from the unit's roots. See F-467-1.
