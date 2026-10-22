# Sprint a02 findings

## BWorldCamera, CBattleRoyalBox, CBattleRoyalDisplay

- BWorldCamera.cpp is reconstructed and Matching. Two header facts made it link: `basic_string::~basic_string` is defined out of class, and `do_replace` computes the inserted length with `distance(first, last)`.
- CBattleRoyalBox.cpp and CBattleRoyalDisplay.cpp are new splits (`0x8006A550` to `0x8006B464`, `0x8006B470` to `0x8006BEE0`) with reconstructed sources, NonMatching.
- CBattleRoyalBox::ExportBattleRoyalData differs only in vector push_back and reserve trait selection for `SD_BattleRoyalData::Data`.

## Traps

- **`BWorldObj` virtual setters are header inlines.** `SetVisible`, `SetRotation`, `SetScale`, `SetPosition` and `GetDrawLayers` are emitted as weak functions in the first unit whose vtable needs them (BWorldCamera.cpp, BWorldLightSet.cpp). Defining them out of line in a .cpp shifts the whole DOL.
- **A zero in `.sdata` is a pooled literal, not a variable.** `lbl_804B5BD0` and `gEmptyText` in the display unit are string-pool entries; zero-initialised globals always land in `.sbss`. Mixed-mode linking of a Matching unit that owns such a pool fails with undefined `gEmptyText` from original objects.
- **Const vector iterators are class types in the original.** `CBattleRoyalBox::CBattleRoyalBox(const std::vector<Data>&)` keeps its iterators in memory, matching only when iterating through a non-const `std::vector<Data>::iterator`.

- **An in-class `basic_string` destructor is emitted early.** CodeWarrior emits an in-class inline destructor of a member immediately after any function containing `new T`. The original emits it at the end of the unit, so the destructor is defined out of class (`include/string`). This moved the weak-function order of BWorldCamera, CBattleRoyalDisplay (wstring) and CColorConst into line.
- **Unnamed original RTTI blocks weak deduplication.** `lbl_804B5AA8` and `lbl_804B5AB0` were the `std::length_error` and `std::logic_error` RTTI objects; without those weak names a source object's own copies were kept, growing `.sdata` by 8 and `.data` by 32 bytes.
- **String literals are pooled at parse time, deleter RTTI at code generation.** In CColorConst the original has the `shared_ptr_deleter` RTTI before the `"_c"` literal while the compiled object has it after, and CBattleRoyalDisplay and CDisconMsg show the same class of `.sdata`/`.data` order difference. The only remaining DOL mismatch of CColorConst is this 8-byte swap (all 17 functions match). Moving the literal into an inline helper changes the generated code, `-inline deferred`, `-ipa function` and `-ipa off` do not help.
