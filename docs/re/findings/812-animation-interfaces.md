# F-812-1: Layout animation interface prerequisite

## Scope and ownership

Issue #812 owns the complete retained `nw4r/lyt/lyt_animation.cpp` inventory: 27 functions / 5,396 instruction bytes at `0x802AF0C0`–`0x802B0674`. Its first retained functions are the original anonymous-namespace step and Hermite curve evaluators. This prerequisite extends six existing canonical headers with the complete animation interfaces, their resource records and the coordinated primitive-array deletion helper. It changes no source, split, object status or decompiled-byte credit.

Existing AnimTransform layout and virtual order, AnimResource storage and all unrelated classes are preserved. The complete Material and TexMap interfaces belong to #808/#811; owner-approved external copies are used only for diagnostic consumers, and their headers are unchanged here. Both the #808 source owner and delegated #809 Layout owner approved exact reuse of the six-line header-only `7903932717f2b57dffe26ed4bf147c18b3fbe9d8` contribution: Layout::DeletePrimArray null-checks and frees a primitive array without running element destructors. Native AnimTransformBasic destruction reproduces all 132 bytes with this helper. It changes no Layout storage, virtual order, existing templates or source ownership.

The #808 first-file owner also approved verbatim integration of the complete TextBox header and its [F-808-3 evidence](808-textbox-interface.md), commit `44fbc117ada6cce9e82f096871b5c29ad1bae1b0`. This coordinated prerequisite preserves the proven `0x140` class, original virtual order, tag-processor offset `0xF8` and draw-cache flag word at `0x104`. It takes no TextBox source, split or completion ownership. The separate RTTI null guard remains in PR #856 rather than being duplicated here.

Final coordinated scope also includes the exact #808-owned Bounding header from `488f31c3738eade3663af9344269a10e31dc42ce` and the #808-approved delegated #809 Layout/Pane additions from `e3c2d36e2f825c5620e4862b5c94208952ee95f5`. Bounding adds no class storage beyond Pane; its resource record likewise adds no fields beyond the original Pane record. Its five retained native functions / 172 bytes were independently proved by #808. The Layout resource is its original block header, origin byte and aligned Size; the one-argument NewObj allocator and two Pane traversal accessors preserve every existing class member and virtual slot. All source/split ownership remains with the respective issues.

## Provenance

The primary reference is CC0 zeldaret/ss `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`. Its `src/nw4r/lyt/lyt_animation.cpp` SHA-256 is `c4a9c69ddea04a25075740d6c683e70a35d8a090996b3a4d3f6ea85e47075641`; animation header `ad1195e44faf9c133a5df7079e4a928bbd7885a3a8befe03e1c88059de5d0c37`; resource header `3f98e7b86a04a73c05a98df5ac38a4759d414839392a29a8870bf8af793d2ce7`; type header `599275814fcae16a2b2ab2a3bed89cae6c9bc496cc791cbcb97f122133c22d8f`. The repository LICENSE is CC0 1.0 Universal, SHA-256 `a2010f343487d3f7618affe54f789f5487602331c0a8d03f49e9a7c547cf0499`. No inspected source or header contains a separate license exception.

CC0 Xenoblade `ea25691b2029394ab777bda7060139e517a13cba` supplies an older compatible comparison. Its animation Bind interfaces omit this target's disable parameters, so those declarations are preserved from the existing canonical header and checked against native calls. Reference comments and unused diagnostic stubs are omitted.

## Native layouts

| Type | Size | Storage |
| --- | --- | --- |
| AnimTransform | `0x14` | Existing vptr, node at 4, resource pointer at C, frame at 10 |
| AnimTransformBasic | `0x20` | Existing base, file-resource pointer array at 14, animation-link array at 18, unsigned-short count at 1C |
| AnimResource | `0x10` | Existing file-header, animation, tag and share pointers at 0/4/8/C |
| detail::AnimPaneTree | `0x28` | Resource at 0, pane index at 10, link count at 12, nine material indices at 14, material count at 26 |

Native constructors and the complete animation/bind/tree functions reproduce these offsets. The AnimTransformBasic vtable has seven original entries in destructor, two SetResource, two Bind and two Animate order. Its native 36-byte table begins at `0x803C7228`; the four following bytes are outside that object. The complete native constant pool is 32 bytes at `0x804BB200`.

The resource declarations preserve the original field order and types for AnimationBlock (20 bytes), AnimationTagBlock (28), AnimationShareBlock (16), AnimationContent (24), AnimationInfo (8), AnimationTarget (12), StepKey (8), and HermiteKey (12). Group and share references follow the original 20-byte and 36-byte record formats. Existing resource structs and padding remain unchanged.

Pane::SetSRTElement retains the original float-element view of its contiguous translation/rotation/scale/size storage. The existing Pane layout and virtual methods are untouched. TestFileVersion reads the two version bytes and accepts major zero with minor eight through ten. The descriptive helper IsSupportedMinorVersion expresses the separately evaluated minor predicate visible in the native instructions; its name is project terminology, not a recovered symbol.

## Diagnostic and baseline evidence

The complete external source consumer compiled under existing Wii/1.1 NW4R flags. Every one of the 27 original issue functions has its native size and every non-relocation word. The paired audit checks all 67 retained text relocations, all seven vtable entries, every vtable byte and the complete constant pool: 74 relocations and 68 initialized bytes exact. These diagnostics do not establish source-selected linking or source completion.

Before any tracked header change, the untouched baseline passed `python configure.py`, all 484 plain Ninja targets, and explicit frozen verification, all exit zero. SHA-256 values for all 477 compiled source objects were captured. The header proposal then passed a fresh 484-target plain Ninja build and explicit frozen verification, exit zero. Of the 477 existing source objects, 474 are byte-identical. The remaining lyt_common, lyt_pane and lyt_window objects differ solely in digits within compiler-generated local constant names in `.strtab`; all other section bytes, including code, data, relocations, symbol table and compiler metadata, are identical. An independent comparison validates that exact restriction and exits zero.

The complete animation consumer was also recompiled using these tracked animation headers and only owner-approved pending Material/TexMap/Layout prerequisites. All 27 instruction streams, 74 paired relocations and 68 initialized bytes remain exact. The source delivery must separately prove actual source-object comparison and source-selected full-DOL identity after its prerequisite interfaces integrate.

After folding the exact TextBox header, the #809 owner's canonical-header consumer was compiled from this worktree's tracked animation, types, resources, common, pane and TextBox headers. The external overlay adds only the #809-owned proposed Layout resource/one-argument allocator and Pane traversal accessors, the exact separate #856 guard, and owner-approved #855 Material and #808 Bounding prerequisites. All 19 retained Layout functions / 4,144 instruction bytes, 84 text and 14 data relocation targets, its 64-byte vtable/data section and eight-byte constant pool match the native executable. The original allocator small-BSS object remains four bytes. These diagnostics establish consumer compatibility without claiming Layout source completion.

The fresh TextBox-inclusive build reproduces the same 477-object baseline result: 474 whole objects identical and only the same three restricted compiler-local name differences. Animation and Layout native consumer audits both exit zero with the exact tracked TextBox declaration. No additional project-wide trap was discovered.

For final integration onto actual merged #855 (`8a29e94b907f6c7c2fa32c4ce59446c1a9848095`), all Material/TexMap interfaces and the #856 RTTI guard come directly from main. The animation diagnostic now compiles using only this worktree's tracked include tree. The Layout diagnostic likewise uses tracked declarations except the exact #809-owned Picture header from approved commit `516e173e`, isolated as its sole external header prerequisite. Picture source/header ownership remains in #809's source delivery. Both native audits retain their complete counts above. The verified #855 tree is identical to its actual squash snapshot; all 498 existing source objects were captured from that verified build as the current-main control before the final fresh header build.

## Traps

The older animation interfaces drop the disable argument. Retaining only their class names and sizes is insufficient: preserve the proven virtual signatures and native argument forwarding.

Flattening the reference version predicate into one Boolean expression produced a 256-byte AnimResource::Set and changed register allocation. Separating the supported-minor predicate with ordinary inline C++ reproduces the complete native 260-byte function. This is a measured source distinction for this unit, not a general compiler flag rule.

The source compiler object also contains unretained original helpers and a weak base vtable. Source acceptance must check retained function order, final native addresses, actual source-only link inputs and the full frozen artifact; it cannot assume raw object offsets already equal final linked offsets.

Additional header declarations changed three existing objects' compiler-local constant counters. Raw object digests therefore differ even though every section except those digits in `.strtab` is identical. Inspect the actual sections and strings before accepting this distinction; a digest mismatch alone is not permission to overlook code, data or relocation changes. The same behavior was independently observed in [F-807-1](807-layout-interfaces.md).
