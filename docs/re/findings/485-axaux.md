# Issue 485: AX auxiliary audio buffers

## F-485-1: AXAux unit and reconstruction

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, frozen manifest `config/targets/wpse-v1.json`. Source follows the CC0-1.0 [ogws AXAux.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX/AXAux.c) and AXAux.h at commit `f5a1bb5ed7692a9e09e737bc1236f592274dcf73`. License checked through GitHub repository license metadata. Comments were removed; the established project integer types replace the reference type include.

The issue begins with `AXAcquireVoice` at `0x802DA7C0` and `AXSetVoicePriority` at `0x802DA950`, whose AXAlloc translation unit begins in #484. PR #553 initially left them as original fallbacks; F-485-3 completes their unit. The complete AXAux unit starts with `__AXAuxInit` at `0x802DA9F0` and ends after `AXGetAuxCCallback` at `0x802DB284`, followed by alignment padding and AXCL. Its 22 functions have 2,072 code bytes and occupy 2,196 bytes including alignment. `.text` maps to DOL file offsets `0x28ACB0`-`0x28B544`.

All unit data is uninitialized: `.bss` `0x804594A0`-`0x8045C620` contains two 0x1200-byte auxiliary buffers and one 0xD80-byte buffer, each aligned to 32 bytes; `.sbss` `0x804B9850`-`0x804B9898` contains ring positions, DSP pointers, contexts, callbacks and three 3-byte clear arrays with alignment padding. Initialization, buffer stride and channel-offset accesses prove these layouts. The three-entry ring has 96 32-bit samples per channel; auxiliary A/B have four channels and C has three. `AXAuxCallback` returns void and takes channel-buffer and context pointers. Original names are supported by the reference source and matching code; confidence high.

Wii/1.0 with existing `cflags_sdk` produces 22/22 functions at 100% in objdiff-cli 3.8.2, and all 12,744 data bytes match. PR #570 recovered the external `u32 __AXClMode` symbol at `0x804B98B0` in the neighboring AXCL unit; its independent evidence is [F-486-1](486-axcl.md). The source now links with AXAux marked `Matching` and the complete DOL passes frozen-manifest SHA-256 and direct byte verification. All 2,072 code bytes are linked from reconstructed source.

After resolving shared-file rebase conflicts while preserving both sides, `python configure.py --build-dir build/axaux-clean` and `.venv/Scripts/ninja.exe` exited 0 from a fresh output directory. `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/axaux-clean/wpse-v1` exited 0. The clean objdiff report confirms 22/22 functions, all data, and complete source linkage. The existing build reports a harmless force-active pooled-data warning; exact verification passes.

PR #553 was partial because the two AXAlloc functions needed #484 coordination. Its AXAux changes did not alter AXAlloc ranges. No additional reverse-engineering trap emerged during dependency integration; F-485-2 remains the reusable finding from this work.

## Traps

### F-485-2: Function pointer truth tests

The project C `NULL` macro expands to `((void*)0)`. With Wii/1.0, comparing an AX callback function pointer against this expression emits a zero-register setup and `cmplw`; the target uses `cmpwi`. In `__AXGetAuxAInput`, the reference spelling produced 0x34 bytes with a different comparison and shared zero store. In `__AXProcessAux`, it also enlarged the function from target 0x3AC to 0x3B8. Testing the function pointer directly (`if (__AXCallbackAuxA)`) and negating it for the zero case produces all target instructions without changing the shared NULL definition. The final 22-function objdiff report confirms the fix; this observation is specific to these Wii/1.0 callback comparisons.

The final integration extends the imported Petari `include/revolution/ax.h` in place with the verified auxiliary API and channel definitions from the cited ogws header. The existing callback typedef is reused. OS/cache and memory declarations come from the imported shared headers; no duplicate auxiliary header remains. This follows D-0013.

## F-485-3: AXAlloc completes issue 485

The user authorized the minimum prerequisite unit integration needed to finish the original issue. Issue #484 was open and unassigned when #485 was reclaimed. AXAlloc starts at `0x802DA500` in #484 and ends at `0x802DA9EC`, covering the two remaining #485 functions. This integration leaves the other #484 units unfinished.

Source follows CC0-1.0 [ogws AXAlloc.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX/AXAlloc.c). Existing imported `AXVPB` and callback types are reused. The allocation API and priority constants extend `include/revolution/ax.h`. Target accesses confirm next/previous/callback-stack pointers at offsets 0/4/8, priority at 0xC, callback at 0x10, context at 0x14, sync at 0x1C, depop at 0x20 and parameter block at 0x28. Priority zero is the free list and priorities 1 through 31 are active voices.

The nine retained functions are `__AXGetStackHead` (0x802DA500, 0x14), `__AXServiceCallbackStack` (0x802DA520, 0xAC), `__AXAllocInit` (0x802DA5D0, 0xB0), `__AXPushFreeStack` (0x802DA680, 0x1C), `__AXPushCallbackStack` (0x802DA6A0, 0x10), `__AXRemoveFromStack` (0x802DA6B0, 0x84), `AXFreeVoice` (0x802DA740, 0x7C), `AXAcquireVoice` (0x802DA7C0, 0x184), and `AXSetVoicePriority` (0x802DA950, 0x9C). Reference helper functions inline naturally. Original names have high confidence from identical reference behavior and target comparison.

The 32-entry pointer arrays `__AXStackHead` and `__AXStackTail` occupy `.bss` 0x804593A0-0x80459420 and 0x80459420-0x804594A0. The callback-list pointer occupies `.sbss` 0x804B9848-0x804B984C; the following four bytes are alignment padding. The external `__AXSetPBDefault` at 0x802DD260 is named from reference call sites and its parameter-block reset behavior; its size, type and split remain unchanged.

Split-only verification passed first. Wii/1.0 with existing `cflags_sdk` then matches all nine functions (1,212 code bytes) and all 260 data bytes in objdiff. With AXAlloc marked `Matching`, `python configure.py`, `.venv/Scripts/ninja.exe`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit 0. The source-linked DOL passes the frozen digest and direct byte comparison. Together with AXAux, all 24 functions and 2,616 code bytes assigned to #485 are reconstructed and source linked.

No new trap emerged. The callback truth-test workaround in F-485-2 also applies to this unit.
