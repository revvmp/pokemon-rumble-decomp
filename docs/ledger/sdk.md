# RVL_SDK ledger

One row per translation unit. Status is `Matching`, `NonMatching`, or `Equivalent`. See [the ledger guide](README.md).

| Unit | Address ranges | Source | Library and flags | Status | Evidence | Remaining differences and scratches | Provenance |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `RVL_SDK/base/PPCArch.c` | `.text` `0x802B1800`–`0x802B1A10`; `.data` `0x803C7280`–`0x803C72B8` | `src/RVL_SDK/base/PPCArch.c` | RVL_SDK, Wii/1.0, `cflags_sdk`, default alignment | `Matching`, linked | objdiff 23/23 functions 100%; full verification passes; F-463-1 | None. `PPCMfwpar` is dead-stripped by the linker | Follows Petari CC0 `src/RVL_SDK/base/PPCArch.c` at `e0ab141`; headers in `include/revolution/` written for this project after Petari's |
