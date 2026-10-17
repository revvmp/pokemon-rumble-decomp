# F-400-1: NW4R shape-animation objects

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue #400 owns 24 functions and 2,384 code bytes from `0x80258D50` through `0x80259734`. Its first two functions are `AnmObjChr::GetTypeName` and `GetTypeObj` in the #399-owned unit. The remaining 22 functions belong to `g3d_anmshp.cpp`, which also supplies the first two functions of #401. The complete shape-animation unit retains 24 functions and 2,384 code bytes; neighboring contributions receive no duplicate reconstruction credit.

## Unit and layouts

| Section | Range | Contents |
| --- | --- | --- |
| `.text` | `0x80258D90`–`0x8025977C` | Base and resource shape-animation methods |
| `.rodata` | `0x8036C7F0`–`0x8036C814` | Base and resource RTTI names, 16 and 20 bytes |
| `.data` | `0x803C5CC8`–`0x803C5D68` | Resource and base vtables, each 0x50 bytes |
| `.sdata2` | `0x804BABE0`–`0x804BABF0` | Float zero, float one and signed-integer conversion bias |

The last two functions, supplied to #401, are `AnmObjShp::GetTypeName` at `0x80259740` (0x30 bytes) and `GetTypeObj` at `0x80259770` (0x0C). The native base constructor and `TestDefined` are stripped. The reference's node and blend classes have no retained RTTI or vtables here. Node declarations and implementations remain in source because their use of the inline base destructor establishes its native emission order. The blend class is omitted; no unrelated work-memory interfaces are imported solely for its unused implementation.

The complete retained base class has the established `AnmObj` prefix, a binding count at 0x10 and a binding pointer at 0x14, size 0x18. The resource class adds protected `FrameCtrl` at 0x18, its resource at 0x2C, vertex-set array at 0x30 and result-cache pointer at 0x34, size 0x38. The native allocation and inlined construction path prove these fields. Each vertex-set record is 12 bytes and each optional cached shape result is 0x218 bytes, as evidenced by the allocation multipliers and existing resource declarations. Node-only members retain reference evidence; that class has no retained native representation here.

The virtual tables preserve the inherited object/frame interfaces, binding/release/result operations and the shape-specific attach, detach, detach-all and weight methods. Their last five slots identify otherwise ambiguous small routines: attach at `0x80258DF0`, detach at `0x80258E00`, detach-all at `0x80258E30`, set-weight at `0x80258E10` and get-weight at `0x80258E20`. Comparing instruction streams alone cannot distinguish the two empty routines.

The resource type-name symbol was initially estimated as 24 bytes, including its four following alignment bytes. The source initializer proves a 20-byte record; its symbol size and the unit's final readonly-data extent exclude that padding. Four bytes remain before the next record at `0x8036C818`. The existing shared animation-policy table remains with its earlier owning unit.

## Source and behavior

Source and class declarations derive from CC0 `zeldaret/ss` revision `57a709b2128eb163a18e2969d561c9abb7d1fcee`, `src/nw4r/g3d/g3d_anmshp.cpp` and its matching header. Neither file has a separate license notice. Comments and the stripped blend declarations and implementations are removed. Specific existing headers replace the aggregate include, null pointers use `nullptr`, and explicit template arguments preserve the original utility calls in the presence of SDK macros. No existing shared types or compiler policy change.

The resource constructor allocates the binding array, vertex-set references and optional result cache together, with native two- and four-byte alignment. Binding resolves vertex-position animation records by name and fills position, normal and color resource references for each vertex set. Frame and update-rate changes refresh the optional cache under the native conditions. Get-result rejects undefined or invalid bindings, returns a cached result when available, or evaluates the animation into the caller's result.

## Validation

The corrected original-object split build passes configure, Ninja and the explicit frozen-manifest verifier before source selection. Actual source compilation uses the established Wii/1.1 `cflags_nw4r` policy. All 24 retained functions and all four allocated sections score 100% in objdiff. An independent relocation audit resolves all 49 instruction relocations to native targets and verifies every instruction byte for all 2,384 native code bytes.

With original node source ordering retained, the source-selected full Ninja link and explicit frozen verifier both exit zero. The final ELF preserves all 24 native function addresses and sizes, and the actual linker inputs select the reconstructed object. All 375 baseline source objects preserve their code, data and relocation targets. The rebuilt DOL is byte-identical to the frozen original, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

The subsequent latest-main conflict-resolution build uses `python configure.py --build-dir build/400-merge1`, Ninja and the explicit verifier for `build/400-merge1/wpse-v1`; all exit zero. The native audit still checks all 24 functions and 49 relocation targets. It preserves 374 whole baseline objects. The remaining `extras.c` object retains every original byte and relocation, with only the four-byte `strncasecmp` forwarding branch added by merged #721. This independently audited main change leaves the full frozen DOL exact.

Merged #399 / #761 supplies `AnmObjChr::GetTypeName` at `0x80258D50` (0x30 bytes) and `GetTypeObj` at `0x80258D80` (0x0C). A separate audit compares both actual compiled character-animation bodies, including their relocations, against the frozen DOL. Combined with this unit's 22 owned methods, every function in the original #400 inventory is now reconstructed and source-selected: 24 functions / 2,384 bytes, all at their original ELF addresses and extents. The two shape RTTI methods supplied to #401 remain explicitly separate, with no duplicate byte credit.

After rebasing onto main `5c3fe6d5`, the fresh `build/400-merge2` configure, Ninja, actual-directory frozen verifier, whole-unit instruction/49-relocation audit, full original-issue audit and source/ELF checks all exit zero. 373 whole baseline source objects remain unchanged. The two bounded main changes are independently checked: `extras.c` preserves all original 160 bytes and relocations plus #721's four-byte forwarding branch; #833 adds fourteen unreferenced local GX helpers / 196 bytes to NCD, each identical to an existing FIFO writer. Every prior NCD function body and relocation and all data sections remain unchanged. The complete DOL remains byte-identical to the frozen original.

## Traps

- SDK `RoundUp` is a function-like macro. Including `ut_algorithm.h` first protects its template declarations but does not prevent a later `ut::RoundUp(...)` call from expanding after an SDK include. `ut::RoundUp<u32>(...)` preserves the intended template and reproduces the native allocation instructions without changing shared headers.
- An estimated RTTI extent can cross the actual source-unit boundary. dtk rejected the split ending at `0x8036C814` while the last symbol still included padding through `0x8036C818`. Correcting that owned symbol to its 20-byte initializer preserves both the split-only frozen build and the subsequent alignment gap.
- Pair native virtual-table slots when naming identical empty methods. Here `SetWeight` and `DetachAll` both compile to a single return, but occupy distinct evidenced slots and addresses.
- Removing stripped source can change retained weak-function emission order. Omitting the node implementation moved the base destructor after the resource constructor: objdiff and isolated native-body audits still passed, but full linking placed the constructor 0x40 bytes early and changed callers. Retaining the original node implementation emits the destructor before the constructor without forced stubs, flag changes or binary padding.
