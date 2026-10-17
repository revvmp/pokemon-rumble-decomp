# RVL_SDK ledger

One row per translation unit. Status is `Matching`, `NonMatching`, or `Equivalent`. See [the ledger guide](README.md).

| Unit | Address ranges | Source | Library and flags | Status | Evidence | Remaining differences and scratches | Provenance |
| --- | --- | --- | --- | --- | --- | --- | --- |
| CNT (`cnt.c`) | `.text` `801778F0–80178530`; `.rodata` `803667C8–80366A40`; `.data` `803B9E60–803B9FF0`; `.sdata` `804B80F0–804B80F8`; `.sbss` `804B9100–804B9108` | `src/RVL_SDK/cnt/cnt.c` | Wii/1.0, `cflags_base`, default 16-byte function alignment | NonMatching | [F-286-1](../re/findings/286-cnt.md); split-only full build and verifier pass | Three of twelve functions implemented: init, shutdown, length. Nine functions and complete data remain; all CNT still links from original fallback | Independently written from target disassembly; public RB3 interface used only to corroborate names |
| `RVL_SDK/base/PPCArch.c` | `.text` `0x802B1800`–`0x802B1A10`; `.data` `0x803C7280`–`0x803C72B8` | `src/RVL_SDK/base/PPCArch.c` | RVL_SDK, Wii/1.0, `cflags_sdk`, default alignment | `Matching`, linked | objdiff 23/23 functions 100%; full verification passes; F-463-1 | None. `PPCMfwpar` is dead-stripped by the linker | Follows Petari CC0 `src/RVL_SDK/base/PPCArch.c` at `e0ab141`; headers in `include/revolution/` written for this project after Petari's |
