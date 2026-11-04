# MSL C-locale character tables

The narrow classification, lowercase and uppercase tables occupy 0x80398488 through 0x80398888. Their existing symbols are __ctype_mapC, __lower_mapC and __upper_mapC. The corresponding wide tables occupy 0x803989B0 through 0x80398FB0. Their descriptive recovered names are gWideCTypeMapC, gWideLowerMapC and gWideUpperMapC; these are not claimed original symbol spellings.

The classification arrays are 256 unsigned-short entries, using the masks documented by include/wctype.h and consumed through _loc_ctype_cmpt. The narrow case arrays contain 256 unsigned bytes each; the wide case arrays contain 256 two-byte wchar_t values each. Exact comparison proves all four conversion arrays are the ASCII case conversion with every other code point unchanged. Narrow and wide classification arrays are byte-identical; entries above127 have no classification flags. These are typed lookup data, not instructions or embedded executable bytes.

Two ordinary C translation units define the six arrays. The recovered ranges total2560bytes, have no relocations, and contain no functions. No runtime read of original data is used to construct the arrays. Values and dimensions were checked against the frozen original. After adding the exact splits and descriptive wide-table symbols, the normal configure/ninja pipeline exits0 and the frozen main.dol verifier reportsPASS. No original hash was changed, no fake function was needed, and no unit tests were added.

The subsequent [locale reconstruction](msl-locale-data.md) confirms conventional reference names for the wide arrays. They are now __wctype_mapC, __wlower_mapC and __wupper_mapC; the descriptive names above record the initial inference. All three names resolve to the same verified target addresses and contents.
