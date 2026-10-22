# Sprint a02 findings

## BWorldCamera, CBattleRoyalBox, CBattleRoyalDisplay

- BWorldCamera.cpp is reconstructed (36 of 38 functions exact) and left NonMatching. The out-of-line `basic_string` destructor is emitted directly after `CreateMotion(const char*)` in the compiled object but after the `BWorldObj` inline virtuals in the original, which shifts every following byte.
- CBattleRoyalBox.cpp and CBattleRoyalDisplay.cpp are new splits (`0x8006A550` to `0x8006B464`, `0x8006B470` to `0x8006BEE0`) with reconstructed sources, NonMatching.
- CBattleRoyalBox::ExportBattleRoyalData differs only in vector push_back and reserve trait selection for `SD_BattleRoyalData::Data`.

## Traps

- **`BWorldObj` virtual setters are header inlines.** `SetVisible`, `SetRotation`, `SetScale`, `SetPosition` and `GetDrawLayers` are emitted as weak functions in the first unit whose vtable needs them (BWorldCamera.cpp, BWorldLightSet.cpp). Defining them out of line in a .cpp shifts the whole DOL.
- **A zero in `.sdata` is a pooled literal, not a variable.** `lbl_804B5BD0` and `gEmptyText` in the display unit are string-pool entries; zero-initialised globals always land in `.sbss`. Mixed-mode linking of a Matching unit that owns such a pool fails with undefined `gEmptyText` from original objects.
- **Const vector iterators are class types in the original.** `CBattleRoyalBox::CBattleRoyalBox(const std::vector<Data>&)` keeps its iterators in memory, matching only when iterating through a non-const `std::vector<Data>::iterator`.
