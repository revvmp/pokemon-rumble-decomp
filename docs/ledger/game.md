# Game code ledger

One row per translation unit. Status is `Matching`, `NonMatching`, or `Equivalent`. See [the ledger guide](README.md).

| Unit | Address ranges | Source | Library and flags | Status | Evidence | Remaining differences and scratches | Provenance |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `game/BController.cpp` | `extab` `0x800066C0–0x800066C8`; `extabindex` `0x80044220–0x8004422C`; `.text` `0x80052500–0x800528D8`; `.data` `0x80399380–0x80399440`; `.sdata` `0x804B5A20–0x804B5A40`; `.sdata2` `0x804B9C60–0x804B9C88` | `src/game/BController.cpp` | Game, Wii/1.1, `cflags_game` | Matching, linked | [F-735-1](../re/findings/735-game-compiler.md); objdiff 19/19 functions 100%; full DOL verification | None; member and method names are descriptive | Original reconstruction from RTTI, vtables and callers |
| `game/CDlgPiiList.cpp` | `extab` `0x800102D0–0x80012F68`; `extabindex` `0x800463B0–0x80046AD0`; `.text` `0x800896E0–0x80092EB0`; `.data` `0x8039DF80–0x8039F518`; `.sdata` `0x804B6360–0x804B64A8`; `.sbss` `0x804B8FD8–0x804B8FE0`; `.sdata2` `0x804B9DE0–0x804B9DF0` | None yet | Game, Wii/1.1, `cflags_game` | NonMatching, split only | [F-82-1](../re/findings/82-cdlgpiilist.md); split build verifies | All 241 functions unreconstructed; spans #82 to #91 | Boundaries from RTTI, extabindex and data references |
