# WUD Thumb assembly with pinned Zig, 2026-11-04

## Result and policy boundary

The already acquired Zig 0.13.0 toolchain assembles and links the readable WUD controller patch without Keystone. Native LLVM assembly, linking, and section extraction reproduce the 180-byte payload and all seven eight-byte replacement blocks exactly. Rebuilding the seven installation records also reproduces all 92 bytes.

The investigation began as a private tooling experiment. The owner subsequently authorized the narrow WUD/OS firmware-assembly exception during the same session. That authorization is separate from proof that this controller revision was originally hand-written assembly; this experiment supplies no new original-language provenance. The coordinator owns the exception record and production source/build acceptance.

This lane changed no canonical source, configuration, original data, or expected hash. It handed the native tool route and readable source packet to the WUD implementation owner.

## Inputs and source adaptation

The firmware owner's private `vm_wud_firmware/patches.json` identifies the controller image as `BCM2045B3_002.003.014.0167`, the payload address as `0x00089970`, and the seven replacement addresses and installation slots. Its `controller_patch.s` and seven `patch_*.s` files contain readable Thumb instructions and named data literals. The [existing WUD transport finding](504-wud-firmware-patch.md) establishes the little-endian download and installation record formats.

LLVM accepted the instruction syntax directly. However, copying Keystone's absolute code-address `EQU` declarations verbatim caused short conditional/unconditional branches to fail range checks: an ELF assembler does not have Keystone's separately supplied origin. The working source keeps the instructions unchanged and expresses the location through ordinary ELF sections, symbols, and a linker script.

Each source occupies a named `.text.<source-stem>` section. The linker assigns each section the exact address from the patch manifest. The actual payload entry points use `.global`, `.type ..., %function`, and `.thumb_func`; branches from the replacement sections resolve to those definitions. Calls and branches to existing controller-ROM code use external symbols defined by the linker script.

External Thumb code symbol values set bit zero, as normal ARM ELF symbol-state metadata. Thus a controller code address such as `0x00005A94` has the linker symbol value `0x00005A95`; the linked branch still reaches the original even instruction address. This avoids interworking veneers or manual branch encoding. The exact section sizes and all original branch words agree.

Data addresses and immediate constants remain ordinary named assembler constants. The ten payload data literals are retained as word data. The named zero halfword between reset entry and helper is preserved. No instruction is replaced with a numeric word array, and no object or output instruction is patched.

The first successful standalone packet is `tools/local/tail-20261104/sort/DSP/wud_zig/linked/`. Its eight assembly files and `wud_patch.ld` remain frozen for the implementation owner. A second independently resolved invocation writes `linked_canonical/`.

## Native command route

The compiler is the same pinned portable Zig executable already acquired by the [DSP host-tool setup](ax-dsp-host-tool-20261104.md), SHA-256:

`2e44af5bbf7a72ef8cbdae370284687c95d65a19affa469d2ad0364d905b8e84`

The final private driver resolves its actual location from canonical `tools/local/dsp-assembler/build.json` and `config/dsp_assembler.json`, then checks that digest. It does not assume the prototype compiler directory exists.

The real command sequence is:

```text
zig.exe cc -target thumb-freestanding-eabi -mcpu=arm7tdmi -c SOURCE.s -o SOURCE.o
zig.exe ld.lld -m armelf -T wud_patch.ld -o wud_patch.elf OBJECTS...
zig.exe objcopy -O binary --only-section=.text.SOURCE_STEM wud_patch.elf OUTPUT.bin
```

All compiler, linker, and extraction return codes are checked through native Python subprocesses. The Thumb target and ARM7TDMI CPU select ARMv4T instructions. The compiler emits a harmless unused `-ffreestanding` warning for assembly-only input; all eight successful assembler invocations return zero. No diagnostic-suppression or encoding-specific flags are required.

Each extraction selects one already linked section. It excludes ELF headers, attributes, and address gaps; it does not rewrite the selected bytes. The resulting lengths are exactly 180 for the payload and eight for each replacement.

## Exact results

| Source | Controller address | Bytes | Exact |
| --- | --- | --- | --- |
| `controller_patch.s` | `0x00089970` | 180 | Yes |
| `patch_compare.s` | `0x000165BC` | 8 | Yes |
| `patch_connection_control.s` | `0x00005A84` | 8 | Yes |
| `patch_device_state.s` | `0x00006100` | 8 | Yes |
| `patch_firmware_byte_state.s` | `0x00019FCC` | 8 | Yes |
| `patch_connection_limit.s` | `0x0001623C` | 8 | Yes |
| `patch_connection_branch.s` | `0x00011204` | 8 | Yes |
| `patch_controller_reset.s` | `0x00002F74` | 8 | Yes |

Payload SHA-256: `e39ef5d17542050d2e560c7edc7dc5b477eb4613bd9f4268ed6f6b76ad504fca`.

Installation-record SHA-256: `3a5f6a9f6a120cb10a4224a4bcc35db8e75459d62ebaf5b9879180b9e8de5f2c`.

Both the prototype compiler path and the canonical receipt-resolved compiler produce these exact results. `link-audit.json` and `canonical-link-audit.json` record every command, exit status, diagnostic, section length, difference list, and hash.

Run `python tools/local/tail-20261104/sort/DSP/wud_zig/link.py` for the canonical-tool private comparison. This driver reads reference binaries only after assembly to validate the outputs. It is a research validator, not the production generator; the WUD owner was explicitly handed the requirement that original reference binaries must not become generation inputs.

## Traps

- An absolute code-address constant can work in an origin-aware in-memory assembler but fail or encode differently in an ELF assembler. Use relocatable code symbols and link each section at its real address rather than adding instruction-specific offsets.
- A Thumb symbol's low bit is state metadata, not an off-by-one instruction address. Preserve that state when resolving ROM continuations through a native ARM linker.
- Extracting the complete linked address span would include large gaps between firmware patch sites. Extract each named section independently and serialize its transport address separately.
