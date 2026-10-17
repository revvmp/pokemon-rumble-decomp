# F-365-1: complete PFAPI utility source

## Scope and provenance

Issue #365 lists 26 functions and 2,088 code bytes. The seven utility
functions at `0x8021A2B0` through `0x8021A464` form one complete retained
unit, with 404 function bytes and 32 bytes of inter-function alignment.
The preceding three survey entries are a codepage-unit tail. No preceding
codepage split or source is taken by this change. Later standard API
wrappers also remain unfinished; this contribution does not complete #365.

The unit follows CC0 Petari `e0ab141914921726a595573dc48d3f13d0922efe`,
`src/RVL_SDK/vf/pf_api_util.c` and its associated header. FA's established
prefix convention removes VF's `VFi` prefix; see
[F-350-2](350-fa-common.md#f-350-2-unit-boundaries-names-and-remaining-work).
The CC0 Wii Menu reference `koopthekoopa/wii-ipl` at
`f4c09f4001c5a0b9af734ad1e3f80a54c5e5723c`, `config/43U/symbols.txt`
and `splits.txt`, independently names every retained FA utility function
and establishes the following one-function standard API files. Its
PFAPI_convertDriverError and PFAPI_convertReturnValue4feof match the two
extra native functions by order, exact size, and behavior.

The reference's first function, table lookup, return-value conversion,
pointer conversion, and unmount conversion occur in the same retained
order. FA retains two additional conversions between them.

## Native functions and storage

| Address | Size | Name | Evidence |
| --- | --- | --- | --- |
| `0x8021A2B0` | `0xB4` | PFAPI_ParseOpenModeString | Reference name; parses r/w/a, optional b and +. |
| `0x8021A370` | `0x50` | PFAPI_convertError | Reference name; maps positive error codes through the table and maps 4096 to 5. |
| `0x8021A3C0` | `0x24` | PFAPI_convertDriverError | Original FA map name; maps positive errors below 160 to 1 and preserves other values. |
| `0x8021A3F0` | `0x10` | PFAPI_convertReturnValue | Reference name; returns 0 for success and -1 otherwise. |
| `0x8021A400` | `0x20` | PFAPI_convertReturnValue4feof | Original FA map name; preserves 1, otherwise returns 0 or -1. |
| `0x8021A420` | `0x18` | PFAPI_convertReturnValue2NULL | Reference name; returns the pointer on success and NULL otherwise. |
| `0x8021A440` | `0x24` | PFAPI_convertReturnValue4unmount | Reference name; preserves 0 and 1, maps other values to -1. |

The `.text` split is `0x8021A2B0–0x8021A464`, ending at the last retained
function's actual end. The next API entry starts at `0x8021A470`; its
alignment padding stays outside this split. The `.data` split is exactly
`0x803C4278–0x803C4318`, the 40 signed integers of
`pf_error_to_api_error`. The native table differs from the pinned VF table
at index 11: FA stores 201, whereas VF stores 13. All 160 bytes are
reconstructed as an ordinary integer initializer and compared exactly.
The next jump table at `0x803C4318` belongs to other code and is excluded.

No additional sections, global state, class layout, compiler override, or
PF_VOLUME change is introduced. Wii/1.0 and existing `cflags_sdk` reproduce
the complete object.

## Validation

The fresh baseline on main `7ac17ab362b42c370bc5fcd4d76b94c7feaa027a`
passed `python configure.py --map`, plain Ninja (513 steps), and explicit
frozen-manifest verification. The split-only baseline then passed plain
Ninja and frozen verification before source registration. Registration as
NonMatching passed the same pipeline, with `.text`, `.data`, and all seven
functions at 100% in the actual objdiff report.

With the source selected as Matching, configure, plain Ninja, frozen
verification, and an independent whole-object audit all exit 0. The audit
pairs both table-address relocations, compares all 436 text bytes including
alignment and all 160 data bytes, confirms all seven actual ELF addresses
and native sizes, checks the source object is selected without the original
fallback, and compares every byte of the final DOL. Its frozen SHA-256 is
`ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`.
The exact committed latest-main gate is recorded in the pull request before
any eventual merge.

## Traps

- The error conversion accepts positive values below 160, but its table is
  160 bytes, not 160 integers. Both the target and the pinned reference
  retain this bound. Increasing the initializer to 160 elements would
  overwrite the independently owned following data range.
- Explicit early returns and source operand order matter. An else-if
  chain using `0 < err` preserves the native signed comparisons in
  PFAPI_convertError; separate conditions let Wii/1.0 fold the range test
  and merge return values. The preserving-one helper matches with ordinary
  tests for 1, then nonzero; an equivalent bit expression merges its return
  register and changes six instruction words.

# F-365-2: standard chdir wrapper

`pf_chdir.c` is a separate standard API file in the pinned Wii Menu FA
split inventory. Its sole native function, `pf2_chdir` at `0x8021A5D0`,
is exactly 80 bytes and ends at `0x8021A620`. The CC0 Wii Menu VF
implementation supplies the ordinary source structure: initialize an OEM
PF_STR, call the directory operation on success, record a string error in
the volume set otherwise, and convert the return value. The native code
and established PF_STR/volume-set prefix independently prove this behavior.
No additional data or state belongs to the unit.

The one directly referenced outside primitive at `0x80205980` is named
`PFDIR_chdir` and declared in the canonical `pf_dir.h`. It takes one PF_STR
pointer, unlike the already declared two-argument internal
PFDIR_p_chdir. Its original range, size, type, source status, and storage
remain unchanged, and its body receives no source credit here.

The split-only baseline passed plain Ninja and frozen verification before
source registration. NonMatching registration produced a 100% text
comparison and passed full verification. With the source selected as
Matching, plain Ninja, explicit frozen verification, and the independent
source audit all exit 0. The audit pairs all five relocations, compares all
80 text bytes, confirms the native ELF address/size and source-only link
input, rejects unexpected allocated data, and compares the complete DOL.
Together the two units reconstruct eight of #365's 26 functions, 484 of
2,088 code bytes. Eighteen functions remain unfinished.

## Traps

- The smaller VF source subset does not identify every FA standard API.
  Native pointer-sized or counter-sized return values alone can suggest
  the wrong file operation. The complete pinned FA map and per-file split
  inventory distinguish cdelete, cinsert, create, chdmod, and chmod before
  declarations are established. Cross-check each lower native call rather
  than extrapolating from the public VF subset.
- Rebased standard-wrapper declarations can duplicate newly reconstructed
  lower interfaces. Retain the complete canonical PFDIR declarations from
  main, including its u32 createdir mode, and remove the redundant additions.
  The public signed mode forwards the same native 32-bit value. The #363
  owner also approved the signed-drive PFVOL_GetVolumeFromDrvChar declaration
  needed by the independently owned volume_apiadd unit. These declarations
  receive no source credit and require the expanded existing-object gate.
