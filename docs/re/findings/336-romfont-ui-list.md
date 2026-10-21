# F-336-1: CRomFont and CUIList inventory corrections

## Status

Issue #336 remains incomplete. Its original inventory contains 22 functions and 6,068 instruction bytes. The independent CRomFont translation unit now links from reconstructed source with all ten retained functions, 1,364 instruction bytes, and every allocated section exact against the frozen executable. Twelve original inventory functions remain outside this accepted unit; the resource-loader function is subsequently accepted separately in [F-336-3](336-resource-font.md), leaving eleven issue functions unresolved.

The accepted unit uses Wii/1.1 and unchanged `cflags_game`. Configure, full Ninja, explicit `all_source`, and frozen-manifest verification all exit zero. The map identifies CRomFont.o as the owner of every retained font function, exception entry, vtable, type name, base array, and three typeinfo objects. The link input selects `build/wpse-v1/src/game/CRomFont.o`, with no original object for this unit.

## Names, ownership, and boundaries

The constructor at `0x801D9B40` stores the vtable at `0x803C0FB0`. The associated native RTTI identifies `CRomFont`, its immediate base `CAnyEncFont<nw4r::ut::RomFont>`, and the SDK `nw4r::ut::RomFont` base. These spellings come from the executable, rather than the block survey.

The preceding function at `0x801D9A10` removes a ResFont resource, retrieves a shared resource through `0x80066930`, assigns its shared owner, and calls `ResFont::SetResource`. It belongs to preceding resource-font context. Its inclusion in the original #336 survey does not authorize a new source split or taking the preceding first owner's file.

The CRomFont retained function group ends at `0x801DA0C0`. The empty destructor there belongs to IUIListItem: callers are destructors of CDlgCollectionItem, CDlgInfoItem, and CUIMenuItem, whose native RTTI names IUIListItem as their base. Its typeinfo is at `0x804B5FD0`, pointing to the original name at `0x8039C390`. The functions at `0x801DA100` and `0x801DA110` are referenced by the following CUIList vtable, not the font vtable. Thus the following unit begins with an inherited weak destructor rather than its strong constructor.

The CUIList constructor at `0x801DA120` calls BUILayout at `0x801C9EE0`; its first own store is at offset `0x134` and its last own word is at `0x17C`, establishing a `0x134` base extent and `0x180` complete object. BUILayout itself constructs BUI at offset zero and CLayout at offset `0x4C`; its destructor confirms both identities. Complete base members and virtual contracts remain under investigation with the #327 owner.

The accepted font `.data` interval is `0x803C0FB0–0x803C1090`, containing the vtable and RTTI names/base arrays. Three typeinfo objects occupy `0x804B8430–0x804B8448`. Their final source-selected bytes and addresses equal the original.

The verified executable's exception index identifies nine retained font entries: constructor, base destructor, derived destructor, the five conversion methods, and the depth guard destructor. Their contiguous index interval is `0x8005199C–0x80051A08`; their exception tables occupy `0x8003E9BC–0x8003EB10`. GetEncoding needs no entry. All retained exception and index bytes equal the original after actual source selection. The outside `336-font-extab.py` inventory reads the verified linked ELF and candidate object independently.

## Native font layout and behavior

| Offset | Native evidence |
| --- | --- |
| `0x00–0x1B` | Canonical SDK RomFont base |
| `0x1C–0x5F` | Embedded encoding converter; native initialization calls `0x801CA450` |
| `0x60` | FontEncoding, initialized to UTF16 and returned by GetEncoding |
| `0x64` | Signed recursive conversion depth |
| `0x68` | Owned font buffer pointer, held through shared_ptr<unsigned char> |
| `0x6C` | Shared-owner control pointer |

The converter uses the canonical owner-reviewed `CEncodingConverter` interface from #941: size `0x44`, two ENC contexts followed by three member-function pointers. Its class spelling is descriptive; the enclosing `CAnyEncFont` spelling is recovered RTTI. The font's complete size is `0x70`.

SetAlternateChar, GetCharWidth, GetCharWidths, GetGlyph, and HasGlyph call the underlying RomFont implementation directly when the signed conversion depth is positive. Otherwise a scoped guard increments the depth, converts through the converter's member-function pointer at offset `0x38`, calls the SDK implementation, and decrements the depth. The retained guard destructor at `0x801DA070` also restores the depth during exception cleanup. The candidate guard name is descriptive; no original guard name has been recovered.

The constructor allocates a 32-byte-aligned MEM2 buffer of `RomFont::GetRequireBufferSize()`, installs GameMemory::Free as its deleter, loads the font, and initializes UTF16 conversion. Native deleter RTTI identifies a `shared_ptr_deleter<unsigned char, void (*)(void *) throw()>`. The accepted member is shared_ptr<unsigned char>: a local void* allocation result, cast to u8* when resetting the owner, reproduces all constructor bytes and the native shared_ptr<unsigned char> cleanup at `0x800657E0`. The earlier void-owner diagnostic also reproduced constructor instructions, but would introduce a differently named cleanup specialization. Thus instruction agreement alone did not establish the member type. The retained native cleanup and exact whole-unit link resolve that ambiguity.

## Independent instruction checks

| Address | Function identity | Native bytes | Result |
| --- | --- | ---: | --- |
| `0x801D9B40` | CRomFont constructor | 320 | Exact with u8 owner and void* allocation local |
| `0x801D9C80` | CAnyEncFont<RomFont> destructor | 88 | Exact |
| `0x801D9CE0` | CRomFont destructor | 180 | Exact |
| `0x801D9DA0` | SetAlternateChar | 128 | Exact |
| `0x801D9E20` | GetCharWidth | 128 | Exact |
| `0x801D9EA0` | GetCharWidths | 156 | Exact |
| `0x801D9F40` | GetGlyph | 148 | Exact |
| `0x801D9FE0` | HasGlyph | 128 | Exact |
| `0x801DA060` | GetEncoding | 8 | Exact |
| `0x801DA070` | Scoped conversion-depth guard destructor | 80 | Exact |

The rejected u8* allocation-local variant differs at constructor offsets `0x34`, `0x38`, `0x68`, `0x84`, `0x90`, `0x9C`, and `0xB8`, exchanging r29/r30 for the constant one and allocated buffer pointer. Both Wii/1.1 and Wii/1.3 reproduce that gap. Preserving the allocator's actual void* return type in the local removes every difference without changing the owner type, compiler flags, comparison, or callee targets.

The reproducible outside instruction investigation uses `336-compile-romfont.py` and `336-audit-romfont.py`. The audit resolves each relocation to a native address and compares the resulting instruction words. The preserved void-view experiment is an earlier successful constructor diagnostic, not the accepted member type.

The bounded canonical shared_ptr<void> support was separately accepted in PR #1022, merged as `a1442a6146`, after owner review, unchanged SHA-256 values for all 705 current source objects, and frozen-DOL verification. See [F-336-2](336-shared-ptr-void.md). It remains a valid independent interface extension; the final font source does not require an erased owner.

## Retained ordering and final acceptance

The scoped guard stores one reference to the signed 32-bit depth, increments it on construction, and decrements it on ordinary or exceptional exit. A natural generic RAII counter instantiated with int preserves this exact native lifetime, representation, and operation set. Its helper name and generic source form are inferred, not recovered original spelling. Under the unchanged game profile, this template helper is emitted after all font virtual methods, exactly as in the native image. The non-template helper was emitted after the first conversion method; moving its constructor or inline destructor definition did not recover the order. A strong out-of-line destructor was emitted before the template methods and was rejected.

| Allocated interval | Source-selected exact bytes |
| --- | ---: |
| `extab` `0x8003E9BC–0x8003EB10` | 340 |
| `extabindex` `0x8005199C–0x80051A08` | 108 |
| `.text` `0x801D9B40–0x801DA0C0` | 1,408 including 44 alignment bytes |
| `.data` `0x803C0FB0–0x803C1090` | 224 including final alignment |
| `.sdata` `0x804B8430–0x804B8448` | 24 |

The source object contains other weak template and SDK definitions. The ordinary linker discards their duplicate or unreferenced instances; the map records these as unused. Object-level raw data and exception sizes therefore exceed the retained native sections, so their fuzzy object percentages are not an acceptance measure. The actual linked intervals, all pointer targets, and frozen DOL bytes are exact. No sections, retention rules, compiler flags, source-byte replacements, or helper callers were forced.

The outside `336-linked-proof.py` compares every allocated interval directly to immutable main.dol slices and queries Ninja's actual all_source inputs. All 706 declared source objects are present: the new font plus the original 705, whose whole-object SHA-256 values remain unchanged. The map and build.ninja establish source-only ownership for the new unit. `unitdiff.py game/CRomFont.cpp` reports ten of ten functions and all 1,364 instruction bytes at 100%; explicit `ninja all_source` exits zero with no pending work. The default and map builds both delete the old DOL first and exit zero, and frozen-manifest verification exits zero for their actual output.

## Traps

- A matching custom shared-owner RTTI identifies the allocated/deleter specialization, not necessarily the public shared_ptr view. Keep those two type claims separate.
- Constructor instruction identity did not distinguish a void owner from a u8 owner with a void* allocation local. Inspect cleanup specialization and the full linked unit before deciding the owner template argument.
- Identical empty virtual-destructor instructions do not establish class identity. The initial scratch association of `0x801DA0C0` with shared_ptr_deleter_common was rejected: the actual canonical common destructor is already at `0x80055BB0`, and independent callers plus RTTI prove IUIListItem here. Remove that false association before choosing file boundaries or naming symbols.
- Assigning a newly constructed shared_ptr adds retain/release work absent from the native constructor. The normal reset operation recovers the native size; direct temporary swap produces the same seven remaining register differences. A named temporary extends cleanup lifetime and makes the constructor 24 bytes too large.
- The base FontEncoding member must be read when calling InitReaderFunc in the candidate SetEncoding implementation. Passing the formal parameter produces an extra constant instruction in this constructor. Both forms have the same API behavior, so this observation supports the measured implementation rather than a new interface or compiler flag.
- Per-function agreement cannot establish retained weak-function order, RTTI layout, exception-table coverage, or source membership. All require actual full-unit and executable verification before Matching selection.
