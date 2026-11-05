# AX DSP command tables and DPL2 routines

## Scope and evidence boundary

This private 2026-11-04 reconstruction covers instruction-memory words `0x0D02` through `0x0F39` of the 8,192-byte AX image. It follows the [image investigation](ax-dsp-image-20261104.md) and [command identities](ax-dsp-identities-20261104.md). The sealed image SHA-256 remains `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`.

The private assembly is independently written around recovered command, channel and loop roles. It uses code labels for table destinations and control flow, named DMA registers and memory roles, frame-derived transfer sizes, and symbolic coefficient-ROM bases. It contains no numeric instruction arrays or pasted numeric disassembly. No source file, assembler, generated binary, build rule or progress credit is installed in production by this lane.

The archived AX build inventory identifies an original `dpl2.asm` input and related assembly files, but does not authenticate the archive or prove that its generated revision is this image. The [D-0015](../../DECISIONS.md#d-0015-assembly-source-only-where-the-original-was-assembly) acceptance question remains with the coordinator. Exact private assembly is not a full-source acceptance claim.

## DPL2 command and buffer roles

The matched host writer in `src/RVL_SDK/ax/AXCL.c` supplies the command names and arguments. `COMMAND_UPL_AUXA_MIX_LRSC` and its AUX B counterpart consume one volume word followed by six host addresses. The first two addresses receive the three normal AUX channels and the additional DPL2 channel. The remaining four addresses supply the processed L, R, Ls and Rs channels for mixing into the main buffers. The callback arrays in `src/RVL_SDK/ax/AXAux.c` independently identify the fourth channel as Rs and disable the separate AUX C callback in DPL2 mode.

| DSP entry | Reconstructed name | Role |
| --- | --- | --- |
| `0x0DBB` | `AxDspUploadAuxAMixLRSC` | Upload AUX A channels and load old/new return volume |
| `0x0DDD` | `AxDspUploadAuxBMixLRSC` | Corresponding AUX B operation |
| `0x0DFF` | `AxDspMixAuxLRSC` | Generate a return-volume ramp and read/mix four returned channels |
| `0x0E55` | `AxDspMixAuxReturnChannel` | Accumulate 96 gain-scaled 32-bit samples while preserving the command cursor |
| `0x0E81` | `AxDspAddSubToLR` | Read the saved surround pair, add its first channel into main L, and subtract its second from main R |
| `0x0EC4` | `AxDspOutputDpl2` | Save Ls/Rs, generate the master-volume ramp, convert/interleave stereo output, and send the synchronization mail |

The DPL2 DMA paths establish role aliases for existing memory: main Ls is the main S buffer at DMEM `0x0180`; main Rs uses the AUX C L buffer at `0x0880`; AUX A Rs uses AUX C R at `0x0940`; AUX B Rs uses AUX C S at `0x0A00`. The interleaved 16-bit output reuses AUX A L at `0x0400`. These are mode-specific roles, not new storage allocations. They do not establish the host-level meaning of the upper AUX C mixing-table selector bit, whose interpretation remains qualified in the identity findings.

Every full-rate channel contains 96 samples. Its 32-bit working representation is 192 DSP words or 384 bytes. Three-channel AUX uploads are 1,152 bytes, and the saved surround pair is 768 bytes. The final output is also 384 bytes because it contains 96 interleaved pairs of 16-bit samples. The source distinguishes `AX_STEREO_OUTPUT_BYTES` from the 32-bit `AX_STEREO_BYTES`, despite the former equaling one 32-bit channel's byte count.

Both return and master ramps use 683 and 682, respectively the ceiling and floor of `65536 / 96`. Their alternating multiply/accumulate schedule emits 95 intermediate volume values and stores the requested final volume as sample 96. Pair-processing and stereo-conversion loop bounds are written in terms of the same 96-sample count. The source retains the DSP's paired operations and pipeline prologue/epilogue; it does not replace them with a higher-level arithmetic approximation.

## Symbolic instruction-memory tables

`tools/local/tail-20261104/layout_dsp/ax_tables_research.ds` declares all 185 words at `0x0D02` through `0x0DBA` as semantic table entries. The command table contains the fifteen named handlers established by the host command writer. The Main, AUX A and AUX B tables each contain 32 entries. AUX C contains two 32-entry blocks, with a separate `AxDspAuxCSharedInputMixTable` label on its second block. The final tables select four remote-mix cases, three SRC routines, and three coefficient-ROM bases.

Each mixing entry names its channel and ramp combination. Repeated entries preserve the observed flag behavior: stereo ramping handles all indices with that flag set, and surround ramping selects the same routine whether or not the separate constant-surround flag is also set. The second AUX C block uses the shared-input wrappers for L/R combinations while its surround-only cases reuse the ordinary AUX C routines. These wrapper names and addresses were cross-checked with the separate filter/mixing source lane; they are real labels in its private `ax_mix_dispatch.ds`.

The three coefficient words are `AX_SRC_COEFFICIENTS_8KHZ`, `AX_SRC_COEFFICIENTS_12KHZ`, and `AX_SRC_COEFFICIENTS_16KHZ`. They refer to data-memory ROM, unlike the preceding code-label entries. No instruction or code-address array is substituted for the table source.

## Private validation

`tools/local/tail-20261104/layout_dsp/ax_dpl2_research.ds` contains the named routines. `check_dpl2.py` supplies external symbol bindings for an isolated diagnostic, invokes the pinned unmodified encoding implementation in `tools/local/dsp-tools/dolphin-assembler/dsp-inspect.exe`, checks the sealed input hash, and directly compares only the owned interval. The leading fill created by `org` is excluded explicitly.

The real assembly and comparison command exits zero. Words `0x0DBB` through `0x0F39` reproduce all 766 bytes with zero differences. Both range hashes are `dac9a429fc52d85f0cec49ade5cda1e0d284bd200cd96de0d2b8ea22d916f205`; the source hash is `203c176aec35b7d0322fb6e8e468097aa185c4e190b1070a0ef1355bde8ddefd`. `dpl2-comparison.json` records this partial result, also repeated with the shared `ax_symbols.inc` constants.

The subsequent `check_tables_dpl2.py` command assembles both sources with those same shared constants and diagnostic bindings for the other lanes' exported routines. It exits zero and reproduces all 370 table bytes, with range SHA-256 `0df1ba041b8eae9c877b6214426dc6fb3f24ef42f9ebe0f69d02d66900e8269c`. The complete owned interval is 1,136 bytes, with zero differences and SHA-256 `6413d6d2d8e29456ace0f9c45046c1b8713a9dbd1f137b80b5607e1fcf23b7b4`. The table source hash is `60b596f97df44b01b4456b3b03bc979eaeae0dd396c3022b124711a6cf120f32`. `tables-dpl2-comparison.json` records all three range comparisons. The standalone bindings do not replace the other routines; the shared full-image research assembly must resolve their actual source labels independently.

Actual-label integration subsequently exposed two ordinary-AUX/DPL2 stage-name collisions. Prefixing the DPL2 channel-stage labels with `AxDspDpl2AuxReturn` resolves them without changing any owned bytes. The updated DPL2 source hash is `0553321027eab66b222a975466a191a62066ca5d28872672c254ba5d903188d7`. The [independent complete-source review](ax-dsp-source-review-20261104.md) records a successful actual-label assembly of all 8,192 bytes and distinguishes that result from provenance and production acceptance.

## Traps

- Equal transfer byte counts can describe different sample layouts. The 384-byte AUX channel uses 96 signed 32-bit samples, while the 384-byte final output uses 96 stereo pairs of signed 16-bit samples. Preserve separate semantic constants even when the immediate encodings coincide.
- DPL2 reuses ordinary AUX C storage for three right-surround roles. Name the role aliases from the host command/callback and DMA paths; do not infer an unrelated mixing-table selector's meaning from that reuse alone.
