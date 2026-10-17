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

## F-494-3: Japanese and context conversion units

The remaining five functions in #494 belong to `encjapanese.c` and `encconvert.c`. The user authorized completing these shared files across #493–#495. This supersedes the remaining-work statement in F-494-1 for #494; it does not complete the Unicode, utility, Chinese, or Korean conversion units in the neighboring issues.

Source and header provenance is wii-ipl at the same pinned revision as F-494-1, under its CC0-1.0 LICENSE. The consulted ENC files contain no additional file-level license notice. Source comments and unused public functions are omitted. Japanese character literals are explicit Unicode code points, and all conversion tables remain editable C initializers. No executable bytes are embedded.

| Unit | Sections | Compiler | Retained functions |
| --- | --- | --- | --- |
| `encjapanese.c` | `.text` `0x802EBAD0–0x802ED118`; `.rodata` `0x803718A8–0x8037BB8E`; `.sdata` `0x804B8D60–0x804B8D64` | Wii/1.0, existing `cflags_sdk`, default alignment | 12 |
| `encconvert.c` | `.text` `0x802ED700–0x802EE64C`; `.rodata` `0x8037D690–0x8037E920`; `.data` `0x803DC968–0x803DCA78`; `.sdata` `0x804B8D68–0x804B8D74` | Wii/1.0, existing `cflags_base`, default alignment | 9 |

The Japanese unit contains two public Shift JIS wrappers, four private string converters, and six private helpers. The release omits the public JIS wrappers and direct JIS/Shift JIS conversion entry points. Inlining the retained Kanji conversion helpers requires the existing SDK inline level and file IPA settings. The context unit retains initialization, encoding selection, alternative-character selection, both conversion directions, encoding-name hash lookup, next-character width, and two private dispatchers; the other public reference functions are absent.

Evidence: GNU binutils 2.42-2 disassembly and relocation records, independently located source-table bytes in the frozen DOL, and objdiff-cli 3.8.2 comparisons of each entire object. Relocation-bearing instruction words were initially excluded from diagnostic searches, then checked by objdiff and the full link. External callee names were recovered from the matched callers' branch destinations; their source-unit splits remain with their owning issues.

The Japanese forward table is 18,800 bytes at `0x803718A8`, reverse table 22,257 bytes at `0x80376218`, offset table 512 bytes at `0x8037B910`, and kana table 126 bytes at `0x8037BB10`. Seven pointer-like table words were incorrectly relocated by automatic analysis. The two affected arrays now carry `noreloc`; the interior automatic label in the forward table was removed. The corrected original objects and source objects compare at 100% for every code and data section. The four loaded flags are four-byte BOOLs, with the Japanese definition in its own unit and the other three in the context unit.

### Traps

- Compiling a non-ASCII wide character literal under `-enc SJIS` changed the overline comparison into a two-instruction constant sequence. Replacing the literal with its Unicode value `0x203E` restores the target's single immediate comparison and the exact function size. Explicit code points also preserve the other retained Japanese comparisons.
- Automatic symbol sizes included padding and a spurious interior table boundary. Use source table extents and DOL byte comparisons to establish the real sizes before accepting a data score. The Japanese `.rodata` ends at `0x8037BB8E`; the following two bytes are alignment padding.
- A helper without an emitted function can still be required by an inlined caller. Removing `ENCiConvertKanjiJisToSjis` left an unresolved call and changed `ENCiConvertStringJisToUnicode`; restoring the helper recovered its 100% comparison. The failed link was not treated as a verification result.

### Source-linked validation

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0 after the split-only changes, with source registered NonMatching, and with both units linked Matching. The source-linked report records 12/12 functions and 5,620/5,620 code bytes plus 41,706/41,706 data bytes for Japanese conversion, and 9/9 functions and 3,872/3,872 code bytes plus 5,036/5,036 data bytes for context conversion. Every section compares at 100%. The frozen SHA-256 and direct-byte comparison pass; original inputs and expected hashes are unchanged. Together with the previously matched Latin unit, every function listed in #494 is now reconstructed and linked from source.
