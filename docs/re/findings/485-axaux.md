# Issue 485: AX auxiliary audio buffers

## F-485-1: AXAux unit and reconstruction

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, frozen manifest `config/targets/wpse-v1.json`. Source follows the CC0-1.0 [ogws AXAux.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX/AXAux.c) and AXAux.h at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73`. License checked through GitHub repository license metadata. Comments were removed; the established project integer types replace the reference type include.

The issue begins with `AXAcquireVoice` at `0x802DA7C0` and `AXSetVoicePriority` at `0x802DA950`, whose AXAlloc translation unit begins in #484. They remain untouched original fallbacks pending the owning issue. The complete AXAux unit starts with `__AXAuxInit` at `0x802DA9F0` and ends after `AXGetAuxCCallback` at `0x802DB284`, followed by alignment padding and AXCL. Its 22 functions have 2,072 code bytes and occupy 2,196 bytes including alignment. `.text` maps to DOL file offsets `0x28ACB0`-`0x28B544`.

All unit data is uninitialized: `.bss` `0x804594A0`-`0x8045C620` contains two 0x1200-byte auxiliary buffers and one 0xD80-byte buffer, each aligned to 32 bytes; `.sbss` `0x804B9850`-`0x804B9898` contains ring positions, DSP pointers, contexts, callbacks and three 3-byte clear arrays with alignment padding. Initialization, buffer stride and channel-offset accesses prove these layouts. The three-entry ring has 96 32-bit samples per channel; auxiliary A/B have four channels and C has three. `AXAuxCallback` returns void and takes channel-buffer and context pointers. Original names are supported by the reference source and matching code; confidence high.

Wii/1.0 with existing `cflags_sdk` produces 22/22 functions at 100% in objdiff-cli 3.8.2, and all 12,744 data bytes match. PR #570 recovered the external `u32 __AXClMode` symbol at `0x804B98B0` in the neighboring AXCL unit; its independent evidence is [F-486-1](486-axcl.md). The source now links with AXAux marked `Matching` and the complete DOL passes frozen-manifest SHA-256 and direct byte verification. All 2,072 code bytes are linked from reconstructed source.

After resolving shared-file rebase conflicts while preserving both sides, `python configure.py --build-dir build/axaux-clean` and `.venv/Scripts/ninja.exe` exited 0 from a fresh output directory. `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/axaux-clean/wpse-v1` exited 0. The clean objdiff report confirms 22/22 functions, all data, and complete source linkage. The existing build reports a harmless force-active pooled-data warning; exact verification passes.

Issue #485 remains partial because the two AXAlloc functions need #484 coordination. No AXAlloc ranges were changed. No additional reverse-engineering trap emerged during dependency integration; F-485-2 remains the reusable finding from this work.

## Traps

### F-485-2: Function pointer truth tests

The project C `NULL` macro expands to `((void*)0)`. With Wii/1.0, comparing an AX callback function pointer against this expression emits a zero-register setup and `cmplw`; the target uses `cmpwi`. In `__AXGetAuxAInput`, the reference spelling produced 0x34 bytes with a different comparison and shared zero store. In `__AXProcessAux`, it also enlarged the function from target 0x3AC to 0x3B8. Testing the function pointer directly (`if (__AXCallbackAuxA)`) and negating it for the zero case produces all target instructions without changing the shared NULL definition. The final 22-function objdiff report confirms the fix; this observation is specific to these Wii/1.0 callback comparisons.

The final integration extends the imported Petari `include/revolution/ax.h` in place with the verified auxiliary API and channel definitions from the cited ogws header. The existing callback typedef is reused. OS/cache and memory declarations come from the imported shared headers; no duplicate auxiliary header remains. This follows D-0013.
