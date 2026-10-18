# F-377-1: Effect curve and post-field boundaries

Issue #377 owns the first retained functions of two translation units. The earlier survey boundary through `0x8022F4F4` divides the post-field unit. The preceding four emitter functions already belong to the verified emitter unit from #737.

| Unit | Native sections | Retained functions |
| --- | --- | --- |
| `nw4r/ef/ef_animcurve.cpp` | `.text` `802290A0-8022DFAC`; `.data` `803C5378-803C5578`; `.sdata2` `804BA628-804BA660` | Eight, all owned by #377 |
| `nw4r/ef/ef_postfield.cpp` | `.text` `8022DFB0-80231640`; `.sdata2` `804BA660-804BA698` | Fourteen: nine from #377, four from #767, one from #768 |

Both splits pass the real configure/Ninja/frozen-manifest pipeline without changing `main.dol`. The animation unit remains an original-only split; no placeholder implementation or source credit is introduced. Its two 256-byte tables precede the particle vtable. The post-field unit is registered `NonMatching` and still selects its original object.

## Evidence and provenance

The target is the immutable `wpse-v1` executable. Gekko disassembly, split-object relocation records, native data words, and the verified ELF establish the boundaries and algorithms. `0x8022D910` and `0x8022DC60` are baked U8 and F32 curve executors; they are not post-field geometry routines. Their channel lookup references belong to the animation unit. The sphere classifier at `0x8022DFB0` starts the post-field unit. The last post-field particle step ends exactly at the first particle method, `0x80231640`.

The algorithm reference is [InusualZ/mhtri-decomp at ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/tree/ac489be692a1273afff8574b2402aecba2ae2b8f), `src/ef/ef_postfield.cpp` and `src/ef/ef_animcurve.cpp`, under CC0. Its debug assertions, procedural wrapper types, compiler pragmas, offset scaffolding, and implementation names are not imported. The reconstruction uses the canonical C++ math and emitter interfaces. Original mangled classifier/crossing names already present in this target's symbol map are retained; `GetTypePositionSphere` is inferred from its behavior and the other five classifiers, with medium naming confidence. It is not claimed as a recovered string.

## Candidate source

| Address | Function | Bytes | Candidate result |
| --- | --- | --- | --- |
| `8022DFB0` | `GetTypePositionSphere` | 284 | Exact |
| `8022E0D0` | `GetTypePositionCube` | 224 | Exact |
| `8022E1B0` | `GetTypePositionCylinder` | 200 | Exact |
| `8022E280` | `GetTypePositionCircle` | 260 | Exact |
| `8022E390` | `GetTypePositionRectangle` | 232 | Exact |
| `8022E480` | `GetTypePositionPlane` | 168 | Exact |
| `8022E530` | `crossCheckPlane` | 1172 | Exact |
| `8022E9D0` | `crossCheckRectangle` | 1436 | Exact |
| `8022EF70` | `crossCheckCircle` | 1412 | Exact |

All nine compiled bodies, 5,388 instruction bytes, agree byte for byte with their original split-object bodies. All 148 paired text relocations have equal resolved target addresses, including the inline math and the matrix-vector call. The candidate's first 28 constant bytes are exactly the native constant prefix: minus one, one, zero, no-crossing two, float epsilon, one plus one ULP, and one plus two ULPs. This goes beyond a fuzzy score, but is candidate evidence: the object remains unlinked until all fourteen routines and all 56 native constant bytes are reconstructed. No new source coverage is claimed.

`PostFieldInfo` is a 72-byte record at four-byte alignment. The first three vectors occupy `0x00`, `0x0C`, and `0x18`; the particle-step reference establishes their scale/rotation/translation roles. Speed limit is at `0x24`, speed mode at `0x28`, shape at `0x29`, axis/hemisphere options at `0x2A`, collision mode at `0x2B`, flags at `0x2C`, start age at `0x2E`, collision scale at `0x30`, and the existing ten-byte `EmitterInheritSetting` at `0x3C`, followed by alignment. Member spellings are descriptive. The nine routines directly prove the option and flag offsets; the remaining layout follows the referenced particle-step target accesses and is not claimed as original naming.

Sphere/cube/cylinder return 3 outside, 1 on the surface, and 2 inside. Circle/rectangle use 4 for points in their plane outside the shape and 3 for points off the plane. Plane returns 2 above, 3 below, and 1 on it. Crossing routines return the segment parameter on a hit or 2.0 otherwise. Reflection multiplies the selected velocity and direction components by minus one, then nudges the hit coordinate by float epsilon before transforming the hit point.

## Validation and remaining work

`python configure.py --map`, `.venv/Scripts/ninja`, and the frozen-manifest `verify.py verify` command all exit 0. `tools/re/unitdiff.py nw4r/ef/ef_postfield.cpp` reports nine exact functions. An independent temporary ELF audit checks the nine raw bodies, their symbol sizes, paired resolved relocations, the 28-byte data prefix, actual fallback dependency selection, and complete frozen DOL identity; it exits 0. These checks must be repeated on the exact rebased merge head.

#377 remains open: all eight animation routines need source reconstruction, and the post-field unit also needs the five routines owned by #767 and #768 before source selection. This change establishes the owning split and canonical header for those neighboring bodies; it does not implement their functions. Append their definitions in address order to this file through their own claims. Keep this unit `NonMatching` until complete.

## Traps

- If baked executors at `8022D910`/`8022DC60` are treated as geometry, the post-field split starts too early and captures unrelated lookup data. Their byte-mask lookup and channel copies prove they end the animation unit instead.
- The fuller CC0 reference is under `src/ef/`, outside an older sparse `src/nw4r/ef/` checkout. Inspect the pinned tree before concluding implementation evidence is absent.
- A function-scope crossing parameter initialized to 2.0f before the axis switch preserves the native constant order. Initialize that same parameter and overwrite it on a hit; do not insert unused padding data or a second constant object. Returning a mutable initializer directly from the default path instead makes this compiler keep it live across the switch, hoist its load, and change three functions' register allocation. Local const copies add separate data words. All accepted functions retain the established NW4R flags.
- Importing the reference's `peephole off` flag damages eight of the nine candidate functions. It was tested and rejected; the target's reduced debug checks are not evidence for importing debug build flags.
