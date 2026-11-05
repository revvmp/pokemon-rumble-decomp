# Findings

## 2026-11-04 native source-link continuation

[The complete animation-curve unit](findings/animcurve-20261104-continuation.md) now passes native source linkage and frozen verification. All 194 NW4R units are source-selected; only PiiPropData constructor and Reset remain unfinished CPU functions. [DSP command/table identities](findings/ax-dsp-identities-20261104.md) clarify the separate 8,192-byte coprocessor-code dependency without claiming its reconstruction.

[F32](findings/animcurve-20261104-continuation.md#f32-executor-exact-isolated-function-and-integration) now reproduces all 4,044 bytes and 59 relocations. Normal ninja and frozen verification pass, leaving three unfinished CPU functions. [The final scheduling trace](findings/animcurve-f32-final-schedule.md) shows how an eliminated virtual pointer copy changes a final ready-list tie.

[Rotate](findings/animcurve-rotate-20261104.md) now reproduces every instruction byte and all 58 relocations, leaving four unfinished CPU functions. [The system-message archive](findings/system-message-archive-20261104.md) now rebuilds from semantic JSON and PNG inputs, with ordinary source linkage and frozen main.dol verification passing. [The DSP image](findings/ax-dsp-image-20261104.md) remains an explicit coprocessor-code dependency; identifying its AXWii variant does not reconstruct it.

[CDlgPiiSelector](findings/91-selector-tail.md), [CPlayerPii](findings/player-shadow-tail.md) and [NPpuVM](findings/vm-properties-tail-20261104.md) now pass normal complete source linkage. The loader is newly instruction-exact, leaving five real unfinished functions. [Native compiler alignment](findings/native-alignment-padding.md) replaces all standalone code-padding fallbacks without adding instruction credit. [SDK and data evidence](findings/anonymous-data-20261104.md) recovers the unused typed IdleThread and accounts for the remaining standalone data gaps. The frozen executable is unchanged.

[CLayout](findings/20261104-layout-remote-tail.md#complete-source-link-acceptance), [CPiiListFilterSort](findings/cpiilistfiltersort-source-link.md) and [CGameResult](findings/123-cgameresult-unit.md#complete-source-link-acceptance) now pass the normal complete source link. Both remaining reference-comparator sorts are exact after a traced control-block-load lifetime correction. The filter's original RTTI proves its callback enum, and its parameter block proves a 3-by-6 menu grid; function scoring had hidden both errors. Six unfinished functions remain.

[AnimCurveExecuteChild](findings/animcurve-child-20261104.md) now reproduces all 3,468 function bytes and 27 relocations. Read-only tracing proves that integer-to-float conversion scratch slots alternate through a separate compiler selector; an unused conversion can advance it before its instructions disappear. The ordinary build and frozen verification pass, leaving eight unfinished functions. The animation unit remains NonMatching pending its other three functions and source-link acceptance.

[CModel](findings/329-cmodel-emission.md), [CDlgCollection](findings/cdlgcollection-tail.md) and [CMsgRes](findings/20261104-game-small-tail.md) now pass the normal full source link. CModel's native input grouping preserves code, exception and data order independently without editing compiler output. Collection's final exception-range difference requires the proven nonthrowing accessor contract. [CPlayerPii GetShadowName](findings/player-shadow-tail.md) and [NPpuVM CollectGarbage](findings/vm-properties-tail-20261104.md) are newly exact function bodies; their unit-level acceptance remains separate.

The next accepted sources include [ParticleManager and ArchiveFontBase](findings/particle-font-native-tail-20261104.md), [both JPEG output formats](findings/tmc-jpeg-final-tail.md), [CShadowCamera and CSimpleGround](findings/20261104-game-small-tail.md), [CRemoteMemory](findings/20261104-remote-memory-finish.md), and [CPii and CPiiBox](findings/sort-tail-20261104.md). Each passes the complete frozen main executable comparison. Separate Camera/Ground and CPii/Box source files restore independently emitted literals and metadata. CPii's last link difference was a wrong height multiplier hidden by function scoring; the original word proves `1.3333334f`.

[CUIMenu's complete source link](findings/cuimenu-native-sections.md) proves native compiler section grouping and explicit native linker order in the ordinary build. All 61 functions match, and the final frozen executable passes unchanged. The preparation step copies selected objects byte-for-byte to unique names; it does not rewrite instructions, relocations or reference data. [The CUIList cleanup correction](findings/337-cuilist-tail.md#f-337-7-natural-list-item-vector-cleanup) removes an unnecessary D-0016 stand-in using the later CUIMenu unwind consumer as identity evidence.


## F-0001: WAD container metadata

Python 3.14.0 inspection of the user-supplied WAD identified title ID `0001000157505345` (`WPSE`), region 1 (USA), TMD title version 1, seven contents, and boot index 6. Game name is user-supplied; authenticity, signatures, acquisition history, and retail revision naming are unverified.

Target `wpse-v1`; preserved copy `orig/wpse-v1/reference.wad`; original root file unchanged. Size 27,330,304 bytes; SHA-256 `3edf8518c33ea03a04a0c519ae586a29e9b40a32327be72623277773ac88f680`. The real verifier fingerprint CLI exited 0 and froze `config/targets/wpse-v1-package.json` for package provenance only. Independent Get-FileHash checks agreed for both copies. No executable acceptance manifest or reconstruction exists.

Inspection procedure: Python standard-library pathlib, struct, hashlib; decode WAD header as big-endian `IHH6I`, align sections to 64 bytes. Certificate offset/size 64/2560; ticket 2624/676; TMD 3328/736; encrypted data 4096/26916864; footer 26920960/409344. Footer ends at EOF. TMD signature type `0x10001`; title ID at TMD+0x18c; region at +0x19c; version/count/boot as three unsigned big-endian shorts at +0x1dc. Content records are `IHHQ20s` at +0x1e4, stride 36.

| Content ID / index | Type | Declared plaintext size | Declared SHA-1 |
| --- | --- | --- | --- |
| 00000000 / 0 | 0001 | 409344 | 1a1b1b0f20346e4a8355ab91c00e476d28f06577 |
| 00000001 / 1 | 0001 | 2004320 | 807ff739b2bcfeb46338783a80147bf613d14948 |
| 00000002 / 2 | 8001 | 2156800 | 3644384b0cfc920ef620120e8721bf53c85bda27 |
| 00000003 / 3 | 8001 | 370880 | 096b7bc8616329f32ec0376e438ef5438b0c247a |
| 00000004 / 4 | 8001 | 112448 | 061a4f1d60aec55f0eff6937d52320199f9bb788 |
| 00000005 / 5 | 0001 | 21550432 | e7ccc8c9174a611fc994f8c4d10adb3baf06bc32 |
| 00000006 / 6 | 0001 | 312576 | 8b76b2b6f4b8af0f4132b23a28b79385e1ffb503 |

Format evidence: [WAD](https://wiibrew.org/wiki/WAD_files), [TMD](https://wiibrew.org/wiki/Title_metadata). Metadata observations are confirmed; contents have not been decrypted or hash-validated. Boot index identifies startup content, not necessarily the main game implementation. Next: pin an extractor, validate all content hashes, identify executables and sections, and freeze a separate executable manifest.

Use a stable identifier such as `F-0001` for each entry. Record target, manifest, artifact path and digest, address or file offset, direct observation, exact inspection procedure, tool version, interpretation, confidence, alternatives, affected source or configuration, and next validation step.

New findings from issue-based work go in [findings/](findings/README.md), one file each, rather than in this file. Keep facts separate from hypotheses. Mark overturned conclusions superseded and link the replacement rather than silently erasing evidence history. Do not embed proprietary code dumps or assets in this record.

## F-0002: User extraction validated

The user ran the previously supplied dtk WAD extraction command. Inspection found seven .app files plus certificate, ticket, TMD, and trailer under `orig/rumble-unidentified/extracted/`. All seven sizes and Get-FileHash SHA1 results agree with F-0001. The extraction process exit status was not observed. Installed `dtk.exe --version` reports 1.8.4, commit a0c455e46cab58e1d2e0885623f85089ff0499db; acquisition source and tool digest remain unrecorded.

`dtk.exe dol info orig/rumble-unidentified/extracted/00000006.app` exited 0, reporting entry point 0x81330000, .init at 0x81330000 (file 0x500, size 0x658), and .text at 0x81330660 (file 0xB60, size 0x2A8C4). It warned that extab/etabindex could not be located. This confirms a parseable boot DOL, not its role as the main game implementation.

Running the same dol info command on 00000001.app exited 1 with an out-of-range panic at src/util/dol.rs:62. Its first bytes are 11 A0 7B 3E; it must not be treated as a directly loadable DOL without further format investigation. Compression is a hypothesis, not validated here. Do not assume content 1 is an uncompressed main.dol. No reconstructed code or build match was produced.

## F-0003: Main game DOL recovered from compressed content 1

Target `wpse-v1`. Content 1 is NLZSS-compressed: the header starts `11 A0 7B 3E`, declaring 0x3E7BA0 decompressed bytes. The following real commands both exited 0:

```powershell
.\dtk.exe nlzss decompress orig/rumble-unidentified/extracted/00000001.app -o orig/wpse-v1/main.dol
.\dtk.exe dol info orig/wpse-v1/main.dol
```

Always specify `-o`: this tool's help says decompression otherwise overwrites its input. The F-0002 compression hypothesis is now confirmed; its direct-DOL parse failure does not indicate corrupt extraction.

Tool: dtk 1.8.4, commit a0c455e46cab58e1d2e0885623f85089ff0499db, executable SHA-256 `a89f8a1ab07cde5af83218da4247dfd535ad17f2fdb69bf361a966eec7b8fe25`. User supplied the tool; acquisition source remains unverified. Compressed content SHA-256: `fb9057bb9e834d22e3a3eddc817c1cd4590fa0e384944307a87470e644405928`.

Output `orig/wpse-v1/main.dol`: 4,094,880 bytes; SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Entry point `0x80004050`. Python 3.14.0 independently decoded the big-endian DOL header, checked all ten nonempty section file ranges, and confirmed that the entry lies within a text section. Header text slot 0: offset 0x100, address 0x80004000, size 0x26C0; text slot 1: offset 0x27C0, address 0x80052500, size 0x3004C0. dtk reports slightly smaller logical sizes for some sections; retain the raw header sizes when mapping the file. Full raw layout and dtk report are saved locally at `docs/re/local/wpse-v1/main-dol-layout.json` and `main-dol-info.txt`.

Main-game identification is supported by the large executable, normal startup, and embedded game resources and identifiers: `5:/game2d.arc` at file offset 3765380, `CPauseMenu.cpp` at 3815256, `DEBUG_GetFixedPopPokemonMonsno` at 3828592, and `GetPokemonVoiceId` at 3837064. dtk identifies a candidate `main` at 0x8012BCB0 and reports 4,885 exception-table functions; these are tool-derived analysis results, not an independently established function-count denominator or recovered original symbols. The boot content 6 remains a separate executable; its exact transfer-of-control path has not been traced.

Both root WAD and preserved reference SHA-256 remain equal to F-0001. No expected hashes were changed. No runtime execution, source reconstruction, or compiled match is claimed. Use the decompressed DOL for PowerPC big-endian analysis; further archive/module inventory remains outstanding.

## F-0004: Asset container reconnaissance

Read-only `dtk.exe vfs ls` inspection with the F-0003 tool confirmed that content 5 opens as an archive. Its root includes font.arc.cx, game2d.arc.cx, item.arc.cx, misc.arc.cx, pii_1-151_387-492.arc.cx, post_effect.arc.cx, savebanner.arc.cx, script.arc.cx, title2d.arc.cx, hbm.arc.cx and event, stage, effect, sound, USA, en_US directories. `dtk.exe vfs ls 'orig/rumble-unidentified/extracted/00000005.app:sound'` lists waza_p.brsar. Contents 2, 3, and 4 also list successfully as archives; 3 and 4 expose HomeButtonSe and HomeButton3 respectively. No complete recursive inventory was made.

Direct nested listing of `orig/rumble-unidentified/extracted/00000005.app:pii_1-151_387-492.arc.cx:` fails with 'is not an archive'. Its encoding remains unverified: inspect the header and decompress explicitly if appropriate before opening it as an archive. The suffix alone does not establish the codec. No asset extraction or conversion was performed in this reconnaissance. Preserve compressed originals and separate raw extraction from editable exports.

## Imported metadata observation from 40483bd

The following initial F-0001 observation is preserved as historical evidence from the incoming commit. Its statements about unselected target, missing manifests, and pending extraction are superseded by F-0001 through F-0005 above/below. Its recorded filename and interpreter version belong to that earlier environment, not this session.


- **Target:** not yet selected. **Manifest:** none yet.
- **Artifact:** local file named `Pokemon.Rumble.USA.WiiWare.wad`, 27,330,304 bytes, SHA-256 `3edf8518c33ea03a04a0c519ae586a29e9b40a32327be72623277773ac88f680`. Provenance not yet documented.
- **Procedure:** read-only parse of unencrypted structures with an ad hoc standard-library Python 3.14.2 script: the WAD header, ticket title ID at ticket offset `0x1DC`, and TMD fields at offsets `0x184` (IOS), `0x18C` (title ID), `0x198` (group ID), `0x1DC` (title version), `0x1DE` (content count), `0x1E0` (boot index), and 36-byte content records from `0x1E4`. Sections are 64-byte aligned. Nothing was decrypted.
- **Observations:**
  - Header size `0x20`, type `Is`, certificate chain 2,560 bytes, ticket 676 bytes, TMD 736 bytes, data 26,916,864 bytes, footer 409,344 bytes.
  - Ticket and TMD title ID `00010001-57505345`, ASCII `WPSE`. Group ID `0x3031` (ASCII `01`).
  - TMD system version `00000001-00000038` (IOS56). Title version 1. Seven contents. Boot index 6.

| Index | Content ID | Type | Size (bytes) |
| --- | --- | --- | --- |
| 0 | `00000000` | `0x0001` | 409,344 |
| 1 | `00000001` | `0x0001` | 2,004,320 |
| 2 | `00000002` | `0x8001` | 2,156,800 |
| 3 | `00000003` | `0x8001` | 370,880 |
| 4 | `00000004` | `0x8001` | 112,448 |
| 5 | `00000005` | `0x0001` | 21,550,432 |
| 6 | `00000006` | `0x0001` | 312,576 |

- **Interpretation (hypotheses):** the `E` in `WPSE` and the `01` group ID are consistent with a North American Nintendo-published title, matching the filename hint, but the region is not confirmed. The footer size equals content 0's size; whether this is meaningful is unknown. Content 0 is commonly the banner in WiiWare titles, and the boot content commonly a loader that starts a main executable stored elsewhere. Content type `0x8001` has not been interpreted. Content roles require decryption and inspection.
- **Confidence:** high for the raw field values; low for every interpretation.
- **Next validation:** extract contents with a recorded tool (planned: decomp-toolkit `dtk wad`/`dtk vfs`), confirm each content's format, document provenance, and fingerprint a provenance manifest. See the [roadmap](../ROADMAP.md#phase-1-extract-and-identify).

## F-0005: Recursive reference extraction and repeat validation

Target `wpse-v1`; package and executable fingerprints remain as in F-0001/F-0003. New entry point: `python tools/reference/extract_reference.py`. Full procedure, byte-level checks, tool provenance, directory mapping, and limitations are in [EXTRACTION.md](EXTRACTION.md); machine-readable evidence is in [wpse-v1-extraction.json](wpse-v1-extraction.json), with [CSV](wpse-v1-extraction.csv) and [archive index](ARCHIVE_INDEX.md) for browsing.

Successful output `orig/wpse-v1/extraction/`: 3,420 recorded files, 158,750,916 aggregate bytes (including intermediates and duplicates); 156 U8 archives, 26 NLZSS decompressions, 103 LZH8 decompressions, five Huff8 decompressions, two banner wrappers, six IMD5 wrappers, and four LZ77 tags. All 183 dtk calls exited 0. A fresh second run under `build/wpse-v1/extraction-repeat/` exited 0 and all 3,420 outputs compared byte-for-byte, including identical archive-entry and parent/ancestry records. Local repeat report: `docs/re/local/wpse-v1/extraction-repeat.json`. A repeat invocation against the existing default output exited 1 before mutation, as intended.

Python 3.14.0 drove the real CLI workflow. dtk version, binary digest, and the main DOL are recorded in F-0003. Supporting decoder source was acquired from https://github.com/Plombo/vcromclaim at commit `6e537af6d314c3d8a6c7353645c137572b37b4b6`; `lzh8.py` SHA-256 `ec324292f1b167027b0c4fc70757ab58435ebf3af22051bd17aebaff5a14ff3a`, `huf8.py` SHA-256 `512d4207e0cfb73816728e7d22c8d089af8e342111ece04c254b37a770db2c30`. The extractor enforces tool/source digests before use and adapts the Python 2 decoder interfaces in memory; see extraction documentation. No third-party commented source was vendored into tracked project code.

Confirmed refinements to F-0004: the game's .cx files begin with 0x40 and decode as LZH8, not the NLZSS forms supported by dtk. `dtk nlzss decompress` rejected game2d.arc.cx with Invalid magic number (exit 1). Decoding yields structurally valid U8 archives or native REFF/REFT effect resources. Content 0 and the trailer both contain a valid U8 archive at file offset 0x640, following IMET at 0x80. IMD5 payload lengths and MD5 values validated before unwrapping. All wrapper bytes remain preserved.

A Home Menu sound U8 archive has file-node names containing backslashes (for example wt\HomeButtonSe.wt and midi\00_home_button.mid). The initial validator rejected these; a direct dtk probe failed to create the first file on Windows (exit 1). Normalize relative separators for output, reject absolute/parent traversal, precreate validated parent directories, then use dtk and compare every output to its source slice. Original node names and offsets remain in the JSON. The failed first extraction was moved to ignored `build/wpse-v1/extraction-incomplete/`.

Validation: seven content sizes and SHA-1 values matched the TMD; every U8 file matched its original slice and entry count; all decoded lengths matched their headers; all six IMD5 hashes matched; all recorded file SHA-256 values were independently reread. The 1,581 recognizable native BRRES/RSAR/RLAN/RLYT/RFNT/REFF/REFT headers had exact declared file lengths. Main DOL matches the prior extraction, and `orig/wpse-v1/boot.dol` is a byte-identical convenience copy of content 6. Both original WAD hashes and every prior top-level extracted file remained unchanged.

Native resource internals are not converted or exhaustively parsed. No named .rel, .rso, or .elf files were found; this is not proof of complete code coverage. Game-specific BSP/PKC meanings and the boot/main control flow remain unresolved. No reconstruction, compiler match, or exact game build was produced.

## F-0006: dtk binary provenance resolved

The user-supplied dtk recorded in F-0003 (1.8.4, commit `a0c455e46cab58e1d2e0885623f85089ff0499db`, SHA-256 `a89f8a1ab07cde5af83218da4247dfd535ad17f2fdb69bf361a966eec7b8fe25`) is byte-identical to the official release asset `dtk-windows-x86.exe` of [decomp-toolkit v1.8.4](https://github.com/encounter/decomp-toolkit/releases/tag/v1.8.4). Procedure: `gh release view v1.8.4 -R encounter/decomp-toolkit --json assets` lists that asset's digest as `sha256:a89f8a1a…fe25`; the asset was downloaded with curl into `tools/local/dtk.exe` and `sha256sum` reproduced the digest. The x86_64 asset differs (`6a693e95aa4d30acccc673343302a4abdfa3146bd0fb15cb1a2bce924cbc782a`) and is not used. Confidence: high.

## F-0008: CodeWarrior `.comment` version measured

A one-line C file compiled with `mwcceppc.exe -nodefaults -proc gekko` from the compiler bundle `compilers_20251118.zip` (Wii/1.0, Wii/1.1, Wii/1.3, and GC/3.0a5.2) produced a `.comment` section whose magic is `CodeWarrior`, version byte 15 at offset `0xB`, and compiler bytes `04000001`, read with a standard-library Python ELF parser. `config/wpse-v1/config.yml` therefore sets `mw_comment_version: 15`. The final executable carries no `.comment` section, so this records the tools' behavior, not the original build's.

## F-0009: Boot content is a NAND boot program

`orig/wpse-v1/boot.dol` (content 6) contains `--- NAND BOOT PROGRAM %s (%s:` at file `0x2C3E1` and `nandboot.c` at `0x2C5C8`; ESP content-file failure messages (`ESP_InitLib`, `ESP_GetTitleId`, `ESP_OpenContentFile`, `ESP_ReadContentFile`, `ESP_SeekContentFile`, `ESP_CloseContentFile`) at `0x2C400` to `0x2C588`; `ERROR >>> Program file must be less than 0x%08x bytes.` at `0x2C4DA`; and `CXReadUncompLZ failed: %d` at `0x2C56C`. It references `/shared2/sys/NANDBOOTINFO`, `/shared2/sys/SYSCONF`, and system menu data paths, carries eight RVL_SDK banners dated Jun 22 2009 with tag `0x4302_145`, and contains no `pokemon`, `rumble`, or `WPSE` strings. Its T0 stub programs HID0, MSR, BATs, and segment registers, writes 0 to `0x800000F4`, loads the word at `0x800037FC` (`0x81330000`), and branches there. Its image (`0x81330000` to `0x8137BB00`) does not overlap the main image (`0x80004000` to `0x804BBC28`).

Procedure: `powerpc-eabi-strings.exe -a -t x -n 5` and `powerpc-eabi-objdump.exe -D -b binary -m powerpc:750 -EB --adjust-vma=0x80003400` from binutils 2.42-2, `dtk.exe dol info` 1.8.4, and Python 3.14 header parsing, run by a delegated analysis pass and reviewed here. Interpretation (hypothesis, medium confidence): this is a Nintendo WiiWare NAND loader that reads content 1 through ESP, decompresses it with `CXReadUncompLZ` (consistent with content 1's `0x11` header), and loads the main DOL into low memory. The exact content index it opens is untraced, and no other title's loader was compared. The boot executable is excluded from the first acceptance manifest; [inventory](INVENTORY.md#boot-executable-sections) has its sections.

## F-0010: Split build reproduces main.dol

`python configure.py` followed by `ninja` (dtk 1.8.4, objdiff-cli 3.8.2, Wii/1.1 `mwldeppc.exe`, ninja 1.13.0, Python 3.14.2) analyzes `orig/wpse-v1/main.dol` into 8,782 units and links `build/wpse-v1/main.dol`. With only dtk's automatic analysis, the first link differed: `.dtors` started at `0x80352A98` instead of `0x80352AA0`, shifting later sections, and the linker warned that `global_destructor_chain.c` and `__init_cpp_exceptions.cpp` needed updating. Adding the Wii-linker exception runtime split in `config/wpse-v1/splits.txt` (`.text` `0x80340B24`–`0x80340B94`, `.ctors$10`, `.dtors$10`, `.dtors$15`, `.sdata` `0x804B8F98`–`0x804B8FA0`), whose layout matches Petari's Wii runtime split, made the output identical. `config/targets/wpse-v1.json` was then fingerprinted from `orig/wpse-v1/main.dol` (SHA-256 `ad1a5989…53da`, equal to F-0003) and `tools/verify/verify.py verify` passed.

`Runtime/__init_cpp_exceptions.cpp` was then reconstructed from source. Three issues had to be solved before the linked build matched again, each confirmed with the linker map from `configure.py --map`:

1. Wii compilers align functions to 16 bytes by default. The original places `__fini_cpp_exceptions` at offset `0x3C`, so the runtime library needs `-func_align 4`. objdiff still scored the misaligned unit 100%, because fuzzy matching ignores padding.
2. MWCC rejects `__declspec(section ".ctors$10")` with error 33048 until the section is declared with `#pragma section ".ctors$10"`.
3. The Wii/1.1 linker dead-strips the unreferenced `.ctors$10` and `.dtors$15` entries. dtk-split objects avoid this through `.comment` export flags. `#pragma force_active on` around the table entries reproduces it. Adding `force_active` to `symbols.txt` did not survive, because dtk rewrote the file.

With these, the unit is `Matching` and two consecutive clean builds (build directory, `build.ninja`, and ninja state deleted first) both produced `main.dol` SHA-256 `ad1a5989…53da`, `main.elf` `e235b1f1…0fe2`, and `report.json` `47bae99e…672b`, and both passed verification.

This unit's two functions compile identically under Wii/1.0, Wii/1.0a, Wii/1.1, Wii/1.3, and GC/3.0a5.2, so it does not identify the runtime's original compiler; Wii/1.1 is used for consistency with the linker. Wii/1.1 as the linker is consistent with the evidence but not proven original: with split objects, other Wii linkers were not tried. Source provenance: follows Petari's CC0 `src/Runtime/__init_cpp_exceptions.cpp`; the headers in `include/Runtime/` were written for this project and declare only what this unit uses.

## F-0007: Per-library compiler identification

Build banners in `orig/wpse-v1/main.dol` carry CodeWarrior build codes that correspond to entries in the compiler bundle's `info.txt`: RVL_SDK OS, GX, DVD, AX, PAD, WPAD, KPAD and others `0x4302_145` (Wii/1.0, Ver 4.3 build 145); NW4R EF, G3D, SND, LYT `0x4302_151`, final build Sep 24 2009 (Wii/1.1, build 151); HBM `0x4201_142` (Wii/1.0a); NCD `0x4199_60831` (GC/3.0a5.2). Banner offsets are listed in the [inventory](INVENTORY.md). The banners are hypotheses about the compilers; the experiment below tests them.

Procedure, run by a delegated analysis pass and spot-checked here: 216 CC0 Petari source files (commit `e0ab141`; nw4r ut/lyt/math/db, RVL_SDK libraries, MSL_C, Runtime) were compiled with GC/3.0a3, GC/3.0a5, GC/3.0a5.2, and Wii/1.0RC1, 1.0a, 1.0, 1.1, 1.3, and 1.7, using Petari's per-library flags, and again with `-func_align 4`. Each function's code, with every relocation-bearing instruction word masked, was searched for at 4-byte-aligned offsets in the DOL's `.init` and `.text`. A hit required at least eight unmasked words and one to four locations. Tools: dtk 1.8.4, GNU objdump 2.42, Python 3.14.2. Scripts and full listings are kept in ignored `docs/re/local/wpse-v1/phase2/`.

| Address region (chosen by address) | Matched functions | Result |
| --- | --- | --- |
| RVL_SDK `0x802B1800`–`0x8033F900` | 679 | Only Wii/1.0 at default alignment reproduces all 679. Wii/1.0RC1 and 1.0a miss 12, Wii/1.1 and 1.3 miss 17, Wii/1.7 misses 19, GC compilers miss at least 303. |
| MSL and Runtime `0x8033F900` onward | 98 | Wii/1.1 and Wii/1.3 with `-func_align 4` reproduce all 98. Builds up to Wii/1.0 and Wii/1.7 miss 2, every compiler without `-func_align 4` misses at least 20, GC misses 47. |
| NW4R `0x80292000`–`0x802B1800` | 129 | None reproduces all. Wii/1.0 and 1.1 reproduce 126, other Wii builds 125, GC 60. |
| Second lyt/ut copy `0x80186000`–`0x80195000` | 57 | None reproduces all. Wii/1.0RC1 and 1.0a reproduce 55, later Wii builds 51, GC 32. |

Discriminating functions, verified directly here for the first entry of each list:
- `__CanPushCmdQueue` at `0x802FE530` is reproduced by Wii/1.0. Wii/1.1 inserts a redundant sign extension, offsetting every later word.
- `__num2dec` at `0x80344180` is reproduced by Wii/1.1 with `-func_align 4`. Wii/1.0 is one word shorter.

Further discriminators:
- SDK functions that exclude Wii/1.0RC1 and 1.0a: `OSFatal` `0x802B7120`, `GXSetTevColor` `0x802CFD70`, `NANDCreate` `0x802E4630`.
- SDK functions that exclude Wii/1.1 and later: `UnlockSram` `0x802BB170`, `GXSetIndTexMtx` `0x802CF7A0`, `WPADControlLed` `0x802FA770`.
- MSL functions that exclude Wii/1.7: `__ieee754_log` `0x8034A104`, `__kernel_rem_pio2` `0x8034B174`.

Function alignment, from start addresses: 16-byte for matched SDK, NW4R, and lyt/ut code; packed 4-byte for MSL and Runtime. `-func_align 4` changes code as well as padding, because Wii compilers insert loop-alignment `nop`s relative to their default layout.

Conclusions:
- **RVL_SDK:** Wii/1.0 at default alignment, high confidence. This agrees with its banner.
- **MSL_C and Runtime:** Wii/1.1 or Wii/1.3 with `-func_align 4`, high confidence. 1.1 versus 1.3 is unresolved; GC compilers are excluded.
- **NW4R:** consistent with Wii/1.0 or 1.1, medium confidence. Petari's 2007 source drifts from the 2009 code. The banner suggests 1.1 but the experiment does not prove it.
- **Second lyt/ut copy:** consistent with Wii/1.0RC1 or 1.0a. Hypothesis: it is the Home Button Menu's own copy (`nw4hbm::lyt` strings, HBM `0x4201_142` banner); the addresses were not traced.
- **NCD:** the GC/3.0a5.2 banner was not tested, because no reference source is available.

Caveats: Petari's library sources predate this game's, so a miss does not by itself exclude a compiler, and identical output across compilers is not evidence. Inline and IPA flags were not varied.

## F-0011: objdiff function count includes alignment padding

objdiff-cli 3.8.2 reports `total_functions` 17,034 for `wpse-v1`. Of these, 3,569 are dtk `pad_03_*` symbols. A Python check of `build/wpse-v1/report.json` against `orig/wpse-v1/main.dol` found that every one is zero-filled, none exceeds 12 bytes, and together they total 28,484 bytes. Real functions: 13,454 in `.text` and 11 in `.init`, 13,465 in total, matching the `type:function` entries in `config/wpse-v1/symbols.txt`.

IDA Pro 9.2, run headless with `idat.exe -A -S` on a copy of `docs/re/main.elf.i64`, identifies 13,211 functions. 327 dtk functions are absent from IDA and 73 IDA functions are absent from dtk; the difference is boundary disagreement, not missing code. The tracked database's SHA-256 was unchanged afterwards (`3517119d…ddaa`). dtk's list remains authoritative because the build uses it. Report function progress against 13,465, or exclude `pad_` entries from objdiff counts. Code-byte totals include the padding bytes, about 0.9% of code.

## F-0012: Full executable layout survey

Four parallel surveys covered `.text` 0x80052500–0x80292000. They used relocations from the dtk split objects (`build/wpse-v1/obj`), CodeWarrior RTTI class names, vtables, `.ctors` static initializers, and `shared_ptr` deleters, which each end a source file. Embedded build banners were also used. This corrects F-0003 and the region assumptions behind the first issue set:

- The game region is not all game code. It contains these libraries:

  | Range | Library |
  | --- | --- |
  | 0x801770F4–0x801778F0 | RevoEX SHA-1 and NCD |
  | 0x801778F0–0x80178530 | RVL_SDK CNT |
  | 0x80178530–0x8019DC50 | HBM: core, private `nw4hbm::lyt` and `nw4hbm::ut`, and an HBM sound block |
  | 0x801E8DE0–0x801EE470 | MSL printf |
  | 0x801F6060–0x801FB030 | TMC JPEG |
  | 0x801FB030–0x80225820 | FA / PrFILE2 |
  | 0x80225820–0x8024AE80 | NW4R ef |
  | 0x8024AE80–0x80271E10 | NW4R g3d |
  | 0x80271E10–0x80292D00 | NW4R snd |

  NW4R ut/lyt/math/db then runs from about 0x80292D00 to 0x802B1800.
- The printf at 0x801E8DE0 is the only `printf` family in the DOL, not a copy (`__pformatter` 0x801EA6C0, `vsnprintf` 0x801EB700, `sprintf` 0x801EB910, a wide formatter at 0x801ED280). Unlike the packed MSL at 0x8033F900, its functions are 16-byte aligned.
- Game code is linked in groups, each roughly in ASCII order of file name: B- and C-prefixed classes from 0x80052500 to 0x80131780, then `main.cpp` at about 0x8012B5F0–0x80131780. `CModel` and the G3D extension classes break the order, so files are probably grouped by directory.
- Script binding name tables at 0x80352EC4–0x80354E14 pair 669 bound game functions with their original names. An in-house script VM (`NPpu`) at 0x8016FC10–0x801770F4 runs them.
- `nw4r::ut::CntFileStream` at about 0x801AF870–0x801AFC7C is linked inside the game area with its own static initializer.

The partition into decomp issues is kept with the issue set: 53 regions and 507 issues of 6–77 functions, mean 26.6. Every function in `config/wpse-v1/symbols.txt` except the two already-matched `__init_cpp_exceptions` functions is listed in exactly one issue. Region boundaries and class start addresses are survey estimates; establishing exact unit boundaries is part of each issue. [Workstreams](../WORKSTREAMS.md) indexes the issues.

## F-0013: Reference symbol maps name library functions by size alignment

`tools/re/import_reference_symbols.py` names placeholder library symbols from seven public symbol maps ([sources and licenses](licenses/reference-symbol-maps.md)): xenoblade `jp`, Skyward Sword `SOUE01`, Twilight Princess Wii `RZDE01_02`, Kirby's Return to Dream Land `S72E01`, Wii Sports `RSPE01_01`, Super Mario Galaxy `RMGE01`, and the 20 retail debug maps of riptl/wii-symbols. Method:

- For each library range of F-0012 (RVL_SDK including BTE and NdevExi2A, MSL/Runtime/TRK, NW4R ef, g3d, snd and ut/lyt/math/db, HBM with `nw4hbm`, RevoEX/NCD, TMC JPEG, FA), our functions in address order are aligned with each reference's functions of the same library. dtk references are filtered by source path from their `splits.txt`; the path-less retail maps by name (for example, `nw4r` in the mangled name for NW4R, unmangled C names for the SDK).
- A seed is a maximal run of at least three consecutive exact size matches, contiguous in the reference and inside one reference source file, worth at least 30 bits. Each distinct size in the run counts once, at `-log2` of its frequency among our functions in that range, so runs of repeated small sizes do not qualify. A run is rejected when another run on a nearby diagonal, within 64 entries and with at least half its score, overlaps it on either side; this removes off-by-one shifts through repeated accessor patterns.
- Inside each reference file, gaps before, between and after seeds are filled from runs of at least three matches that are unique in both windows and contain two distinct sizes. A unique size is accepted only in a gap closed by seeds of the same file on both sides, at the same offset from either end.
- Votes from all references are combined. A symbol is renamed only if its current name is a `fn_`, `lbl_` or `dtor_` placeholder, every reference gives the same base name, and the name is not already used. Reference placeholders, `@` and `$` names are ignored. When references give the same base name with different mangled signatures, the name with the most independent games wins, then the reference order in the tool (18 functions).

Confidence: **high** is a seed supported by at least two games (the four Mario Kart Wii regional maps count as one game); **medium** is a single-game seed, a closed-gap unique size, or a gap run supported by two games; **low** is a single-game gap run, which is reported but not applied. Measured against names already present on `main` at `4e5d89da` (contributor names from matched source and evidence), the same procedure gives:

| Tier | Agree | Disagree | Agreement |
| --- | --- | --- | --- |
| high | 2,232 | 8 | 99.6% |
| medium | 384 | 9 | 97.7% |
| low (not applied) | 45 | 6 | 88% |

References disagreed on 31 more already-named functions. Disagreements are revision differences, such as `__OSCancelAlarmsForUserData` where the 2010–2011 maps have `__OSCancelInternalAlarms`, `WBCRead` against `WBCReadDummy`, and `NANDTell` against `NANDGetLength`, or shifts between equal-sized neighbors that a later revision reordered (`ResFile::HasResAnm*`, `NandFileStream::GetSize`). Data objects were aligned the same way within each data section; 41 already-named objects agreed and none disagreed.

Applied at `539bb56e`: 1,257 renames, 963 high and 294 medium. Functions: 119 SDK, 85 MSL/Runtime/TRK, 68 HBM and `nw4hbm`, 82 NW4R ef, 328 g3d, 362 snd, 189 ut/lyt/math/db, and 7 FA (`VFiPFPATH_*` from the retail maps). Data: 12 MSL, 4 HBM, 1 SDK. The references contributing most were Skyward Sword, xenoblade, Kirby and the Wii Sports Resort, Wii Party and Mario Party 9 retail maps. `python configure.py`, `ninja` and `tools/verify/verify.py verify` passed, and dtk's rewrite of `symbols.txt` changed only the renamed names.

Limitations:
- Mangled names carry the reference revision's signature. The 2009 target can differ: `SetPanMode` takes `snd::detail::PanMode` here but `snd::PanMode` in Skyward Sword. A contributor whose source mangles differently renames the symbol to the source's name.
- Names of functions the target revision replaced can be wrong even inside a long run when both have the same size. Rename on contrary evidence from code; the per-symbol record in [reference-symbols.tsv](reference-symbols.tsv) gives tier, method, run length and sources.
- FA, TMC JPEG, CNT, RevoEX and the 16-byte-aligned printf have few or no public maps.

The same alignment gives each reference source file's first and last mapped function on our addresses. [reference-files.tsv](reference-files.tsv) lists 226 consolidated spans by file name, with the references, the number of mapped functions, whether the file's own first and last functions were mapped, and overlapping spans. A span is evidence for a unit boundary, not a split: verify it against `.ctors`, alignment padding and data references before splitting.
