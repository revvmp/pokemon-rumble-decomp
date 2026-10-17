# MetroTRK shared header prerequisite

## F-541-4: Proven communication and portable interfaces

Part of #541, shared with #543 and #544. This prerequisite moves the declaration subset needed by the #541 reconstruction into canonical `include/MetroTRK/` headers before neighboring contributors extend those interfaces. It supplies no implementation, source-coverage credit, original data or build-object changes.

Interfaces and enums adapt [Petari e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe/libs/MetroTRK/include), CC0-1.0 with no file-level exception found in the consulted headers. Declarations are trimmed to APIs exercised by reconstructed target routines, and comments are removed. Existing SDK headers are reused rather than redeclared.

Native accesses prove `CircleBuffer` is `0x1C` bytes: read, write and start pointers at zero, four and eight; size at `0xC`; readable and writable counts at `0x10` and `0x14`; and the four-byte interrupt-state critical section at `0x18`. Its exact `unsigned int` critical-section type agrees with the helper pointer interface. GDEV's callback uses the existing OS interrupt-handler type. `TRKEvent` is twelve bytes with type, ID and buffer ID at zero, four and eight; these fields are corroborated by the event loop's loads and reference interfaces.

The target's complete `TRKBuffer` is `0x88C` bytes: in-use flag at zero, length at four, position at eight and `0x880` data bytes at `0xC`. It omits the reference's unused leading mutex word. `TRKGetBuffer` at `0x8034F928` multiplies the index by `0x88C`; reset at `0x8034F974` clears fields at four/eight and passes buffer plus `0xC` to `TRK_memset`; the position setter at `0x8034F99C` independently accesses those same fields. Dispatcher command `data[4]` therefore resides at offset `0x10`. This layout is independently checked against native functions before publishing the shared type.

The twelve typed command handlers follow the original protocol values and dispatch jump table at `0x803E6270`; Versions and SupportMask are not retained target cases. The enum retains the reference's protocol command constants, without claiming every command has an implementation in this target. Error enum values and widths follow the canonical interface and exercised native branches. No speculative processor state is declared.

## Traps

- Dropping or adding the reference's unused mutex word changes the target ABI. Match the target's `0x88C` buffer stride and independent reset/position accesses before extending message APIs.
- Keep shared declarations in this tree and extend them through the owning issue or a prerequisite header pull request. Do not create a second MetroTRK header family.
