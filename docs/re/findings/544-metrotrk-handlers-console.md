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

## Validation status

The handler and console splits were first built with original objects. Configure, Ninja, and explicit verification against `config/targets/wpse-v1.json` each exited zero. Independent source compilation reproduces the native non-relocation instructions for all 13 retained handlers and all five console functions. The complete message-buffer source independently reproduces all 15 retained candidates. Full source-object, data, relocation, final-address, and frozen-artifact acceptance remain required before selecting these two new units as `Matching`.

## Traps

The reference memory handlers omit a surrounding initial-error check that the target retains. Reintroducing the error initialization and guard restores the exact instruction extents. In the write handler, placing the reply-error declaration before the options declaration restores the native register allocation; changing integer widths or compiler flags is unnecessary.

The reference free-message-buffer function declares its pointer before the error and index. The target needs error, index, pointer declaration order. This reproduces its complete 0x9C-byte body; the reporting call uses a nonvariadic declaration in that unit, while option diagnostics in the handler use the canonical variadic reporting declaration.

The endian flag is small-data addressed. Compiling the message-buffer unit with small-data threshold zero adds high-address loads and changes several routines. Threshold eight reproduces all retained message-buffer instruction streams and the handler's two local four-byte statics.
