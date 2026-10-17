# F-475-1: GXFifo unit and initial reconstruction

Target `wpse-v1`, frozen `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

## Boundaries and names

The linked GXFifo translation unit owns `.text` `0x802CACE0–0x802CB6C0`, `.data` `0x803D60E0–0x803D6110`, `.bss` `0x80454200–0x80454280`, and `.sbss` `0x804B96C0–0x804B96E0`. The `.data` range contains its two overlapping-FIFO diagnostics. GPFifo occupies the first 36 BSS bytes and CPUFifo the next 36; the remaining 56 bytes precede the 64-byte-aligned DVD ticket-view buffer. This padding remains original fallback storage. The eight-byte dtk extent for CPGPLinked includes padding; its observed accesses are bytes.

| Address | Size | Original SDK name | Implemented |
| --- | --- | --- | --- |
| `0x802CACE0` | `0x160` | `GXCPInterruptHandler` | No |
| `0x802CAE40` | `0x8C` | `GXInitFifoBase` | Yes |
| `0x802CAED0` | `0xF8` | `CPGPLinkCheck` | Yes |
| `0x802CAFD0` | `0x1E0` | `GXSetCPUFifo` | No |
| `0x802CB1B0` | `0x290` | `GXSetGPFifo` | No |
| `0x802CB440` | `0x8` | `__GXIsGPFifoReady` | Yes |
| `0x802CB450` | `0x44` | `GXSetBreakPtCallback` | Yes |
| `0x802CB4A0` | `0x7C` | `__GXFifoInit` | Yes |
| `0x802CB520` | `0x1A0` | `__GXCleanGPFifo` | No |

Confidence is high: target instruction sequences, FIFO diagnostic strings, structure assignments, interrupt index 17/mask 0x4000, and reference function order agree. `GXSetBreakPtCallback` returns the prior callback and updates it with interrupts disabled; it is not `GXGetGPStatus`. The following function at `0x802CB6C0` begins GXAttr with attribute-switch code, establishing the unit boundary. The ten vector functions preceding GXInit remain with the earlier vector-unit owner; GXInit and GXAttr are outside this bounded submission.

Evidence was inspected with binutils 2.42-2 `powerpc-eabi-objdump -dr` on the baseline linked ELF and split GXFifo object, and by comparing Petari at `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/gx/GXFifo.c` and its RMGE01 splits/symbols. Runtime addresses in `.text` map to DOL file offsets by subtracting `0x8004FD40`.

## ABI and source status

`include/RVL_SDK/gx/GXFifo.h` records the 128-byte public opaque FIFO object, 36-byte internal FIFO state, byte-sized GXBool, and callback signature. Internal offsets: base 0, top 4, size 8, high/low watermarks 12/16, read/write pointers 20/24, signed count 28, wrap/CPU/GP flags 32/33/34. The target copies nine words and clears 36 bytes, corroborating the layout. No game class or shared OS header is introduced.

Five functions compile at 100% objdiff, totaling 588 of 2,492 function bytes. Both diagnostic strings also match. The unit remains `NonMatching` and links entirely from its original split object. Original globals are declared externally in the partial source; their definitions and four remaining functions must be reconstructed before source linking. `OSGetCurrentThread` corresponds to `0x802BBD90`, outside this claim, and must be named by its owning issue before source linking. No out-of-claim symbol is renamed here.

The implementation follows the CC0 Petari source at the revision above; only the relevant declarations are carried into the new header. No assembly fallback is added to source, and none of these functions counts as linked source progress.

## Validation and continuation

The baseline build, split-only build and verifier passed. After registering the partial source, `python configure.py`, `.venv/Scripts/ninja`, and the frozen-manifest verifier passed. objdiff-cli 3.8.2 `build/wpse-v1/report.json` reports five matching functions, 588 matched bytes, and complete=false for `main/RVL_SDK/gx/GXFifo`.

Next reconstruct the interrupt handler, CPU/GP FIFO binding and cleanup, plus complete original storage. Recover the GXData/CP/PI ABI from the owned neighboring GXInit block, coordinate the external OS name, and validate the complete unit before changing it to Matching. The broader issue remains open. No new generally reusable trap was established; the new evidence is the unit boundary, original names and FIFO ABI above.
