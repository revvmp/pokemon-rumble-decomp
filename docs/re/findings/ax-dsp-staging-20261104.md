# AX DSP production staging packet, 2026-11-04

## Status and layout

The coordinator installed this packet under `src/RVL_SDK/ax/dsp/` and accepted the AX source link on 2026-11-04. The normal configure and ninja commands exited zero, and frozen `main.dol` verification passed. All 29 installed source files have exactly the hashes recorded below. The original AX image fallback is replaced by assembled source and the generated word initializer owned by `AXDSPCode.c`.

The preserved private staging tree is `tools/local/tail-20261104/smallgame/DSP/production_packet/src/RVL_SDK/ax/dsp/`. Its entry is `ax_dsp.ds`; the master resets `INCDIR` to `"."` and is assembled with its own directory as the working directory. The investigator prepared this packet privately; the coordinator owned canonical source/configuration changes and full acceptance. Original artifacts and frozen manifests remain unchanged.

The packet preserves the independently reviewed refined source, with 29 files. Routine files have production names: vectors, runtime, commands, processing, filters, SRC, mix dispatch/kernels, tables, DPL2, interrupts, and task handling. Shared constants and repeated pipeline fragments reside in `include/`. Names containing `research`, the generic runtime `head`, and the temporary `command_voice` name are removed. All non-include source bytes are unchanged; include paths were rewritten to match the staged layout.

## Qualified D-0015 application

The target is positively identified as RVL AXWii by its matched task configuration, command vocabulary and parameter-block layout, together with Dolphin's `4cc52064` identity. Pinned SDK archive metadata explicitly enumerates AX DSP assembly inputs and the assembler/linker toolchain in three package inventories. Together these supply the concrete AX-family original-language evidence accepted in [D-0015's AX application](../../DECISIONS.md#2026-11-04-application-to-ax-dsp).

This application does not authenticate the archive as an official Nintendo publication, establish the exact original source revision, or claim recovery of Nintendo's source text. Source fidelity, exact image assembly, and full frozen-DOL verification were evaluated separately. The qualified conclusion is that the identified AX-family program has specific assembly-language provenance evidence; the source is a new reconstruction whose encoding is independently verified. This decision does not automatically apply to the separate OS initializer. See [provenance details](ax-dsp-identities-20261104.md#original-language-provenance) and [independent source review](ax-dsp-source-review-20261104.md).

## Real build validation

Direct assembly with the original private MSVC inspector, the initial portable Zig inspector, and the reproducible Zig inspector each exits zero without diagnostics. Every output is exactly 8,192 bytes and matches the sealed image SHA-256 `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`. The reproducible inspector used here has SHA-256 `cd9389fbe9fb19eb9f7b63451999fbb3c0353d00a949d957d2b0b005935df1e1`.

The actual `build_dsp.py` CLI also succeeds on the new nested include layout and emits an exact binary, C initializer and depfile. Reconstructing the initializer's 4,096 `u16` elements in big-endian order reproduces the same 8,192 bytes. No original executable is a build-wrapper input. The reference image is read only by the separate acceptance comparison.

`stage_production_source.py` checks all original refined-source hashes, stages only the renamed source closure, invokes the three real assemblers and the wrapper, compares their outputs, and confirms that every staged source hash is unchanged afterward. `production_packet/source-inventory.json` contains the full origin/final hash inventory and observed validation results. The following accepted source inventory is relative to `src/RVL_SDK/ax/dsp/`.

| File | Bytes | SHA-256 |
| --- | --- | --- |
| `ax_commands.ds` | 36761 | `e835cc9bbf4cdd16c271b7a50f0020bb70562c3aa4a013a72e1fa64ab8fd3992` |
| `ax_dpl2.ds` | 8812 | `0553321027eab66b222a975466a191a62066ca5d28872672c254ba5d903188d7` |
| `ax_dsp.ds` | 254 | `a9b687f9fa5d91f77113e2f2ad0b41716ef226808281bd6a360dae5d874d6989` |
| `ax_filters.ds` | 2396 | `3dd1ae5bd6f0fbb9da994deef440d5148deab4de826b0c9600a3d942d37241d0` |
| `ax_interrupts.ds` | 1780 | `5de99ac08f0a1176d550536e6bfe98ae600406a9c560d81d561c4f9f6629f352` |
| `ax_mix_dispatch.ds` | 17241 | `503fc79fe07f26f872fbccb3039e8ef7f58f8b3c58e8f3cc8761e2cabdd5eb71` |
| `ax_mix_kernels.ds` | 5025 | `ce0d3896ee5ed702992643d0a78b49d351a30a0cd0bd01fbc9ee2f0d5ced1697` |
| `ax_processing.ds` | 101 | `5f48966da20e35b8e0c60d98ffd75dffaf3c5078868580827695ee81ab7b5a1d` |
| `ax_runtime.ds` | 2416 | `078f3750269039e4fe058188d22dcddcde3578caca22d900cfac1c5bb2a18a62` |
| `ax_src.ds` | 4382 | `8d7c7a6cd5f5f24f7590acc628e11c59f0ff62058663b118ef7b16d12c232f77` |
| `ax_tables.ds` | 6718 | `60b596f97df44b01b4456b3b03bc979eaeae0dd396c3022b124711a6cf120f32` |
| `ax_task.ds` | 1999 | `99fcbfcc06a4f6c68e97b15831b2ddef42c9870ab0a285349b6ea46f19292c24` |
| `ax_vectors.ds` | 260 | `0549b55bc5c34fb5f2f9fef2ba56651befd54973525cc66840d4e7e75b2daa0b` |
| `include/ax_biquad_even_sample.inc` | 308 | `817d67658f5a501e293816703dc1ef2fb5a54754e4b348a29e47ebc91f5ab68c` |
| `include/ax_biquad_odd_sample.inc` | 331 | `ce259579ba86db4bbdbd36e624271be338d7ee8d86d72a92d143f2803ce72bff` |
| `include/ax_mix_constant_pair.inc` | 252 | `2b372b433ad6769ba29d4d881ae8f6298cdadbc5f8fd7f30bf591f78b15d70b7` |
| `include/ax_mix_constant_prime.inc` | 271 | `b4af59ab1467692f72229e261e5079ec8c791de2cf26001a547e73c1586b44da` |
| `include/ax_mix_left_depop.inc` | 73 | `6a73091f1703c136268e4f3e5c1171ce77c0e2a791b78c840166b05684c64433` |
| `include/ax_mix_ramp_pair.inc` | 311 | `2f1a646ef6d1c3e7c66fa343267f88c53d3c8824f9d9f35f43f712cd189ff073` |
| `include/ax_mix_ramp_prime.inc` | 294 | `2b609e0eee42cf39c3f13a241c048ae9c2e7545a6873a8a77b539464a7cb0e1d` |
| `include/ax_mix_right_depop.inc` | 73 | `a72b832066bac7906c3d5804e1f9575add1881437927a4cd5662f13c487ce23c` |
| `include/ax_remote_mix_finish.inc` | 128 | `de68b278de0c2e9c36ab17879a9298dec7d17b4c2455efb1372c69107760aa81` |
| `include/ax_src_restore_history.inc` | 184 | `9a448186a36aa6cdf98ecbe9fbb20fcfc4d76dbb89ac2d7a597e6a7b6b52beaa` |
| `include/ax_src_save_decoder.inc` | 302 | `7a372f102ca1ca1fe13e13106ce89ec660adb231d04e2267a9673ab6e62ba1af` |
| `include/ax_src_save_history.inc` | 235 | `1fca4c82e1857be0833e747e58676f5415a683018c84ae237194dbf94a112cb1` |
| `include/ax_symbols.inc` | 7225 | `6b6579ee826d99d64e96f3d2018ffdc2ee90ff8d4b3163f4923b08851c9ebc2b` |
| `include/ax_volume_ramp_commit.inc` | 44 | `46f4478c367b77e1950e635f032acbdea6c7b103aa7e1834804dfae5cc539698` |
| `include/ax_volume_ramp_pair.inc` | 65 | `94eda0f56659fa13714d39d67504cce9bb9e9f011970c7a0b76c626db54a6cc3` |
| `include/ax_volume_ramp_prime.inc` | 136 | `c8807043ca42b56982bfcb28f5c1a7a25be9ce9ddb96a93a61292f1d5ddfd484` |

The final inventory JSON SHA-256 is `5dfd9362535efd3f71c9e300990c26ba85fab3fbd27d65e7df3cb6490764979d`. Coordinator-owned tool pinning, normal source linkage, and frozen full-build verification have passed. This acceptance does not include or resolve the separate 128-byte OS `DSPInitCode` dependency, whose [readable candidate and provenance boundary](os-dsp-init-20261104.md) remain unaccepted.

Final source formatting removed one extra trailing blank line from ax_commands.ds. The inventory above records the installed source hash; the private staging manifest preserves the pre-formatting packet. Normal regeneration and frozen verification retain every accepted artifact byte.
