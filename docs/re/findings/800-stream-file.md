# F-800-1: NW4R stream-file reader and loader

Issue #800 owns the complete retained `snd_StrmFile.cpp` unit, nine functions and 2,400 code bytes at `0x80289600`–`0x80289F84`. Its next neighbor, `StrmPlayer`, starts at `0x80289F90`; the intervening twelve bytes are alignment rather than part of this split. No initialized or uninitialized data section belongs to this unit.

## Source and ABI

The reference is the CC0 `zeldaret/ss` revision `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`, `src/nw4r/snd/snd_StrmFile.cpp` and its corresponding header. Neither file has a separate license notice. Its original namespace, class names, members and control flow are retained, with assertions and comments removed. Existing project headers supply the sound types, data-reference helpers and stream interfaces. Wii/1.1 and the existing `cflags_nw4r` settings reproduce every retained function; no per-unit compiler override is needed.

`StrmFileReader` is eight bytes: header pointer at zero and head-block pointer at four. `StrmFileLoader` is twelve bytes: a stream reference at zero and its reader at four. The complete file structures are declared in the unit's header. The file header is `0x28` bytes; the head block is `0x20`; stream-data information is `0x34`. Track tables use eight-byte data references starting at four; extended track records place channel count at eight. The public stream information is `0x40` bytes, and track information is `0x28`, including 32 channel indices. Native field accesses and all reconstructed instructions confirm these layouts.

The imported map signature for `StrmFileLoader::ReadAdpcmInfo` puts `AdpcmParam` in `nw4r::snd`; the existing complete project type and matching source place it in `nw4r::snd::detail`. Only that owned symbol's name changes. Its native address, size and type remain intact. The reader's ADPCM helper is inlined into the retained loader wrapper, so its separate reference definition is discarded by the linker.

## Validation

The original-object split-only build passed configure, Ninja and the unchanged frozen-manifest verifier before source registration. The unit was then compiled as `NonMatching` and compared with objdiff: `.text` scores 100%. A separate audit resolves all twelve relocations to their native targets and compares every instruction byte in all nine retained functions, totaling 2,400 bytes. The audit produces evidence only and does not modify compiler output or provide linker inputs.

After source selection, `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exit zero. The actual linker selects the reconstructed stream-file object. Every retained function has its original final ELF address and extent. All 423 previously compiled source objects preserve their code, data and relocation targets. The rebuilt DOL is byte-identical to the frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

After rebasing onto main `cfbc1e48`, a fresh build in `build/800-merge6` passes configure, Ninja, the actual-directory frozen verifier, native instruction and relocation audits, and final source-selection checks. All 422 other baseline source objects remain unchanged. The shared GX header added by #833 makes the GC compiler emit fourteen additional unreferenced local helpers in NCD, totaling 196 bytes. Every prior NCD function body and relocation remains identical, as do all its data sections. Each added helper has local binding, has no object relocation referring to it, and exactly duplicates an existing FIFO writer of the same argument types. The original 920 text bytes and the added 196 bytes account for the complete new 1,116-byte text section. No source fallback or frozen hash changes are involved; the final DOL remains byte-identical.

## Traps

- Replacing a `sizeof` expression with a signed literal changes `cmplwi` to `cmpwi` in the first header-read comparison. The aligned header size is an unsigned 64-byte constant; retaining its unsigned type reproduces the native instruction.
- The shared map's ADPCM parameter namespace belongs to its reference revision. Reuse the existing project type and correct the owned mangled name after proving the body, rather than introducing a duplicate shared structure to preserve an imported signature.
