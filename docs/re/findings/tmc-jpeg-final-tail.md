# TMC JPEG final tail

## Scope and validation

The 2026-11-04 continuation owns `src/TMC_JPEG/tmcjpeg_rgba8.c` and `src/TMC_JPEG/tmcjpeg_rgb565.c`. It starts from the three remaining functions recorded in [the converter findings](764-tmc-jpeg-converters.md): both setup routines at 99.830505% and `ConvertRGBA8MCU411` at 77.57962%. The preceding honest reconstructions and unsuccessful source controls remain relevant; this session does not replace that history.

All retained candidates use the configured Wii/1.0 C compiler command, copied from the existing Ninja graph and redirected into isolated ignored outputs. The source basename is preserved. No compiler flag, shared header, symbol configuration, original artifact, or object byte is changed by this work. Every function in each complete unit is compared. Full linkage and frozen-artifact verification belong to the coordinator and remain a separate acceptance step.

## Setup operand order solved with C values

Both setup routines now compare at 100%, each retaining its original 236-byte extent:

| Function | Original address | Direct comparison |
| --- | --- | --- |
| `TMCJPEGSetupRGBA8` | `0x801F9764` | 236 bytes, 100% |
| `TMCJPEGSetupRGB565` | `0x801FA424` | 236 bytes, 100% |

The former remaining difference was the operand pair at function offset `0xD0`: the target has `add r0,r0,r5`, while the closest scalar expression produced `add r0,r5,r0`. The scalar carry-first expression generated the correct surrounding schedule. Writing the quotient first corrected the addition's algebraic order but changed the quotient's generation time, register assignment, and schedule.

The retained D-0016 construct is a source-local `TMCJPEGTileCount` struct containing one `int`. `MakeTileCount` returns that struct by value, and `AddTileCounts` takes two such values by value and returns `lhs.value + rhs.value`. Height alignment calls the addition with the quotient constructor first and carry constructor second. This gives the exact target instruction sequence without emitted helpers, stack storage, or additional sections. A C++ value-class experiment first exposed the useful lowering; the retained source is entirely C and uses no language or compiler-setting change.

The calculation is unchanged for every input height: the unsigned halfword gives a quotient in 0 through 16383 and a carry in 0 through 1. Their sum and multiplication by four fit the original destination. The wrapper changes compiler lowering, not arithmetic values or memory accesses. It is not asserted to be original developer source and must be listed in the [fake-match ledger](../FAKEMATCHES.md) under D-0016.

The whole RGBA8 unit retains its 3264-byte `.text` section, exactly seven emitted functions, and no data sections. Its setup and five previously exact converters compare at 100%; its remaining 4:1:1 converter stays at the initial 77.57962%. The whole RGB565 unit retains its 3084-byte `.text` section, exactly seven emitted functions, no data sections, and seven 100% comparisons. No other function changes its score or size.

The coordinator subsequently marked RGB565 `Matching`, completed the native full build, and obtained PASS from the frozen `main.dol` verifier on 2026-11-04. The refreshed native report confirms both setup routines at 100% with no regression among previously exact functions. RGBA8 remains a fallback unit because its 4:1:1 converter is unfinished; its setup result is an exact object comparison, not an independent source-linked unit acceptance.

Reproducible ignored evidence is under `tools/local/tail-20261104/`: `jpeg_setup_cppvalue.py` exposed the C++ control, `jpeg_setup_cvalue.py` found the C equivalent, and `jpeg_setup_clean.py` produced the retained helpers. The final complete-unit source, object, compiler log, and objdiff JSON are in each `TMC_JPEG_tmcjpeg_rgba8/jpeg_retained_setup/` and `TMC_JPEG_tmcjpeg_rgb565/jpeg_retained_setup/` directory.

## Additional rejected controls

Before the value-return solution, this session tested joint width/height temporary lifetimes, scalarized aggregate views, wide integer views, intrinsic identities, loop unrolling, inline lowering controls, backend optimization controls, parameter reuse, full-width bitfield records, neutral arithmetic, and statement expressions. None completed setup. Failed compilations and unsupported controls are excluded from binary conclusions. The compiler trace used an isolated Frida installation and the existing Wii/1.1 trace addresses, not Wii/1.0 addresses; final comparisons always used the configured Wii/1.0 compiler.

The lost 88.01911% RGBA8 4:1:1 scratch candidate was reconstructed from the prior findings. Redundant unsigned row-product views, following the accepted 4:2:2 control, reached 88.178345% but did not match. These partial variants are not retained in production source.

The private C permuter was restored at upstream revision `8556c81d80d1c1af98a858c8f4dc951357f29139` under ignored `tools/local/tail-20261104/jpeg-permuter/`. It is experiment tooling only; no upstream source is incorporated into the reconstruction. The native compiler adapter replaces only the selected function inside the full original unit, preserving the real header layout and source basename. A constrained 4000-candidate run reproduced the preceding best search score of 1590 from 1615. Those are search scores, not objdiff percentages or acceptance evidence.

## Traps

- A by-value scalar helper is not equivalent as a compiler control to a helper returning a one-field struct followed by a by-value struct addition. The latter completed both setup routines after the former and direct aggregate locals had failed. Preserve the two-stage value construction when reducing this control.
- A lower permuter score can come from wrong arithmetic. A beam-search candidate masked a green-channel multiplication with `0xFF`, changing valid pixel values while improving the search score. That candidate and its descendants were rejected. Audit masks and narrowing conversions before accepting any numerical improvement; exact native comparison remains mandatory.
