# MetroTRK communication and circular-buffer units

## F-541-1: GDEV, critical sections and circular buffers

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. Issue #541 contains 32 functions and 2,880 function bytes. This pass reconstructs 17 functions and 1,264 function bytes in three complete C translation units. The preceding MSL wrappers and C++ reference-count release routines, four trap entry points, cache flushing, main loop, polling, dispatch and startup remain outside this delivery. The issue remains incomplete.

| Unit under `MetroTRK/gamedev/cust_connection/` | Text extent | Retained functions / bytes | Other sections |
| --- | --- | --- | --- |
| `cc/exi2/GCN/EXI2_GDEV_GCN/main.c` | `0x8034D914`–`0x8034DB8C` | 10 / 632 | BSS `0x804B3550`–`0x804B3A6C`; SBSS `0x804B9BF8`–`0x804B9BFC` |
| `utils/gc/MWCriticalSection_gc.c` | `0x8034DB8C`–`0x8034DBC8` | 3 / 60 | None |
| `utils/common/CircleBuffer.c` | `0x8034DBC8`–`0x8034DE04` | 4 / 572 | None |

The source and three public headers adapt [Petari at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe/src/MetroTRK/gamedev/cust_connection). The consulted files have no file-specific copyright or license exception; the root license is CC0-1.0. Comments are removed. The unretained `OutputData` debugging loop is omitted. The implementation uses the existing `NdevExi2A/DebuggerDriver.h` interfaces rather than redeclaring those SDK functions.

## Function inventory

| Address | Extent | Recovered name |
| --- | --- | --- |
| `0x8034D914` | `0x3C` | `gdev_cc_initialize` |
| `0x8034D950` | `0x8` | `gdev_cc_shutdown` |
| `0x8034D958` | `0x24` | `gdev_cc_open` |
| `0x8034D97C` | `0x8` | `gdev_cc_close` |
| `0x8034D984` | `0xB4` | `gdev_cc_read` |
| `0x8034DA38` | `0x74` | `gdev_cc_write` |
| `0x8034DAAC` | `0x24` | `gdev_cc_pre_continue` |
| `0x8034DAD0` | `0x24` | `gdev_cc_post_stop` |
| `0x8034DAF4` | `0x74` | `gdev_cc_peek` |
| `0x8034DB68` | `0x24` | `gdev_cc_initinterrupts` |
| `0x8034DB8C` | `0x4` | `MWInitializeCriticalSection` |
| `0x8034DB90` | `0x30` | `MWEnterCriticalSection` |
| `0x8034DBC0` | `0x8` | `MWExitCriticalSection` |
| `0x8034DBC8` | `0x8` | `CBGetBytesAvailableForRead` |
| `0x8034DBD0` | `0x24` | `CircleBufferInitialize` |
| `0x8034DBF4` | `0x108` | `CircleBufferWriteBytes` |
| `0x8034DCFC` | `0x108` | `CircleBufferReadBytes` |

The seven utility names follow reference function order, exact native extents, paired call targets and target field accesses. The ten GDEV names were already present in target metadata and are corroborated by the same comparisons. No unrelated function split or type changes.

## Compiler and ordering evidence

Direct probes use the pinned compilers, dtk 1.8.4, objdiff-cli 3.8.2 and the actual target DOL. Wii/1.0 and Wii/1.1 reproduce every retained non-relocation instruction stream with four-byte function alignment. This does not distinguish the two versions. Wii/1.1 is selected consistently with the neighboring runtime, using measured MetroTRK settings: ordinary automatic inlining, read-only string reuse, load/store-multiple enabled, SDA/SDA2 disabled, and GDEV's own eight-byte SDA limit. No game or SDK-wide flags change.

GC/2.7 misses GDEV open/read/peek and two critical-section functions; its circular-buffer initializer is `0x50` rather than `0x24` bytes. GC/3.0a3 misses one word in GDEV open and two words in each circular-buffer transfer routine. Comparable-project flags are not evidence that those compilers built this target.

Petari's deferred-inlining setting produces identical per-function bodies but reverses their emitted order. All three units score 100% in objdiff while the final DOL differs in 1,077 bytes. Ordinary automatic inlining restores all 17 target function offsets. A second link exposed different static-storage placement: with both globals tentative, the compiler puts the control block before the receive buffer, growing BSS by four bytes and shifting later globals. Merely reversing their declarations or enabling file IPA does not fix it. Explicit zero initialization of the receive buffer emits it before the tentative control block and reproduces the native `0x51C`-byte BSS extent without adding padding objects.

## Proven interfaces and layouts

`gRecvBuf` is the `0x500`-byte receive buffer at `0x804B3550`. `gRecvCB` starts at `0x804B3A50` and occupies `0x1C` bytes. The survey's `0x20` extent included four bytes of alignment before `gDBCommTable` at `0x804B3A70`. `gIsInitialized` is a four-byte flag at `0x804B9BF8`; the survey's eight-byte extent included alignment before `TRK_mainError` at `0x804B9C00`. The splits end at the objects' actual ends.

| `CircleBuffer` offset | Member | Storage |
| --- | --- | --- |
| `0x00` | `read_ptr` | `u8*` |
| `0x04` | `write_ptr` | `u8*` |
| `0x08` | `start_ptr` | `u8*` |
| `0x0C` | `size` | `u32` |
| `0x10` | `mBytesToRead` | `s32` |
| `0x14` | `mBytesToWrite` | `u32` |
| `0x18` | `mCriticalSection` | `unsigned int` |

The reference's `u32` critical-section member conflicts with its `unsigned int*` helper interface under this project's types, where `u32` is `unsigned long`. The member uses the helper's exact unsigned-int storage type; both are four bytes, and native word accesses and the complete structure size are unchanged. `MWEnterCriticalSection` stores the previous interrupt state and `MWExitCriticalSection` restores it through existing OS declarations. Their original no-op initializer is retained.

The GDEV callback uses the established `__OSInterruptHandler` type consumed by `DBInitComm`, including its channel/context arguments. This removes the reference's incompatible function-pointer-to-`int*` cast. The flag-output parameter remains the original opaque pointer interface. No existing shared SDK header is changed.

## Validation

Baseline, split-only and source-selected standard configure/build/frozen-verification workflows pass. All 17 source functions have native extents, source-object offsets and final ELF addresses. Every compared section in all three units scores 100%, and all 340 pre-existing source objects remain byte-identical. The three units are selected from source as `Matching`.

`python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit zero. The final DOL is 4,094,880 bytes, matches the frozen reference byte for byte and retains SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The final symbols also place the receive buffer, control block and initialized flag at their native addresses and extents. No original, hash, manifest or maintenance-only file changes.

## Remaining work

Fifteen #541 functions and 1,616 function bytes still require acceptance. Preserve the original fallback for these functions, identify the C++ class before reconstructing its release methods, and establish whole source-unit boundaries for MetroTRK startup and polling across #542. This partial delivery does not close #541 or claim reconstruction of the complete debugger.

## Traps

- **Function identity does not prove native order.** Deferred inlining can emit matching bodies in reverse order. Check source-object offsets and final ELF addresses in addition to objdiff scores.
- **A 100% BSS score does not prove global placement.** Both layouts contain zero bytes. Check symbol offsets, section extent and final relocations; GDEV needs its buffer before its control block.
- **The custom build path does not rewrite the frozen check list.** `build.sha1` names `build/wpse-v1/main.dol`. A custom-directory build failed its check because that path was absent. Use the standard entry point and an explicit frozen verification of its actual output; do not modify expected hashes or accept a stale default-path artifact.
