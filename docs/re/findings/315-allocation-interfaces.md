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
