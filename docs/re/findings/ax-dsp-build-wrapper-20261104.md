# DSP assembly build wrapper, 2026-11-04

## Scope

The accepted `tools/dsp/build_dsp.py` connects readable DSP assembly to an ordinary generated C initializer. The coordinator installed the reviewed wrapper unchanged, integrated the 29-file program under `src/RVL_SDK/ax/dsp`, and linked `src/RVL_SDK/ax/AXDSPCode.c` from source. Normal configure/ninja exited zero and frozen whole-DOL verification passed. The real production Ninja dependency records contain all 31 inputs. Repeat clean-build validation is recorded separately by the coordinator.

The private development packet remains under `tools/local/tail-20261104/selector/DSP/build_packet`. The assembler's pinned acquisition and build contract is `config/dsp_assembler.json`, exposed through `python tools/setup/acquire_dsp_assembler.py`; the configured local executable is `tools/local/dsp-assembler/dsp-inspect.exe`.

The wrapper accepts an assembler, a master assembly source, binary/include/depfile output paths, and an expected byte size. It receives no original DOL, reference image, manifest, or expected content hash. It invokes the pinned inspector's real `assemble INPUT OUTPUT` command from the source file's parent directory. The binary remains the assembler's unchanged output.

The C initializer defaults to the DSP-word contract: each pair of assembled big-endian bytes becomes one four-digit hexadecimal `u16` initializer. The 8,192-byte image therefore produces 4,096 elements, matching the existing `extern u16 axDspSlave[]` interface in `include/revolution/ax.h`. An explicit `--element-width 8` emits byte-valued elements for a source owner whose actual API requires `u8`; `--element-width 16` is the default. The include contains only initializer values, commas, indentation, and newlines; the ordinary C owner supplies the typed array declaration.

## Build behavior

The wrapper scans recursive literal `INCLUDE` and `INCDIR` directives before running the assembler. Missing files, cycles, unsupported nonliteral paths, and paths containing characters that the assembler's operand parser treats as separators fail explicitly. All 29 files in the complete readable source tree are discovered. The generated GCC-style depfile lists those files, the assembler, and the wrapper, for 31 dependencies on both the binary and C-include outputs.

Dependency discovery follows the inspected assembler's state, rather than assuming conventional include-directory behavior. `INCDIR` is relative to the assembler's working directory and persists after returning from a nested include. It also persists between the assembler's first and second passes. The scanner follows both passes and records the union of their actual input paths. The portable master begins with `incdir "."`, so both passes have the same intended starting directory.

Input/output validation rejects direct path collisions, resolved aliases, hardlink collisions, duplicate outputs, non-file outputs, and output symlinks. Protected inputs include every recursively discovered source, the assembler executable, and the wrapper. Source contents are checked again after assembly so an edited input cannot silently publish an image assembled from a mixed source snapshot.

The assembler writes to a temporary file beside the requested binary. Expected size is checked before any final output is replaced. The C initializer and depfile are also staged beside their destinations. Each final file is replaced atomically after all staging succeeds; the publisher keeps copies of existing outputs to roll back a failed publication. Recovery attempts every already-replaced destination, and a failed restore retains and reports the original backup path instead of deleting it. This is individual-file atomic replacement with recovery, not a filesystem transaction spanning all three files. Identical outputs retain their existing timestamps. Assembler, dependency, and size failures leave the previously published outputs unchanged and remove temporary files.

The byte count defaults to 8,192 and can be set with `--size`; it must be positive and even. A representative invocation from the repository root is:

```powershell
python tools/dsp/build_dsp.py --assembler tools/local/dsp-assembler/dsp-inspect.exe --source src/RVL_SDK/ax/dsp/ax_dsp.ds --binary build/wpse-v1/dsp/ax_dsp.bin --c-include build/wpse-v1/include/ax_dsp.inc --depfile build/wpse-v1/dsp/ax_dsp.d --size 8192
```

The normal entry point remains `python configure.py` followed by `.venv/Scripts/ninja`. The `dsp_assembly` rule runs before C compilation, produces both binary and include outputs, uses GCC depfile loading and `restat`, and depends explicitly on the assembler and wrapper. Its binary, include, and depfile paths derive from the selected `--build-dir` and target version; the command quotes those paths and the assembler. `AXDSPCode.c` declares the existing 32-byte-aligned `u16 axDspSlave[4096]` array and includes the generated values through the selected output include directory.

The complete source tree and provenance review are documented by the [DSP identity/reconstruction investigation](ax-dsp-identities-20261104.md). The exercises below copy source files into private validation workspaces; they do not edit canonical source or run a second global build.

## Actual end-to-end evidence

`exercise_build_dsp.py` in the same private packet runs the real wrapper, the real assembler, and a real private Ninja build. It is an end-to-end diagnostic, with no unit-test framework or mocked assembler. Its verification-side comparison reads the sealed original image; the build wrapper does not.

All 20 cases pass:

- The complete refined source assembles to 8,192 bytes with SHA-256 `d6d5952c5e6c55705513d89e58ba12314db23d8758c4acfd2c75b81b7140c0ed`, matching every frozen DSP image byte. The C initializer has exactly 4,096 words and reconstructs those same bytes in big-endian order.
- Explicit byte-element mode produces 8,192 values that reconstruct the same assembled bytes; the default remains 4,096 `u16` values.
- Replacing the first readable `nop` with `halt` changes exactly its expected instruction word. Restoring the source restores the original image.
- An invalid instruction, incorrect expected size, missing include, nonliteral include, recursive include, direct source/output overlap, recursive-include/output overlap, duplicate outputs, and an input hardlink used as an output all fail. Each preserves the three preceding successful outputs and leaves no staged output files.
- An actual Windows file handle prevents replacement of the depfile after the new binary and initializer are published. The wrapper reports failure and restores both earlier outputs to their accepted contents; the locked depfile remains unchanged. No filesystem operation is mocked.
- A small real-assembler source demonstrates `INCDIR` persistence across passes: its first include resolves once from the master directory and once from a nested directory. The depfile retains both actual source files.
- Ninja loads all 31 dependencies for both outputs, including paths containing spaces, `#`, and `$`. A second invocation reports no work. Editing a deeply included mixing kernel triggers a real rebuild and changes the assembled image; restoring it restores exact bytes.

The evidence is `build_packet/build_wrapper_evidence.json`, with each command's exit code, stdout, stderr, and the content comparison results. The verified wrapper SHA-256 is `2412ea532274c68b9ae3c8a23dad101cce81bc789b496726d37c015668e36447`. The exercised MSVC inspector SHA-256 is `22f7d1df9088188a391f81af3f26bfa4694c2980938069148564fc95456db7ab`.

A preliminary positive cross-check used the portable Zig-built inspector with SHA-256 `cd9389fbe9fb19eb9f7b63451999fbb3c0353d00a949d957d2b0b005935df1e1` and the renamed, nested production staging tree. It discovered all 29 sources, exited zero, reproduced the exact 8,192-byte image, and emitted 4,096 words that reconstruct those bytes. `portable_wrapper_evidence.json` records that result.

The final pinned inspector adds explicit output flush/close checking and has SHA-256 `b9afa1a0a47628f6b15464394792707f8a1bc1ba7f27be064b18801d762d47fd`. Independent review with that executable confirms the exact image, both element widths, wrong-size output preservation, quoted-tab include normalization, and all 378 instruction-memory table bytes. The evidence is `tools/local/tail-20261104/layout_dsp/portable-review/review.json`. The installed wrapper retains SHA-256 `2412ea532274c68b9ae3c8a23dad101cce81bc789b496726d37c015668e36447`; the configured executable matches the final tool hash. These private checks supplement the separate normal-build and frozen whole-DOL acceptance above.

## Traps

- DSP instruction bytes and the host array element type are separate contracts. Emitting 8,192 byte-valued elements into the existing `u16` array would double its size. Decode each assembled big-endian pair into one DSP word.
- The pinned assembler retains `INCDIR` across both nested includes and assembly passes. A conventional per-file include resolver can omit real dependencies. Reset the master directory explicitly and follow the tool's actual global state.
- The inspector can silently treat a missing include as empty text. Validate the complete literal include closure before invoking it; a zero assembler exit alone is insufficient.
- Depfile output targets must preserve the caller's path spelling relative to Ninja's working directory. Resolving every target to an absolute path can detach the depfile from a relative build edge. Dependencies may remain absolute; the real Ninja checks validate drive-colon, space, hash, and dollar escaping.
- A successful dependency scan does not guarantee a successful assembly. Stage and size-check all generated artifacts before publishing them, and treat a nonzero assembler/Ninja exit as failure even when older outputs remain on disk.

The missing-include behavior was also exercised directly through the raw inspector: a source containing `nop` followed by an absent include exits zero without diagnostics and produces only the `0000` instruction word. `build_packet/missing-include-inspector/evidence.json` preserves this independent check.
