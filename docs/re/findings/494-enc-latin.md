# F-494-1: ENC Latin conversion unit

## Boundaries and provenance

Target `wpse-v1`, `orig/wpse-v1/main.dol`, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The complete `enclatin.c` unit owns `.text` `0x802ED120–0x802ED700` and `.rodata` `0x8037BB90–0x8037D690`. It has two generic converters followed by twenty private conversion wrappers; all references to the thirteen forward tables and seven reverse hash tables stay within this data range. DOL `.text` file offsets are runtime address minus `0x8004FD40`.

Source and table provenance: [wii-ipl enclatin.c](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/src/enc/enclatin.c), with ENC type, Latin and utility headers at the same revision. The repository's root LICENSE is CC0-1.0; these files have no additional file-level license notice. Comments, unused public wrappers and unused reverse maps were removed. Unicode character literals became explicit code points to avoid source-encoding dependence. Tables remain editable C initializers; no executable bytes or generated binary payloads are embedded.

The issue's leading function, `0x802ECFB0`, is the tail of the preceding Japanese conversion unit. `0x802ED700` begins `encconvert.c`, continuing past the issue into #495. Those five functions remain original fallback; this bounded change does not complete #494.

## Names and layouts

`ENCiConvertStringLatinToUnicode` at `0x802ED120` has size `0x1F8`; `ENCiConvertStringUnicodeToLatin` at `0x802ED320` has size `0x298`. Twenty 12-byte private wrappers follow at 16-byte boundaries `0x802ED5C0–0x802ED6F0`. In address order they are Latin1, Latin2, Latin3, Greek, Latin6 and Latin9 forward/reverse pairs, Win1252 reverse, and Win1250, Win1253, Macroman, Macgreek, Macce, Ibm850 and Ibm852 forward conversions. These names have high confidence from reference function order, table references and identical compiler output.

The forward tables are thirteen arrays of 128 `u16` entries, beginning at `0x8037BB90`, stride `0x100`, in the order Latin1, Latin2, Latin3, Greek, Latin6, Latin9, Win1250, Win1253, Macroman, Macgreek, Macce, Ibm850 and Ibm852. Reverse maps are seven arrays of 128 four-byte records beginning `0x8037C890`, stride `0x200`: Latin1, Latin2, Latin3, Greek, Latin6, Latin9, Win1252. Each record has `u16 c16` at offset 0, `u8 cc` at 2 and `u8 next` at 3, size/alignment 4/2. Target loads confirm every field.

The input/output sizes are signed 32-bit counts, source and destination validity are `BOOL`, and the break enum values are keep/CRLF/CR/LF = 0/1/2/3. Latin conversion uses 16-bit big-endian internal characters and skips a leading `0xFEFF` BOM; `0xFFFE` returns invalid format. Hash lookup follows byte-sized collision links.

## Validation

`python configure.py`, `.venv/Scripts/ninja` and the frozen-manifest verifier each exited 0 for the split-only build. With source registered Matching, objdiff-cli 3.8.2 reports 22/22 matching functions, 1,408/1,408 matching code bytes and 6,912/6,912 matching data bytes. Wii/1.0 with `cflags_base` and default alignment produces this result.

The three utility names outside this claim are `ENCiCheckParameters` (`0x802EA030`), `ENCiCheckBreakType` (`0x802EA130`) and `ENCiWriteBreakType` (`0x802EA170`), owned by #493. These names are coordinated through the separate #493 dependency PR. After applying that dependency and the boundary alignment below, source-linked configure, build and frozen-manifest verification each exited 0. The rebuilt main.dol retains SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.

## Reusable lesson

The reference's `latin1_unicode_array` contains Windows-1252 mappings in its C1 range and is used for both Latin1 and Win1252 forward conversion. Substituting a standards-only ISO-8859-1 table changes target data and behavior. Preserve the observed map; the full 6,912-byte source table comparison confirms it.

## F-494-2: Alignment of the next fallback unit

The first source-link attempt had 100% code/data objdiff but failed full verification at DOL header offset `0x6F`. `nm -n` showed the next function, originally `0x802ED700`, at `0x802ED6FC`: the source object ends its final 12-byte wrapper without the split original's trailing four bytes, while the next fallback symbol had only inferred four-byte alignment. The later `.ctors`/`.dtors` addresses also shifted.

Setting `align:16` on the owned next-function symbol at `0x802ED700` restores the observed unit boundary. A rebuilt full DOL then passes both dtk SHA-1 and frozen SHA-256/direct-byte comparison. No reference hashes or bytes changed. This does not rename or reconstruct that next function, and no ownership beyond #494 is assumed.
