# NW4R ledger

One row per translation unit. Status is `Matching`, `NonMatching`, or `Equivalent`. See [the ledger guide](README.md).

| Unit | Address ranges | Source | Library and flags | Status | Evidence | Remaining differences and scratches | Provenance |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `nw4r/ef/ef_draworder.cpp` | `.text` `0x80225820–0x80225940`; `.data` `0x803C52D0–0x803C52E8` | `src/nw4r/ef/ef_draworder.cpp` | NW4R.ef, Wii/1.1, `cflags_nw4r` | Matching, linked | [F-375-1](../re/findings/375-ef-draworder-effect.md); objdiff 3/3 functions 100%; full DOL verification | None | CC0 ogws `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` |
| `nw4r/ef/ef_effect.cpp` | `.text` `0x80225940–0x80226544`; `.data` `0x803C52E8–0x803C5308`; `.sdata2` `0x804BA5E0–0x804BA5E8` | `src/nw4r/ef/ef_effect.cpp` | NW4R.ef, Wii/1.1, `cflags_nw4r` | Matching, linked | [F-375-1](../re/findings/375-ef-draworder-effect.md); objdiff 15/15 functions 100%; full DOL verification | None; 2009 changes to `CreateEmitter`, `Calc`, and `Emitter` layout recorded in the finding | CC0 ogws `f5a1bb5ed7692a9e09e737bc1236f592274dcf73` |
