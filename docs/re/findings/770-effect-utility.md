# F-770-1: complete effect utility translation unit

## Ownership and scope

Target `wpse-v1`, frozen `config/targets/wpse-v1.json`. The native `nw4r/ef/ef_util.cpp` starts at `0x80235320` in #770 and ends at `0x80236298`. It contains this issue's seven functions and the eight utility functions at the beginning of #380. The first-function ownership rule assigns the complete split/source to #770. The #380 claim was released on 2026-10-17, leaving precisely this utility tail blocked; its issue body still mentioned the former owner. Its completed handle/emitter-form units are unchanged.

The source is Matching and source-linked: all 15 functions, 3,856 instruction bytes, and 72 data bytes compare identically. Of those instruction bytes, seven C++ functions account for 2,460 and eight native paired-single assembly routines account for 1,396. The assembly is explicitly accounted as assembly, not C/C++ decompilation credit. No original object fallback remains inside this translation unit. Its section extent includes 104 alignment bytes, which are not instruction credit.

## Source provenance and native algorithm evidence

The six main C++ bodies and inverse-scale body follow CC0 Etrainquandt17/Whippy `8ee40c841475db862231f37b8cb64196b613cc17`, `src/nw4r/ef/ef_util.cpp`, adapted to the repository's existing math interfaces and with comments removed. The older CC0 ogws source `27fa94a593096eed7442efc4e70981fd52025b9c` was checked but has an earlier rotation/normalization implementation.

CC0 InusualZ/mhtri-decomp `ac489be692a1273afff8574b2402aecba2ae2b8f`, `src/ef/ef_util.cpp`, supplies independently reconstructed paired-single algorithm evidence: vector normalization, radians-to-table interpolation, Euler Y-axis and matrix construction, row/column scaling, and the reciprocal square root step. Its debug revision has different scheduling/registers and assertions. Those assertions and its placeholder C names are not imported. The assembly routines here are reconstructed against this target with the existing typed NW4R signatures, symbolic data/table references, and descriptive local labels. No embedded instruction words, code-byte arrays, or absolute function/data addresses are used in source.

Existing function names and interfaces are retained from `ef_util.h` and the native symbol configuration. Constant names are chosen descriptive names; their original spelling is unknown. The file creates no class and needs no shared-header changes.

## Native boundaries and data

| Section | Extent | Storage |
| --- | --- | --- |
| text | `0x80235320`-`0x80236298` | Fifteen functions and inter-function alignment |
| sdata | `0x804B88F0`-`0x804B88F8` | Writable reciprocal-square-root coefficient pair `{3.0f, 0.5f}` |
| sdata2 | `0x804BA730`-`0x804BA770` | Typed constants shared by C++ and paired-single routines |

The read-only pool is `{65536,65536}`, radians-to-fidx `40.7436638f`, one, FLT_EPSILON, zero, FLT_MIN, minus one (float), minus one (double), half, three, 65536, minimum nonzero scale `0.00001f`, inverse-zero scale `100000.0f`, and a separate scalar minimum-scale constant. Every byte matches the frozen target. Addressable one-element arrays retain assembly constants at their definition position; the radians conversion is shared explicitly with the C++ cosine lookup. The writable pair's inferred 16-byte symbol included eight following alignment bytes; its actual object is eight bytes, and the split leaves that padding outside the unit.

The helpers use the already-reconstructed `math::detail::gSinCosTbl`. GQR3 quantization and table interpolation preserve the native large-angle reduction and sine sign handling. The two scale overloads have different roles: one scales matrix columns and one scales rows, substituting the small nonzero scale only for zero components. The inverse-scale C++ body substitutes 100000 for a zero input component before calling the existing matrix scaler.

## Function inventory

| Address | Size | Symbol | Representation |
| --- | --- | --- | --- |
| `0x80235320` | `0x84` | `UtlistToArray__Q24nw4r2efFPCQ34nw4r2ut4ListPPvi` | C++ |
| `0x802353B0` | `0x104` | `GetDirMtxY__Q24nw4r2efFPQ34nw4r4math5MTX34RCQ34nw4r4math4VEC3` | C++ |
| `0x802354C0` | `0x114` | `MtxGetRotationMtx__Q24nw4r2efFRCQ34nw4r4math5MTX34PQ34nw4r4math5MTX34` | C++ |
| `0x802355E0` | `0x16C` | `MtxGetRotation__Q24nw4r2efFRCQ34nw4r4math5MTX34PQ34nw4r4math4VEC3` | C++ |
| `0x80235750` | `0x1C` | `MtxGetTranslate__Q24nw4r2efFRCQ34nw4r4math5MTX34PQ34nw4r4math4VEC3` | C++ |
| `0x80235770` | `0x4D0` | `MtxGetScale__Q24nw4r2efFRCQ34nw4r4math5MTX34PQ34nw4r4math4VEC3` | C++ |
| `0x80235C40` | `0x64` | `Normalize__Q24nw4r2efFPQ34nw4r4math4VEC3PCQ34nw4r4math4VEC3` | Paired-single assembly |
| `0x80235CB0` | `0x74` | `_PSSinCosRad__Q24nw4r2efFPff` | Paired-single assembly |
| `0x80235D30` | `0x78` | `PSSinCosRad__Q24nw4r2efFPfPff` | Paired-single assembly |
| `0x80235DB0` | `0x14C` | `Rotation2VecY__Q24nw4r2efFRCQ34nw4r4math4VEC3PQ34nw4r4math4VEC3` | Paired-single assembly |
| `0x80235F00` | `0x178` | `PSMTX34RotXYZRad__Q24nw4r2efFPQ34nw4r4math5MTX34fff` | Paired-single assembly |
| `0x80236080` | `0x80` | `MTX34ScaleNonZero__Q24nw4r2efFPQ34nw4r4math5MTX34PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | Paired-single assembly |
| `0x80236100` | `0x88` | `MTX34ScaleNonZero__Q24nw4r2efFPQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3PCQ34nw4r4math5MTX34` | Paired-single assembly |
| `0x80236190` | `0xA8` | `MTX34ScaleInv__Q24nw4r2efFPQ34nw4r4math5MTX34PCQ34nw4r4math5MTX34PCQ34nw4r4math4VEC3` | C++ |
| `0x80236240` | `0x58` | `MTXColLen__Q24nw4r2efFPCQ34nw4r4math5MTX34i` | Paired-single assembly |

## Validation

Tool versions are the repository's pinned dtk 1.8.4, objdiff-cli 3.8.2, Wii/1.1 compiler, and binutils 2.42-2. No compiler flags are added: this unit uses the established `cflags_nw4r`. The split-only and NonMatching builds passed default Ninja and explicit frozen verification. The source-linked build also passes: 15/15 functions and text/sdata/sdata2 are each 100%, and the complete DOL passes exact byte comparison. Final committed-head validation after rebasing is recorded in the PR.

No other unit receives duplicate source credit. Existing original-object fallbacks elsewhere in the executable remain outside this issue's scope. Runtime execution was not used as a substitute for exact comparison.

## Traps

- GNU objdump's default PowerPC decoding interprets several paired-single opcodes as unrelated newer vector instructions. `-M gekko` gives the verified `psq_l`, `ps_madds0`, `ps_merge*`, and `ps_cmpu*` instructions. Compare the decoded assembly by recompiling it; do not infer behavior from default VSX mnemonics.
- CodeWarrior can defer scalar const objects referenced only by assembly and allocate them in a different order from the C++ literals. Addressable typed arrays place the observed constants deterministically and let the C++ and assembly paths share their radians conversion. No flags or binary blobs are needed.
- A direct double `atan2` call with casts changes floating-point register allocation from the reference's float wrapper. A local inline float interface reproduces the native argument evaluation and calls the same double kernel. The radians multiplication operand order must also remain factor first.
- A newer reference may contain useful native assembly missing from the earlier library reconstruction. Compare the actual algorithms and compiler output; omit that revision's debug assertions and placeholder names rather than importing its source wholesale.
