# Issue 489: RVL_SDK MEM, CX, and DSP units

## F-489-1: Unit boundaries in 0x802E18F0–0x802E4630

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). Evidence from the dtk 1.8.4 split assembly (`build/wpse-v1/asm/auto_03_802E18F0_text.s`), callers elsewhere in the executable, the referenced data, the CC0 symbol maps in `riptl/wii-symbols` (`RMKP01`: identical sizes for `DSPCheckMailToDSP` through `__DSP_insert_task`, `CXGetUncompressedSize`, `CXiVerifyHuffmanTable_` `0x1B0`, `CXiLHVerifyTable` `0x1F4`, `RCAddCount_` `0x1E8`, `RCSearch_` `0x94`), and the function order of the CC0 `xbret/xenoblade` CX stubs. Tools: Wii/1.0 `mwcceppc.exe` with `cflags_sdk`, binutils 2.42, objdiff-cli 3.8.2.

| Unit | `.text` | Other sections | Identifying evidence |
| --- | --- | --- | --- |
| `mem_frameHeap.c` | `0x802E18F0`–`0x802E1B70` | none | `'FRMH'` signature passed to `MEMiInitHeapHead`; calls to `MEMiFinalizeHeap`, `memset` |
| `mem_allocator.c` | `0x802E1B70`–`0x802E1BD0` | `.sdata2` `0x804BB4E8`–`0x804BB4F0` | the 8-byte function table holds the two expheap allocator callbacks |
| `mem_list.c` | `0x802E1BD0`–`0x802E1CF0` | none | named by #488 |
| `CXStreamingUncompression.c` | `0x802E1CF0`–`0x802E2100` | none | context initializer with tree at `+0x20`; reader calls `CXiVerifyHuffmanTable_` |
| `CXUncompression.c` | `0x802E2100`–`0x802E23F0` | none | 64-byte size reader; LZ and Huffman decoders without bounds checks |
| `CXSecureUncompression.c` | `0x802E23F0`–`0x802E3AE0` | none | the game dispatcher at `0x801A7A40` calls the RL, LZ, Huffman, diff, LH, and LRC entry points |
| `dsp.c` | `0x802E3AE0`–`0x802E3D40` | `.data` `0x803DC210`–`0x803DC290`; `.sdata` `0x804B8BE8`–`0x804B8BEC`; `.sbss` `0x804B9988`–`0x804B998C` | DSP banner, Feb 27 2009 10:01:57, `0x4302_145`; `DSPInit(): Build Date` string |
| `dsp_debug.c` | `0x802E3D40`–`0x802E3D90` | none | variadic empty stub called by `DSPInit` and `__DSP_boot_task` |
| `dsp_task.c` | `0x802E3D90`–`0x802E4628` | `.data` `0x803DC290`–`0x803DC3D0`; `.sbss` `0x804B9990`–`0x804B99A8` | `__DSP_boot_task()` strings; task list globals |

No MEM unit heap code is present. The link order is `Streaming`, `Uncompression`, `Secure`, not ASCII order. `CXSecureUncompressAny` and `CXUncompressAny` are absent. `dsp_task.c` runs past this block into #490's range; its last four functions are F-490-4's. This issue owns the whole file, so it also completes the remaining part of #490.

The `.sbss` order is the reverse of the definition order in `dsp_task.c`: `__DSP_rude_task_pending` `0x804B9990`, `__DSP_rude_task` `0x9994`, `__DSP_tmp_task` `0x9998`, `__DSP_last_task` `0x999C`, `__DSP_first_task` `0x99A0`, `__DSP_curr_task` `0x99A4`. The assignment comes from `DSPInit`'s chained store order and `DSPAssertTask`. Confidence high.

## F-489-2: Expression forms the compiler requires

- `DSPTaskInfo::state`, `priority`, and `flags` must be `vu32`. With plain `u32`, `DSPAddTask` and `__DSP_remove_task` reorder their two stores.
- The dead-stripped `DSPAssertInt` and `__DSP_add_task` must stay in the source. `DSPAssertTask` inlines the first, and the second contributes the `__DSP_add_task() : Added task` string to `dsp_task.c`'s `.data`.
- In `CXReadUncompHuffman`, the end-flag read must go through `*(vu8*)treep`, which yields the original's two loads of the node byte. Leaving the inner loop after the last word needs `goto`, so the jump lands on the trailing source-size check. This revision also handles a short final word: when `destCount <= destTmpCnt / 8` it shifts `destTmp` right by `32 - destTmpCnt` and forces the count to 32.
- In both Huffman decoders, the node byte must be copied into a local and shifted in place (`node = *treep; node <<= shift;`) before the mask is tested. Masking it in the initializer gives a different register assignment.
- In `CXSecureUncompressHuffman`, the type byte must be read through `*(const u32*)`. The size uses `*(u32*)`, and the original keeps the two loads apart.
- `CXSecureUnfilterDiff` compares a `u8` type byte, which gives the signed `cmpwi`.
- In `CXSecureUncompressLH`, the declaration order `destSize`, `destCnt`, `pSrc`, `huffTable12` gives the original assignment of `r31` down to `r28`, and `offset` must be declared before `offset_bit`. The bit reader is an inline `BitReader` that returns `-1` when `cnt > srcSize`.
- `RCSearch_` needs `__attribute__((noinline))`; otherwise it is inlined into the range decoder.

## F-489-3: CXSecureUncompressLRC remains 99.93%

Ten of the eleven `CXSecureUncompression.c` functions match. `CXSecureUncompressLRC` differs in three instructions only: the original keeps the byte counter of the first range-decoder renormalization in `r19`, the register later used for the match length, while the rebuild uses `r25` for both counters. These variants did not change the assignment: declaration order and scope of `val`, `length`, `offset`, and `readCnt`; a `u32` counter; the `inline` keyword versus auto-inlining; and passing the source pointer by address instead of returning a count. The unit stays `NonMatching` and links the original object. Full verification passes.

## F-489-4: Data and text extents

The compiled `dsp.c` `.sdata` is 4 bytes, and its `.sbss` is 4 bytes. Splitting the original 8-byte extents, with the symbols sized `0x8`, made dtk reject the split or shifted later small data by 4 bytes. The splits end at `0x804B8BEC` and `0x804B998C`, with `__DSPVersion` and `__DSP_init_flag` sized `0x4`, so the alignment padding stays in the automatic objects. `dsp_task.c` compiles `.data` `0x13B` bytes and `.text` ending at `0x802E4628`. Its splits end at `0x803DC3D0` and `0x802E4628`, which keeps the 16-byte data gap before NAND's banner at `0x803DC3E0` and the 8-byte text gap before `NANDCreate`. `lbl_803DC290` was resized to `0x140`.

## Traps

- **A failed dtk split leaves a stale `main.dol`.** When a split range ended inside a symbol, `ninja` failed at the SPLIT step but left the previous link in place. Comparing that DOL showed misleading early differences. Always check `ninja`'s exit status before reading DOL diffs. See F-489-4.
- **Dead-stripped SDK functions still own data and inlining.** Keep unlinked functions such as `__DSP_add_task` in the source when their strings or inlined bodies appear in the binary. See F-489-2.
