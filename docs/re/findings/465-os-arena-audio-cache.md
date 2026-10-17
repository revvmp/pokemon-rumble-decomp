# Issue 465: OS arena, audio initialization, and cache

## F-465-1: Unit boundaries and recovered names

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, manifest `config/targets/wpse-v1.json`. Evidence uses dtk 1.8.4 split assembly, GNU binutils 2.42 object disassembly, Wii/1.0 compilation with existing `cflags_sdk`, and objdiff-cli 3.8.2.

| Unit | Text | Other sections | Functions |
| --- | --- | --- | --- |
| OSArena | `0x802B3A40`-`0x802B3B60` | `.sdata` `0x804B8A60`-`0x804B8A68`; `.sbss` `0x804B9498`-`0x804B94A0` | 14 |
| OSAudioSystem | `0x802B3B60`-`0x802B4020` | `.data` `0x803C7770`-`0x803C77F0` | 3 |
| OSCache | `0x802B4020`-`0x802B46C8` | `.data` `0x803C77F0`-`0x803C7A28` | 20 |

Arena's four pointers are established by every getter/setter relocation. The high-end allocator at `0x802B3B40` rounds the high pointer down, subtracts the requested size, rounds down again, stores the new high pointer, and returns it. Its name `OSAllocFromMEM1ArenaHi` follows the SDK API and its low-end counterpart. It is absent from the pinned Petari source and was reconstructed from these operations. Separating the initial getter assignment from the rounding expression reproduces the observed register allocation; combining them in the initializer produced equivalent operations with different registers.

Cache function order and instructions establish `DCStoreRange`, `DCStoreRangeNoSync`, `__LCEnable`, `LCEnable`, `LCLoadBlocks`, `LCStoreBlocks`, `LCStoreData`, `LCQueueLength`, and `LCQueueWait`. The load-block function sets DMA direction/start bits `0x12`, versus store-block bits `0x2`; queue length extracts HID2 bits using `rlwinm ...,8,28,31`. These two functions are absent from the pinned Petari implementation and were reconstructed using its neighboring register-oriented routines. No functions cross into neighboring issues.

Source provenance: Petari CC0, commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/os/OSArena.c`, `OSAudioSystem.c`, `OSCache.c`, and relevant `libs/RVL_SDK/include/revolution/os/` headers. Comments were removed. Unit headers include used interfaces. After #561 merged, this contribution reuses its complete shared `OSContext` header unchanged; its fields and `OSDumpContext` declaration agree with the independently prepared header. Cache hardware access retains the reference's handwritten CodeWarrior assembly; it is disclosed low-level assembly, not inferred high-level C. Audio retains the reference's 128-byte DSP bootstrap table; no DSP source reconstruction is claimed.

## F-465-2: Cache terminal padding and pooled data

An initial cache text range ending at `0x802B46D0` included eight terminal zero bytes. Although all 20 functions scored 100%, source linking put the following `__OSLoadFPUContext` at `0x802B46C8` instead of `0x802B46D0`, shifted subsequent text by eight bytes, and failed verification first at DOL header byte `0x6F`. The compiler emits cache text length `0x6A8`, ending at `0x802B46C8`. Preserve the following eight-byte alignment gap outside this source unit; do not absorb it without also establishing the next unit's alignment.

The cache strings occupy compiler `.data` size `0x232` plus linker alignment up to `0x238`. Original dtk data labels split the pool into `@878` and an unnamed remainder, whereas the compiler emits `...data.0`, `error`, and individual string labels. Objdiff therefore reports no matched cache data symbols even when offsets and bytes agree. Whole-artifact verification is the acceptance gate; data-symbol matching is separately disclosed.

## Validation

All 37 functions (2,980 code bytes excluding padding) match at 100% and all three units link from source. `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit 0. Verification directly compares the full DOL against the unchanged frozen manifest. The preserved eight-byte gap becomes an additional objdiff padding entry, increasing the report denominator by eight bytes and one entry; it is not a newly recovered function. No original hashes changed. No unit tests were added.

## Traps

A source split can swallow required terminal padding. OSCache scored 100% but linking moved the next unit back eight bytes when its split included terminal alignment bytes. End its split at the compiler text extent and preserve the gap until the next unit alignment is established; see F-465-2 above.
