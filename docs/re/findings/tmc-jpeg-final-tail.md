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

The retained D-0016 construct is a source-local `TMCJPEGValue` struct containing one `int`. `MakeValue` returns that struct by value, and `AddValues` takes two such values by value and returns `lhs.value + rhs.value`. Height alignment calls the addition with the quotient constructor first and carry constructor second. This gives the exact target instruction sequence without emitted helpers, stack storage, or additional sections. A C++ value-class experiment first exposed the useful lowering; the retained source is entirely C and uses no language or compiler-setting change. The initial setup-only candidate called this type `TMCJPEGTileCount`; the final cleanup gives it a general name because RGBA8 also uses it for color sums.

The calculation is unchanged for every input height: the unsigned halfword gives a quotient in 0 through 16383 and a carry in 0 through 1. Their sum and multiplication by four fit the original destination. The wrapper changes compiler lowering, not arithmetic values or memory accesses. It is not asserted to be original developer source and must be listed in the [fake-match ledger](../FAKEMATCHES.md) under D-0016.

At the setup-only stage, the whole RGBA8 unit retained its 3264-byte `.text` section, exactly seven emitted functions, and no data sections. Its setup and five previously exact converters compared at 100%; its remaining 4:1:1 converter stayed at the initial 77.57962%. The whole RGB565 unit retained its 3084-byte `.text` section, exactly seven emitted functions, no data sections, and seven 100% comparisons. No other function changed its score or size.

The coordinator subsequently marked RGB565 `Matching`, completed the native full build, and obtained PASS from the frozen `main.dol` verifier on 2026-11-04. The refreshed native report confirmed both setup routines at 100% with no regression among previously exact functions. RGBA8 remained a fallback unit at this checkpoint because its 4:1:1 converter was unfinished. The following continuation completed that converter.

Reproducible ignored evidence is under `tools/local/tail-20261104/`: `jpeg_setup_cppvalue.py` exposed the C++ control, `jpeg_setup_cvalue.py` found the C equivalent, and `jpeg_setup_clean.py` produced the retained helpers. The final complete-unit source, object, compiler log, and objdiff JSON are in each `TMC_JPEG_tmcjpeg_rgba8/jpeg_retained_setup/` and `TMC_JPEG_tmcjpeg_rgb565/jpeg_retained_setup/` directory.

## RGBA8 411 values and register priority

`ConvertRGBA8MCU411` at `0x801F9850` now reproduces all 628 native bytes in direct comparison. The cleaned production source compares all fourteen functions in the two JPEG units at 100%. RGBA8 still emits exactly seven functions and 3264 bytes of `.text`; RGB565 still emits exactly seven functions and 3084 bytes. Neither unit emits helper functions or data sections.

The retained D-0016 controls are:

- `TMCJPEGTexelValue`, an enum covering the complete unsigned-halfword range, holds the reused texel and second-pixel channel values. The green and blue declarations retain their observed register priority. The reused texel holds the first pixel's AR halfword, the second pixel's red byte after masking, and the third pixel's GB halfword.
- Three redundant `u32` views preserve allocation for the second pixel's AR row product, third pixel's GB row product, and fourth pixel's AR row product. Each product is already unsigned because `tileStride` is `u32`, so these casts do not change its value. This extends the previously accepted unsigned-row-product control from the 4:2:2 converter.
- The second pixel's weighted sum uses `MakeValue`, `AddValues`, and `AddValuesReversed` to preserve value construction and addition order. Its red contribution is first assigned to a one-int `TMCJPEGValue` local. The third pixel passes its AR and GB values by value to `SumRGBA8Luma`.

All component values remain in 0 through 255. Their weighted contributions use the original coefficients 77, 150, and 29; their total is at most 65280. Adding 128 and shifting by eight therefore preserves the original luma calculation without overflow. Masking the second AR texel in place discards only its unused alpha byte before its red contribution is calculated. The third texel assignment replaces that value before GB is consumed. The helper expressions preserve the same loads, stores, arithmetic, and branch destinations as the native function.

The decisive trace sequence was narrower than the final aggregate score suggested. After value helpers, unsigned row views, an enum texel, and explicit channel declarations, only the red and blue multiplication-result registers differed. An enum red-contribution local improved this to two differing instructions but reused the texel's `r18`. Precomputing the red contribution as a one-int struct value instead gave the required `r30`, with blue remaining in `r22`. The final source then reproduced every instruction. Ordinary scalar locals and additional direct value copies did not provide the same control.

The exact experimental candidate is `TMC_JPEG_tmcjpeg_rgba8/jpeg_redvalue_0/`. `jpeg_rgba_clean.py` removes unused experiment helpers, restores ordinary texture macros where no unsigned view is needed, and gives the retained value helpers their final names. Fresh comparisons of the actual production files are in both `TMC_JPEG_*/jpeg_retained_complete/` directories. These contain the complete sources, compiler logs, objects, and objdiff JSON.

The coordinator selected RGBA8 source, completed the normal native build with exit status zero, and obtained PASS from formal frozen `main.dol` verification (`build/wpse-v1/verification.json`). Both JPEG units are accepted as source linked. This completes all three functions assigned to this JPEG continuation; the native report also places the complete RVL_SDK category at 100% matched and source linked.

## Additional rejected controls

Before the value-return solution, this session tested joint width/height temporary lifetimes, scalarized aggregate views, wide integer views, intrinsic identities, loop unrolling, inline lowering controls, backend optimization controls, parameter reuse, full-width bitfield records, neutral arithmetic, and statement expressions. None completed setup. Failed compilations and unsupported controls are excluded from binary conclusions. The compiler trace used an isolated Frida installation and the existing Wii/1.1 trace addresses, not Wii/1.0 addresses; final comparisons always used the configured Wii/1.0 compiler.

The lost 88.01911% RGBA8 4:1:1 scratch candidate was reconstructed from the prior findings. Redundant unsigned row-product views, following the accepted 4:2:2 control, reached 88.178345% but did not match. These partial variants are not retained in production source.

The private C permuter was restored at upstream revision `8556c81d80d1c1af98a858c8f4dc951357f29139` under ignored `tools/local/tail-20261104/jpeg-permuter/`. It is experiment tooling only; no upstream source is incorporated into the reconstruction. The native compiler adapter replaces only the selected function inside the full original unit, preserving the real header layout and source basename. A constrained 4000-candidate run reproduced the preceding best search score of 1590 from 1615. Those are search scores, not objdiff percentages or acceptance evidence.

## Traps

- A by-value scalar helper is not equivalent as a compiler control to a helper returning a one-field struct followed by a by-value struct addition. The latter completed both setup routines after the former and direct aggregate locals had failed. Preserve the two-stage value construction when reducing this control.
- An enum local and a one-field struct local can have different register priority despite carrying the same bounded integer. RGBA8's enum red contribution reused the texel register and left two mismatches; constructing the contribution as a returned struct value completed the function. This is a measured local control, not a general rule for all integer expressions.
- A lower permuter score can come from wrong arithmetic. A beam-search candidate masked a green-channel multiplication with `0xFF`, changing valid pixel values while improving the search score. That candidate and its descendants were rejected. Audit masks and narrowing conversions before accepting any numerical improvement; exact native comparison remains mandatory.
