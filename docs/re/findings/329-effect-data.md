# Effect descriptor table and names

## Schema and provenance

src/game/EffectInfos.cpp reconstructs gEffectInfos as 696 typed EffectInfo aggregates. The existing 24-byte schema contains a signed ID, three string pointers (effect name, node name, target node name), a signed type, and a capture-group boolean. All boolean bytes in the original are 0 or 1; all three trailing padding bytes per record are zero. The type and declaration are isolated in include/game/EffectInfo.h, included by CEffect.h, preserving the existing layout and callers.

The table and 585 distinct strings are decoded from the original executable sealed by config/targets/wpse-v1.json. Each string is an ordinary named character array with the exact original spelling and observed four-byte alignment. Source identifiers derive from the original names with a descriptive sEffectName prefix. The table refers to these arrays and uses nullptr for absent references. No numeric executable address or raw byte array appears in the source.

An initial version used string literals directly in aggregate rows. It reproduced every string and total section size, but first-use pooling differed from original storage order for Mii_warp, atk_range, and atk_normal_tyuusin_DAIBAKUHATU. Explicit named string objects preserve the existing storage and all references without compiler timing probes or fake code.

## Verification

Direct compilation with the configured Wii/1.1 game flags exits zero and produces no functions. The readonly section contains 1409 compiler pointer relocations. Resolving each relocation through its named source section and offset reproduces every original table byte. This address resolution is a comparison diagnostic only; it never modifies compiled objects or build artifacts. Both string sections compare directly without normalization.

| Section | Original range | Bytes | Relocations | SHA-256 |
| --- | --- | ---: | ---: | --- |
| .rodata | 0x803550E0?0x80359220 | 16704 | 1409 | 35d108da13738b23def3458119090e4dbdeebad20c89b42d4eb30b9433824d62 |
| .data | 0x803AFC38?0x803B302D | 13301 | 0 | d3ca467c05f7e5231571be509736e853c0d9a228bd5c7aac6fb7ad43af1111dc |
| .sdata | 0x804B7740?0x804B77D8 | 152 | 0 | 923b42f2d81ef971a82126102bfaf0be4e54c05a4bca4d7ac3a5f7dad0afab45 |

The data boundaries end at the last owned character or terminator, leaving subsequent padding to the following unit. Repeatable direct verification is in ignored tools/local/session/329-effect-data-check.py, with object output in 329-effect-data/EffectInfos.o. Root integrated all three sections as Matching; ninja exited zero and frozen-manifest verification passed. No fake match or unit test was needed.
