# F-443-4: Complete WaveSound source

Target `wpse-v1`. The complete owned WaveSound unit retains fifteen functions / 864 native bytes at `0x80291420`-`0x802917DC`, including nine original #443 entries and six #444 tail entries. It uses the complete canonical WsdPlayer header from F-444-3 and the reference SoundInstanceManager template. Source follows CC0 zeldaret/ss `57a709b2128eb163a18e2969d561c9abb7d1fcee`, whose file identifies kiwi515/ogws provenance, adapted to the existing canonical APIs and actual target offsets.

## Layout and storage

WaveSound derives from BasicSound. Its embedded WsdPlayer starts at 0x110 and occupies 0x118 bytes; temporary WaveSoundHandle pointer is at 0x228, SoundInstanceManager pointer at 0x22C and prepared flag at 0x230, giving class size 0x234. Original constructor and native typed accesses establish every offset. SoundInstanceManager contains its four-byte MemoryPool, twelve-byte priority list and 24-byte OSMutex, size 0x28; the target list node offset is 0xF0. Its instantiated Free and UpdatePriority behavior matches the original caller instructions.

The 0x38-byte original WaveSound virtual table is at `0x803C68C8`-`0x803C6900`. Pairing its original entries with actual source relocations establishes each repeated short virtual method and preserves inherited BasicSound methods. RuntimeTypeInfo is four bytes at `0x804B93B0`; its prior survey size included padding. Typed original initialization operands establish both its destination and its BasicSound parent at `0x804B9348`. The sole source initializer pointer at `0x80352A68`-`0x80352A6C` points to `__sinit_\snd_WaveSound_cpp` at `0x802917D0`.

This file first retains the weak WsdPlayer destructor at `0x80291490`, before the separate WsdPlayer unit. Its complete native body and final address match; later weak duplicates resolve to it normally.

## Acceptance

After combining the owned reader and player source checkpoints, a fresh default 466-step build with only the WaveSound split added exits 0, followed by explicit frozen verification. NonMatching all_source compilation uses unchanged Wii/1.1 cflags_nw4r. All fifteen functions, .text, .ctors, .data and .sbss score 100% in objdiff. Normal Matching source selection passes full default Ninja and explicit actual-output frozen verification.

The independent audit checks fifteen native extents / 864 bytes, all non-relocated instruction words, all 26 paired code relocations, all thirteen paired vtable/constructor-pointer relocations, all 56 raw vtable bytes, four-byte type-info native extent, every final function address and both final data addresses. Actual source selection excludes the original WaveSound object. Full rebuilt DOL directly equals 4,094,880 frozen reference bytes, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. No original executable bytes implement source data.

The combined PR retains separate original #443/#444 inventory audits and counts source bytes once. It receives its final exact committed-head latest-main gate after the complete Util dependency merges. Scope covers only main.dol, excluding boot loader, other contents and WAD packaging.

## Traps

- A reference class's embedded player size determines the later handle/manager offsets. Use the target's full 0x118-byte player and constructor stores; stale reference offset comments are not evidence.
- A source-only virtual destructor can still be the first retained weak copy in another file. Preserve the original address and compare its real native body in the retaining file.
