# Artifact and section inventory

Target `wpse-v1` has a preserved package at `orig/wpse-v1/reference.wad` and frozen provenance manifest `config/targets/wpse-v1-package.json`. Seven declared contents are recorded in [F-0001](FINDINGS.md#f-0001-wad-container-metadata). All seven extracted contents under `orig/rumble-unidentified/extracted/` match declared sizes and SHA-1 values. Content 6 parses as a boot DOL; content 1 contains the NLZSS-compressed main game DOL. See [F-0002](FINDINGS.md#f-0002-user-extraction-validated). Full executable roles and section inventory remain pending.

## Artifact records

For each discovered artifact record target, relative path, kind, size, SHA-256, parent container, extraction procedure, role, acceptance-manifest membership, and reconstruction strategy. Explicitly list excluded or unexamined content and the reason. Distinguish a preserved asset from an original executable dependency.

## Section records

For each executable record format, entry point, sections, file offsets, load addresses, sizes, alignment, permissions, zero-initialized regions, relocation behavior, and dependencies where observed. Mark unavailable information unknown. Link each record to the evidence in `FINDINGS.md`.

## Coverage review

Reconcile every container entry and executable module against this inventory before claiming full coverage. Establish the total executable code size and function-count method before reporting percentages. Review package metadata and final container reproduction separately from payload identity.

## Main executable

`orig/wpse-v1/main.dol` is a derived original, not reconstructed source. Size 4,094,880 bytes; SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`; entry 0x80004050. Ten nonempty on-disk sections have validated bounds. See [F-0003](FINDINGS.md#f-0003-main-game-dol-recovered-from-compressed-content-1) for command, raw section report, identification evidence, and limits. No executable acceptance manifest has been frozen yet.

## Asset containers

Recursive extraction is recorded in F-0005: 156 U8 archives, 134 decompressions, 3,420 files including metadata, compressed parents, wrappers, derived files, and duplicates. Browse [the archive index](ARCHIVE_INDEX.md), [CSV](wpse-v1-extraction.csv), or [full provenance JSON](wpse-v1-extraction.json). All seven original contents are retained in `orig/wpse-v1/extraction/contents/`; native BRRES, RSAR, textures, layouts, fonts, effects, messages, and BSP/PKC resources remain intact. Their internal subresources and semantic formats have not been exhaustively analyzed or converted.

No named REL, RSO, ELF, linker map, or selfile.sel was found in the recorded tree. This does not establish absence of embedded executable code. The separately preserved boot DOL at `orig/wpse-v1/boot.dol` is identical to content 6 (SHA-256 c66a2944f09f383a164f7d1e78528361982894ddd0a46cb76a6f4685a8e409dd). Full raw section records and executable acceptance scope remain pending.
