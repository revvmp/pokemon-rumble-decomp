# AX DSP command and voice reconstruction, 2026-11-04

## Scope and status

The accepted command/voice source in `src/RVL_SDK/ax/dsp/ax_commands.ds` reconstructs DSP word addresses `0x008A` through `0x06A8`, inclusive. The actual Dolphin assembler produces all 3,134 bytes exactly, with no binary repair, copied instruction-word arrays, or original-image input to assembly. After the complete readable DSP program was integrated, the coordinator's normal configure/ninja exited zero and frozen whole-DOL verification passed. The private research comparisons below preserve the evidence leading to that acceptance.

The reference is the 8,192-byte `axDspSlave` image with SHA-256 `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`. All addresses here are DSP word addresses. IMEM and DMEM remain distinct address spaces. The [image investigation](ax-dsp-image-20261104.md) documents the inspector and exact tool roundtrip; the [identity investigation](ax-dsp-identities-20261104.md) documents the host command protocol, parameter-block fields, and qualified AX-family assembly provenance.

The initial authored source is `tools/local/tail-20261104/selector/DSP/command_voice.ds`; the production copy incorporates reviewed count and output-buffer naming refinements. Its labels describe command handlers, voice-processing stages, ramps, filter paths, and hardware-loop endpoints. DMA, mailbox, AXPB, frame-size, buffer, and cached-function references use named symbols. Every direct branch, call, and hardware-loop endpoint is symbolic. Paired DSP instructions and the scheduled pipeline prologues/epilogues remain explicit. No original DSP routine spelling is claimed for these descriptive labels.

The research image uses the shared `tools/local/tail-20261104/smallgame/DSP/ax_symbols.inc`; production uses `src/RVL_SDK/ax/dsp/include/ax_symbols.inc`. The standalone diagnostic wrapper supplies only external routine/table address declarations and includes the authored fragment. Those external declarations are excluded from the combined image, where the owning source fragments define real labels. The fragment's `org 0x008a` and `warnpc 0x06a9` state its boundary; there are no internal origin directives hiding missing code.

## Recovered organization

The matched PowerPC writer in `src/RVL_SDK/ax/AXCL.c` supplies the command identities and argument order. `include/revolution/ax.h` supplies `AXSTUDIO`, the exact 320-byte `AXPB` layout, and 96/18 sample counts. The DSP data accesses independently agree with those layouts.

| Routine | Start | End, exclusive | Bytes | Comparison |
| --- | --- | --- | ---: | --- |
| `AxDspSetup` | `0x008A` | `0x0222` | 816 | Exact |
| `AxDspAddToLR` | `0x0222` | `0x024F` | 90 | Exact |
| `AxDspSubToLR` | `0x024F` | `0x027E` | 94 | Exact |
| `AxDspProcess` | `0x027E` | `0x049B` | 1,082 | Exact |
| `AxDspMixAuxA` | `0x049B` | `0x04B4` | 50 | Exact |
| `AxDspMixAuxB` | `0x04B4` | `0x04CD` | 50 | Exact |
| `AxDspMixAuxC` | `0x04CD` | `0x04E6` | 50 | Exact |
| `AxDspMixAuxReturn` | `0x04E6` | `0x057B` | 298 | Exact |
| `AxDspCompressor` | `0x057B` | `0x0609` | 284 | Exact |
| `AxDspOutput` | `0x0609` | `0x0670` | 206 | Exact |
| `AxDspEnd` | `0x0670` | `0x067A` | 20 | Exact |
| `AxDspWmOutput` | `0x067A` | `0x06A9` | 94 | Exact |

### Studio initialization

SETUP transfers the 120-byte studio state to scratch DMEM. Its twenty six-byte records each contain a 32-bit current depop value and 16-bit step. Twelve main/AUX channels receive 96 pairs of DSP words each; eight remote channels receive 18 pairs each. A zero current value takes the clear-buffer path. A nonzero value writes the initial pair and advances by the signed step for the remaining samples. The unrolled source follows the proven `AXSTUDIO` field order, including the alternating remote main/AUX records.

This identifies the main L/R/S buffers at DMEM `0x0000`, `0x00C0`, and `0x0180`, the AUX groups beginning at `0x0400`, `0x0640`, and `0x0880`, remote main buffers beginning at `0x0240`, and remote AUX buffers beginning at `0x0AC0`. These are 32-bit accumulation buffers. The equal numerical value `0x0D08` used for scratch storage is unrelated to the command-table entry at IMEM `0x0D08`.

### Voice processing

PROCESS transfers the first parameter block, caches its selected SRC/coefficient and main/AUX/remote mix pointers, and prepares sample pointers from the ITD fields. With ITD enabled, the prior 64-byte history is loaded at DMEM `0x0C40`; common decoded samples begin at `0x0C60`, and left/right pointers include their respective history shifts. Without ITD, all three pointers use the common buffer.

An active voice invokes the selected SRC routine, creates the 96-sample envelope ramp, and applies it to the common samples. Optional low-pass and biquad filters then run with the histories and coefficients stored in AXPB. The twelve main/AUX and eight remote depop words are cleared before their mixing callbacks populate them. Four cached main/AUX functions are called in order.

For remote output, a separate optional low-pass or biquad path produces the remote filter input. The unfiltered path copies the same 96 samples. A four-tap conversion then generates 18 remote samples with fixed step `5 + 0x5555 / 65536` and coefficient-table base `0x1000`; this observation does not establish a separate original source-level rate name. Four history samples and the fractional address are written back. Eight cached remote functions mix alternating main/AUX outputs and update the corresponding depop fields.

The ITD shifts approach their targets by at most one per processing pass. Both enabled and stopped voices reach the common save path: enabled ITD writes its next 64-byte history from DMEM `0x0CA0`, the whole AXPB is written to its current host address, and a nonzero next pointer starts the following transfer. The command cursor is restored only after the linked list ends. These paths and their timing remain intact rather than being simplified from high-level intent.

### AUX return, compressor, and output

Each AUX handler saves the new return volume, uploads its three accumulation channels, and invokes the common return mixer. The helper builds a 96-sample ramp between the previous and requested volume, downloads the return samples into the same AUX region, and adds the scaled L/R/S results into the main buffers. The two fixed-point ramp coefficients, `0x02AB` and `0x02AA`, are the ceiling and floor of `65536 / 96`; their scheduled order and final requested-volume store are preserved.

The compressor scans the left/right accumulators against the command threshold, maintains its release counter, selects the corresponding coefficient frame from the supplied host table, and applies it to both channels. The reconstruction retains the original transfer-offset arithmetic, including the separate attack and release paths. It does not infer or reconstruct the host table generator from these accesses.

OUTPUT first uploads the surround channel, constructs the master-volume ramp, scales and interleaves the stereo samples into an AUX-area scratch buffer, and transfers the 384-byte result. It then sends `DSP_MAIL_SYNC` and waits for acknowledgment. WM_OUTPUT converts and transfers four sets of 18 remote samples, overlapping conversion of the next speaker with the current DMA. END sends `DSP_MAIL_YIELD` and branches to the task-mail receiver. The following two-word jump back to command reception at PC `0x0678` is also retained; no current caller or fallthrough path to that separate entry was established here.

## Reproducible partial validation

Run from the repository root:

```powershell
python tools/local/tail-20261104/selector/DSP/verify_command_voice.py
```

The script first checks the sealed reference size and SHA-256, runs the actual private assembler, requires exit zero and the exact final word address, then compares every byte of each routine above. It writes `command_voice_comparison.json` and `assembly.log`. It neither copies original bytes into the output nor modifies the assembled image. All twelve comparisons pass, covering the entire contiguous range without gaps.

The reconstructed range SHA-256 is `a29f83bac3a13ea0106974b934a85a17e797cd15f82a9e2f2c67cf081d22559f`. The verified authored-source SHA-256 is `df746ade402e63f868d6c7b1aa261d9ab6a80e2308d08e6ba9f1cb41ef30e710`. The tool SHA-256 is `22f7d1df9088188a391f81af3f26bfa4694c2980938069148564fc95456db7ab`. The report additionally records each routine digest and the shared-symbol file digest.

These routine comparisons remain partial diagnostics. Production acceptance comes from the coordinator's separate normal build and frozen whole-DOL verification, using the [reviewed assembly wrapper](ax-dsp-build-wrapper-20261104.md). The archived build inventory provides concrete AX-family `.asm` filenames, but its authenticity and connection to this exact image revision remain qualified as documented in the identity investigation and D-0015 review.

## Traps

- A DSP buffer count can denote words, bytes, or samples. Main/AUX accumulation uses two DSP words per sample; remote output DMA uses two bytes per sample. Equal constants such as `0x24` therefore require separate semantic names.
- A stopped voice still traverses the shared history/parameter save path. Moving the state test ahead of ITD setup or returning early would change the actual program even if a high-level mixer model treats the voice as silent.
- A reused scratch address does not imply a single persistent data type. Studio records, gain ramps, remote filter history, and DMA transfer staging share DMEM `0x0D08` at different times. Preserve their lifetimes and the IMEM/DMEM distinction.
- Exact assembly encoding does not by itself establish source provenance or production acceptance. Keep the standalone external-address declarations and leading `org` fill outside claims about reconstructed routines; the combined image must resolve external symbols from their owning fragments.
