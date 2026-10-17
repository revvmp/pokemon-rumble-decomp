# MetroTRK transport and event interfaces

## F-542-1: Target interfaces before source integration

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. The declarations derive from [Petari revision e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe), CC0, and native instructions. The original files and headers have no file-level license exception. This header prerequisite changes no executable split or source selection, and claims no new source-byte credit. It supports the neighboring #541, #543, #544 and #545 consumers.

## Communication table

`gDBCommTable` occupies `.bss` `0x804B3A70`–`0x804B3A98`, exactly 0x28 bytes. `InitMetroTRKCommTable` at `0x8034E6CC` assigns the established GDEV callbacks in the order below. Pointer signatures agree with the existing GDEV header; interrupt callbacks use the existing `__OSInterruptHandler` type.

| Offset | Member |
| --- | --- |
| `0x00` | `initialize_func` |
| `0x04` | `init_interrupts_func` |
| `0x08` | `shutdown_func` |
| `0x0C` | `peek_func` |
| `0x10` | `read_func` |
| `0x14` | `write_func` |
| `0x18` | `open_func` |
| `0x1C` | `close_func` |
| `0x20` | `pre_continue_func` |
| `0x24` | `post_stop_func` |

`TRK_Use_BBA` is the byte at `0x804B9C10`. The target's hardware-ID-two path returns success without installing a UDP table or setting this flag; the larger reference BBA branch is absent here.

The native `TRKInitializeIntDrivenUART` at `0x8034E808` consumes the pointer in `r5`: its signature has three arguments, unlike the four-argument older reference. The neighboring initializer passes constants 1 and 0, followed by the address of `gTRKInputPendingPtr`. The first two arguments are unused by this implementation. The `BOOL` types and parameter names describe those flag-shaped call arguments; their original spelling is not recovered. The pointer-to-pointer argument is forwarded to the table's initialization callback. The table then opens the transport. `InitializeProgramEndTrap` has no defined result and its callers ignore a result; the interface declares it `void`.

`UARTError` is the existing signed integer typedef from `MSL_C/UART.h`. The transport readers and writers return zero or minus one through the signed `(-error | error) >> 31` expression. No second typedef tree is added.

## Event queue

`gTRKEventQueue` starts at `0x804B3A98`. The five routines at `0x8034EA38`–`0x8034EBD4` access the following complete layout. Unlike the old reference header, this target has no leading mutex word.

| Offset | Member | Size |
| --- | --- | --- |
| `0x00` | `count` | 4 |
| `0x04` | `next` | 4 |
| `0x08` | `events[2]` | 24 |
| `0x20` | `eventID` | 4 |

The queue size is 0x24; four following bytes align the next unit. `TRKEvent` remains the existing 12-byte type, with event type, event ID and message-buffer ID at offsets 0, 4 and 8. The queue APIs retain the declarations already supplied for #544 and add the initializer declaration.

## Notification interface and diagnostic evidence

`TRKDoNotifyStopped` at `0x8034E9A8` accepts a `MessageCommandID`. It obtains a message buffer, selects stop information for command 0x90 and exception information otherwise, sends the request, and releases the response and request buffers. Its `TRKRequestSend` call has two arguments, as established by #543. The two target-information declarations retain the reference's distinct return types; the notification routine ignores both results.

Scratch compilation with the existing Wii/1.1 MetroTRK policy reproduces all nineteen function sizes and all non-relocation instruction words across the transport, notification and queue candidates. This is diagnostic evidence only until source selection and the full frozen build are verified. No candidate source is selected by this header prerequisite.

## Traps

- The older reference has both a four-argument UART initializer and a leading event-queue mutex field. Target callers and native member accesses prove the three-argument interface and mutex-free queue here; importing either old declaration unchanged misrepresents this ABI.
- A relocation mask can conceal an incorrect one-instruction function. `EnableMetroTRKInterrupts` is a branch to `EnableEXI2Interrupts`, not an empty return. Pair every relocation target before accepting such a match.
- Both SDK entry assembly routines retain a trailing unreachable `blr`. The first `InitMetroTRK` survey extent omitted its word at `0x8034E37C`; the BBA routine includes its word at `0x8034E410`. Their owning unit is coordinated with #541.

## Coordinated processor and packet interfaces

The prerequisite also includes the owner-approved complete processor-state declarations and native C/C++ layout probes in [F-541-6](541-processor-state-layout.md). The #543 owner supplies the existing packet-support and message-buffer API declarations from its verified source delivery, including the two-argument request/send exchange. These additive declarations preserve the existing buffer layout and canonical types and let the coordinated #541-#545 consumers include one interface. No source unit is selected by this header delivery and no source-byte credit is claimed.
