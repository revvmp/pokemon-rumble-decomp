# F-809-1: Layout and Picture source boundaries and native probes

Issue #809 lists 28 retained functions / 5,012 bytes: eighteen Layout functions starting at `0x802A6240` and ten Picture functions at `0x802A7110`–`0x802A768C`. The actual Layout unit begins with the 468-byte SetTagProcessorImpl helper at `0x802A6060`, owned by #808. Owner #808 approved exclusive coordinated whole-Layout preparation here, retaining ownership and requiring exact range/header review before publication. This delivery must use Part of #808 for that helper, and never count its source twice or close the remaining #808 inventory prematurely.

## References and canonical interfaces

The original Layout/Picture source and headers are from CC0 zeldaret/ss `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`; root license and file-level notices were inspected. Original library reference code is diagnostic evidence, not proof that another revision's class layout applies here. Existing canonical Layout is 0x20 and must remain unchanged in storage and virtual order. Owner-approved external Material and Animation overlays from #808/#812 are used only for scratch probes until their bounded canonical prerequisites merge.

Picture's speculative unused one-argument constructor body in the reference is omitted from the candidate. The retained resource constructor, destructor, Append, color accessors, drawing, type-info accessor and static initializer all have direct native proof. The original typed Size equality helper is needed; it compares width and height. Canonical Material texture APIs are coordinated with the owner rather than redeclared locally.

## Complete Picture probe

All ten retained Picture functions / 1,336 bytes match every native instruction after applying 38 independently decoded text relocations. Thirty vtable and constructor-table relocations also match, for 68 paired sites total. All 124 native vtable bytes at `0x803C6F38` and the float at `0x804BB188` match. Four-byte typeInfo storage is at `0x804B9418`; the constructor-table pointer at `0x80352A88` targets the native twelve-byte initializer at `0x802A7680`. The unit's text ends at `0x802A768C`, not the next aligned function start.

The canonical compiled Pane is 0xD8; Picture's four colors begin at 0xD8 and its eight-byte TexCoordAry at 0xE8, giving a 0xF0 class. The matching constructor and all consumed member accesses, plus Layout's matching Picture allocation immediate, establish those offsets. This is scratch instruction/data evidence; source-selected split/link/frozen acceptance is still required before Matching.

## Layout probe and remaining evidence

Sixteen of nineteen whole-Layout functions currently match all non-relocation instruction words and sizes, including the 0x360-byte Build and 0x29C-byte BindAnimationAuto. The 0x20 class storage is unchanged. The first helper and SetTagProcessor each require the target's newer TextBox interface; BuildPaneObj differs in one immediate, allocating native TextBox size 0x140 rather than the reference header's 0x104. Native SetTagProcessor stores its pointer at 0xF8 and sets flag 0x8 in the word at 0x104. Owner #808 independently established the same newer class evidence and is investigating its remaining semantic members. No guessed padding, substitute class or storage shim is introduced.

The target DynamicCast path includes a null guard missing from main's bounded RuntimeTypeInfo template. A diagnostic additive null guard restores the corresponding first native sequence, but a canonical shared-header change awaits complete consumer proof, baseline comparison and owner/root approval. Additive Pane parent/child-list accessors, one-argument Layout NewObj and primitive DeletePrimArray helpers are kept in scratch pending the same review. Owner #812 independently verified its native animation destructor with the original DeletePrimArray helper.

## Traps

- Picture's compiler-generated static initializer shares its basename with the HBM private Picture unit. Preserve the compiler-proven local symbol scope to avoid an exported-name collision; the two anonymous Layout helpers are also local. The native linker-name gate below passes with those scopes.

- New worktrees have no default DOL. A custom-directory Ninja link can produce its actual DOL and then fail the SHA-list CHECK because it still references `build/wpse-v1/main.dol`. The initial custom baseline exited one at that CHECK; it is not a passing baseline. Run the real default build and inspect its successful exit and frozen artifact before proceeding. Never copy an output into the expected path or edit expected hashes.
- Whole-reference TextBox declarations are too old for this target. Native allocation size and cache-invalidating setter instructions prove a real revision difference. Keep Layout scratch work intact while the owning issue establishes the full canonical type; do not fill the unknown tail with guessed fields or padding.
- Macro RoundUp/RoundDown from allocator headers can collide with the NW4R algorithm template declarations. Include the canonical algorithm header before the allocator-using Layout header, as the existing consumers do; no compiler flags or alternate algorithm definitions are needed.

The subsequent real default baseline (`python configure.py`, plain Ninja and explicit frozen verification of `build/wpse-v1`) each exited zero on main `7a1a3b81`; 479 baseline source-object digests were captured before source/header edits.


## Primitive Layout deletion prerequisite

The first-owner review approved the exact six-line DeletePrimArray template added in commit `7903932717f2b57dffe26ed4bf147c18b3fbe9d8`: null-check the pointer and call existing FreeMemory, without invoking element destructors. Existing Layout API, storage and virtual order are unchanged. Owner #812 independently matches its full native 84-byte AnimTransformBasic destructor with this helper. Configure, plain default Ninja and frozen verification all pass with the actual tracked helper. Of 479 baseline source objects, 476 are whole-file identical; the remaining three renumber compiler-local metadata. Every allocated section's type, flags, size, alignment and bytes and every canonical relocation target/addend are unchanged in all 479 objects. The prerequisite is folded into shared header PR #857 with owner/root approval, so the later Layout source must drop its duplicate after that merge.


## Native TextBox cache roles established

Independent inspection of the native function at `0x802A91D0` resolves the previously unknown six-float tail. At `0x802A923C`–`0x802A9274`, it compares Pane size width/height (`0x4C`/`0x50`) against fields `0x118`/`0x11C`; changed values are copied into those fields and dirty flag `0x80` is set in the word at `0x104`. These two floats form a cached Size. At `0x802A9288` the routine takes the address of the four floats starting at `0x108`, computes rectangle edges, and at `0x802A9498`–`0x802A94B4` copies them into the hidden sixteen-byte Rect return object. Fields `0x108`–`0x117` therefore form a cached ut::Rect, not three unrelated float pairs together with the Size. The call at `0x802A8040` constructs a TextWriter and invokes this routine with return/this/writer arguments, supporting the GetTextDrawRect identity. Owner #808 independently accepted these field roles.

Together with the owner's established old members through `0x103`, cache flags at `0x104`, border colors at `0x120`/`0x124`, border width at `0x128`, GDLObj at `0x12C`–`0x13B` and display-list buffer pointer at `0x13C`, this establishes the full native 0x140 storage without guessed tail padding. The earlier remaining-layout note records the initial diagnostic checkpoint; canonical header review and complete Layout relocation/source-link proof remain required. Owner #808 prepares that header and validates the TextBox methods before Layout publication.


## Complete Layout consumer and shared RTTI proof

Owner-approved TextBox header commit `44fbc117ada6cce9e82f096871b5c29ad1bae1b0` supplies the exact complete 0x140 declaration. With that unchanged declaration, all nineteen Layout functions / 4,144 bytes now match every native instruction after applying 84 independently decoded text relocations. All fourteen vtable relocations and 64 native vtable bytes at `0x803C6EF8`, the two float constants at `0x804BB180`/`0x804BB184`, and four-byte allocator storage at `0x804B9410` are exact: 98 paired sites total. Together with Picture this is 29 functions / 5,480 bytes and 166 paired sites. The original #809 inventory remains 28 functions / 5,012 bytes; the extra 468-byte first Layout helper is accounted under #808 ownership, once.

The needed RuntimeTypeInfo null guard is already approved in PR #856. Its exact local header at commit `54af4f5cfd91a398f50dd6eb18005a6ce4f30b2b` is text-identical to the diagnostic proposal, and the complete nineteen-function Layout proof passes using that header unchanged. No duplicate canonical guard edit is published here. Independent global diagnostic validation parsed the actual Ninja dependency graph, identified all twelve RuntimeTypeInfo consumers on the verified `7a1a3b81` baseline, and recompiled each with its exact pinned Ninja compiler command plus an isolated guard include overlay. All 479 existing objects preserve every allocated section's type, flags, size, alignment and bytes, and all canonical relocation targets/addends; 476 also preserve whole-file hashes. Unaffected objects are verified against their captured baseline hashes. This is a dependency-scoped compiler diagnostic, supplemented by the baseline's real Ninja/frozen gate; final source-selected integration still requires its own complete current-main gate.

The complete TextBox interface is coordinated into PR #857 alongside the animation and primitive-deletion prerequisites. Layout/Picture source, splits, final native extents, all allocated sections and actual source-only link inputs must still be verified before marking either unit Matching. No source progress is claimed from the diagnostic compiler outputs alone.


## Split-only native extent verification

The complete Layout and Picture section splits pass configure, plain default Ninja and explicit frozen verification (all exit zero) before either source object is registered. The survey sizes included alignment padding after Picture's vtable (0x80 versus actual 0x7C), the two four-byte allocator/type-info allocations and Picture's single float. dtk correctly rejected the cuts through the overlong survey symbols. Trimming only those four owned extents to the independently proved native allocations permits the real link and reproduces the frozen DOL. The split ends remain 0x802A7108 and 0x802A768C, leaving following function alignment to the next unit.

Owner #812 independently repeated the complete nineteen-function Layout consumer audit against its actual tracked PR #857 headers after the exact owner-approved TextBox declaration was folded: 4,144 code bytes, 84 text and fourteen data relocation sites, 64 vtable bytes, eight constant bytes and four allocator BSS bytes all match. This supplements the independent scratch proof; source-selected acceptance remains pending.


## First-owner publication review

Owner #808 reviewed and explicitly approved the exact additive Layout/Pane header diff and complete Layout source on 2026-10-17: resource Layout record, one-argument NewObj, GetParent and GetChildList. All existing class storage, virtual order and templates remain intact. The reviewed native ranges are text 0x802A6060–0x802A7108, data 0x803C6EF8–0x803C6F38, sbss 0x804B9410–0x804B9414 and sdata2 0x804BB180–0x804BB188. The shared first helper stays accounted once under #808. Bounding must use the owner's complete canonical header from commit 273bbc80, already backed by five native methods and a frozen build; no parallel class is introduced.


## Native linker names and private initializer scope

Fourteen name-only changes connect the native Layout/Picture identities and their approved TextBox/Bounding callees. Compiler ELF symbol binding marks both anonymous Layout helpers and the generated Picture initializer local. The initializer basename is also used by the HBM private Picture unit, so retain local scope on all three owned private functions rather than exporting a colliding static initializer. No outside size, type or split is changed. Configure, plain default Ninja and explicit frozen verification again exit zero with these names and scopes. Final source-selected gates remain pending.
