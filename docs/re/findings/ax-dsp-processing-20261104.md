# AX DSP filters, sample conversion, and mixing, 2026-11-04

## Scope and acceptance boundary

The reconstruction covers DSP instruction-memory word addresses `0x06A9` through `0x0D01`, inclusive. It assembles to the original 3,250 bytes, and all 82 routine/dispatch entry intervals compare exactly. The complete image reproduces all 8,192 original bytes and is now selected from symbolic source in the normal AX build.

The coordinator accepted the [AX application of D-0015](../../DECISIONS.md#2026-11-04-application-to-ax-dsp) and integrated the readable source. Native ninja exited zero and the full frozen-DOL comparison passed. The [image investigation](ax-dsp-image-20261104.md) identifies the sealed input; the [identity investigation](ax-dsp-identities-20261104.md#original-language-provenance) records concrete AX-family assembly filenames in pinned SDK inventory metadata, with archive authenticity and exact image revision still qualified. No proprietary assembly body was retrieved or reused.

Addresses in this finding are DSP words. Instruction memory and data memory are separate. The output file stores each word as two bytes.

## Source organization

The private body is `tools/local/tail-20261104/sort/DSP/ax_processing.ds`. It includes four semantic bodies:

| Body | DSP words, inclusive | Role |
| --- | --- | --- |
| `ax_filters.ds` | `0x06A9–0x0717` | First-order low-pass and biquad filters |
| `ax_src.ds` | `0x0718–0x081C` | Four-tap, linear, bypass conversion and decoder state loading |
| `ax_mix_dispatch.ds` | `0x081D–0x0BCE` | Empty operation and 68 channel/ramp dispatch wrappers |
| `ax_mix_kernels.ds` | `0x0BCF–0x0D01` | Four main/AUX and three remote mixer kernels |

The body has no `ORG`, duplicate state definitions, numeric opcode arrays, or numerically bound code targets. The standalone wrapper establishes its initial address; the combined image reaches it from the preceding owner's real source. All calls, branches, and hardware-loop endpoints name labels. Parameter-block fields, sample buffers, frame counts, wrapping constants, hardware registers, and persistent sample pointers use shared names.

The inspected assembler implements `INCLUDE`, `INCDIR`, `EQU`, `ORG`, `WARNPC`, and word data directives, but no parameterized macro facility was found. Named include fragments express repeated operations without introducing a custom preprocessor: alternating biquad sample operations, SRC history transfer, decoder-state save, constant/ramped mix pairs, ramp generation, and depop result extraction. Each inclusion emits ordinary instructions in place. The per-routine `WARNPC` limits detect overrun; direct byte comparison supplies the stronger exact-layout check.

`ax_processing_routines.json` records the 82 entry addresses for the private verifier and the other table owner. It contains names and boundaries, not executable words.

## Filter contracts

| Entry | Name | Caller-provided state and buffers |
| --- | --- | --- |
| `0x06A9` | `AxDspLowPass` | `AR0` output, `AR1` input, `AR3` points to `AXPB.lpf.yn1` |
| `0x06C1` | `AxDspBiquad` | `AR0` points to `AXPB.biquad.xn1`, `AR1` input, `AR2` output |

The callers select these entries using `AXPB.lpf.on` and `AXPB.biquad.on`. The field sequence is independently established by `include/revolution/ax.h`.

The low-pass filter retains the starting history pointer in `IX0`, reads its previous output and the `a0`/`b0` coefficients, and interleaves multiplication with the next sample/coefficient loads. Priming, 47 looped pairs, and the final drain cover 96 samples. It writes the final output back to `yn1`. The saved history pointer is restored before that write.

The biquad copies `xn1` and `xn2` to the two-word buffer at DMEM `AX_BIQUAD_HISTORY`, retains `yn1` and `yn2` in the two halves of `AX1`, and copies the five coefficients into `AX_BIQUAD_COEFFICIENTS`. The address wrapping masks are 1 for the two-word history and 4 for the five-word coefficient sequence. Alternating sample bodies rotate which output-history register receives the new result. The final body omits the next-sample prefetch. The routine restores both input histories and both output histories to the parameter block, disables the temporary wrapping, and restores the expected multiplication mode.

These names describe the verified operations. Original DSP routine spelling is not claimed.

## Conversion and decoder state

| Entry | Name | Behavior |
| --- | --- | --- |
| `0x0718` | `AxDspSrcFourTap` | Decode as needed, select a fractional-phase coefficient quartet, produce 96 samples |
| `0x0769` | `AxDspSrcLinear` | Decode as needed and interpolate adjacent samples; retain its distinct zero-fraction path |
| `0x07BE` | `AxDspSrcBypass` | Read and store 96 decoder samples directly |
| `0x07E4` | `AxDspLoadDecoderState` | Load address, ADPCM, and hardware decoder state from the parameter block |

The decoder loader starts at `AX_PB_FORMAT`. Its sequential reads write the format, loop/end/current addresses, all sixteen ADPCM coefficients, gain, predictor/scale, and both decoder histories. Its final `AR2` points at `AXPB.src.ratioHi`, which supplies the calling converters' next inputs.

Four-tap conversion restores four samples into the DMEM history ring, uses the 16.16 ratio and fractional address to determine decoder reads, and selects coefficients from `AX_COEFFICIENT_BASE`. The coefficient selector points to DMEM ROM, not instruction-memory code. Linear conversion uses the same ratio/history structure but retains a separate zero-fraction branch and the target's exact weighted two-sample sequence.

All three converters write the four final samples back through `AX_PB_SRC_HISTORY_LAST`. Four-tap and linear conversion also preserve the fractional position. The common save sequence writes decoder `YN2`, `YN1`, predictor/scale, and current address back to their named parameter-block fields. Bypass retains its two executed `NOP` instructions inside each sample-loop iteration; they are not image padding.

The scratch/history buffer at DMEM `0x0D08` is unrelated to the command-table entry at the same numerical instruction-memory address. Likewise the biquad coefficient storage at DMEM `0x0D03` is separate from the command table beginning at IMEM `0x0D02`.

## Channel dispatch and mixer contracts

The 68 nonempty wrappers are built around named input pointer, volume, destination, and depop fields. Main, AUX A, AUX B, and AUX C each have fourteen entries: left, right, stereo, stereo ramp, five surround combinations, and five surround-ramp combinations. Their bases are `0x081E`, `0x08E0`, `0x09A2`, and `0x0A64`.

A single-channel wrapper supplies `AR0` with samples, `AR1` with its parameter-block volume, and `AR2` with the accumulation buffer. For a constant volume, `AR3` becomes a second cursor into that output buffer. For a ramp, it starts at `AX_RAMP_BUFFER`. Stereo wrappers additionally retain the right sample and output pointers in `IX0` and `IX1`. A surround combination mixes surround first, then jumps to its corresponding left/right/stereo wrapper. The returned last scaled sample is stored into the matching depop field.

The twelve additional wrappers at `0x0B26` use `AX_COMMON_SAMPLE_POINTER` for both left and right inputs. Their names contain `AuxCSharedInput`; their pure surround cases share the ordinary AUX C entries. The extra selector bit's higher-level meaning remains unproven, so these wrappers do not invent a DPL2 identity.

| Entry | Name | Samples |
| --- | --- | --- |
| `0x0BCF` | `AxDspMixMonoConstant` | 96 |
| `0x0BE9` | `AxDspMixStereoConstant` | 96 per channel |
| `0x0C1F` | `AxDspMixMonoRamp` | 96 |
| `0x0C4F` | `AxDspMixStereoRamp` | 96 per channel |
| `0x0CAE` | `AxDspRemoteMixNone` | Zero depop return |
| `0x0CB1` | `AxDspRemoteMixConstant` | 18 |
| `0x0CCE` | `AxDspRemoteMixRamp` | 18 |

The kernels multiply 16-bit sample inputs by their channel volume and accumulate into two-word outputs. Constant and ramped pairs have separate named sequences because the latter load a new volume per sample. Ramp generation fills scratch with successive volume values and commits the next frame's volume to the parameter block. The main kernels use 47 looped pairs after priming; remote kernels use eight. These are loop counts, not total pair counts. Stereo kernels preserve the two separate depop results in `AX0.L` and `AX1.H`.

## Exact assembly evidence

Run from the repository root:

```powershell
python tools/local/tail-20261104/sort/DSP/check_processing.py
```

This invokes the real private assembler on `ax_processing_research.ds`, requires its exit status to be zero, verifies the sealed original-image hash, and compares the assigned range without normalization. It also compares every routine interval and records all input-source hashes in `ax_processing_exact.json`.

The final invocation exited zero with no assembler output or warnings. The private assembler SHA-256 was `22f7d1df9088188a391f81af3f26bfa4694c2980938069148564fc95456db7ab`. The 3,250-byte range has SHA-256 `b3c11b0d7613fc0bdf3c7c7c3a4ce189bc7797c6d94a28ce7bb37ced6744d8c3`, zero differing bytes, and 82 of 82 exact entry intervals. The zero-filled prefix produced by this partial wrapper is excluded from the comparison and is not claimed as reconstructed code.

The runtime owner subsequently assembled the actual-label combined image. Its `smallgame/DSP/combined_comparison.json` records all 8,192 bytes equal to the frozen DSP image, SHA-256 `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`, and source hashes for these final warning-free fragments. The combined assembly resolves cross-owner references through their source labels.

The accepted processing bodies now reside under `src/RVL_SDK/ax/dsp/`, with their repeated-operation fragments under its `include/` directory. The production master `ax_dsp.ds` and [reproducibly built host assembler](ax-dsp-host-tool-20261104.md#canonical-integration-and-failure-propagation) generate the initializer consumed by `AXDSPCode.c`. The coordinator's normal native build and full frozen-DOL verification passed with that source selected. The private paths above remain the per-routine qualification evidence.

## Traps

- A hardware-loop endpoint is the address of its last executed instruction, not the next instruction. Naming a forward end label and subtracting one assembled correctly here but produced first-pass underflow warnings because the unresolved label temporarily evaluated to zero. Labels on the actual final instruction removed every warning without changing a byte.
- A pipeline loop count can look two samples short when read in isolation. These kernels process two priming samples plus 47 pairs for the 96-sample frame, or two plus eight pairs for the 18-sample remote frame. Retain priming and drain operations when reconstructing repeated bodies.
- Matching the standalone owned range does not establish its omitted prefix. This wrapper's leading `ORG` creates zeros solely to position the range; only the combined source supplies and verifies the earlier instructions.
