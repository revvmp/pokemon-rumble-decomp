# Pokemon species tables and model names

## Reconstructed data

src/game/PiiSpeciesTables.cpp defines gPiiSpeciesData as 560 typed PiiSpeciesData aggregates and gSpeciesFormOffsets as 508 unsigned 16-bit indexes. Each species record is 28 bytes. The existing field schema is preserved; its data fields are public so the records can be initialized as aggregates. Access control changes no layout or executable behavior.

The record layout is: species/form/reserved in the first 16-bit word (9/5/2 bits); bone type/fly height/reserved in byte 2 (4/2/2 bits); ASize/reserved in byte 3 (3/5 bits); scale percent/reserved in bytes 4?5 (10/6 bits); motion rate/reserved in byte 6 (7/1 bits); primary and secondary type in bytes 7?8 (5/3 bits each); gender flags in byte 9; boss/default-male/reserved in byte 10 (1/1/6 bits); reserved byte 11; two model-name pointers at 12 and 16; and float coefficients at 20 and 24. Every reserved bit in all 560 original records is zero. Named numeric initializers and ordinary compiler bitfield layout reproduce these values without packed-word substitutes.

All 635 distinct model names use their exact embedded original spelling. The table uses ordinary string literals; configured string pooling reproduces duplicate references and first-use order independently in .data and .sdata. All 1120 model pointers are compiler relocations. No literal numeric address is part of the reconstructed source. Floating-point values use short decimal literals whose compiled values were verified against the original bytes.

## Provenance and object verification

All values are decoded from the original executable sealed by config/targets/wpse-v1.json. The source has no runtime dependency on an extracted binary. Direct compilation with configured Wii/1.1 game flags exits zero. The compiled object contains no functions. Its sections match the original ranges below; for the readonly pointer table, each compiler relocation is resolved using its referenced source-section base and offset before comparison. This resolution is diagnostic only and does not modify the object or build artifacts.

| Section | Original range | Bytes | Relocations | SHA-256 |
| --- | --- | ---: | ---: | --- |
| .rodata | 0x8035DF88?0x803620C0 | 16696 | 1120 | 2cc48c67cc028092ed5aaa0a894edac38281cab1e063ff82a21902c042078a76 |
| .data | 0x803B8730?0x803B9D82 | 5714 | 0 | 12f6daffb321435b9c7558305ebd974874ecd31cf87e223c25811e500bb426ff |
| .sdata | 0x804B7B78?0x804B80DF | 1383 | 0 | b9c8577d4461d1d97655c8c33f2c02363216dd07786b9a58a82832e47de2acbe |

The readonly range comprises the 560-record table (0x3D40 bytes) followed immediately by 508 form offsets (0x3F8 bytes). The two string ranges end at the final string terminator, leaving following alignment padding outside the source split. Exact comparison is repeatable with ignored tools/local/session/329-species-data-check.py; output is in 329-species-data/PiiSpeciesTables.o.

Root integrated all three sections as a Matching source unit; ninja exited zero and frozen-manifest verification passed. No fake match or unit test was needed.
