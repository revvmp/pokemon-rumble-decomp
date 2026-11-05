# Artifact and section inventory

Target `wpse-v1` has a preserved package at `orig/wpse-v1/reference.wad` and frozen provenance manifest `config/targets/wpse-v1-package.json`. Seven declared contents are recorded in [F-0001](FINDINGS.md#f-0001-wad-container-metadata). All seven extracted contents under `orig/rumble-unidentified/extracted/` match declared sizes and SHA-1 values. Content 6 parses as a boot DOL; content 1 contains the NLZSS-compressed main game DOL. See [F-0002](FINDINGS.md#f-0002-user-extraction-validated). Executable roles and section layouts are recorded below and in F-0009; reconstruction acceptance remains scoped to main.dol.

## Artifact records

For each discovered artifact record target, relative path, kind, size, SHA-256, parent container, extraction procedure, role, acceptance-manifest membership, and reconstruction strategy. Explicitly list excluded or unexamined content and the reason. Distinguish a preserved asset from an original executable dependency.

## Section records

For each executable record format, entry point, sections, file offsets, load addresses, sizes, alignment, permissions, zero-initialized regions, relocation behavior, and dependencies where observed. Mark unavailable information unknown. Link each record to the evidence in `FINDINGS.md`.

## Coverage review

Reconcile every container entry and executable module against this inventory before claiming full coverage. Establish the total executable code size and function-count method before reporting percentages. Review package metadata and final container reproduction separately from payload identity.

## Main executable

`orig/wpse-v1/main.dol` is a derived original, not reconstructed source. Size 4,094,880 bytes; SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`; entry 0x80004050. Ten nonempty on-disk sections have validated bounds. See [F-0003](FINDINGS.md#f-0003-main-game-dol-recovered-from-compressed-content-1) for command, raw section report, identification evidence, and limits. The executable acceptance manifest is frozen at config/targets/wpse-v1.json. The completed reconstruction passes its exact comparison; see [final acceptance](findings/main-source-completion-20261104.md).

## Asset containers

Recursive extraction is recorded in F-0005: 156 U8 archives, 134 decompressions, 3,420 files including metadata, compressed parents, wrappers, derived files, and duplicates. Browse [the archive index](ARCHIVE_INDEX.md), [CSV](wpse-v1-extraction.csv), or [full provenance JSON](wpse-v1-extraction.json). All seven original contents are retained in `orig/wpse-v1/extraction/contents/`; native BRRES, RSAR, textures, layouts, fonts, effects, messages, and BSP/PKC resources remain intact. Their internal subresources and semantic formats have not been exhaustively analyzed or converted.

No named REL, RSO, ELF, linker map, or selfile.sel was found in the recorded tree. This does not establish absence of embedded executable code. The separately preserved boot DOL at `orig/wpse-v1/boot.dol` is identical to content 6 (SHA-256 c66a2944f09f383a164f7d1e78528361982894ddd0a46cb76a6f4685a8e409dd). Executable acceptance scope is `main.dol` only, frozen in `config/targets/wpse-v1.json`; the boot loader is excluded because it is a separate loader program, recorded in [F-0009](FINDINGS.md#f-0009-boot-content-is-a-nand-boot-program).

## Main executable sections

`orig/wpse-v1/main.dol`: 4,094,880 bytes, SHA-1 `fd9a2c00c97e420a42355e2c27f3dc0ebbd3d8f9`, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`, entry `0x80004050`. Header BSS `0x803E6580` size `0xD56A8`, ending `0x804BBC28`. Raw header sizes are the runtime `_rom_copy_info` sizes (file `0x2718`, address `0x80006618`) rounded up to `0x20`; dtk reports the logical sizes. Evidence: [F-0009](FINDINGS.md#f-0009-boot-content-is-a-nand-boot-program) procedure.

| Slot | Name | File offset | Address | Raw size | End | Logical size |
| --- | --- | --- | --- | --- | --- | --- |
| T0 | `.init` | `0x100` | `0x80004000` | `0x26C0` | `0x800066C0` | `0x26BC` |
| T1 | `.text` | `0x27C0` | `0x80052500` | `0x3004C0` | `0x803529C0` | `0x3004BC` |
| D0 | `extab` | `0x302C80` | `0x800066C0` | `0x3DB60` | `0x80044220` | `0x3DB5C` |
| D1 | `extabindex` | `0x3407E0` | `0x80044220` | `0xE2E0` | `0x80052500` | `0xE2DC` |
| D2 | `.ctors` | `0x34EAC0` | `0x803529C0` | `0xE0` | `0x80352AA0` | `0xDC` |
| D3 | `.dtors` | `0x34EBA0` | `0x80352AA0` | `0x20` | `0x80352AC0` | `0xC` |
| D4 | `.rodata` | `0x34EBC0` | `0x80352AC0` | `0x468C0` | `0x80399380` | `0x468C0` |
| D5 | `.data` | `0x395480` | `0x80399380` | `0x4D1E0` | `0x803E6560` | `0x4D1D8` |
| D6 | `.sdata` | `0x3E2660` | `0x804B5A20` | `0x35A0` | `0x804B8FC0` | `0x35A0` |
| D7 | `.sdata2` | `0x3E5C00` | `0x804B9C60` | `0x1FA0` | `0x804BBC00` | `0x1F9C` |

Other slots are empty and `.sdata2` ends at end of file. BSS splits into `.bss` `0x803E6580` (`0xCF490`), `.sbss` `0x804B8FC0` (`0xC84`), and `.bss2` `0x804BBC00` (`0x28`). `.init` holds the MetroTRK interrupt vector table at `0x800046E4` to `0x80006618`.

## Boot executable sections

`orig/wpse-v1/boot.dol`: 312,576 bytes, SHA-1 `8b76b2b6f4b8af0f4132b23a28b79385e1ffb503` (equal to TMD content 6), SHA-256 `c66a2944f09f383a164f7d1e78528361982894ddd0a46cb76a6f4685a8e409dd`. Header entry `0x00003400` (physical); dtk reports `0x81330000`. Header BSS field `0x81340000` size `0x10000` lies inside D0.

| Slot | File offset | Address | Raw size | End |
| --- | --- | --- | --- | --- |
| T0 | `0x100` | `0x80003400` | `0x400` | `0x80003800` |
| D0 | `0x500` | `0x81330000` | `0x4BB00` | `0x8137BB00` |

dtk splits D0 by its `_rom_copy_info` (`0x813305D4`) and `_bss_init_info` (`0x81330640`) into `.init` `0x81330000`/`0x658`, `.text` `0x81330660`/`0x2A8C4`, `.ctors` `0x8135AF40`/`0x4`, `.dtors` `0x8135AF60`/`0x4`, four data sections from `0x8135AF80` to `0x8136D77C`, and BSS `0x8136D780`/`0x47C` and `0x8136DC00`/`0xBDF0`. dtk warns that extab and extabindex could not be located. The boot image range does not overlap the main image range.
