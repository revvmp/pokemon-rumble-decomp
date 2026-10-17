# F-345-1: wide printf source reconstruction

## Scope and provenance

Issue #345 surveys 29 functions in `0x801E8DE0`–`0x801EE468`. This change reconstructs the independent `MSL_C/wprintf.c` unit: 13 functions, 10,788 function bytes and 84 bytes of inter-function alignment. The narrow formatter remains original-object fallback. Its first eight-byte custom-format callback is at `0x801E8DD0`, inside open neighboring issue #344; permission to own that file's split is pending. No narrow source, split, or object registration is included here, and #345 remains unfinished.

The starting reference is [Petari `src/MSL_C/wprintf.c` at e0ab141914921726a595573dc48d3f13d0922efe](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/MSL_C/wprintf.c), under CC0. Its file contains no separate license notice. `struct lconv` follows that reference's locale header. The reference lacks this target's positional arguments and custom-format extensions; those are reconstructed from target instructions and references. Decompiler views of an external database copy were reading aids only. No decompiler text or original executable bytes are used as source.

## Compiler and boundaries

Wii/1.1 with `cflags_sdk`, explicit `-fp_contract off`, C language mode and default 16-byte function alignment reproduces the complete unit. This is a separate `MSL_C.Printf` library configuration, not a change to the runtime-wide compiler policy. Wii/1.3 also reproduced the investigated instruction streams; Wii/1.0, GC/3.0a3, `-O3` and size optimization did not. The selected configuration is established by the full linked artifact below.

| Section | Half-open range | Bytes |
| --- | --- | --- |
| `extab` | `0x80043478`–`0x800434B8` | 64 |
| `extabindex` | `0x800521DC`–`0x8005223C` | 96 |
| `.text` | `0x801EB9F0`–`0x801EE468` | 10,872 |
| `.data` | `0x803C3198`–`0x803C35D0` | 1,080 |
| `.sdata` | `0x804B8670`–`0x804B86A0` | 48 |
| `.sdata2` | `0x804BA5B0`–`0x804BA5B8` | 8 |

The source's last one-byte empty narrow string is at `0x804B869C`; the split includes the required alignment through `0x804B86A0`. The source object holds 45 payload bytes here, with the remaining three bytes supplied by link alignment. The full DOL comparison proves the result.

## Functions and names

| Address | Function | Bytes | Naming evidence |
| --- | --- | --- | --- |
| `0x801EB9F0` | `__wDefaultFormat` | 8 | Descriptive, returns zero; custom callback initializer |
| `0x801EBA00` | `parse_format` | 1,812 | Reference and target behavior |
| `0x801EC120` | `long2str` | 588 | Reference |
| `0x801EC370` | `longlong2str` | 708 | Reference |
| `0x801EC640` | `double2hex` | 928 | Reference |
| `0x801EC9E0` | `round_decimal` | 300 | Reference |
| `0x801ECB10` | `float2str` | 1,892 | Reference |
| `0x801ED280` | `__wpformatter` | 4,048 | Reference and callers |
| `0x801EE250` | `__wStringWrite` | 108 | Reference |
| `0x801EE2C0` | `__wCountWrite` | 8 | Descriptive, returns one |
| `0x801EE2D0` | `swprintf` | 228 | Reference |
| `0x801EE3C0` | `vswprintf` | 136 | Reference |
| `0x801EE450` | `__wCountPrintf` | 24 | Descriptive count-only formatter wrapper |

Private helpers are local symbols. `__wprintf_custom_format` at `0x804B8670` is a four-byte function pointer initialized to `__wDefaultFormat`; its name is descriptive, medium confidence. Its five parameters are opening delimiter, closing delimiter, unsigned word argument, output buffer, and capacity. Parentheses and angle brackets consume no argument; square and curly brackets consume an unsigned word. The separate count writer returns one and is not the custom callback.

The private `print_format` is 0x28 bytes: flags at 0–4, wide conversion at 6, width at 8, precision at 0xC, argument position at 0x10, eight-byte argument union at 0x18, and captured delimiter pointers at 0x20/0x24. Nine positional descriptors are initialized and populated before formatting. Both `%S` capitalization and the custom delimiter cases are retained. Initial positional-index recognition uses the narrow digit table, while later width recognition uses the wide table; these are distinct target accesses.

The additive `lconv` declaration is 0x38 bytes and preserves the reference field order. Only `decimal_point` at offset zero is used here. Existing locale declarations are unchanged. The fallback object at `0x803E6078` is named `__lconv` without changing its size or split. Outside calls named for linking are `towupper` (`0x801EF670`), `toupper` (`0x801EF680`), `mbtowc` (`0x803453CC`), `mbstowcs` (`0x8034544C`) and `memchr` (`0x80345690`); their ownership is unchanged.

## Validation

The split-only build passed before source registration. The whole source unit was then compiled as `NonMatching`, compared, and linked as `Matching` only after all sections matched. All 13 functions and every compared allocated section score 100% in objdiff, including the data jump tables and exception metadata. Canonical relocation inspection was used to resolve the case-order trap below.

Commands run from the issue worktree:

```text
python configure.py                                             exit 0
.venv/Scripts/ninja all_source                                  exit 0
tools/local/objdiff-cli.exe diff -p . -u main/MSL_C/wprintf -o <scratch>/345-wprintf-diff.json
                                                               exit 0, 13/13 and all allocated sections 100%
.venv/Scripts/ninja                                             exit 0, new source-linked main.dol produced
python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1
                                                               exit 0, PASS main.dol
```

The rebuilt `main.dol` SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`, equal to the frozen reference. This covers the main executable only, not boot content, data contents or WAD packaging. The linked `wprintf` unit has no original-object fallback. A before/after comparison of all 324 pre-existing source objects found identical code, data and relocation records after the additive locale declaration.

## Traps

- Identical instructions can still conceal incorrect switch bindings. The positional prepass initially reproduced every instruction but its `[`, `{` and `c` jump-table relocations selected the wrong otherwise identical-sized blocks. Ordering the unsigned custom cases, pointer case, then character case reproduced all three target addends. Check data relocations as well as instructions before accepting the source.
- A split ending at the last one-byte string (`0x804B869D`) makes the following automatic split misaligned. The evidenced boundary is `0x804B86A0`; preserve the three alignment bytes and verify the full link.
- The reference formatter has a smaller format descriptor and no positional/custom extensions. Importing it unchanged is insufficient; the proven 0x28-byte descriptor and prepass are required. In particular, the wide prepass uses narrow digit classification for the initial positional index.
