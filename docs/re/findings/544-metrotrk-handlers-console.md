# F-544-2: MetroTRK message handlers and console I/O

Issue #544 owns 25 functions / 5,896 code bytes from `0x8034F9CC` through `0x803510D4`. It spans the tail of the #543-owned message-buffer unit, the complete message-handler unit, and the beginning of the console unit. The latter also owns its two following #545 helper functions. The complete message-buffer delivery and its nine #544 functions are coordinated with #543; no source bytes are credited twice.

## Boundaries and reference

| Complete source file | Native extents | Retained functions / bytes |
| --- | --- | --- |
| `Portable/msgbuf.c` | `.text` `0x8034F86C`-`0x80350078`; `.data` `0x803E64B8`-`0x803E64E0`; `.bss` `0x804B3AC0`-`0x804B5464` | 15 / 2,060, including nine #544 functions / 1,708 bytes |
| `Portable/msghndlr.c` | `.text` `0x80350078`-`0x80350FFC`; `.data` `0x803E64E0`-`0x803E6548`; `.sbss` `0x804B9C30`-`0x804B9C38` | 13 / 3,972 |
| `Export/mslsupp.c` | `.text` `0x80350FFC`-`0x80351188` | 5 / 396, including two #545 helpers / 180 bytes |

The reference is Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`. [Provenance](../licenses/544-metrotrk-NOTICE.md) records the individual files and license. SDK names are retained. Source uses the canonical interfaces established by #541 and [F-544-1](544-metrotrk-protocol-headers.md).

The measured MetroTRK configuration uses Wii/1.1, four-byte function alignment, automatic inlining, load/store-multiple instructions, and small-data threshold eight for these units. Wii/1.0 also reproduces the message-buffer candidates, so that comparison does not uniquely identify the compiler version. Handler diagnostics require pooled writable strings. The console file has no retained data or static storage.

## Protocol and control flow

`IsTRKConnected` is the four-byte connection state at `0x804B9C34`; `g_CurrentSequence` is the four-byte handler sequence counter immediately before it. The reference declaration order emits those two small-data objects in the target order. Each standard acknowledgement increments the handler sequence twice and sends the second value. The separate two-byte transport sequence counter owned by #543 remains a distinct object.

The 64-byte reply header's proven layout is documented in F-544-1. Read and write memory validate the extended-memory option, preserve the native initial-error guard, and use an aligned 0x820-byte transfer buffer. Read memory adjusts its payload pointer by the low five address bits when the protocol's data-space option is set. Register handlers preserve the original register classes and error conversion. Continue, step and stop preserve the target-state checks and acknowledgement order. Console wrappers require serial I/O to be enabled and map the debugger trap's low-byte result to the three file I/O outcomes.

The connection getter's two-instruction body has many weak masked search hits; its identity additionally follows the load from the connection-state object and the console access helper's outgoing call. Reset and override likewise share their instruction shapes and are distinguished by their reset and exception-vector-copy callees. The outgoing call names are recovered by pairing each source relocation with its actual native branch destination. Outside symbols receive names only; their sizes, types, and splits remain unchanged.

## Original issue inventory

| Address | Size | Recovered name | Source file |
| --- | --- | --- | --- |
| `0x8034F9CC` | `0xA4` | `TRKAppendBuffer` | `msgbuf.c` |
| `0x8034FA70` | `0x90` | `TRKReadBuffer` | `msgbuf.c` |
| `0x8034FB00` | `0xD0` | `TRKAppendBuffer1_ui32` | `msgbuf.c` |
| `0x8034FBD0` | `0xF4` | `TRKAppendBuffer1_ui64` | `msgbuf.c` |
| `0x8034FCC4` | `0x64` | `TRKAppendBuffer_ui8` | `msgbuf.c` |
| `0x8034FD28` | `0xF0` | `TRKAppendBuffer_ui32` | `msgbuf.c` |
| `0x8034FE18` | `0xE0` | `TRKReadBuffer1_ui64` | `msgbuf.c` |
| `0x8034FEF8` | `0x98` | `TRKReadBuffer_ui8` | `msgbuf.c` |
| `0x8034FF90` | `0xE8` | `TRKReadBuffer_ui32` | `msgbuf.c` |
| `0x80350078` | `0x8` | `GetTRKConnected` | `msghndlr.c` |
| `0x80350080` | `0x70` | `TRKDoConnect` | `msghndlr.c` |
| `0x803500F0` | `0x88` | `TRKDoDisconnect` | `msghndlr.c` |
| `0x80350178` | `0x6C` | `TRKDoReset` | `msghndlr.c` |
| `0x803501E4` | `0x6C` | `TRKDoOverride` | `msghndlr.c` |
| `0x80350250` | `0x234` | `TRKDoReadMemory` | `msghndlr.c` |
| `0x80350484` | `0x210` | `TRKDoWriteMemory` | `msghndlr.c` |
| `0x80350694` | `0x1FC` | `TRKDoReadRegisters` | `msghndlr.c` |
| `0x80350890` | `0x2A0` | `TRKDoWriteRegisters` | `msghndlr.c` |
| `0x80350B30` | `0xC4` | `TRKDoContinue` | `msghndlr.c` |
| `0x80350BF4` | `0x284` | `TRKDoStep` | `msghndlr.c` |
| `0x80350E78` | `0xB8` | `TRKDoStop` | `msghndlr.c` |
| `0x80350F30` | `0xCC` | `TRKDoSetOption` | `msghndlr.c` |
| `0x80350FFC` | `0x68` | `__read_console` | `mslsupp.c` |
| `0x80351064` | `0x68` | `__TRK_write_console` | `mslsupp.c` |
| `0x803510CC` | `0x8` | `__read_file` | `mslsupp.c` |

## Validation

The handler and console splits were first built with original objects. Configure, Ninja, and explicit verification against `config/targets/wpse-v1.json` each exited zero. A fresh source build uses `python configure.py --no-progress --build-dir build/544-source-1`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, and explicit frozen verification of `build/544-source-1/wpse-v1`; every command exits zero. Handler `.text`, `.data`, and `.sbss`, console `.text`, and message-buffer `.text`, `.data`, and `.bss` all compare at 100% in complete original/source objdiff comparisons.

The original issue inventory above contains exactly 25 functions / 5,896 bytes. The independent audit confirms their source symbol extents and native final ELF addresses, checks that all three unit inputs in the actual Ninja link are source objects, and directly compares the complete generated DOL with the immutable reference. Frozen SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Scope is the main game executable only; boot content 6, data contents, and WAD packaging remain excluded.

These checks accept both new units as `Matching`. The pull request records the final verified head after the #543 source dependency merges and the branch rebases onto current `main`. Message-buffer source receives no duplicate ledger row here. Its nine #544 functions are accepted through the complete file delivered by its owner. The #541 owner approved the canonical Dolphin reset/vector-copy declaration header used by the two retained callers.

## Traps

The reference memory handlers omit a surrounding initial-error check that the target retains. Reintroducing the error initialization and guard restores the exact instruction extents. In the write handler, placing the reply-error declaration before the options declaration restores the native register allocation; changing integer widths or compiler flags is unnecessary.

The reference free-message-buffer function declares its pointer before the error and index. The target needs error, index, pointer declaration order. This reproduces its complete 0x9C-byte body; the reporting call uses a nonvariadic declaration in that unit, while option diagnostics in the handler use the canonical variadic reporting declaration.

The endian flag is small-data addressed. Compiling the message-buffer unit with small-data threshold zero adds high-address loads and changes several routines. Threshold eight reproduces all retained message-buffer instruction streams and the handler's two local four-byte statics.
