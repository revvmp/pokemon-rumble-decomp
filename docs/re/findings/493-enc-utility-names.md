# F-493-1: ENC utility symbol names

Target `wpse-v1`, frozen executable manifest `config/targets/wpse-v1.json`. This bounded dependency change names three functions within #493 to allow the source-reconstructed Latin unit in #494 to link. All other work in #493 remains fallback; no new source coverage is claimed by this naming change.

| Address | Size | Previous label | Name |
| --- | --- | --- | --- |
| `0x802EA030` | `0xF8` | `fn_802EA030` | `ENCiCheckParameters` |
| `0x802EA130` | `0x34` | `fn_802EA130` | `ENCiCheckBreakType` |
| `0x802EA170` | `0x100` | `fn_802EA170` | `ENCiWriteBreakType` |

Names have high confidence from [wii-ipl encutility.c](https://github.com/koopthekoopa/wii-ipl/blob/f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c/libs/RVL_SDK/src/enc/encutility.c), declarations at the same pinned revision, and the target's call sites and instructions inspected with binutils 2.42-2. The reference is CC0-1.0 with no additional notice on the consulted ENC files. No reference source is copied in this change.

The parameter checker takes eight PowerPC integer arguments: destination validity, input/output destination counts, output destination validity, source validity, input/output source counts, output source-limited flag. The two break helpers accept unsigned current/next characters and destination pointer/character width/break enum/destination validity respectively. These call signatures are exercised by the two Latin generic converters reconstructed in #494, whose object code matches 100%.

All functions are in `.text`; DOL file offsets are runtime address minus `0x8004FD40`. Symbol addresses, sizes and attributes remain unchanged. No split or source registration changes occur. No new reusable tooling trap emerged.

Validation commands: `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1`. Results are recorded in the pull request after execution.

During the final rebase, the incoming `docs/re/SYMBOLS.md` at `origin/main` contained byte `0x96`, causing strict UTF-8 decoding of `git show origin/main:docs/re/SYMBOLS.md` to fail. The conflict resolution preserved incoming bytes and appended only this issue's ASCII section; no global encoding conversion was performed. New files remain UTF-8. This is an observation of that revision, not evidence that all project Markdown uses another encoding.

## F-493-2: Product information ends at 0x802E9D14

The first three functions in #493 are `__SCF1` (`0x802E9A90`, size `0x170`), `SCGetProductArea` (`0x802E9C00`, `0x84`) and `SCGetProductGameRegion` (`0x802E9C90`, `0x84`). Pinned Petari `src/RVL_SDK/sc/scapi_prdinfo.c` at `e0ab141914921726a595573dc48d3f13d0922efe` reproduces their non-relocation instruction streams with Wii/1.0 and the existing SDK flags. The file's first function lies inside #493; no neighboring split ownership is taken.

The unit owns `.text` `0x802E9A90–0x802E9D14`, `.data` `0x803DC840–0x803DC8A0`, and `.sdata` `0x804B8D48–0x804B8D55`. The area table has fourteen five-byte records, including its sentinel; the game-region table has six four-byte records. Source tables and both keyword strings occur byte-for-byte in the original DOL.

## F-493-3: WENC is compiled with GC/3.0a5.2

The four-byte-aligned function at `0x802E9D14` is `WENCGetEncodeData`, not part of SC or ENC. Its eight-double table at `0x80371868` is `0.8984375` four times, then `1.19921875`, `1.59765625`, `2.0`, and `2.3984375`, identifying the reference Wii Remote ADPCM encoder. Pinned Petari `src/RVL_SDK/wenc/wenc.c` and the already-imported `include/revolution/wenc.h` provide the original interface and state names.

Compiling the same source and flags across candidate compilers gives:

| Compiler | Function size | Non-relocation instruction comparison |
| --- | --- | --- |
| GC/3.0a3, GC/3.0a5 | `0x298` | Different |
| GC/3.0a5.2 | `0x2D8` | Identical |
| Wii/1.0a, Wii/1.0 | `0x2D8` | 53 differing words |
| Wii/1.1, Wii/1.3 | `0x2DC` | Different |

All accepted candidates used four-byte function alignment, SDK optimization and file IPA. GC/2.7 rejected the IPA option and is not excluded by this experiment. GC/3.0a5.2 is the matching configured compiler, not a claim to have excluded every historical compiler. The WENC split owns `.text` `0x802E9D14–0x802E9FEC`, `.rodata` `0x80371868–0x803718A8`, and `.sdata2` `0x804BB4F0–0x804BB4F8`.

## F-493-4: Utility and Unicode unit boundaries

`encutility.c` owns `.text` `0x802E9FF0–0x802EA270`, `.data` `0x803DC8A0–0x803DC8E6`, `.sdata` `0x804B8D58–0x804B8D5C`, and `.sbss` `0x804B99D0–0x804B99D4`. Four functions reproduce the existing target streams with Wii/1.0 SDK flags. The reference's 2010 version macro was replaced with the observed target banner, `<< RVL_SDK - ENC` followed by the target's release metadata: February 27, 2009, 10:05:42, compiler code `0x4302_145`. The complete source literal preserves the target's spaces and tab; no frozen fingerprint changes.

`encunicode.c` owns `.text` `0x802EA270–0x802EBAC8` and `.data` `0x803DC8E8–0x803DC968`. It contains eight retained string converters plus two out-of-line helpers. Other helpers inline with the existing SDK inline level and file IPA. Public convenience trampolines, BOM setters and unused UTF-32/UTF-8 public converter bodies are absent from this release and omitted. The 128-byte Base64 table is source data and matches the original.

ENC source provenance is wii-ipl at `f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, as in F-493-1. The consulted Petari and wii-ipl files have no additional file-level notices beyond their root CC0-1.0 licenses. Comments are stripped; original names and editable data initializers are retained. All source reconstruction here is C, with no executable-byte payloads.

Evidence uses dtk 1.8.4, GNU binutils 2.42-2 symbols/relocations/disassembly, candidate CodeWarrior objects, direct source-data searches in the frozen DOL, and whole-object objdiff comparisons. The eleven Japanese functions listed in #493 were already linked Matching by #494 in PR #583; these four units cover the remaining eighteen functions.

## Traps

- Packed function starts inside the SDK region can identify a different compiler: WENC's GC/3.0a5.2 output matches, while Wii/1.0 differs despite the same function size. Keep the compiler override on WENC rather than changing the SDK library globally.
- This revision's `sc.h` consumes `OSThreadQueue` without including its complete declaration through `os.h`. Including the existing `revolution/os/OSThread.h` before `sc.h` compiles the product-info source without inventing or duplicating a shared type. Existing repeated typedef warnings are not treated as build failures.
- The SC code ends at `0x802E9D14`, immediately before the four-byte-aligned WENC entry; extending it to the next sixteen-byte boundary would overlap another unit. Version pointer and registration flag extents also exclude their following alignment padding.

## Source-linked validation

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` each exited 0 for the baseline, split-only and source-linked builds. Four complete `objdiff-cli diff` comparisons exited 0 and show 100% for all code and data sections.

| Unit | Matching functions | Matching code bytes | Matching data bytes |
| --- | --- | --- | --- |
| `scapi_prdinfo.c` | 3/3 | 632/632 | 109/109 |
| `wenc.c` | 1/1 | 728/728 | 72/72 |
| `encutility.c` | 4/4 | 612/612 | 78/78 |
| `encunicode.c` | 10/10 | 6156/6156 | 128/128 |

The frozen SHA-256 and direct-byte comparison pass; original artifacts and expected hashes remain unchanged. The four new units link 18 functions and 8,128 code bytes from source. Together with the previously matched Japanese unit, all 29 functions listed in #493 are reconstructed and linked.
