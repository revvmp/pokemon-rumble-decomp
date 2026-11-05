# WUD firmware generator validation, 2026-11-04

## Scope and result

The reviewed WUD generator assembles eight readable Thumb sources with the pinned Zig toolchain, links them at their controller addresses, and generates the download and installation records consumed by the existing WUD interface. The owner authorized this narrow firmware-assembly exception under [D-0017](../../DECISIONS.md#d-0017-two-owner-approved-embedded-firmware-assembly-exceptions). Original-language provenance remains separate from that authorization; see [the native assembly investigation](wud-thumb-zig-20261104.md).

The final helper is `tools/firmware/build_wud.py`, SHA-256 `76ef7a016cecc19872498a238bdeaf55aff4d207f45978eb9e2d2027a4a4c635`. It reuses the accepted DSP dependency and publication helpers without modifying them, SHA-256 `2412ea532274c68b9ae3c8a23dad101cce81bc789b496726d37c015668e36447`. The coordinator installed the frozen packet, then ordinary configure and Ninja both exited zero. Frozen whole-DOL verification passed, and the complete compiled `WUD.o` remains byte-identical to its preceding accepted output. Repeat clean-build evidence is recorded separately by the coordinator.

This validation lane changed no canonical implementation, configuration, original input, or expected hash. Its private copies and actual CLI/Ninja exercises are under `tools/local/tail-20261104/selector/WUD`; `wrapper_evidence.json` records all 37 successful checks, including each process exit, stdout, stderr, and comparison result. These are end-to-end exercises with the real compiler, linker, extractor and Ninja, without mocked filesystem operations or unit tests.

## Generation contract

The generator accepts the semantic patch manifest, the acquired tool installation and pinned tool manifest, two binary outputs, two C-initializer outputs, and a depfile. It reads no original DOL, reference binary, or expected content hash. Reference comparison belongs only to the independent validation harness.

The tool receipt resolves the genuine Zig executable, whose digest is checked before use. The actual native route is `zig cc` for ARM7TDMI Thumb assembly, `zig ld.lld` with the authored linker script, and `zig objcopy` for each named linked section. All eight allocated ELF sections must have their expected address, executable flags, and exact extent. The payload is 180 bytes; each of the seven replacement sections is eight bytes.

The download record contains a little-endian destination and length followed by the payload, for 188 bytes. The installation table contains a one-byte count followed by seven typed 13-byte records: slot, four little-endian address bytes, and eight instruction bytes. It is 92 bytes in total. Both C initializers reconstruct their binary counterparts exactly.

The manifest rejects duplicate fields or slots, invalid integer types, odd or out-of-range addresses, overlapping extents, unexpected source paths, and missing sources. Assembly imports through `.include` or `.incbin` are rejected wherever they occur, including after labels or semicolons. The bounded linker script rejects `INCLUDE`, `INPUT`, `GROUP`, `SEARCH_DIR`, and `STARTUP`, preventing untracked imported inputs.

The generator retains the exact bytes it validates from their first read and checks them again before publication. All five outputs are staged before publication and use the existing atomic-per-file publisher with rollback. A process or validation failure preserves earlier outputs. This provides recovery across the output set, not a filesystem-wide transaction.

## Actual end-to-end results

The native output matches all reference bytes:

| Artifact | Bytes | SHA-256 |
| --- | --- | --- |
| Download record | 188 | `fde41feb3ac0bab076e24e613fd40768e3c099bcd7f34c1f746129188990ebae` |
| Installation table | 92 | `3a5f6a9f6a120cb10a4224a4bcc35db8e75459d62ebaf5b9879180b9e8de5f2c` |

Changing the readable connection-limit instruction from `movs r0, #40` to `movs r0, #41` changes its expected installation byte and leaves the download payload unchanged. Restoring the source restores both exact artifacts.

Duplicate slots and addresses, overlapping ranges, odd/negative/overflowing addresses, Boolean numeric fields, invalid slots, missing or escaped source paths, and duplicate JSON fields all fail without changing the five accepted outputs. The same holds for direct manifest/source output collisions, duplicate outputs, and a source hardlink used as an output. Invalid assembly, a ten-byte replacement section, labelled `.incbin`, semicolon-prefixed `.include`, linker imports, and a mismatched acquisition receipt are rejected.

An actual open Windows file handle blocks publication of the command initializer after earlier changed outputs have been replaced. The helper reports failure and restores every preceding output. A separate run edits a source only after the first real object file appears; the final input-stability check rejects the mixed snapshot and preserves all accepted outputs. Both checks use real files and processes.

Private Ninja builds run in a directory containing spaces, `#`, and `$`. Its dependency database records all 15 inputs for each of the four generated outputs: eight assembly files, linker script, patch manifest, generator, shared publisher, tool manifest, acquisition receipt, and Zig executable. An unchanged second build reports no work. Editing an assembly input triggers regeneration and the expected byte change; restoring it restores the exact records. Updating the generator also triggers regeneration, followed by another no-op build.

The first 33 cases ran against helper `e3ad161da355cc0be3a53b24531b3fda84383726ad402c07f29bbb66b02d2177`. While they ran, review added only `STARTUP` to the rejected linker directives. The harness detected that candidate change rather than silently certifying the newer file. The final four checks first verify that exact one-token guard change, then prove the final helper's native positive output, `STARTUP` rejection, Ninja generator dependency rebuild, and no-op behavior. Both revisions and the transition are retained in the evidence.

## Reproduction

The ordinary build owner configures the generator before compiling WUD. An equivalent direct invocation is:

```powershell
python tools/firmware/build_wud.py --manifest src/RVL_SDK/wud/firmware/patches.json --tool-install tools/local/dsp-assembler --tool-manifest config/dsp_assembler.json --binary build/wpse-v1/firmware/wud_patch.bin --commands-binary build/wpse-v1/firmware/wud_patch_commands.bin --c-include build/wpse-v1/include/wud_patch_data.inc --commands-include build/wpse-v1/include/wud_patch_commands.inc --depfile build/wpse-v1/firmware/wud_patch.d
```

The diagnostic `exercise_wud.py` owns only private copies. `finalize_wud.py` records the reviewed guard-only transition described above. The coordinator retains sole ownership of ordinary configure/Ninja, full executable verification, and acceptance claims.

## Traps

- Searching for import directives only at line starts misses legal labelled and semicolon-separated assembly forms. The bounded comment-free source contract permits rejection of the import tokens wherever they appear.
- Reading inputs again to create a snapshot after validation creates a validation-to-snapshot gap. Preserve the bytes from the first validated read and compare subsequent reads with them.
- A successful native generation does not by itself certify Ninja dependency behavior. Load the real depfile through Ninja and verify source changes, generator changes, escaped paths, and an unchanged no-op build.
