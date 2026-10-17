# F-453-2: debug text queue template reconstruction

## Scope and source fidelity

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. The contiguous narrow and wide template instantiations occupy `.text` `0x802A1950` through `0x802A346C`: 62 functions and 6,576 instruction bytes, excluding alignment. #453 owns the first retained function and thus the split and header. The native file also covers all 27 functions of #454 and the first 12 functions of #455. Their source-unit coverage is recorded explicitly; this finding does not claim the later math functions as new source.

The class and descriptive method names are chosen names, not recovered mangled symbols. `DbgPrintBase` follows the adjacent NW4R text utilities and the CC0 Xenoblade file `libs/nw4r/src/db/db_DbgPrintBase.cpp`, which is stripped in that reference and retains only a dummy CharWriter call. That file supports the library/file identification, but provides no implementation for these methods. No implementation was copied from it. The duplicate `PrintfCopy`, `VPrintfCopy`, and `AppendCopy` names distinguish observed duplicate entry points without guessing an unobserved queue policy. Original spellings remain unknown.

The implementation is independently reconstructed from native PowerPC instructions. Shared Font, Color, List, and TextWriter interfaces retain their existing repository provenance. Reference discovery inspected CC0 xbret/xenoblade `ea25691b2029394ab777bda7060139e517a13cba`, kiwi515/ogws `27fa94a593096eed7442efc4e70981fd52025b9c`, zeldaret/ss `d3d8e3cbf3d353b2231f06690965cac5366e28a1`, and Petari. Public retail maps supplied no debug-print names; no map text or code is imported.

## Layout and storage

| Storage | Extent or offset | Evidence |
| --- | --- | --- |
| Class | `0x1C` bytes | Both factories allocate 28 bytes |
| Font pointer | `0x00`, 4 bytes | Constructor, accessors, Draw |
| Intrusive entry list | `0x04`, 12 bytes | List_Init, list walkers |
| GXColor | `0x10`, 4 bytes | Byte-copy setter, word-copy getter, constructor |
| Requested font height | `0x14`, f32 | Default -1, accessors, Draw's SetFontSize call |
| Visibility | `0x18`, bool | Default true, accessors, conditional drawing |
| Entry | x/y/frames/length at `0/4/8/0xC`; link at `0x10`; text at `0x18` | Allocation, append, drawing, removal |
| Narrow heap and singleton | `.sbss` `0x804B93F8` and `0x804B93FC` | Narrow allocator and factories |
| Wide heap and singleton | `.sbss` `0x804B9400` and `0x804B9404` | Separate wide instantiation |
| Constants | `.sdata2` `0x804BB0F8`-`0x804BB10C` | -1, zero, signed integer conversion double, one |

Text records allocate `24 + length` bytes and copy exactly `length` bytes in both instantiations. The wide version does not multiply that byte count by two. This observed behavior is preserved rather than corrected. Format scratch space is half the allocatable heap extent after subtracting the 28-byte queue object size. Single-frame entries allocate from the front; longer-lived entries allocate from the back. Formatting uses temporary allocations and frees them after copying into a retained entry.

Draw creates a TextWriter, returns without processing entries when there is no font, applies color and optional font height, then iterates safely while removing expired entries. Visibility suppresses drawing but still ages entries. The viewport overload builds an orthographic projection and identity model matrix before calling Draw.

## Reconstruction and acceptance

Wii/1.1 and the established `cflags_nw4r` reproduce all 62 methods. No new compiler flags are retained. The append declaration uses the compiler-supported `never_inline` attribute to preserve the observed out-of-line append calls. Color values use the aggregate ABI: the setter accepts GXColor by value, while factories and construction use the NW4R Color value. This distinction reproduces both byte-copy ordering and factory register allocation.

The split-only baseline passed default Ninja and explicit exact verification. The source-linked build passed default Ninja and the frozen verifier. Objdiff-cli 3.8.2 reports 62/62 functions, 6,576/6,576 code bytes, and 36/36 data bytes matching; text, sbss, and sdata2 are each 100%. The exact committed merge head is revalidated after rebasing, with commands and exit codes in the PR.

Only directly referenced outside functions were named, including TextWriter constructors/destructors and Print for char and wchar_t, CharWriter SetupGX, UpdateVertexColor, and the single-height SetFontSize. Their splits, sizes, and source status are unchanged. The shared CharWriter declaration prerequisite merged separately in PR #867.

## Traps

- A struct address in r4 does not establish a reference parameter. GXColor by value also uses that ABI. Testing the by-value color interface reproduced the target's serial setter and constructor/factory scheduling; adding scheduling flags did not.
- A source unit ends at its final object, not the next alignment boundary. The final float at `0x804BB108` is four bytes; its inferred eight-byte symbol included padding. Resizing it and ending sdata2 at `0x804BB10C` gives 100% section comparison and preserves the exact DOL. Taking the padding into the split gave only 90.909% for sdata2 even though the complete link matched.
- The two Draw overloads need different semantics: the configured float is font height, and viewport bounds convert left/top/right/bottom in their native order. F-453-1 records the font-height callee evidence.
