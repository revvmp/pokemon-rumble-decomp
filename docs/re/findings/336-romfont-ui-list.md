# F-336-1: CRomFont and CUIList inventory corrections

## Status

Issue #336 remains incomplete. This record accepts no reconstructed source bytes, changes no source selection, and does not establish a verified whole-unit link. Its original inventory contains 22 functions and 6,068 instruction bytes.

The independent CRomFont candidate now reproduces all ten retained function instruction streams, including every independently resolved relocation target: 1,364 instruction bytes and 41 targets. The remaining acceptance gates are whole-unit allocated data, exception tables, retained weak order, actual source selection, and the frozen executable. No source has been registered.

## Names, ownership, and boundaries

The constructor at `0x801D9B40` stores the vtable at `0x803C0FB0`. The associated native RTTI identifies `CRomFont`, its immediate base `CAnyEncFont<nw4r::ut::RomFont>`, and the SDK `nw4r::ut::RomFont` base. These spellings come from the executable, rather than the block survey.

The preceding function at `0x801D9A10` removes a ResFont resource, retrieves a shared resource through `0x80066930`, assigns its shared owner, and calls `ResFont::SetResource`. It belongs to preceding resource-font context. Its inclusion in the original #336 survey does not authorize a new source split or taking the preceding first owner's file.

The CRomFont retained function group ends at `0x801DA0C0`. The empty destructor there belongs to IUIListItem: callers are destructors of CDlgCollectionItem, CDlgInfoItem, and CUIMenuItem, whose native RTTI names IUIListItem as their base. Its typeinfo is at `0x804B5FD0`, pointing to the original name at `0x8039C390`. The functions at `0x801DA100` and `0x801DA110` are referenced by the following CUIList vtable, not the font vtable. Thus the following unit begins with an inherited weak destructor rather than its strong constructor.

The CUIList constructor at `0x801DA120` calls BUILayout at `0x801C9EE0`; its first own store is at offset `0x134` and its last own word is at `0x17C`, establishing a `0x134` base extent and `0x180` complete object. BUILayout itself constructs BUI at offset zero and CLayout at offset `0x4C`; its destructor confirms both identities. Complete base members and virtual contracts remain under investigation with the #327 owner.

The proposed font `.data` interval is `0x803C0FB0–0x803C1090`, containing the vtable and RTTI names/base arrays. Three typeinfo objects occupy `0x804B8430–0x804B8448`. These are boundary hypotheses pending complete object, exception-table, and linked-layout proof; no split has been registered.

The verified executable's exception index identifies nine retained font entries: constructor, base destructor, derived destructor, the five conversion methods, and the depth guard destructor. Their contiguous index interval is `0x8005199C–0x80051A08`; their exception tables occupy `0x8003E9BC–0x8003EB10`. GetEncoding needs no entry. This establishes the native interval, not candidate exception-table identity. The outside `336-font-extab.py` inventory reads the verified linked ELF and candidate object independently.

## Native font layout and behavior

| Offset | Native evidence |
| --- | --- |
| `0x00–0x1B` | Canonical SDK RomFont base |
| `0x1C–0x5F` | Embedded encoding converter; native initialization calls `0x801CA450` |
| `0x60` | FontEncoding, initialized to UTF16 and returned by GetEncoding |
| `0x64` | Signed recursive conversion depth |
| `0x68` | Owned font buffer pointer, held through a shared_ptr<void> view |
| `0x6C` | Shared-owner control pointer |

The converter uses the canonical owner-reviewed `CEncodingConverter` interface from #941: size `0x44`, two ENC contexts followed by three member-function pointers. Its class spelling is descriptive; the enclosing `CAnyEncFont` spelling is recovered RTTI. The font's complete size is `0x70`.

SetAlternateChar, GetCharWidth, GetCharWidths, GetGlyph, and HasGlyph call the underlying RomFont implementation directly when the signed conversion depth is positive. Otherwise a scoped guard increments the depth, converts through the converter's member-function pointer at offset `0x38`, calls the SDK implementation, and decrements the depth. The retained guard destructor at `0x801DA070` also restores the depth during exception cleanup. The candidate guard name is descriptive; no original guard name has been recovered.

The constructor allocates a 32-byte-aligned MEM2 buffer of `RomFont::GetRequireBufferSize()`, installs GameMemory::Free as its deleter, loads the font, and initializes UTF16 conversion. Native deleter RTTI identifies a `shared_ptr_deleter<unsigned char, void (*)(void *) throw()>`; it proves the allocated object type used by that control block. It does not, by itself, uniquely identify the enclosing shared_ptr's template parameter. The ordinary shared_ptr<void> owner view, reset from an unsigned-char allocation, produces the exact native constructor where a shared_ptr<unsigned char> member leaves seven register differences. This is strong compiler evidence for the erased owner view, rather than a claim that RTTI uniquely encodes that member type.

## Independent instruction checks

| Address | Function identity | Native bytes | Result |
| --- | --- | ---: | --- |
| `0x801D9B40` | CRomFont constructor | 320 | Exact with shared_ptr<void> owner view |
| `0x801D9C80` | CAnyEncFont<RomFont> destructor | 88 | Exact |
| `0x801D9CE0` | CRomFont destructor | 180 | Exact |
| `0x801D9DA0` | SetAlternateChar | 128 | Exact |
| `0x801D9E20` | GetCharWidth | 128 | Exact |
| `0x801D9EA0` | GetCharWidths | 156 | Exact |
| `0x801D9F40` | GetGlyph | 148 | Exact |
| `0x801D9FE0` | HasGlyph | 128 | Exact |
| `0x801DA060` | GetEncoding | 8 | Exact |
| `0x801DA070` | Scoped conversion-depth guard destructor | 80 | Exact |

The rejected shared_ptr<unsigned char> owner candidate differs at constructor offsets `0x34`, `0x38`, `0x68`, `0x84`, `0x90`, `0x9C`, and `0xB8`, exchanging r29/r30 for the constant one and allocated buffer pointer. Wii/1.1 and Wii/1.3 both reproduce that gap. The void owner view removes every difference under the existing Wii/1.1 game flags, without changes to instruction comparison or callee targets.

The reproducible outside investigation uses `336-probe-void-owner.py`, `336-compile-romfont.py`, `336-audit-romfont.py`, and `336-reference/romfont-void-native-proof.json`. The audit resolves each relocation to a native address and compares the resulting instruction words. The void-view experiment needs a normal internal reference-type trait because the current canonical shared_ptr forms the invalid type void& unconditionally. That bounded standard-library extension requires owner review and full existing-consumer preservation before publication. These experiments are separate from the build pipeline, so no source-selected or frozen-DOL acceptance is claimed.

The bounded canonical shared_ptr prerequisite was subsequently accepted in PR #1022, merged as `a1442a6146`, after owner review, unchanged SHA-256 values for all 705 current source objects, and frozen-DOL verification. See [F-336-2](336-shared-ptr-void.md). The ten-function native instruction probe passed again after rebasing onto that merge. No font source selection was changed.

## Traps

- A matching custom shared-owner RTTI identifies the allocated/deleter specialization, not necessarily the public shared_ptr view. Keep those two type claims separate.
- Identical empty virtual-destructor instructions do not establish class identity. The initial scratch association of `0x801DA0C0` with shared_ptr_deleter_common was rejected: the actual canonical common destructor is already at `0x80055BB0`, and independent callers plus RTTI prove IUIListItem here. Remove that false association before choosing file boundaries or naming symbols.
- Assigning a newly constructed shared_ptr adds retain/release work absent from the native constructor. The normal reset operation recovers the native size; direct temporary swap produces the same seven remaining register differences. A named temporary extends cleanup lifetime and makes the constructor 24 bytes too large.
- The base FontEncoding member must be read when calling InitReaderFunc in the candidate SetEncoding implementation. Passing the formal parameter produces an extra constant instruction in this constructor. Both forms have the same API behavior, so this observation supports the measured implementation rather than a new interface or compiler flag.
- Per-function agreement cannot establish retained weak-function order, RTTI layout, exception-table coverage, or source membership. All still require actual full-unit and executable verification before Matching selection.
