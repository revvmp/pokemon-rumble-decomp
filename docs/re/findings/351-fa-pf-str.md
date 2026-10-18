# F-351-1: FA `pf_str.c` completion and the start of the driver layer

Target: `wpse-v1`, frozen `orig/wpse-v1/main.dol`. Issue: #351 (continues #350).

`PFSTR_StrNCmp` (`0x801FB9D0`) and `PFSTR_ToUpperNStr` (`0x801FBAF0`) end `pf_str.c` at `0x801FBBDC`. With them all 11 functions are identical, and the unit is `Matching` and linked under Wii/1.0 `cflags_sdk` (F-350-1). `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0.

How FA's `PFSTR_StrNCmp` differs from VF:

- It also takes the narrow-string path when `target` is 3, the `p_mark` position.
- Its offset is added unscaled to narrow positions and as a 16-bit index to wide positions.

`PFSTR_ToUpperNStr` writes the terminator twice at the same address in the wide branch, once inside the branch and once after it, with no increment in between.

[F-351-3](351-driver-reconstruction.md) now reconstructs and accepts the rest of #351, `0x801FBBE0` to `0x801FC5A4`. The original investigation below identified FA's driver layer, the counterpart of VF `pf_driver.c`, in a different order:

1. Two volume-label copies at volume offset `0x1648`.
2. A label writer that reads and rewrites the root directory entry.
3. Two loops over 26 volume records of stride `0x1FAC`, with two records per unrolled `0x3F58` iteration, that set flag bits at offset `0x1F8C` (corrected by F-351-3).
4. Five flag accessors.
5. Init, finalize, mount, unmount, format, `lread` and `lwrite` wrappers. Each maps PDM error 21 to `0x1000` and records the driver error at volume set offset `0x44` and volume offset `0x1F84`.
6. Three disk-manager wrappers into the PDM layer.

The initial layout and unit-boundary blockers are resolved by the later primary FA map, independent stride evidence, complete canonical volume/PDM headers and actual split/source-linked acceptance in [F-351-3](351-driver-reconstruction.md). The old `0x3F58` record-size hypothesis was the unrolled two-record step. No original hash, compiler flag or shared volume layout is changed by this reconstruction.

## Traps

- **Statement order in a `for` increment list is visible in scheduling.** Trigger: `PFSTR_ToUpperNStr` emitted the count decrement before the pointer increment. Writing `num--, wp++` in the increment list matches the target. Evidence: F-351-1.
