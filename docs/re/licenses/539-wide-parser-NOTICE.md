# Wide floating-point scanner provenance for issue 539

`src/MSL_C/wstrtold.c` adapts the independently available scanner in [Petari, commit e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MSL_C/strtold.c). The repository's root license is CC0-1.0; the consulted scanner has no additional file-level copyright or license notice.

The initial algorithm comparison also consulted [InusualZ/mhtri-decomp, commit ac489be692a1273afff8574b2402aecba2ae2b8f](https://github.com/InusualZ/mhtri-decomp/blob/ac489be692a1273afff8574b2402aecba2ae2b8f/src/MSL_C/strtold.c), whose root license is CC0-1.0 and whose consulted scanner has no additional file-level notice. Its C wrapper types, metadata comments and unmatched code-generation context were not imported. Its same-sized narrow parser is not claimed to match this target.

Wide/narrow helper selection, radix type, exponent-state changes, literal extent, ordinary float infinity handling and the wrapper were reconstructed against the immutable target instructions. The new implementation uses the repository's canonical `decimal`, locale, `wchar_t`, `wint_t` and wide-reader interfaces. Source names that are descriptive or inferred are identified in [F-539-3](../findings/539-wide-parser.md).

The earlier math and wide-character sources audited with this issue retain their separate [existing notices](539-msl-math-NOTICE.md).
