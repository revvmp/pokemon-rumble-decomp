# F-59-2: Inherited ICalc matrix slot

The original ICalcDraw declaration predates the completed ICalc contract. It redundantly redeclared CalcMtx, even after #991 established the concrete base method. The #56 combo display subsequently consumed that existing declaration and named the empty function at `0x80063DC0` as ICalcDraw::CalcMtx. That producer spelling was inferred from the header rather than independent native override evidence.

CDisplayPhaseCtrl supplies stronger evidence: its native RTTI identifies ICalc directly, its vtable's third slot points to `0x80063DC0`, and its independently reconstructed complete source reproduces native code, EH, data and frozen DOL when the target is ICalc::CalcMtx. Keeping the redundant derived override creates a second retained base function and breaks the frozen DOL when both units are source selected. The correction removes only the redundant ICalcDraw override and gives the existing target its base-method identity. Virtual slot order and all member layouts remain unchanged.

The bounded prerequisite compares every current declared source object before and after the declaration correction. Allocated bytes, section types/flags/lengths/alignment and sorted relocation identities remain exact after pairing only the two spellings of this same native method, except objects that previously emitted both empty overrides. Those lose one duplicate four-byte function and its twelve bytes of function alignment. Every remaining function's bytes and actual relocation identities are preserved, and every nontext allocation is exact. Every linked allocated ELF section and the frozen DOL remain byte-identical. Existing CComboDisplay and CEnemyInfo/Facility consumers are included. No outside source body, split or unit status is changed. Detailed receipts are under `C:/MassStorage/GitHub/59-prereq-controls/`.

## Traps

A derived class declaration can retain a redundant override after a base contract is corrected. A matching empty body alone cannot distinguish inherited versus overridden ownership. Compare actual native base RTTI and consumers that instantiate the base method before assigning the shared function's class identity.
