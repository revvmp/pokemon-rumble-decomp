# Issue 36: Boot loader role and reconstruction scope

## F-36-1: The boot program loads content 1 by fixed index and starts it as a DOL

Target `wpse-v1`, artifact `orig/wpse-v1/boot.dol` (SHA-256 `c66a2944…09dd`, TMD content 6, sections in [inventory](../INVENTORY.md#boot-executable-sections)).

Procedure. The D0 image (file `0x500`–`0x4C000`) was disassembled with `powerpc-eabi-objdump -D -b binary -m powerpc:750 -EB --adjust-vma=0x81330000` (GNU binutils 2.42). A Python 3.14 script resolved `lis`/`addi` pairs to find the code that uses the strings `ESP_OpenContentFile failed: %d` and its neighbours at `0x8135BF00`–`0x8135C0D0` (file `0x2C400`–`0x2C5D0`). The callees were named by comparing them with `orig/wpse-v1/main.dol` functions whose names are in `config/wpse-v1/symbols.txt`, with relocation-bearing immediates masked (the method of F-0007). `ESP_InitLib`, `ESP_GetTitleId`, `ESP_OpenContentFile`, `ESP_ReadContentFile`, `ESP_SeekContentFile`, `ESP_CloseContentFile` and `ESP_CloseLib` match at `0x81350F30`–`0x81351600`. Each call's failure path prints the error message for that function.

Observation. One state machine at `0x81330CA0`–`0x81331530` handles loading. Its state word is at `-0x7D20(r13)`, it dispatches through a jump table at `0x8135C0F0`, its states range from 0 to 13, and a state above 13 calls the panic routine with `nandboot.c` and line 939. In order, it:

1. Allocates 8 KB from the MEM1 arena, stores it at `0x800000F4`, initializes ESP and reads the title ID. A failure in either step sets state 13.
2. Calls `ESP_OpenContentFile` with the immediate `li r3,1` at `0x81331054`. The content index is the constant 1 and does not come from the TMD or a table.
3. Reads a 32-byte header with `ESP_ReadContentFile`. It gets the file size by seeking to the end (`whence` 2) and then seeks back to the start (`whence` 0).
4. Compares the size with `0x900000`. If the routine at `0x81350BB0` returns 1, a larger file prints `ERROR >>> Program file must be less than 0x%08x bytes.` and halts. Otherwise it prints the `WARNING` form and continues.
5. Masks the first header byte with `0xF0`. If the result is `0x10`, the loader takes the LZ path (state 7). Otherwise it reads the file raw (state 6). In the LZ path, a detached reader thread is created (OSCreateThread-shaped call at `0x8133A310` with a 4 KB stack, priority 15 and attribute 1). That thread reads the content in 16 KB `ESP_ReadContentFile` chunks (`0x81330BE0`), and the main loop decompresses the data with the routines at `0x8134DF40` and `0x8134DF80`, which the `CXReadUncompLZ failed: %d` message follows.
6. Copies the first 256 bytes of the result as a DOL header and clears BSS from header offsets `0xD8` and `0xDC`. It then copies 7 text sections (offset `0x00`, address `0x48`, size `0x90`) and 11 data sections (offsets `0x1C`, `0x64`, `0xAC`).
7. Closes the content file and ESP. Back in the caller at `0x81330B70`, it loads the header's entry word (offset `0xE0`) and jumps to it through `0x81330AC0`. That routine zeroes the general registers, then clears and flushes `0x20000` cache lines from `0x81330000`, which erases the loader before it branches to the entry point.

Content 1 begins with byte `0x11` (F-0003), so it takes the LZ path. Its decompressed form is `orig/wpse-v1/main.dol`, entry point `0x80004050`. This confirms the role that F-0009 proposed as a hypothesis: the boot program is a generic WiiWare NAND loader for content 1. Confidence high. The `0x81350BB0` predicate that turns the size warning into a fatal error was not identified. The boot program also contains `PRODUCTION MODE` and `DEVELOPMENT MODE` strings, but the predicate's link to them is unverified.

## F-36-2: Most of the boot program is RVL_SDK code shared with main.dol

Procedure. Every function in `config/wpse-v1/symbols.txt` within `0x802B1800`–`0x80340000` of size at least 32 bytes (1,859 functions) was searched for in the boot `.text` image, with `bl` targets and 16-bit displacements and immediates masked. Matches had to cover the whole function.

Observation. 450 functions totalling `0x1E618` bytes (124,440) occur in the boot program. That is about 71% of its `.text` (`0x2A8C4`, 174,276 bytes). Examples are PPCArch, OS arena allocation, the whole ESP block, IPC and `IOS_OpenAsync`. All eight RVL_SDK banners in the boot program carry the build tag `0x4302_145` dated Jun 22 2009, as the main executable's SDK does (F-0007). The boot program contains no `pokemon`, `rumble` or `WPSE` strings (F-0009). Its own code is limited to `nandboot.c` (about `0x81330000`–`0x81331560`), the CX LZ decompressor, the NAND boot info and system file handling, and the multilingual "system files are corrupted" screen.

Interpretation. The boot program is Nintendo-supplied loader code linked with the same SDK release, not Pokemon Rumble game code. Once the main executable's RVL_SDK units match, about seven tenths of the boot program could reuse their sources unchanged. The SDK size estimate counts some trailing alignment padding inside main.dol symbol sizes and should be treated as approximate.

## Scope recommendation

Recommendation: keep `boot.dol` out of scope for the current acceptance target. Reasons:

- It is a generic loader that opens content 1 by a fixed index and contains nothing specific to this title (F-36-1, F-0009).
- The project's verified output is `main.dol` (`config/targets/wpse-v1.json`). Rebuilding the boot program would need a second target manifest, a second dtk configuration and its own splits, which adds an executable without adding game code.
- Most of its code is RVL_SDK that the main executable already contains (F-36-2), so reconstructing it later costs little once the SDK work is done.

If a maintainer later puts it in scope, it should be a separate target (for example `wpse-v1-boot`) with its own manifest fingerprinted from `orig/wpse-v1/boot.dol`, reusing the matched SDK sources, and with `nandboot.c` and the CX decompressor as its only new units. Accepting or changing this recommendation is a maintainer decision for `docs/DECISIONS.md`. No new `kind:decomp` issues are opened, because no units are newly in scope.

## Traps

- **The boot executable's DOL entry is a physical address.** Its header entry is `0x00003400`, and T0 at `0x80003400` is a stub that jumps to the word at `0x800037FC` (`0x81330000`). To get matching addresses, disassemble D0 with VMA `0x81330000`, not the header entry. Evidence: F-0009 and F-36-1.
