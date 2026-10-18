# F-315-2: allocation operator interfaces and C allocation exception guarantees

## Scope

This bounded prerequisite extends the existing new and stdlib headers for the allocation operators owned by #315. It preserves placement new and the existing allocator interface. No source object, split, symbol metadata or source-coverage credit changes.

The complete standard bad_alloc declaration derives from the existing exception interface and has no added storage. Its constructors, assignment, destructor and what method are declarations only. These ordinary standard API signatures and inheritance are an interface reconstruction, not recovered implementations of those methods. The target directly proves the std::bad_alloc exception identity and its use in the allocation-operator filter. The new_handler type is the void, no-argument function pointer invoked by native new. The global handler definition remains outside this header.

## Native consumers

An independently expressed diagnostic uses only the canonical project headers, actual Wii/1.1 game flags, and real C++ operator definitions. It reproduces every instruction byte in four native functions, with all relocations paired to their actual targets:

| Address | Operator | Bytes |
| --- | --- | --- |
| 801B1450 | new | 136 |
| 801B14E0 | new array | 152 |
| 801B1580 | delete | 16 |
| 801B1590 | delete array | 16 |

The complete 320 instruction bytes, 132 exception bytes and both native exception indices match. Native targets are malloc at 80342F10, free at 80342FB0, the handler word at 804B9BC0, unexpected at 80341A60 and the bad_alloc filter string at 803BF3B8.

Both deletes retain a final unreachable BLR. Its presence at 801B158C and 801B159C is proved by the frozen executable and ordinary compiler emission; the twelve-byte IDA estimates omit retained instructions.

The nothrow compiler attributes on malloc and free express a stronger code-generation guarantee than an empty dynamic exception specification. Without the malloc guarantee, the new exception region starts after its malloc call, at offset 30. Native starts that region only after the handler call, at offset 50. Adding the ordinary attribute reproduces the complete original exception record. Without the free guarantee, delete gains an unexpected-handler scaffold and no longer matches its native sixteen-byte body. A throw() declaration alone does not resolve this. Wii/1.3 reproduces the same rejected result without the attributes; no flags or pragmas were changed.

New still handles allocation failure by calling the configured handler and returns null when no handler exists. No failure behavior is substituted to obtain the match.

## Existing-source and retained-output proof

The same-base control is main 4f439910, including the accepted PR881 common-control extension. It uses the isolated rumble-315 worktree and a new build-315-header-control output directory. Original and proposed headers both run explicit all_source, plain Ninja and the actual-directory frozen verifier. All exit zero.

The complete baseline contains 517 source objects. All preserve allocated section bytes, sizes, flags, alignments, raw relocation records and symbol attributes. Of those, 506 whole object files remain byte-identical; eleven differ only in compiler-private numeric symbol spellings. The enriched comparison normalizes only names matching @ followed by digits, retaining every symbol value, extent, binding/type and section identity. No additional function or data emission occurs.

The complete ELF header, section headers, non-NOBITS payloads and symbol attributes remain unchanged except 35 numeric local-symbol spellings in strtab. NOBITS sections have no file payload. This is not a claim that the entire ELF file has the same hash. Whole main.dol is byte-identical to the control and frozen original, SHA-256 ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da.

## Rebased complete baseline

A fresh control/proposed build on actual base `ea4ffb474229f4c66f35b85f42af48631c1de1b1` contains 532 source objects. Explicit `all_source`, plain default Ninja, and the actual-directory frozen verifier all exit zero for both configurations. All 532 preserve allocated payloads, flags, alignment, raw relocations and normalized symbol attributes; 521 whole files are identical and eleven differ only in numeric local-symbol spellings. The complete linked ELF header/section metadata and all other payloads agree, with the same 35 numeric symbol-name changes. The entire DOL is byte-identical. The canonical operator consumer again proves four bodies/320 instruction bytes, every target, all 132 exception bytes and both indices.

`C:/MassStorage/GitHub/315-latest-header-gate.py`, its log and JSON record this repeat in `build-315-header-latest`. Shared origin/main advanced while the gate ran; the actual base is the verified branch parent. The original header bytes on the later shared ref were independently confirmed identical. Future reproduction records the parent directly rather than a mutable shared ref. This checkpoint does not waive the fresh-main ancestry guard before merge.

## Exact current-main accepted owner control

On actual main `813a7c745d081a82bf86e3ef76d7fbd54fc02e68`, the accepted PR879 owner snapshot `a70262465efb08731e782639dfbb20d1056ac232` supplies the control. An independent empty git diff and byte-identical `git ls-tree -r -z` output establish exact tracked-tree equivalence, including images, with SHA-256 `54e7e8c48313bad4a78eec51d6429bab6b17f2677319bb283083a34789df2b3c`. All 535 declared source-object hashes and every saved linked artifact hash are verified before copying the immutable control under `315-reference/905-control-813a7c74`. The control is reused from its actual successful owner build; no control bytes are inserted into the proposed build.

The proposed branch then runs configure, explicit all_source, plain default Ninja, the actual-directory frozen verifier, and both canonical consumer compiler/audit commands, all exiting zero. Its exact Ninja-declared 535-object set agrees with the control. Every allocated payload, section flag/alignment, raw relocation and normalized symbol attribute is unchanged. Of those, 524 entire object files are identical and eleven differ only in compiler numeric labels. The complete ELF retains the same header/section metadata and all other payloads, with 35 numeric local-symbol spelling changes. The entire DOL is byte-identical. The canonical four-operator consumer independently retains its exact 320 instruction bytes, all targets, 132 exception bytes and both indices.

`315-capture-879-control.py`, `315-proposed-813-gate.py`, their logs, and `315-proposed-813-proof.json` reproduce snapshot validation and the proposed checks. This avoids repeating an already accepted identical control build. The final exact committed-head repeat and fresh-main ancestry guard remain required before merge.

## Reproduction

Diagnostic source and tools remain outside the checkout:

- C:/MassStorage/GitHub/315-header-consumer.cpp and 315-compile-header-consumer.py: canonical-only operator consumer.
- 315-audit-new.py, using 315-header-consumer.o for the canonical run: paired instruction, exception and index comparison.
- 315-capture-header-control.py and 315-verify-headers.py: configure, explicit all_source, plain Ninja and frozen comparison.
- 315-enriched-header-proof.py: original/proposed complete object flags, alignment, raw relocations and normalized symbol attributes.
- 315-header-enriched-control.json, 315-header-enriched-proposed.json and 315-header-elf-symbol-diffs.json: durable comparison outputs.
- 315-header-control-current.log, 315-header-proposed.log and 315-enriched-header-proof.log: actual command results.

The verifier is invoked with --build-dir build-315-header-control/wpse-v1. The default SHA rule checks the default build directory even when a custom directory is configured, so that rule alone does not establish this custom build's identity.

Source declarations and consumer bodies are independently expressed from native behavior and ordinary standard interfaces. No restricted library implementation is imported. The diagnostic does not publish or credit the four source bodies.

## Traps

- IDA can omit an unreachable retained BLR. Compare the actual instruction bytes and ordinary compiler extent before shrinking a source split.
- An empty dynamic exception specification does not replace a compiler nothrow guarantee on a C allocation function. Compare exception records as well as instructions and call targets.
- New declarations can renumber compiler-private labels while preserving native code, data and all symbol attributes. Distinguish that documented bookkeeping change from whole-file identity.
- For custom build outputs, require Ninja exit zero and explicitly verify the actual directory. A passing default SHA rule can describe a different main.dol.


## Final post-903 current-main control

The accepted PR903 source head `eca5775d71ec38f503b0d129a4b28c8f2a7efb0f` and actual refreshed main `95d4ee867cd60be6c9aac6048888789fc3485ed3` differ only in `docs/images/progress-code.svg`. Independent capture from its unchanged live `build/wpse-v1` confirms that all_source has no pending work and saves all 549 Ninja-declared source objects, the ELF, frozen DOL, config, linker script, Ninja file and both tracked trees under `C:/MassStorage/GitHub/315-reference/905-control-95d4ee86`. The control ELF SHA-256 is `302ba133543f9730c36c34e84fd4e7fe821e5e2f77c1443bcf565561a6cac8c3`; the DOL SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. This reuses the accepted actual build without rebuilding a redundant control or copying control objects into the proposal.

A fresh proposed output `build-315-header-903` passes configure, explicit all_source, plain default Ninja and the actual-directory frozen verifier. Every allocated payload, flag, alignment, raw relocation and normalized symbol attribute in all 549 existing objects is preserved. Of those, 533 whole files are identical and sixteen differ only in numeric compiler-local symbol names. The full ELF preserves its header, section metadata, payloads and symbol attributes, with 55 numeric local-name changes. The complete DOL is byte-identical. The canonical operator consumer now derives its flags directly from the actual BController Ninja command and independently reproduces all 320 code bytes, actual relocation targets, 132 exception bytes and indices. No source bodies are selected or credited.

`315-capture-current-control.py` and `315-proposed-latest-gate.py` reproduce the capture and proposed checks; the latter takes the immutable control directory, actual base SHA and proposed output directory. `315-proposed-latest-proof.json` records all numeric-name differences. The first full invocation reached every native/artifact comparison but rejected its final clean-tree check because the fresh scratch output directory was untracked. Adding only that output path to local git exclusions and repeating the complete gate succeeds with an empty status. No generated output is committed. A committed-head repeat and fresh ancestor/exact-head merge guard are still required.
