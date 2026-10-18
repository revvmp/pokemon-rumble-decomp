# F-351-3: Driver and disk-manager unit reconstruction

Target: `wpse-v1`, issue #351. The original FA names and consecutive sizes in the pinned primary IPL FA map establish the complete driver unit at `0x801FBBE0` to `0x801FC544` (17 functions, 2,308 instruction bytes) followed by `pdm_dskmng.c` at `0x801FC550` to `0x801FC620` (five functions, 168 instruction bytes). The latter includes the two tail functions in #765. The preceding two #351 string functions are already accepted in `pf_str.c`.

## Split-only acceptance

Neither new unit has source registration at this checkpoint. `pf_driver.c` owns only its text. `pdm_dskmng.c` owns its text and `.bss` `0x803EAFA8` to `0x803EB89C`: the canonical `PDM_DISK_SET` is `0x8F4` bytes, and the symbol's prior `0x8F8` extent included the four following alignment bytes. Those bytes remain outside the unit. The next disk unit still starts its BSS at `0x803EB8A0`.

The primary map facts and native directions are detailed in [F-351-2](351-driver-label-interface.md); [F-765-1](765-fa-disk-manager.md) independently establishes the five-wrapper grouping and disk-set extent. Its callback stride evidence agrees with the now-canonical `PF_VOLUME` record size `0x1FAC` and 26 records. The old thirteen-record / `0x3F58` interpretation in F-351-1 is superseded: each of thirteen unrolled iterations handles two records. No volume layout is changed by this split.

`python configure.py --map`, plain `.venv/Scripts/ninja`, and the explicit frozen verifier each exit 0. The previous DOL was removed before building. Both new original split objects remain selected, and the resulting DOL matches the immutable manifest. No hash or compiler setting changes.

## Reconstruction state

Source registration and complete function acceptance follow this verified split checkpoint. No new source credit is claimed by the split alone.

## Traps

- An unrolled loop step is not a record size. The native `0x3F58` step covers two `0x1FAC` volume records; verify the second access and the independent indexed consumers before choosing a public layout.
- The disk-set global ends four bytes before the next unit. Retain only its proven `0x8F4` bytes and leave the next unit's alignment padding outside the owned split.
