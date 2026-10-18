# F-765-2: Complete original disk-manager issue acceptance

Target: `wpse-v1`, original issue #765. Both original functions are accepted from the complete disk-manager unit merged by its first-function owner in PR #970. This is an independent inventory and source-selection audit; it adds no implementation or source-byte credit.

## Original inventory

| Native address | Original size | Function | Source unit | Acceptance |
| --- | --- | --- | --- | --- |
| `0x801FC5B0` | `0x38` | `pdm_open_partition` | `pdm_dskmng.c` | 100%, exact ELF/map |
| `0x801FC5F0` | `0x30` | `pdm_close_partition` | `pdm_dskmng.c` | 100%, exact ELF/map |

## Containing unit

[F-351-3](351-driver-reconstruction.md) supplies `src/RVL_SDK/fa/pdm_dskmng.c`, its existing SDK header, source selection and the exact native boundaries. The unit spans text `0x801FC550` to `0x801FC620` and BSS `0x803EAFA8` to `0x803EB89C`. All five functions / 168 instruction bytes, the entire 208-byte padded text and 2,292-byte BSS are 100% in a fresh aggregate report and actual unitdiff. All five final ELF symbols have the original addresses and sizes, and all owning map contributions are the compiled disk-manager source object. The final `pdm_disk_set` symbol is exactly `0x8F4` bytes at `0x803EAFA8`; the four following alignment bytes remain outside this unit.

The original F-765-1 scratch result and pending authorization are historical. The first-function owner completed the full split/source/real-link acceptance in #970, including these two neighboring functions. This audit changes only documentation and preserves the existing original names, ABI, implementation, layout, source selection, compiler flags, original files and frozen hashes.

## Verification

The original issue's two addresses and sizes were independently joined to the fresh report. Both compare at 100% and each actual linked ELF/map row has the native address, size and expected source owner. The same checks cover all five native functions in the containing unit. `ninja -t query build/wpse-v1/main.elf` includes the compiled `src/RVL_SDK/fa/pdm_dskmng.o` input and excludes its original `obj/` fallback. The linked disk-set extent is checked independently in `readelf -sW` output. The final DOL is raw-byte-identical to the frozen reference, and the official frozen verifier passes.

Commands, each exit 0:

```
python configure.py --map
.venv/Scripts/ninja
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
python tools/re/unitdiff.py RVL_SDK/fa/pdm_dskmng.c
```

The previous DOL was removed before building; verification followed a known completed successful Ninja run. The outside `765-source-audit.py` additionally verifies the original inventory, final ELF/map/source inputs, storage extent and raw DOL, exiting 0. Latest-main acceptance is repeated through `tools/collab/land.py` before merging.

## Traps

No new matching trap emerged from this independent audit. The BSS-boundary and condition-direction lessons remain in F-351-3 and the historical F-765-1 evidence; no duplicate source credit is claimed.
