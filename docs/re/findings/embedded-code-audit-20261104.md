# Embedded executable-data audit

The bounded source-completeness review found one additional embedded controller program beyond the already disclosed AX DSP image and OS DSP bootstrap: the WUD Bluetooth firmware patch. Its PowerPC transport is reconstructed, but two retained arrays contain controller instructions. A Matching CPU unit and an exact DOL do not by themselves establish reconstruction of that program.

No source or build configuration was changed by this review. The separate [Pii production review](piipropdata-primary-msl-20261104.md#production-fidelity-and-retained-data-review) is complete and exact.

## Audit scope

The review searched `src/`, `include/`, `configure.py`, `tools/build/` and `tools/assets/` for binary includes, assembly data directives, dense hexadecimal initializers, executable/patch/firmware references and code-transfer consumers. It inspected the concrete WUD, DSP, OS exception-vector and MetroTRK program-end paths. Ordinary typed tables, graphics commands, character maps, texture data and accepted handwritten original assembly are outside this additional embedded-program category.

At the initial checkpoint before the concurrent AX source integration, `build/wpse-v1/main_link.json` selected 897 logical units. Its only original-object input was `obj/auto_07_803D95E0_data.o`, the known AX DSP image; every other logical input was under `src/`. This describes that build selection, not complete source reconstruction of every selected initializer.

No `incbin` or raw instruction-word assembly directive was found in the inspected PowerPC source. Apart from the separately owned AX DSP integration, the only binary-style generated include found was `SystemMessageArchive.cpp`'s `system_message.inc`, whose accepted semantic JSON/PNG producer is separately audited in [the archive finding](system-message-archive-20261104.md). The single standalone PowerPC assembly source is the accepted MetroTRK exception table. Runtime exception-vector installation copies those readable assembly bodies; `InitializeProgramEndTrap` writes the four-byte `'END'` marker into `PPCHalt`. Neither is another hidden binary function array. The other dense initializers inspected are numeric conversion, compression, audio-filter, cryptographic, HID, protocol, texture and collation data. The concurrent AX work adds its explicitly disclosed generated `ax_dsp.inc` and readable DSP sources; those are not newly discovered hidden dependencies.

This is a bounded review of the current executable source/build inputs, not an inventory of all programs in the WAD or of controller ROM firmware outside the DOL.

## WUD executable payload

`src/RVL_SDK/wud/WUD.c` retains these exact objects:

| Object | Main-DOL address | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| `_wudPatchData` | `0x803DF1A0` | 188 | `fde41feb3ac0bab076e24e613fd40768e3c099bcd7f34c1f746129188990ebae` |
| `_wudPatchInstallCmd` | `0x803DF25C` | 92 | `3a5f6a9f6a120cb10a4224a4bcc35db8e75459d62ebaf5b9879180b9e8de5f2c` |

The first object's eight-byte little-endian header supplies controller destination `0x00089970` and payload length `0xB4`. The second object contains a count byte and seven thirteen-byte records. Each record comprises a slot byte, a little-endian four-byte controller address and eight bytes of replacement Thumb instructions.

The source gate `__wudCheckFirmwareVersion` invokes the patch path when the low twelve bits of the controller HCI revision equal 167. Its diagnostic identifies the BCM2045 firmware family `002.003.014`. `__wudAppendRuntimePatch` decodes the header; `__wudWritePatch` submits the payload through vendor command `0xFC4C`; `__wudInstallPatch` submits the patch records through `0xFC4F`. These are executable controller replacements, not ordinary HID or radio-configuration tables.

`vm_wud_firmware_audit.py` compares initializer bytes, the unmodified compiled WUD object, the native linked ELF and the frozen DOL. Both objects agree at every byte in all four representations. Its receipt is `tools/local/tail-20261104/vm_wud_firmware/audit.json`. No original-object relocation placeholder is interpreted as payload data; this preserves the correction already documented in [the earlier WUD transport finding](504-wud-firmware-patch.md).

The earlier finding correctly distinguishes the controller payload from PowerPC functions. This review adds the source-completeness consequence: those 280 bytes include embedded executable code, even though they are emitted by a Matching PowerPC C unit. The controller source and its separately approved language exception must therefore remain disclosed alongside CPU reconstruction coverage.

## Instruction and data partition

Addresses below are controller addresses, not PowerPC addresses:

| Range | Bytes | Observed content |
| --- | ---: | --- |
| `0x89970`–`0x89984` | 20 | Updates a word using incoming register values, stores a halfword and continues in ROM at `0x5A94` |
| `0x89984`–`0x89998` | 20 | Updates byte state, calls ROM at `0xA7B8`, stores a halfword and continues at `0x6110` |
| `0x89998`–`0x899A4` | 12 | Copies a byte into firmware state and continues at `0x1A0D0` |
| `0x899A4`–`0x899A8` | 4 | Literal address `0x00081DD8` |
| `0x899A8`–`0x899B2` | 10 | Calls the local reset helper, clears `r3` and continues at `0x2F78` |
| `0x899B2`–`0x899B4` | 2 | Zero word before the local helper; production spells its valid Thumb interpretation `movs r0, r0` |
| `0x899B4`–`0x89A00` | 76 | Clears control words, waits in a decrement loop, writes a reset command, calls ROM at `0x20904` twice and masks three control words |
| `0x89A00`–`0x89A24` | 36 | Nine address/value/mask literals used by that helper |

The 180-byte payload therefore contains 138 proven instruction bytes, 40 literal bytes and a two-byte zero word whose original padding-versus-instruction role is not established. The seven installation records add 56 instruction bytes. The two full objects contain 194 proven Thumb instruction bytes plus 40 literals, the zero word and 44 bytes of protocol fields. The production source represents the zero word semantically as `movs r0, r0`, preserving its behavior if reached without asserting the original alignment intent. These classifications do not change the project denominator or independently grant new source credit.

The three final masks preserve only selected bits from the low sixteen bits of each 32-bit word; they also clear every upper-half bit. Their private source labels are `KeepLow16ExceptBit0`, `KeepLow16ExceptBits13And14` and `KeepLow16ExceptBit7`, avoiding a claim that only the individually named low bits are cleared.

| Slot | Replacement destination | Proven transfer |
| --- | --- | --- |
| `0x20` | `0x165BC` | Comparison branches to `0x165D4` and `0x165D8` |
| `0x21` | `0x5A84` | Calls payload entry `0x89970` |
| `0x22` | `0x6100` | Calls payload entry `0x89984` |
| `0x23` | `0x19FCC` | Calls payload entry `0x89998`, then branches on comparison |
| `0x24` | `0x1623C` | Substitutes an immediate and branches over a halfword load |
| `0x25` | `0x11204` | Preserves a register move and branches to `0x11254` |
| `0x26` | `0x2F74` | Calls payload entry `0x899A8` |

The partition uses instruction references and literal loads, not linear disassembly alone. For example, decoding the word at `0x899A4` as instructions gives plausible Thumb operations, but the preceding PC-relative load proves it is an address literal.

## Private readable assembly proof

The private `vm_wud_firmware/` packet contains `controller_patch.s`, seven short `patch_*.s` sources, typed `patches.json` slot/address records and `build.py`. The assembly uses descriptive labels, named ROM continuations, explicit hardware constants and typed literal words. It contains no opcode arrays. Its names describe observed operations; they are not recovered original controller symbols or proven names for the ROM functions.

The actual CLI `python tools/local/tail-20261104/vm_wud_firmware/build.py` assembles the sources and writes the two complete payload objects, then independently compares them with the frozen reference bytes. It exits zero. The 180-byte payload has SHA-256 `e39ef5d17542050d2e560c7edc7dc5b477eb4613bd9f4268ed6f6b76ad504fca`; both complete object hashes equal the table above. `build-audit.json` records every source hash and all seven exact generated instruction extents. Reference binary files are private diagnostic inputs used only for comparison, not generation.

The assembler is [Keystone](https://github.com/keystone-engine/keystone), pinned to Python package `keystone-engine==0.9.2` in the private `vm_wud_deps` directory. Its DLL has SHA-256 `3ac03927981fca588c95b2cd1f2f1d64dc33dbd55b362fc7742163647746c3e8`. Capstone 5.0.7 supplies independent read-only Thumb decoding. No shared environment, configured game toolchain or expected reference was changed.

Keystone's fragment handling requires caution here. The diagnostic `.space 2, 0` and `.balign 4, 0` forms produced two later absolute calls targeting `0x20906` instead of `0x20904`; the alignment form also emitted a Thumb-2 NOP. Declaring the known two-byte data alignment as `.hword 0` produces the exact original calls and bytes. This observed local behavior is documented rather than hidden by output patching. The resulting private proof still does not supply an accepted production assembler pipeline.

## Concrete C embedding and native verification

The private encode-only CLI is `vm_wud_firmware/generate.py`. It reads only the assembly files and typed `patches.json` manifest and writes `wud_patch_data.inc`, `wud_patch_commands.inc`, generated binary receipts and `generation.json` beneath the requested output directory. It does not read a DOL, original object, payload reference or comparison script. Its manifest checks include expected fields, bounded even controller addresses, unique slots/addresses, local assembly paths, exact assembled extents and output/input separation.

`vm_wud_compile.py` prepares the minimal candidate:

- The existing `_wudPatchData` initializer includes the generated payload initializer.
- The existing `_wudPatchInstallCmd` initializer includes generated typed records.
- The private `WUDPatchCmd` header view replaces the opaque `data[13]` member with `slot`, `littleEndianAddress[4]` and `thumbCode[8]` fields. Its size and byte alignment remain thirteen and one.
- The installer copies from the address of that record instead of its former first byte-array member. All transport control flow is unchanged.

The source candidate is `RVL_SDK_wud_WUD/vm_wud_typed/WUD.c`; its only header overlay is `vm_wud_firmware/include/revolution/wud/WUDInternal.h`. The compiler command comes directly from the configured normal Wii/1.0 WUD build, with only scratch input/output/include paths changed and dependency-file emission removed. No shared configure/ninja job was run by this lane.

The fresh baseline and candidate contain the same 86 real CPU functions. Their seven allocated sections have identical bytes, relocation types/identities/addends, flags and alignment, and the candidate also agrees with the accepted normal WUD object. The baseline and candidate retain identical objdiff percentages, including existing split padding/literal identity artifacts; no fuzzy score is used as a substitute for the complete comparison. `vm_wud_firmware/compile-audit.json` records this evidence and the exact compiler command.

The isolated native trial replaces only WUD's compiled object in the complete current link. Its outputs are under `links/vm-wud-generated`: native link exited zero, ELF-to-DOL conversion exited zero and the frozen verifier reported PASS. The resulting 4,094,880-byte executable has SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`, with zero differing bytes. The generated data is therefore a concrete, fully checked proposal, not an untested plan. The owner subsequently approved the narrow source-language exception below; the native production route replaces Keystone as described next.

## Pinned native production packet

The production candidate is `tools/local/tail-20261104/vm_wud_production`. Its `ready-files.json` inventories thirteen proposed files: the C owner and typed header, eight readable assembly files, the semantic patch manifest and linker script, and `tools/firmware/build_wud.py`. An unchanged private copy of the already accepted `tools/dsp/build_dsp.py` supports isolated validation; it is not a proposed change to that shared helper.

The [independent Zig investigation](wud-thumb-zig-20261104.md) supplies the native assembly route. Each input occupies an ordinary named ELF code section. Actual cross-file payload calls use exported Thumb function symbols; external ROM continuations are named linker symbols with the normal low Thumb-state bit. The eight section addresses come from the readable linker script and are checked against the typed manifest. No instruction is encoded or modified by Python.

The generator accepts `--manifest`, `--tool-install`, `--tool-manifest`, two binary output paths, two C-include output paths and a depfile path. It resolves Zig from the installed tool's `build.json` receipt and pinned `config/dsp_assembler.json`, validates the receipt's manifest digest and compiler executable digest, and invokes the native assembler, linker and per-section binary extraction. Zig 0.13.0 has executable SHA-256 `2e44af5bbf7a72ef8cbdae370284687c95d65a19affa469d2ad0364d905b8e84`; no new Keystone installation is needed for production.

The wrapper reads no original artifact or expected output content hash. It validates all eight linked sections' addresses, lengths and executable flags, rejects unexpected allocated sections, checks 180-byte and eight-byte generated code extents, and serializes the 188-byte download record and 92-byte typed install record. Manifest validation rejects duplicate JSON fields, duplicate slots, overlapping destinations, invalid Thumb addresses and nonlocal source paths. This bounded assembly set is self-contained: `.include`/`.incbin` tokens and linker file-import directives are rejected rather than silently introducing untracked dependencies.

The shared DSP helper provides the already reviewed path/hardlink collision checks, depfile escaping, staged publication, rollback and unchanged-content timestamp preservation. The WUD generator protects and checks all input snapshots before publication. Its depfile tracks fifteen actual inputs: the eight assembly files, patch manifest, linker script, WUD wrapper, shared publication helper, tool manifest, install receipt and resolved compiler executable. Receipt changes therefore invalidate outputs after a pinned-tool reinstall.

The first real production CLI run exits zero and produces both exact object hashes. Compiling its generated includes with the production C/header candidate produces an entire `WUD.o` file byte-identical to both the accepted normal WUD object and the earlier frozen-SHA-passing candidate, including nonallocated metadata. `vm_wud_production/production-compile-audit.json` records that stronger comparison.

The [independent wrapper validation](wud-build-wrapper-validation-20261104.md) passed all 37 bounded real-CLI and Ninja cases. They include native positive generation, actual assembly edits, manifest and address rejection, input/output aliases, hidden assembly/linker input rejection, tool-receipt mismatch, input mutation during assembly, locked-output rollback, dependency-triggered rebuilding and subsequent no-op builds. All fifteen dependencies were retained for each of the four generated output targets, including paths containing spaces, `#` and `$`. The final wrapper SHA-256 is `76ef7a016cecc19872498a238bdeaf55aff4d207f45978eb9e2d2027a4a4c635`; `selector/WUD/wrapper_evidence.json` records the complete result. The final thirteen-file packet inventory has SHA-256 `f6a89c53edcc1c6d3e9a3eb31bb56ed22197f13c6074657457e8a36dfa83bb2f`. The [independent source review](embedded-firmware-source-review-20261104.md) also accepted the readable Thumb source, typed protocol records and generator's captured-input validation.

The coordinator installed that packet and reported normal production acceptance on 2026-11-04: configure exited zero, ninja exited zero and the frozen DOL verifier passed. The canonical `WUD.o` is byte-identical to the previously accepted object in its entirety, with SHA-256 `992538d0511f5ad587bd5b4f53a9898b068ebbaf4c99e90116b749e985928efe`. At this checkpoint all 897 logical units are selected from source; the normal report retains 13,462 exact CPU functions, 3,078,460 linked CPU-code bytes, 1,788,360 linked data bytes and 902 exact aliases. These CPU/data totals do not turn the approved controller-assembly exception into evidence of original authoring language. Repeated clean-build acceptance remains the coordinator's separate verification step.

## Authoring-language limit

The bounded provenance search found the existing [Swiftshine/key WUD reconstruction](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c) and matching byte arrays in other decompilations. The apparent SDK source at [robojumper/sdk_2009-12-11](https://github.com/robojumper/sdk_2009-12-11/blob/19e2ec48d61a84ee072dc4fb5ac1db9bfd08c766/README.md) explicitly identifies itself as a decompilation, not an original controller-source distribution. It therefore does not prove how the Thumb program was authored.

The [Foxconn module manual](https://fcc.report/FCC-ID/MCLJ27H007/834330.pdf) corroborates the `BCM2045B3_002.003.014.0167` firmware family and Nintendo configuration tools. It does not provide controller C/assembly source or compiler provenance. No original authoring source, assembler attribution or compiler identity for this patch was recovered in the bounded search.

The inline hook blocks use live registers from existing ROM execution, which is consistent with assembly patching. The 76-byte helper has an ordinary callable shape and could have been compiled from C. Neither observation proves the whole original payload was assembly. [D-0015](../../DECISIONS.md#d-0015-assembly-source-only-where-the-original-was-assembly) requires: “An assembly-source unit is allowed only where evidence shows the original unit was assembly” and “Compiled code must never be recovered as assembly.” D-0016 separately requires remaining fake matches to be C or C++.

Before approval, the exact private assembly and full-link proposal remained diagnostic and earned no reconstruction credit. The owner then explicitly replied, “Approve these two assembly exceptions.” [D-0017](../../DECISIONS.md#d-0017-two-owner-approved-embedded-firmware-assembly-exceptions) records that narrow approval for this controller patch and the OS DSP bootstrap. It does not claim original-language recovery, change the rule for other compiled code, or authorize raw machine-code inputs. The coordinator owns canonical integration, complete production verification and repeated clean-build acceptance.

## Traps

- A source-selected CPU unit can still embed executable controller bytes in a C initializer. Audit consumers and target instruction sets separately from CPU matching counts.
- A typed protocol record can include executable instructions. The WUD record's slot and destination fields do not make its eight replacement bytes ordinary data.
- Plausible linear decoding is not a code/data partition. Resolve PC-relative literal loads and installed entry points before classifying bytes.
- Exact reassembly proves encoding, not original authoring language. Do not use it alone to bypass the assembly-source evidence requirement.
