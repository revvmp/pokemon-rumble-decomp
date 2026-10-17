# F-462-3: NW4R layout utilities reconstructed from source

Target `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue #462 owns 24 functions; this change reconstructs the thirteen-function `lyt_common.cpp` unit at `0x802B0C30–0x802B1790`, comprising 2,832 native instruction bytes and 80 inter-function padding bytes. The other eleven issue functions remain original fallback objects.

## Unit and source

| Section | Owned extent |
| --- | --- |
| `.text` | `0x802B0C30–0x802B1790` |
| `.bss` | `0x804524F0–0x80452510`, four default texture coordinates |
| `.sbss` | `0x804B9438–0x804B9439`, local initialization guard |
| `.sdata2` | `0x804BB220–0x804BB228`, zero and one floats |

Source provenance: CC0 xbret/xenoblade `ea25691b2029394ab777bda7060139e517a13cba`, `libs/nw4r/src/lyt/lyt_common.cpp` and its header; compared with CC0 Petari `e0ab141914921726a595573dc48d3f13d0922efe`. The reused files carry no separate license notices. Comments, empty reference stubs, and the unused SetCoord implementation were omitted. There is no embedded original code or assembly fallback in the reconstructed unit.

The established NW4R compiler is Wii/1.1 with the existing `cflags_nw4r` (F-25-1), including default sixteen-byte alignment and file IPA. Existing math, color, allocator, and algorithm headers are reused. The Layout interface prerequisite is merged as #723; its actual integration proof and gate incident are retained in F-462-2.

`TexCoordAry` is eight bytes: capacity at zero, size at one, pointer to four-vector arrays at four. Each texture-coordinate array contains four eight-byte VEC2 values. `Free` goes through Layout::DeleteObj, reproducing the nested null guard in the target; Reserve preserves the allocation result test. DrawQuad subtracts height from y for its bottom vertices in this target.

## Native inventory

| Address | Size | Original symbol |
| --- | --- | --- |
| `0x802B0C30` | `0x2C` | `EqualsResName__Q34nw4r3lyt6detailFPCcPCc` |
| `0x802B0C60` | `0x2C` | `EqualsMaterialName__Q34nw4r3lyt6detailFPCcPCc` |
| `0x802B0C90` | `0x28` | `TestFileHeader__Q34nw4r3lyt6detailFRCQ44nw4r3lyt3res16BinaryFileHeaderUl` |
| `0x802B0CC0` | `0x14` | `__ct__Q44nw4r3lyt6detail11TexCoordAryFv` |
| `0x802B0CE0` | `0x50` | `Free__Q44nw4r3lyt6detail11TexCoordAryFv` |
| `0x802B0D30` | `0x94` | `Reserve__Q44nw4r3lyt6detail11TexCoordAryFUc` |
| `0x802B0DD0` | `0xEC` | `SetSize__Q44nw4r3lyt6detail11TexCoordAryFUc` |
| `0x802B0EC0` | `0x94` | `Copy__Q44nw4r3lyt6detail11TexCoordAryFPCvUc` |
| `0x802B0F60` | `0x68` | `IsModulateVertexColor__Q34nw4r3lyt6detailFPQ34nw4r2ut5ColorUc` |
| `0x802B0FD0` | `0x3C` | `MultipleAlpha__Q34nw4r3lyt6detailFQ34nw4r2ut5ColorUc` |
| `0x802B1010` | `0xE4` | `SetVertexFormat__Q34nw4r3lyt6detailFbUc` |
| `0x802B1100` | `0x560` | `DrawQuad__Q34nw4r3lyt6detailFRCQ34nw4r4math4VEC2RCQ34nw4r3lyt4SizeUcPA4_CQ34nw4r4math4VEC2PCQ34nw4r2ut5Color` |
| `0x802B1660` | `0x130` | `DrawQuad__Q34nw4r3lyt6detailFRCQ34nw4r4math4VEC2RCQ34nw4r3lyt4SizeUcPA4_CQ34nw4r4math4VEC2PCQ34nw4r2ut5ColorUc` |

## Acceptance evidence

The split-only full build and explicit frozen verifier pass before object registration. After registration as NonMatching, objdiff reports 100% for every native function and all four owned sections. The source-selected candidate then passes configure, Ninja, and explicit frozen verification. The full DOL directly equals the immutable reference; expected hashes are unchanged.

The final linked ELF places all thirteen named functions at their native addresses and sizes, the coordinate array at 0x804524F0 with size 32, and its guard at 0x804B9438 with size one. The actual Ninja link inputs select `src/nw4r/lyt/lyt_common.o` and exclude its original `obj/` counterpart. The source-only TestFileHeader overload, array alpha helper, and Color destructor are real C++ bodies discarded by the linker; they add no native function credit.

## Issue scope integration

The initial common-unit partial left eleven issue functions in fallback. The user subsequently approved the shared Arc resource-accessor files across #461 and #462; F-462-5 covers the first ten issue functions (`0x802B08F0–0x802B0C30`) and their owning units. F-462-6 covers the final free `nw4r::lyt::BindAnimation(Group*, AnimTransform*, bool, bool)` at `0x802B1790` in `lyt_util.cpp`. The complete issue inventory is recorded there. This common unit itself remains thirteen native functions and 2,832 code bytes; progress from the other units is counted separately.

## Traps

- The old Petari layout utility reference uses upward addition for the bottom vertices, while this target subtracts height. The newer Xenoblade source reproduces all 344 DrawQuad words outside its three relocations and the full source-selected artifact.
- Freeing texture coordinates directly through Layout::FreeMemory removes the target's redundant null check. The evidenced DeleteObj template preserves the original source relationship and matching instructions.
- Native static-storage symbols include following alignment padding in the survey metadata. The coordinate array is 32 bytes, followed by sixteen bytes of automatic padding; the initialization guard is one byte, followed by seven bytes of padding. The source split ends at actual storage, preserving the following OS objects.
