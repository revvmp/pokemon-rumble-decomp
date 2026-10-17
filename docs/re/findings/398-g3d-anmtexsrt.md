# F-398-1: NW4R texture-transform animation

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue #398 owns 24 functions and 2,680 code bytes from `0x802575A0` through `0x80258084`. Its first two functions are the final `AnmObjTexPat` RTTI methods in the #397-owned unit. The remaining 22 functions belong to `g3d_anmtexsrt.cpp`, which also supplies the first two functions of #399. The full texture-transform unit retains 24 functions and 2,680 native code bytes. Contributions to the neighboring issues receive no duplicate reconstruction credit.

## Boundaries and declarations

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x802575E0`–`0x802580CC` | Texture-transform animation methods and both material-application overloads |
| `.rodata` | `0x8036C798`–`0x8036C7C8` | `AnmObjTexSrt` and `AnmObjTexSrtRes` type names |
| `.data` | `0x803C5B88`–`0x803C5C18` | Resource and base-class vtables, each 0x48 bytes |
| `.sdata2` | `0x804BABB0`–`0x804BABD0` | Zero, one, signed-integer conversion bias, matrix epsilon and double one |

The first retained function is `AnmObjTexSrt::TestExistence`. Its constructor, `TestDefined`, node implementation and override implementation are stripped from this release. The source retains these original class members, as the adjacent animation units do. The last two retained functions are `AnmObjTexSrt::GetTypeName` at `0x80258090` (0x30 bytes) and `GetTypeObj` at `0x802580C0` (0x0C), supplied to #399. The first two functions owned by #398, at `0x802575A0` and `0x802575D0`, instead belong to `AnmObjTexPat`; the #397 owner supplies them.

The complete `AnmObjTexSrt` declaration extends the established `AnmObj` base. Its binding count and binding pointer are at offsets 0x10 and 0x14, with size 0x18. `AnmObjTexSrtRes` adds the protected `FrameCtrl` base at 0x18, followed by its resource pointer at 0x2C and result-cache pointer at 0x30, making its complete size 0x34. The native construction path allocates this size and initializes the evidenced fields; the cache entries are `TexSrtAnmResult` objects of size 0xE8. The complete node and override declarations retain the reference's order, but their additional fields have reference-only evidence because those classes are stripped here.

The native base type-name record occupies 20 bytes followed by four alignment bytes. Its former symbol estimate included that padding; its size is corrected from 0x18 to 0x14. Its initializer and the 24-byte resource type-name record match the frozen data. The existing shared animation-policy table remains owned by the earlier visibility unit.

## Source and compiler

The implementation and class declaration come from CC0 `zeldaret/ss` revision `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/g3d/g3d_anmtexsrt.cpp` and its matching header. The source files have no separate file-level license notices. Comments are removed, aggregate includes are replaced by the existing specific project headers, and null pointers use `nullptr`. The bounded `g3d_dcc.h` adds the reference's texture-matrix function declarations without importing its unrelated world-matrix attribute class.

The existing Wii/1.1 `cflags_nw4r` policy reproduces the target. The first material-application overload remains out of line, expressed by the reference's noinline attribute. The reference's standard `ldexpf` wrapper is expressed using the available double `ldexp` declaration and an explicit conversion to `f32`; the target calls `ldexp` and rounds to single precision, and all resulting instructions match exactly. No compiler or library-wide flag changes are needed.

The native call to `CalcTexMtx` at `0x80261D10` agrees with the canonical name imported on main, the reference's four-argument interface and the paired relocation in the original unit. Its metadata and owning split remain unchanged by this delivery.

## Validation

The original-object split build passed configure, Ninja and the frozen verifier before source registration. The unit was registered NonMatching, compiled from its actual source, and compared with objdiff before source selection. All 24 retained functions and every `.text`, `.rodata`, `.data` and `.sdata2` section score 100%.

An independent audit resolves all 53 instruction relocations to native targets and compares every instruction byte for all 24 functions, totaling 2,680 bytes. The selected source object then links successfully; Ninja exits zero and the explicit frozen-manifest verifier passes. The final ELF retains all native function addresses and sizes, and the actual link inputs select the reconstructed object. Against the split-only baseline, 367 whole source objects retain identical code, data and relocation targets. The remaining `MSL_C/extras.o` preserves its original 160 bytes and two relocations, with only the four-byte `strncasecmp` forwarding branch added by merged #721. The rebuilt DOL is byte-identical and retains SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The #397-owned texture-pattern unit merged in PR #750. A fresh combined build independently audits its two #398 RTTI tails at `0x802575A0` and `0x802575D0`, including their native bodies, relocation targets, final addresses, sizes and actual source-object selection. Together with the 22 owned functions here, all 24 functions and 2,680 native code bytes in the original #398 inventory are reconstructed and source-selected. The two #399 functions supplied by this unit receive no duplicate issue credit.

## Traps

- A type-name symbol estimate can include alignment padding. The base record here is 20 bytes, rather than its initial 24-byte estimate. Correcting only the owned symbol extent makes objdiff reflect the actual initializer while preserving the original section boundary and bytes.
- Inlining the first material-application overload enlarges the second from the native 0x1AC bytes to 0x230. The original noinline declaration restores the native call and exact instructions without changing the library's compiler flags.
- Include `ut_algorithm.h` before headers that bring in the SDK memory macros, as established in F-395-1.
