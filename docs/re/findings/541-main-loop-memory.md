# MetroTRK main loop and word-based memory helpers

## F-541-2: Four more accepted native functions

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. This pass accepts four functions and 908 code bytes in three complete C units. Together with [F-541-1](541-metrotrk-communication.md), #541 now has 21 accepted functions and 2,172 function bytes; eleven functions and 708 bytes remain.

| Unit under `MetroTRK/debugger/embedded/MetroTRK/Portable/` | Sections | Retained functions / bytes |
| --- | --- | --- |
| `main_TRK.c` | Text `0x8034DE3C`–`0x8034DE78`; SBSS `0x804B9C00`–`0x804B9C04` | 1 / 60 |
| `mainloop.c` | Text `0x8034DE78`–`0x8034DF64` | 1 / 236 |
| `mem_TRK.c` | Text `0x8034DF64`–`0x8034E1C8` | 2 / 612 |

All three are selected from source as `Matching`. The four final ELF addresses and extents agree with the original, all compared sections score 100%, and the full DOL passes frozen verification. All 343 pre-existing source objects, including the three communication units from F-541-1, remain byte-identical.

## Main and event loop

`TRK_main` at `0x8034DE3C` has the original `0x3C` extent. It records initialization status in the four-byte static `TRK_mainError` at `0x804B9C00`, enters the welcome/main-loop path on success, then records and returns termination status. The four following bytes are alignment before the next unit's storage, not part of this object.

`TRKNubMainLoop` at `0x8034DE78` has the original `0xEC` extent. It dispatches requests, stops on the shutdown event, services breakpoint/exception and support events, and destructs each consumed event. With no event available, it alternates input polling and target continuation using the existing input-pending pointer. The native load of `gTRKInputPendingPtr` uses SDA; both main units require the measured eight-byte SDA limit rather than the MetroTRK default of zero.

The typed `TRKEvent` is twelve bytes: event type at zero, event ID at four and message-buffer ID at eight. The six event values and four-byte enum representation agree with the native branches and stack accesses. The canonical error enum and API signatures come from the reference; this pass exercises the zero success value and enum width, not every unused protocol error constant. `TRKBuffer` remains an opaque C type because these two functions only pass its pointer. No speculative processor-state layout is added.

These bodies and the needed declaration subsets adapt [Petari at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe/src/MetroTRK/debugger/embedded/MetroTRK/Portable), CC0-1.0 with no file-level exception found in the consulted files. Explicit includes replace the reference's implicit function declarations. No existing shared SDK header changes.

## Memory access behavior and names

The routines at `0x8034DF64` / `0x13C` and `0x8034E0A0` / `0x128` are `TRK_memcpy` and `TRK_memset`. They are not the later polling routines: the existing `TRKGetInput` is at `0x8034EDF0`, outside #541. The copy name is corroborated by the paired reference call from `InitializeProgramEndTrap` at `0x8034E954`, and by twelve-byte event copies in `TRKGetNextEvent`. The fill routine supplies the debugger's zeroed reply buffers. These call roles, return values and contiguous unit order corroborate the original MetroTRK interfaces.

Unlike Petari's plain byte-copy implementation, this target accesses bytes through aligned 32-bit words. For a byte address, the aligned word begins at the address with its low two bits cleared. The big-endian byte position is `(3 - byteOffset) * 8`. Reads shift and narrow the word. Writes clear the selected byte mask and insert the new value, preserving the neighboring three bytes. Copy advances source and destination; fill narrows its value once and advances destination. Both return the original destination.

The C implementation is independently written from those operations and target instructions. Its small `ReadMemoryByte` and `WriteMemoryByte` helpers have descriptive chosen names; no original local-helper names are claimed. The fill loop expresses its word update directly. The compiler unrolls the copy twice and the fill four times, reproducing every one of the 153 native instruction words without relocation exclusions. No raw instruction words, original objects or executable-byte arrays are embedded in source.

## Matching constraints and rejected forms

An initial low-bit-mask expression for the byte offset generated a smaller copy and a different fill stream. Subtracting the aligned word pointer from the byte pointer reproduces the native pointer-difference sequence. The original helper declaration form then left 21 copy and 53 fill differences. Reordering the helper's offset/shift/pointer/mask declarations resolved the copy; a direct fill loop with the destination pointer declared before its narrowed byte value resolved the remaining register differences. These are ordinary C declarations and operations, without register constraints or altered compiler flags.

Compiler selection remains Wii/1.1 with measured `cflags_trk` and four-byte function alignment. This pass proves that selection for the source, without claiming compiler uniqueness. The memory unit uses the default zero SDA limits; the two main units override SDA to eight. No source-only functions or data are retained by these units.

## Validation and remaining scope

The standard baseline, split-only and source-selected configure/build/verification stages each pass. `python configure.py`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit zero. The 4,094,880-byte DOL matches directly and retains frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

Remaining #541 addresses are `0x8034D83C`, `0x8034D840`, `0x8034D84C`, `0x8034D8C4`, `0x8034D8F4`, `0x8034D8FC`, `0x8034D904`, `0x8034D90C`, `0x8034DE04`, `0x8034E1C8` and `0x8034E2E8`. They comprise the two MSL wrappers, two C++ reference-count release routines, four trap entries, cache flushing, message dispatch and debugger startup. Preserve their original fallbacks until individually accepted. #541 remains incomplete.

## Traps

- **Address adjacency does not establish a subsystem.** The two routines after the main loop are word-based memory helpers, while polling lives later. Trace their loads, stores, return values and callers before selecting a reference file.
- **Byte-equivalent formulas need not generate the same accesses.** A low-bit mask and a byte-pointer difference describe the same offset but produce different native streams here. Preserve the proven aligned-word operations and pointer arithmetic; a normal byte-copy loop would change the debugger's access widths.
