# CHPBar and CIconCollection

Issue 138 owns the complete CHPBar unit at `0x800BE9E0`–`0x800C0490` and complete CIconCollection unit at `0x800C0490`–`0x800C0C58`. Its original inventory is 31 functions and 8,656 instruction bytes. Neither prerequisite evidence nor one complete unit closes that inventory.

## F-138-1: native icon producer

An outside complete class declaration and ordinary C++ implementation reproduce all thirteen CIconCollection function extents, all 1,928 instruction bytes after every relocation is resolved, eleven exception records totaling 344 bytes with their cleanup targets, and 43 retained typed data/BSS/SDA atoms. The scratch source, reference-header overlay and independent code/data/EH receipts are under `C:/MassStorage/GitHub/138-reference/`. These are diagnostic producer proofs; source selection and the frozen whole-DOL gate remain required.

The original anonymous class name and file name are pinned by the deleter RTTI. It implements the four-slot IIconCollection interface without a virtual destructor and contains CLayout members at 4 and 236, giving an overall size of 468. Its two pointer tables contain five controller types, respectively two button names and four controller/channel names. Native pointer-table entries are mutable; const-qualified entries change load scheduling in the type/channel getter. The getter uses the const Material texture-array overload, independently pinned by its native target `0x802AD2D0`.

Both controller-pointer overloads query the controller type. If extension-controller input is disabled and the type is Wii Remote, they dynamically cast to CControllerWiiRemote and read its existing Boolean at offset 52. A set flag selects the Classic icon type. The additive inferred spelling `UsesClassic() const` exposes that already complete field without changing its layout, virtual table or existing methods. Its inline implementation exactly reproduces both native icon consumers.

On current base `5a5b22116b2b480095b034fdc42a8827211bfad9`, the same-base control at `594fab556596425690ddc55b946d205fae889528` builds all 751 existing source objects before and after the getter. Of those, 749 whole objects are identical; two differ only in numerical compiler-local symbol names. All allocated sections, raw relocation records and named symbol attributes remain identical. The linked ELF differs only in numerical labels, and both DOLs are raw-identical to the frozen executable. The guard repeats these checks inside the official landing verifier after every rebase.

## F-138-2: HP damage-object contract audit

The existing canonical CHPBar declaration is 316 bytes (`0x13C`). Its constructor and list destructor were independently proved during issue 54. The original Update also allocates a nested CDamage object of 268 bytes. Observed fields are an integer at zero, a CLayout at four, and a shared pointer to a motion program at 236. Current constructor, destructor, Update and CalcView traces do not explain the trailing 24 bytes. No opaque tail, fabricated member type or fixed-size allocation substitute is proposed. This contract must be recovered before claiming the complete CHPBar class.

## F-138-3: complete source-selected icon acceptance

The canonical private class header declares each controller-pointer overload before its type/index overload. Base-interface slot order stays unchanged, because every derived method overrides an existing slot. That derived declaration order reproduces the original deferred inline-function placement: pointer-button at `0x800C0680`, type-button at `0x800C0740`, pointer-controller at `0x800C0850`, and type-controller at `0x800C0930`. The initial declaration in base slot order reproduced every function separately but reordered these four retained functions in the complete link; that diagnostic was rejected.

The split-only default build first reproduced the frozen DOL. The actual `Matching` source-selected icon object then reproduced the full frozen DOL after the derived declaration correction. `configure.py --map`, Ninja and the explicit frozen verifier exit zero. All thirteen retained functions score 100% in unitdiff. This accepts the complete icon unit locally; the eighteen CHPBar functions, 6,728 original code bytes, remain incomplete, so issue 138 is not complete.

The icon extents are `extab` `0x8001C7B0–0x8001C908`, `extabindex` `0x800489D8–0x80048A5C`, `.text` `0x800C0490–0x800C0C58`, `.ctors` `0x803529D8–0x803529DC`, `.data` `0x803A45D0–0x803A4934`, `.bss` `0x803E6620–0x803E6630`, `.sdata` `0x804B6A10–0x804B6A30`, `.sbss` `0x804B9018–0x804B9020`, and `.sdata2` `0x804B9F30–0x804B9F38`. The global-registration node is twelve actual bytes with four native inter-object alignment bytes in its linked BSS extent; the floating literal is four actual bytes with four linked alignment bytes. No padding member or executable data array is added.

The bounded remote getter merged separately through PR1167 as `7eef978e21`, from exact verified head `22cc3cec98b160d9141aa5e10e92759f1c2d1e70`. Its official landing gate repeated the same-base control over all 751 existing source objects: 749 whole-identical and two differing solely in numerical compiler-local names, with every allocated byte, raw relocation and named symbol attribute unchanged and the frozen DOL raw-identical. This source-unit change therefore contains no shared controller-header edit. The icon gate repeats exact retained addresses, sizes and bytes, strict MAP columns, source-only Ninja graph checks and frozen verification after each rebase. Issue 138 stays open for the separate CHPBar unit.

## Traps

- Const-qualified pointer entries can change instruction scheduling even when their string values agree. The mutable table declaration reproduces the native controller getter without forced registers or source barriers.
- Opcode-only comparison concealed a wrong Material overload target. Resolving every relocation exposed the non-const texture-array call; a normal const Material pointer local selects the original overload with unchanged instructions.
- Complete class declarations must preserve first declaration order. Moving the anonymous class declaration before the controller headers moves deferred RTTI groups. Keeping shared declarations first preserves the native retained data ordering.
- Independently exact functions can be ordered incorrectly in the linked unit. The derived override declaration order, rather than the fixed inherited virtual slots, controls these overloaded inline groups; see F-138-3.
