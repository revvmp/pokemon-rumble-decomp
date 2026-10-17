# WUD HID descriptor and registration

## F-504-6: Native descriptor and device registration paths

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. WUD's first initialized object is the 217-byte HID report descriptor at `0x803DF0B8`. The reference-derived name `_wudWiiRemoteDescriptor` has medium original-spelling confidence. Its ordinary C byte-array initializer follows the pinned SDK source and reproduces all 217 target bytes. It is naturally referenced by the reconstructed registration function, rather than retained through a dummy call or an unrelated preservation object.

The old 0x21C-byte metadata record at that address mixed the descriptor, alignment and later literals. The descriptor now has its proven 0xD9-byte extent; three alignment bytes precede the remaining coarse 0x140-byte record at `0x803DF194`. The rest of the unit data still needs reconstruction. A full build after this metadata correction succeeds and the frozen DOL verifies; no original bytes or expected hashes change.

| Function | Address | Native bytes | Remaining non-relocation differences |
| --- | --- | --- | --- |
| `WUDiRegisterDevice` | `0x8030E3E0` | `0x154` | Four folded literal addresses |
| `WUDiRemoveDevice` | `0x8030E540` | `0x180` | Eight folded literal addresses |

Both functions reproduce their exact original lengths. Every differing non-relocation instruction is an `addi` with the same opcode and register operands; only the literal offset differs. Each source/target address pair resolves to a byte-identical, NUL-terminated message or device-name literal. All other non-relocation instruction words are identical. This comparison does not replace full unit data, relocation or linked acceptance. Both remain part of the `NonMatching` WUD source, with its original object still selected for the final link.

Registration adds the device address and link key to BTA, and supplies the 217-byte descriptor to HID-host registration for a Nintendo RVL-CNT device or a Nintendo RVL-WBC device when `WUDIsLinkedWBC` is true. Removal applies the same HID-device condition, removes the Bluetooth device, updates standard/simple device counts and clears the 0x60-byte device record. The target's two 16-byte device-name comparisons and relocated `_linkedWBC` access prove the Balance Board condition. The older Petari reference alone omits that condition; the newer key reference corroborates it.

The retained calls identify outside SDK functions `BTA_HhAddDev` at `0x80319718` and `BTA_HhRemoveDev` at `0x803197C0`. Only their names change in metadata; their types, extents and owning splits stay unchanged.

At this HID-registration pass, the source contained 45 of 86 retained WUD functions, totaling 6,936 function bytes. The original 42-function raw audit still proves all 42 non-relocation streams, totaling 4,848 function bytes, exactly. The security callback retains 32 instruction differences; registration and removal retain four and eight folded literal-address differences respectively. Whole-unit diagnostics are text 27.619307%, data 4.8480787%, BSS 37.36787%, sdata 0%, sbss 33.962265% and sdata2 100%. No WUD linked source-byte credit is added.

## Validation

The real `all_source` target compiles this source. The original 42-function raw audit passes; direct comparisons verify the 217 descriptor bytes, both function lengths and all twelve differing literal operands. A full Ninja build exits zero before explicit frozen verification of `build/504-source-header-refresh-1/wpse-v1`. The 41 missing functions, complete data layout, relocated references and existing security-callback differences remain unfinished.

## Provenance

The descriptor and functional registration/removal bodies follow [Petari WUD.c at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/wud/WUD.c), CC0 with no file-specific exception found. The Balance Board condition follows the actual target and [Swiftshine/key WUD.c at 7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2](https://github.com/Swiftshine/key/blob/7f2a9ab9ad66d8e2f6eac52acc2c01dff0bd03a2/src/revolution/WPAD/WUD.c), CC0. Comments and debug annotations are removed; no decompiler output is included.

## Traps

- **Older WUD registration omits Balance Board handling.** A reference-only reconstruction can skip a target name comparison and linked-board check. Confirm the RVL-CNT/RVL-WBC condition against both retained functions before reusing an older SDK implementation.
- **An initial data label can span several kinds of object.** The former 0x21C-byte record included a 217-byte protocol descriptor and later literals. Preserve the descriptor's exact extent and alignment while leaving the unreconstructed remainder explicit.
