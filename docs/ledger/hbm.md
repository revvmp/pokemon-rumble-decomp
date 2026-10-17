# Home Button Menu ledger

One row per translation unit. Status is `Matching`, `NonMatching`, or `Equivalent`. See [the ledger guide](README.md).

| Unit | Address ranges | Source | Library and flags | Status | Evidence | Remaining differences and scratches | Provenance |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `RVL_SDK/hbm/HBMFrameController.cpp` | `.text` `0x80178530`–`0x801786DC`; `.rodata` `0x80366A40`–`0x80366A48`; `.data` `0x803B9FF0`–`0x803BA018` | `src/RVL_SDK/hbm/HBMFrameController.cpp` | `homebuttonLib`, Wii/1.0a, `cflags_hbm` | Matching, linked | [F-287-1](../re/findings/287-hbm-core.md#f-287-1-hbm-compiler-flags-and-first-unit-boundaries); objdiff 3/3 functions 100%; full DOL verifies | None | xenoblade `ea25691b`, CC0 |
| `RVL_SDK/hbm/HBMAnmController.cpp` | `.text` `0x801786E0`–`0x801787F0`; `.data` `0x803BA018`–`0x803BA060` | `src/RVL_SDK/hbm/HBMAnmController.cpp` | `homebuttonLib`, Wii/1.0a, `cflags_hbm` | Matching, linked | [F-287-1](../re/findings/287-hbm-core.md#f-287-1-hbm-compiler-flags-and-first-unit-boundaries); objdiff 3/3 functions 100%; full DOL verifies | None | xenoblade `ea25691b`, CC0; `nw4hbm` headers per [F-287-2](../re/findings/287-hbm-core.md#f-287-2-home-button-menu-layout-library-version) |
