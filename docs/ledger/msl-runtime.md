# MSL_C, runtime, and MetroTRK ledger

One row per translation unit. Status is `Matching`, `NonMatching`, or `Equivalent`. See [the ledger guide](README.md).

| Unit | Address ranges | Source | Library and flags | Status | Evidence | Remaining differences and scratches | Provenance |
| --- | --- | --- | --- | --- | --- | --- | --- |
| `Runtime/__init_cpp_exceptions.cpp` | `.text` `0x80340B24`–`0x80340B94`; `.ctors$10` `0x803529C0`; `.dtors$10`, `.dtors$15` `0x80352AA0`–`0x80352AA8`; `.sdata` `0x804B8F98` | `src/Runtime/__init_cpp_exceptions.cpp` | Runtime.PPCEABI.H, Wii/1.1, `cflags_runtime` with `-func_align 4` | `Matching`, linked | Full verification on two clean builds; F-0010 | None | Follows Petari CC0 source; headers written for this project |
