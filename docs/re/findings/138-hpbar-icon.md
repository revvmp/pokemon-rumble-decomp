# CHPBar and CIconCollection

Issue 138 owns the complete CHPBar unit at `0x800BE9E0`–`0x800C0490` and complete CIconCollection unit at `0x800C0490`–`0x800C0C58`. Its original inventory is 31 functions and 8,656 instruction bytes. Neither prerequisite evidence nor one complete unit closes that inventory.

## F-138-1: native icon producer

An outside complete class declaration and ordinary C++ implementation reproduce all thirteen CIconCollection function extents, all 1,928 instruction bytes after every relocation is resolved, eleven exception records totaling 344 bytes with their cleanup targets, and 43 retained typed data/BSS/SDA atoms. The scratch source, reference-header overlay and independent code/data/EH receipts are under `C:/MassStorage/GitHub/138-reference/`. These are diagnostic producer proofs; source selection and the frozen whole-DOL gate remain required.

The original anonymous class name and file name are pinned by the deleter RTTI. It implements the four-slot IIconCollection interface without a virtual destructor and contains CLayout members at 4 and 236, giving an overall size of 468. Its two pointer tables contain five controller types, respectively two button names and four controller/channel names. Native pointer-table entries are mutable; const-qualified entries change load scheduling in the type/channel getter. The getter uses the const Material texture-array overload, independently pinned by its native target `0x802AD2D0`.

Both controller-pointer overloads query the controller type. If extension-controller input is disabled and the type is Wii Remote, they dynamically cast to CControllerWiiRemote and read its existing Boolean at offset 52. A set flag selects the Classic icon type. The additive inferred spelling `UsesClassic() const` exposes that already complete field without changing its layout, virtual table or existing methods. Its inline implementation exactly reproduces both native icon consumers.

On current base `5a5b22116b2b480095b034fdc42a8827211bfad9`, the same-base control at `594fab556596425690ddc55b946d205fae889528` builds all 751 existing source objects before and after the getter. Of those, 749 whole objects are identical; two differ only in numerical compiler-local symbol names. All allocated sections, raw relocation records and named symbol attributes remain identical. The linked ELF differs only in numerical labels, and both DOLs are raw-identical to the frozen executable. The guard repeats these checks inside the official landing verifier after every rebase.

## F-138-2: HP damage-object contract audit

The existing canonical CHPBar declaration is 316 bytes (`0x13C`). Its constructor and list destructor were independently proved during issue 54. The original Update also allocates a nested CDamage object of 268 bytes. Observed fields are an integer at zero, a CLayout at four, and a shared pointer to a motion program at 236. Current constructor, destructor, Update and CalcView traces do not explain the trailing 24 bytes. No opaque tail, fabricated member type or fixed-size allocation substitute is proposed. This contract must be recovered before claiming the complete CHPBar class.

## Traps

- Const-qualified pointer entries can change instruction scheduling even when their string values agree. The mutable table declaration reproduces the native controller getter without forced registers or source barriers.
- Opcode-only comparison concealed a wrong Material overload target. Resolving every relocation exposed the non-const texture-array call; a normal const Material pointer local selects the original overload with unchanged instructions.
- Complete class declarations must preserve first declaration order. Moving the anonymous class declaration before the controller headers moves deferred RTTI groups. Keeping shared declarations first preserves the native retained data ordering.
