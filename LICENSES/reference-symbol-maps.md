# Reference symbol map sources

`tools/re/import_reference_symbols.py` reads only symbol names, addresses and sizes from these maps, and source-file paths from their `splits.txt`. It copies no code. The pinned commits are listed in the tool and fetched with its `fetch` command.

| Label | Repository | Commit | Files read | License |
| --- | --- | --- | --- | --- |
| `xenoblade-jp` | [xbret/xenoblade](https://github.com/xbret/xenoblade) | `ea25691b2029394ab777bda7060139e517a13cba` | `config/jp/symbols.txt`, `splits.txt` | CC0-1.0 (root `LICENSE`) |
| `ss-SOUE01` | [zeldaret/ss](https://github.com/zeldaret/ss) | `787b0b1e9d30e3c1d91ced61350b059ffc5a2411` | `config/SOUE01/symbols.txt`, `splits.txt` | CC0-1.0 (root `LICENSE`) |
| `tp-RZDE01_02` | [zeldaret/tp](https://github.com/zeldaret/tp) | `c8fa8c9e2aab72cf4e5db0e5d1c84a9ea6ee6eb0` | `config/RZDE01_02/symbols.txt`, `splits.txt` | CC0-1.0 (root `LICENSE.md`) |
| `kdc-S72E01` | [Swiftshine/kdc](https://github.com/Swiftshine/kdc) | `81b52d6a31b6dd671bea0268f36a874477ee2bf6` | `config/S72E01/symbols.txt`, `splits.txt` | CC0-1.0 (root `LICENSE`) |
| `ogws-RSPE01_01` | [kiwi515/ogws](https://github.com/kiwi515/ogws) | `27fa94a593096eed7442efc4e70981fd52025b9c` | `config/RSPE01_01/symbols.txt`, `splits.txt` | CC0-1.0 (root `LICENSE`) |
| `petari-RMGE01` | [SMGCommunity/Petari](https://github.com/SMGCommunity/Petari) | `e0ab141914921726a595573dc48d3f13d0922efe` | `config/RMGE01/symbols.txt`, `splits.txt` | CC0-1.0 (root `LICENSE`) |
| `wii-symbols-<id>` | [riptl/wii-symbols](https://github.com/riptl/wii-symbols) | `34d03db117a7e0007ca3ef3c8bc7402dad1c2925` | all 20 `symbols/<id>.txt` maps | No license file or statement in the repository (GitHub reports none). Used only for function names, which are factual identifiers; no files are copied. Earlier findings that called these maps CC0 were mistaken. |

All CC0 repositories were checked at the listed commit for a root license file. Imported names are recorded per symbol, with their sources, in [reference-symbols.tsv](../reference-symbols.tsv).
