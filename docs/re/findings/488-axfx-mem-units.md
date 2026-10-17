# Issue 488: AXFX and MEM translation units

## F-488-1: Unit boundaries in 0x802DF190–0x802E18F0

Target `wpse-v1`, `orig/wpse-v1/main.dol` (manifest `config/targets/wpse-v1.json`). The issue listed only `mem_heapCommon` and `mem_expHeap` as reference matches, but its block mostly holds AXFX code. Evidence came from the dtk split assembly (`build/wpse-v1/asm/auto_03_802ABD4C_text.s`), the data at the referenced addresses, and the reconstruction itself. Tools: dtk 1.8.4, GNU objdump 2.42, Wii/1.0 `mwcceppc.exe`, objdiff-cli 3.8.2.

| Unit | `.text` | Other sections | Identifying evidence |
| --- | --- | --- | --- |
| `AXFXReverbStdExp.c` | `0x802DF190`–`0x802DFEC0` | `.data` `0x803DB780`–`0x803DB810`; `.sdata2` `0x804BB478`–`0x804BB4B8` | `__EarlySizeTable` `{163, 317, …, 1283}` and `__FilterSizeTable` at the referenced `.data`; `active` at `0xB0`; `32000.0f`; first function computes the memory size |
| `AXFXChorusExp.c` | `0x802DFEC0`–`0x802E09E0` | `.sdata2` `0x804BB4B8`–`0x804BB4E8` | `AXFXChorusExpGetMemSize` returns `0x9600` (3 × 3200 × 4); delay size `0xC80`; `active` at `0x7C` |
| `AXFXLfoTable.c` | `0x802E09E0`–`0x802E09F0` | `.data` `0x803DB810`–`0x803DBA10` | 128-entry fixed-point sine table |
| `AXFXSrcCoef.c` | `0x802E09F0`–`0x802E0A10` | `.data` `0x803DBA10`–`0x803DC210` | 512-float 12 kHz resampling table |
| `AXFXHooks.c` | `0x802E0A10`–`0x802E0A60` | `.sdata` `0x804B8BE0`–`0x804B8BE8` | default hooks call `OSAllocFromHeap`/`OSFreeToHeap` with `__OSCurrHeap`; setter and getter of the hook pair |
| `mem_heapCommon.c` | `0x802E0A60`–`0x802E1010` | `.bss` `0x8046ECA0`–`0x8046ECC8`; `.sbss` `0x804B9980`–`0x804B9988` | recursive `FindContainHeap_`; root list, mutex, and initialized flag |
| `mem_expHeap.c` | `0x802E1010`–`0x802E18F0` | none | `'EXPH'` signature; `'FR'`/`'UD'` block signatures |

The function before `0x802DF190` belongs to a different effect structure (fields up to `0x54`), so `AXFXReverbStdExp.c` starts in this block. `AXFXChorusExpSettings` is absent from the binary; the `0x1B0` function at `0x802E0130` reinitializes the delay without calling shutdown and init, so it is `AXFXChorusExpSettingsUpdate`. The non-Exp wrappers (`AXFXReverbStd`, `AXFXChorus`) are not in this region.

`mem_expHeap.c` ends with `MEMSetGroupIDForExpHeap` at `0x802E18A0`, which issue #489 lists. This issue owns the file because its first function is here, so the whole file was reconstructed. `0x802E18F0` begins the frame heap (`'FRMH'`), followed by the allocator at `0x802E1B70` and `mem_list.c` at `0x802E1BD0`.

The original `.bss` extent `0x8046ECA0`–`0x8046ECE0` includes 0x18 bytes of padding before `s_homeDir`. Splitting it whole made the rebuilt `.bss` 0x28 bytes, shifted `.sdata` by `-0x20`, and failed verification at DOL header byte `0x7F`. Ending the split at `0x8046ECC8` leaves the padding in the following automatic object and restores the original layout.

Confidence: high for every boundary; each unit is `Matching` and the full build verifies.

## F-488-2: Expression forms the original compiler requires

Observed while matching with Wii/1.0 and the `cflags_sdk` flags in `configure.py`:

- `mem_expHeap.c` is a newer SDK revision than Petari's. `AllocUsedBlockFromFreeBlock_` also keeps the alignment margin as a free block only when `useMarginOfAlign` is set, checked against the allocation direction. `RemoveMBlock_`, `InsertMBlock_`, and `InitMBlock_` are inlined. `InitMBlock_` must take a non-const `MemRegion*`, or `RecycleRegion_` reloads the region from the stack. In `MEMFreeToExpHeap`, the handle and block pointers must be computed after `LockHeap`.
- `AllocFromHead_` and `AllocFromTail_` test `pMBlkHdFound != NULL` and return the allocation inside the branch.
- `AXFXChorusExpInit` assigns `active = 1`, like `AXFXReverbStdExpInit`, rather than or-ing the flag.
- In the chorus `__InitParams`, `(32000.0f / rate) / 256.0f` reproduces `fmuls f7, f1, f0`. The compiler turns the division into a multiply by `0.00390625f`. Writing the multiply by `0.00390625f` directly, in either operand order, gives `fmuls f7, f0, f1`.
- `powf` is the MSL inline wrapper around `pow`, which is the 4-byte branch stub at `0x8034D548`.

## Symbols

Names given in `config/wpse-v1/symbols.txt`, all `Matching` and verified:

| Address | Name | Scope | Evidence |
| --- | --- | --- | --- |
| `0x802DF190`–`0x802DFC10` | `AXFXReverbStdExpGetMemSize`, `Init`, `Settings`, `SettingsUpdate`, `Shutdown`, `Callback`, `__AllocDelayLine`, `__BzeroDelayLines`, `__FreeDelayLine`, `__InitParams` | global, last four local | SDK names from the CC0 wii-ipl source; byte match |
| `0x802DFEC0`–`0x802E08D0` | `AXFXChorusExpGetMemSize`, `Init`, `SettingsUpdate`, `Shutdown`, `Callback`, `__InitParams`, `__CalcLFO` | global, last two local | as above |
| `0x802E09E0`, `0x802E09F0` | `__AXFXGetLfoSinTable`, `__AXFXGetSrcCoef` | global | as above |
| `0x802E0A10`–`0x802E0A40` | `__AXFXAllocFunction`, `__AXFXFreeFunction`, `AXFXSetHooks`, `AXFXGetHooks` | first two local | Petari source |
| `0x802E0A60`–`0x802E0ED0` | `FindContainHeap_`, `MEMiInitHeapHead`, `MEMiFinalizeHeap`, `MEMFindContainHeap` | first local | Petari source |
| `0x802E1010`–`0x802E18A0` | `AllocUsedBlockFromFreeBlock_`, `AllocFromHead_`, `AllocFromTail_`, `RecycleRegion_`, `MEMCreateExpHeapEx`, `MEMDestroyExpHeap`, `MEMAllocFromExpHeapEx`, `MEMFreeToExpHeap`, `MEMGetAllocatableSizeForExpHeapEx`, `MEMSetGroupIDForExpHeap` | first four local | Petari source |
| `0x803DB780`, `0x803DB7A0`, `0x803DB810`, `0x803DBA10` | `__EarlySizeTable`, `__FilterSizeTable`, `__SinTableFixedPoint`, `__SrcTab12khz` | local | data match |
| `0x804B8BE0`, `0x804B8BE4` | `__AXFXAlloc`, `__AXFXFree` | global | data match |
| `0x8046ECA0`, `0x8046ECB0`, `0x804B9980` | `sRootList` (0xC), `sRootMutex` (0x18), `sRootListInitialized` (0x4) | local | link layout |

External symbols outside this block were named because the reconstructed objects reference them by name: `OSAllocFromHeap` `0x802B38C0`, `OSFreeToHeap` `0x802B39C0`, `OSInitMutex` `0x802B9F00`, `OSLockMutex` `0x802B9F40`, `OSUnlockMutex` `0x802BA020`, `__cvt_fp2unsigned` `0x8034033C`, `pow` `0x8034D548`, `__OSCurrHeap` `0x804B8A58`, and the `mem_list.c` functions `MEMInitList` `0x802E1BD0`, `MEMAppendListObject` `0x802E1BF0`, `MEMRemoveListObject` `0x802E1C60`, `MEMGetNextListObject` `0x802E1CD0`. The names come from call patterns that match the SDK sources and from the link: a wrong address would change the output bytes. Confidence: high.

Layouts: `MEMiHeapHead` is 0x3C bytes, with `childList` at `0xC`, `heapStart` at `0x18`, `heapEnd` at `0x1C`, `mutex` at `0x20`, and `attribute` at `0x38`. `MEMiExpHeapHead` follows it, with `groupID` at `0x10` and `feature` at `0x12`. `AXFX_REVERBSTD_EXP` is 0xE8 bytes, with `active` at `0xB0`. `AXFX_CHORUS_EXP` is 0xA0 bytes, with `active` at `0x7C`. All are declared in `include/revolution/`.
