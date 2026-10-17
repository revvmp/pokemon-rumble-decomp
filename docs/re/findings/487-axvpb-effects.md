# F-487-1: AX voice-parameter completion and effects

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`. The 25 functions and 7,732 instruction bytes assigned to #487 belong to five source units. AXVPB continues the already matching implementation fragment from #486; its first five functions and 2,224 bytes remain matched. The complete AXVPB boundary now includes its seven later functions and original storage, rather than treating the earlier implementation boundary as an original translation-unit end. Other source units start within #487. No original hashes, compiler manifest or progress images change.

## Source provenance and naming

AXVPB and AXProf use [ogws AX sources](https://github.com/doldecomp/ogws/tree/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/AX), pinned `f5a1bb5ed7692a9e09e737bc1236f592274dcf73`. Existing AXVPB function bodies from #486 are preserved; original static buffers and later routines follow that reference. The low-pass coefficient formula uses the explicit float casts and float pi value shown by [wii-ipl AXVPB.c](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/src/ax/AXVPB.c). Descriptive `filterTerm` and `pole` locals replace reference register-style temporary names.

High-reverb wrappers and processing use [Petari AXFX sources](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/axfx), pinned `e0ab141914921726a595573dc48d3f13d0922efe`, with the unlinked memory-size API and `powf` expression corroborated by [wii-ipl AXFXReverbHiExp.c](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/src/axfx/AXFXReverbHiExp.c). Retaining that genuine SDK API preserves the shared literal pool; it does not add a target function or source-coverage count, since the linker discards its unreferenced body. The similarly unreferenced AXVPB dump helper is not counted either. Complete object comparison and the frozen source-linked DOL verify this behavior.

The delay implementation is `AXFXDelayExp`, rather than the legacy per-channel integer-millisecond `AXFXDelay`. Its original public names and complete structure come from [wii-ipl AXFXDelay.h](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/include/revolution/axfx/AXFXDelay.h). The expanded delay function bodies are reconstructed as readable C from target control flow and arithmetic, following the SDK initialization/shutdown pattern of the consulted effect sources. Private `__InitParams` follows the established reference helper convention; that spelling is inferred, not embedded evidence. The unrelated DPL2 reference inspected from mscharged `582f61ba6779d423ef79bbb499aeb7f9f0fa5221` uses the legacy layout and is not copied.

All adopted reference repositories are CC0-1.0. Their root licenses were checked; consulted AX/AXFX source and header files have no additional file-level notices. Comments and decompilation helper annotations are removed. Existing imported `include/revolution/ax.h` and `axfx.h` are extended with the new interfaces/types, leaving existing shared types unchanged. There are no duplicate SDK header trees, source byte arrays containing executable instructions, unit tests, comments or docstrings.

## Exact boundaries

All ends are exclusive. Source-linked validation establishes each boundary.

| Unit | Section | Start | End |
| --- | --- | --- | --- |
| `AXVPB.c` | `.text` | `802DC9E0` | `802DD798` |
| `AXVPB.c` | `.sbss` | `804B9950` | `804B996C` |
| `AXVPB.c` | `.data` | `803D8578` | `803D8608` |
| `AXVPB.c` | `.bss` | `8045D5A0` | `8046ECA0` |
| `AXVPB.c` | `.sdata2` | `804BB3F8` | `804BB418` |
| `AXProf.c` | `.text` | `802DD7A0` | `802DD7E0` |
| `AXProf.c` | `.sbss` | `804B9970` | `804B9980` |
| `AXFXReverbHi.c` | `.text` | `802DD7E0` | `802DD884` |
| `AXFXReverbHi.c` | `.sdata2` | `804BB418` | `804BB420` |
| `AXFXReverbHiExp.c` | `.text` | `802DD890` | `802DE6B4` |
| `AXFXReverbHiExp.c` | `.sdata2` | `804BB420` | `804BB460` |
| `AXFXReverbHiExp.c` | `.data` | `803DB5E0` | `803DB780` |
| `AXFXDelayExp.c` | `.text` | `802DE6C0` | `802DF184` |
| `AXFXDelayExp.c` | `.sdata2` | `804BB460` | `804BB474` |

AXVPB owns three original arrays: 96 320-byte AXPB records, 96 64-byte ITD buffers and 96 360-byte AXVPB records, totaling 71,424 BSS bytes. The target initializer sets 96 voices and those three pointers, clears their extents and links the parameter records. The existing imported member layout is corroborated by the initialization loops and the earlier matching service routines. Mix-cycle tables contain 32 and four unsigned words. Seven small state objects occupy 28 bytes. AXPROFILE remains the imported 56-byte type, with six time values, voice count and alignment padding; its single retained getter advances a ring of caller-supplied profile records.

High-reverb data consists of an eight-by-three early-length table, eight-by-three coefficient table and seven-by-eight filter-length table. These 416 bytes and all float pools are byte-identical to the original.

## F-487-2: Expanded delay behavior and layout

`AXFX_DELAY_EXP` is 0x58 bytes. Three line pointers occupy 0/4/8, current position 0xC, length/max length 0x10/0x14, feedback gain 0x18, three previous low-pass outputs 0x1C, low-pass coefficient 0x28, output/send integer gains 0x2C/0x30, active flags 0x34, maximum/current delay floats 0x38/0x3C, feedback/low-pass floats 0x40/0x44, input/output bus pointers 0x48/0x4C, and output/send floats 0x50/0x54. These offsets match the pinned public header and every target access.

The memory query computes twelve bytes per stereo-channel sample triplet, with sample count obtained by truncating 32 times the maximum delay in milliseconds. Init rejects nonpositive maxima, allocates three signed-word lines, and clamps a positive sub-sample maximum to one sample. Settings frees and reinitializes; SettingsUpdate resets parameters without allocation. All error paths call shutdown and restore the previous interrupt state.

Parameter validation checks delay against its maximum, feedback in [0,1), and low-pass/output/send values in [0,1]. It clears the maximum allocated lengths, clamps the current sample length to at least one, calculates fixed-point gains at scale 128, limits the complementary low-pass coefficient to 0.95, and clears filter history. The callback processes 96 samples. It reads delayed samples, filters the current bus plus optional input bus, writes feedback into the circular lines, and scales delayed samples into the output bus and optional send bus. Every arithmetic and branch instruction matches the target; no float-to-int conversion helper is replaced with a fabricated result.

## Traps

- Matching instructions alone missed literal-pool differences in the filter routine. The explicit `(f32)cos` and `(f32)sqrt` source forms from wii-ipl preserve original literal order; inline wrappers produced the same non-relocation instructions but a different pool. No padding constants or forced literal objects are added.
- The high-reverb source must retain its genuine unlinked memory-size API so 32000.0f enters the shared pool before initialization's zero literal. `powf`, as in the pinned wii-ipl source, also preserves the original exponent/base literal order. The final linker discards the unused function body, and exact object/data comparison plus full DOL verification confirms the result.
- Incrementing the circular position separately after the last channel write adds a reload because the line pointer can alias the structure. The original-style `lineS[curPos++]` expression preserves the observed ordering. Function-level sample/filter locals also reproduce the target register allocation without register names or inline assembly.

## Source-linked validation

Baseline, split-only and source-linked builds exited 0 and passed frozen verification. `python configure.py --no-progress`, `.venv/Scripts/ninja all_source`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0. Five complete `objdiff-cli diff` comparisons exited 0 and report every code/data section at 100%.

| Unit | Retained functions | Instruction bytes |
| --- | --- | --- |
| `AXVPB.c` | 12 | 3,472 |
| `AXProf.c` | 1 | 64 |
| `AXFXReverbHi.c` | 3 | 140 |
| `AXFXReverbHiExp.c` | 7 | 3,556 |
| `AXFXDelayExp.c` | 7 | 2,724 |

AXVPB's first five functions and 2,224 bytes were already source linked under #486. The added seven AXVPB routines contribute 1,248 bytes. Thus new #487 coverage is exactly 25 functions and 7,732 bytes; all reconstructed units together cover 30 retained functions and 9,956 bytes. No fallback remains within these five units. The final rebased commit and repeated clean validation are recorded in the pull request. Frozen main game DOL SHA256 remains `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`; scope excludes boot content 6, data contents and WAD packaging.
