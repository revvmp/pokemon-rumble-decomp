# F-778-1: G3D draw shared interfaces

Issue #778 owns all thirteen retained functions in `g3d_draw.cpp`, from `0x80262880` through `0x80264720`, comprising 7,772 function bytes. The header prerequisite is combined with #774 in PR #816 by owner coordination. It adds the original draw replacement structure, public draw entry point, model draw-mode enum and MSL sorting facilities. Main's existing workmem, light, GPU and resource headers are preserved unchanged.

## Provenance and scope

The source and headers derive from zeldaret/ss commit `787b0b1e9d30e3c1d91ced61350b059ffc5a2411`. The root license is CC0-1.0; each reused file was inspected and has no separate file-level notice. Comments were removed. The bounded `g3d_scnobj.h` currently declares only the original draw-mode enum consumed by the draw source. It does not import unproven scene-object classes or take their source splits.

The MSL algorithm header retains the existing find/move/swap definitions and adds the original min_element, selection sort, sort132 and sort templates. The original never_inline attribute on sort132 reproduces the native retained helper. No existing definitions, flags, source registrations or frozen hashes change.

## Native evidence

A complete scratch compilation using the existing Wii/1.1 NW4R profile matches all thirteen native function sizes and instruction streams after applying all 103 independently decoded relocation targets. This includes both `std::sort` instantiations at `0x80263940` and `0x80263F00`, each `0x5BC` bytes, and the `0x260`-byte helper at `0x802644C0`.

`DrawResMdlReplacement` is `0x40` bytes: flag at zero, visibility array at four, then texture, TLUT, texture SRT, channel, generation mode, material miscellaneous, pixel display-list, TEV-color display-list, indirect matrix/scale display-list, texture-coordinate display-list, TEV, position, normal and color table pointers at consecutive four-byte offsets through `0x3C`. Native setup, replacement drawing and lighting accesses validate these types and offsets. The draw-mode enum preserves the reference spelling `REDMDL_DRAWMODE_SORT_NONE` and original bit values.

Main's canonical `workmem::MdlZ` is twelve bytes: float depth at zero followed by u16 priority, node, material and shape identifiers at four, six, eight and ten. Both native comparators, the sorting instantiations and draw loops independently exercise its stride and fields. Its existing temporary-buffer accessor remains unchanged.

The scratch object's two shuffle statics match native `0x804B8978` and `0x804B897C`, with initial value -4. The three consumed float constants match the twelve bytes at `0x804BAC98`. Header-only work receives no reconstruction-byte credit; source-selected final acceptance belongs to #778's source PR.

## Validation

`python C:/MassStorage/GitHub/778-audit-probe.py`, `778-infer-relocations.py` and `778-audit-native.py` exited zero. On main `994417bc26d0cd0c0f52db7660528fc830ac86bd`, the fresh baseline and expanded proposal both passed configure and Ninja. Explicit frozen verification of `build/774-header-expanded/wpse-v1` passed, its actual DOL equals the original, and all 449 existing compiled source objects retained identical SHA-256 hashes. A second draw probe using only the actual proposed and main include tree also matched all thirteen functions and 103 paired relocation targets. The merge gate repeats against latest main.

## Traps

The sorting templates emit six unreferenced integral constants into the scratch object after the three float constants. These are not additional native data allocations: final source selection must demonstrate linker removal rather than extending the native split over neighboring data. The source-selected acceptance will record the final retained data extent.
