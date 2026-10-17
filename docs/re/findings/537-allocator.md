# MSL allocation pools

## F-537-2: Complete allocation unit and issue inventory

The ten remaining functions occupy `.text` `0x803422E0`–`0x803430E0`, exactly 3,584 bytes. Their source is reconstructed from [mscharged-decomp alloc.c at 734c3f65dcb5b59f01fda2cf4aaab8249c2da13f](https://github.com/yannicksuter/mscharged-decomp/blob/734c3f65dcb5b59f01fda2cf4aaab8249c2da13f/src/MSL/alloc.c), under its [CC0 license](https://github.com/yannicksuter/mscharged-decomp/blob/734c3f65dcb5b59f01fda2cf4aaab8249c2da13f/LICENSE). The source file contains no separate license notice. Its one code comment is omitted. Unused alternative-allocator declarations are omitted, and the used pool layouts are declared in `include/MSL_C/alloc.h`.

This supersedes the remaining-work section in [F-537-1](537-msl-io-exceptions.md). The newly available reference supplies all six bodies absent from the previously checked references. The function at `0x80342F10` is `malloc`; the earlier proposed `__pool_free` name was incorrect. The reference inlines `__pool_free` into the retained `free` routine.

| Address | Size | Function |
| --- | --- | --- |
| `0x803422E0` | `0x204` | `Block_subBlock` |
| `0x803424E4` | `0x150` | `Block_link` |
| `0x80342634` | `0xA8` | `SubBlock_merge_next` |
| `0x803426DC` | `0x240` | `allocate_from_var_pools` |
| `0x8034291C` | `0xDC` | `soft_allocate_from_var_pools` |
| `0x803429F8` | `0x124` | `FixBlock_construct` |
| `0x80342B1C` | `0x208` | `allocate_from_fixed_pools` |
| `0x80342D24` | `0x1EC` | `deallocate_from_fixed_pools` |
| `0x80342F10` | `0xA0` | `malloc` |
| `0x80342FB0` | `0x130` | `free` |

The normal Wii/1.1 `cflags_msl` compiler policy reproduces every non-relocation instruction word and every function extent without extra flags or assembly. Objdiff-cli 3.8.2 reports 100% for `.text`, `.rodata`, `.bss` and `.sbss`. The six fixed sizes, 4, 12, 20, 36, 52 and 68, occupy `.rodata` `0x80398390`–`0x803983A8`. The pool occupies `.bss` `0x804B31F8`–`0x804B3230`, and its one-byte initialization flag is at `0x804B9BC8`. The `.sbss` split extends to the next eight-byte boundary at `0x804B9BD0`; the flag's symbol size remains one byte. Ending the split immediately after the flag creates an automatic object at an invalid alignment.

The two external calls identify `__sys_alloc` at `0x801B1370` and `__sys_free` at `0x801B13F0`. Their names alone change outside this issue; their sizes, types and splits are preserved.

## Layouts

| Type | Size | Ordered members and offsets |
| --- | --- | --- |
| `Block` | `0x10` | `prev` 0, `next` 4, `max_size` 8, `size` C |
| `SubBlock` | `0x10` | `size` 0, `block` 4, `prev` 8, `next` C |
| `FixBlock` | `0x14` | `prev_` 0, `next_` 4, `client_size_` 8, `start_` C, `n_allocated_` 10 |
| `FixSubBlock` | `0x8` | `block_` 0, `next_` 4 |
| `FixStart` | `0x8` | `tail_` 0, `head_` 4 |
| `__mem_pool_obj` | `0x34` | `start_` 0, six `fix_start` entries at 4 |
| `__mem_pool` | `0x38` | fourteen reserved pointer slots, matching the public opaque pool storage |

Names and spelling come from the matching reference. `get_malloc_pool` zeroes the 0x34-byte object view within the 0x38-byte reserved storage, as the target does.

## Acceptance

The split-only build and frozen verifier first pass with the original object. The source was then registered `NonMatching`, compiled and compared, before being selected as `Matching`. `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit zero with the compiled allocator selected.

An inventory audit checks all 40 issue functions and their 17,184 bytes across all seven units listed in F-537-1. All seven actual source objects appear in the final link inputs; no original object for these units appears. Every issue function retains its native address and size. The complete DOL is directly byte-identical to the frozen reference, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The 339 pre-existing compiled source objects preserve their code, data, storage extents and relocation records. This adds only the allocator's ten functions and 3,584 bytes; the other thirty functions were already linked from source.

Current whole-object diagnostics for the existing `Gecko_ExceptionPPC.cpp` are not uniformly 100%: source includes linker-stripped exception records and literals, and `__unexpected` has a pooled-literal correspondence difference. Its code-section score is 99.973175%, while the final source-selected artifact is exact. This qualifies F-537-1's historical blanket whole-object comparison statement. The newly accepted allocator has no such diagnostic difference. The other five existing units compare at 100% in every reported section. No existing exception source, compiler policy or source selection is changed.

## Traps

- A reference search can become stale. F-537-1's six missing bodies are present in the newly available pinned CC0 reference and compile exactly without changing flags. Recheck source availability before independently rewriting a known library algorithm.
- A one-byte small-BSS object can require a split through the following alignment boundary. Ending this split at `0x804B9BC9` is rejected by dtk; retaining the one-byte symbol but ending the unit at `0x804B9BD0` permits exact linking.
- Existing source objects can contain unretained exception metadata. Audit actual source selection and the full frozen artifact independently of whole-object fuzzy diagnostics, and report those diagnostic limitations explicitly.
