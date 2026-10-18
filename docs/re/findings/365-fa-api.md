# F-365-1: complete PFAPI utility source

## Scope and provenance

Issue #365 lists 26 functions and 2,088 code bytes. The seven utility
functions at `0x8021A2B0` through `0x8021A464` form one complete retained
unit, with 404 function bytes and 32 bytes of inter-function alignment.
The preceding three survey entries are a codepage-unit tail. This initial
utility contribution took no preceding codepage source and left the later
standard wrappers unfinished. F-365-4 records their subsequent combined
completion without changing the utility unit's original source credit.

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

## Final prerequisite integration

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
2,088 code bytes. At that checkpoint eighteen functions remained; F-365-4
records their subsequent combined completion.

## Traps

The expanded prerequisite's fresh default --map build on pinned main
ea4ffb4742 passes plain Ninja, explicit report generation, all_source,
committed-head Ninja, and frozen verification. Both complete source units
retain 100% allocated sections and every native byte, with all seven
paired relocations, actual ELF/map addresses, and source-only link inputs.
The Ninja-declared inventory has 534 source objects. All 532 pre-existing
objects preserve every allocated section and relocation; 531 are whole-file
identical. The sole pf_dir difference is numeric local names in .strtab.
Compiling the same actual source against the original canonical headers
reproduces its original SHA-256 exactly. All shared metadata outside the
two owned objects/splits and nine owned symbol names is byte-identical to
the pinned base. The coordinated declarations and typed PF_LOCK receive
no extra reconstructed-function credit.

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
- An unused argument can survive a native-exact C forwarding wrapper without
  establishing the original interface. The native dispatcher at 0x8021CBB8
  supplies only drive in r3 to pf2_detach, leaving r4 as the jump-table case
  address; the public stub at 0x8021CFF0 likewise submits only drive. The
  lower PFVOL_detach at 0x80217E00 does not read r4. Both pending interfaces
  therefore take one signed drive argument, unlike the public VF revision's
  two-argument detach. The #363 body owner and #369 caller owner independently
  confirm this contract before canonical publication.
- Union-merging an older ledger append retained an obsolete neighboring
  pf_dir row as patch context. Compare the entire final diff to the pinned
  base and remove that duplicate, retaining the current owner's row and
  only this issue's two new unit records.

# F-365-3: remaining narrow standard API units

The original FA map and split inventory at the same pinned Wii Menu revision
identify fifteen additional one-function standard files owned by #365.
Their source follows ordinary FA/VF error conversion and PF_STR setup, with
actual native lower calls determining each operation and public argument
width. The separate follow-up begins from the verified utility/chdir
candidate rather than changing that prerequisite's accepted source scope.

| File stem | Native start | Code bytes |
| --- | --- | ---: |
| pf_attach | 8021A470 | 176 |
| pf_buffering | 8021A520 | 44 |
| pf_cdelete | 8021A550 | 124 |
| pf_chdmod | 8021A620 | 96 |
| pf_chmod | 8021A680 | 96 |
| pf_cinsert | 8021A6E0 | 124 |
| pf_closedir | 8021A760 | 36 |
| pf_combine | 8021A790 | 124 |
| pf_create | 8021A810 | 92 |
| pf_createdir | 8021A870 | 116 |
| pf_cut | 8021A8F0 | 136 |
| pf_derrnum | 8021A980 | 40 |
| pf_detach | 8021A9B0 | 40 |
| pf_divide | 8021A9E0 | 144 |
| pf_errnum | 8021AA70 | 36 |

All fifteen ordinary C candidates reproduce their complete native
instruction words. Their split-only baselines pass configure, plain Ninja,
and explicit frozen verification. With the coordinated canonical lower-call
headers, the NonMatching pipeline passes and every actual object, function,
and section scores 100%. After selecting all fifteen source objects as
Matching, plain Ninja and explicit frozen verification exit 0. Independent
actual-object audits pair all 61 relocations, compare all 1,424 native code
bytes, confirm each ELF address and size, reject unexpected allocated data,
confirm source-only map and Ninja link inputs, and compare the entire DOL.
This follow-up's final combined and current-main gates remain required
before merge. The earlier utility/chdir prerequisite remains separately
accounted as eight functions and 484 code bytes.

The attach wrapper takes a driver-table pointer array and an optional NAND
backing-file record. Its lower PFVOL_attach forwards the record's initial
string and word at offset 0x50 to the native NAND notification operation at
0x8021F8E0. The pinned original FA map names that operation
fa_nanddrv_NotifyNANDFile; its target size and behavior agree. Public
FAAttach names the corresponding arguments fsPathNAND and fsSizeNAND. The
outer target wrapper at 0x8021D6C0 copies 0x4E path bytes into command offset
0x11C and writes the backing-file size at 0x16C. A descriptive PF_NAND_FILE
therefore has char path[78] and u32 size, with natural alignment placing
size at 80 and total extent 84. A scratch translation unit including the
actual canonical candidate header, compiled with the real FA Wii/1.0 Ninja
command, emits the three layout constants 84, 80, and 78 for sizeof, the
size-member offset, and path extent respectively. The original array bound is not uniquely
recoverable: char[80] would have the same ABI and accesses. The chosen 78
follows the actual copy bound and introduces no explicit padding member.
These names describe observed behavior rather than claiming original type
or field spelling. No PF_VOLUME layout is changed.

The first three #365 rows are the tail of the CP932 unit whose first
function at 0x80219D80 belongs to #364. After a separate legitimate #364
claim and isolated split/source verification, the complete unit is now
consolidated into this follow-up. Its six functions total 1,280 code bytes:
1,100 belong to #364 and 180 to #365. The complete 45-by-189 u16 mapping
is byte-identical to both pinned references and the native 17,010-byte
payload. F-364-1 records its full source-selected proof, data-boundary
correction and native Unicode-width behavior. The combined current-main
acceptance passes as recorded in F-365-4 below; no duplicate source credit
is assigned to either issue. The separate 120-byte volume_apiadd source
also passes its baseline, NonMatching and source-selected gates in F-364-3.

## Traps

- A later conflict-free rebase replayed an older ledger normalization and
  removed a neighboring PFENT acceptance row added on main. Preserve every
  outside row from the pinned current base, even when that base contains
  both historical and newer records; update only owned rows and append
  owned units. A clean textual merge does not establish preservation.
- The original FA map distinguishes narrow detach and buffering at
  0x8021A9B0 and 0x8021A520 from rmvvol and setclstlink in the following
  standard block. Reusing VF subset names there creates real link-name
  collisions even when each independent body matches.
- Public chdmod/chmod attributes and buffering mode are 32-bit values,
  while their lower calls consume u8. Declaring the public argument u8
  removes or reschedules the native truncation. Preserve both contracts.
- The native create wrapper does not initialize its local PF_FILE pointer
  on the generic PFSTR error branch; actual PFSTR_InitStr returns zero.
  Importing the VF NULL initialization adds instructions. The ordinary
  source preserves the observed native control flow without forcing code
  generation through casts or flags.

## F-365-4: complete original inventory and combined source proof

The fresh combined default build on actual main 1d16cf44e8 passes configure
--map, Ninja with four compile jobs, final plain Ninja, actual report,
explicit all_source and frozen verification. Independent native source
audits cover all nineteen relevant units / thirty functions / 3,308 code
bytes / 93 paired text relocations, the complete CP932 table and utility
data, actual ELF extents, native map contributions and source-only link
inputs. The new follow-up contributes twenty-two functions / 2,824 code
bytes in seventeen units. The earlier #882 eight functions / 484 bytes
retain their separate credit. All 534 existing main source objects remain
whole-file identical; Ninja declares exactly 551 actual source objects.

The independent original-issue audit reads the original #365 inventory,
checks every native start and size against the actual final ELF, and runs
the strict whole-source-unit audits. All twenty-six original functions /
2,088 bytes are covered: three CP932 tails, seven utility helpers and
sixteen standard wrappers. The entire immutable DOL is exact. This completes
#365's source inventory; the final committed-head/latest-main merge gate
still applies. The separate #364 contribution is four functions / 1,220
bytes; its twenty-two main-volume functions remain incomplete.

## F-365-5: paired addresses must agree with canonical callee identity

The first full source-selected volume link was 128 bytes short although
the isolated volume instruction and relocation proof passed. The label
comparisons in getvol and rmvvol called pf_strncmp, while the scratch
address map incorrectly bound that spelling to 0x801FB270. The accepted
canonical pf_clib source establishes that address as pf_memcmp; pf_strncmp
is at 0x801FB380. The wrong call left the retained 124-byte pf_memcmp
unreferenced, and linker alignment accounted for the remaining four bytes.
Both eleven-byte label comparisons now use the actual pf_memcmp interface.
No source padding, compiler flags, or frozen expectations were changed.

### Traps

A paired-relocation proof can accept the wrong external interface if its
name-to-address map repeats the reconstruction hypothesis. Check each
callee identity against current canonical metadata and accepted source,
then require the source-selected link and immutable DOL comparison.

## F-365-6: combined canonical source acceptance

The combined actual source gate passes all48 relevant complete units,124
functions,36,512 code bytes and1,156 paired text relocations. Independent
original-inventory checks cover #363 all23 rows/8,748 bytes, #364 all26
rows/13,660 bytes and #365 all26 rows/2,088 bytes. The path unit also
contributes23 #362 rows/10,476 bytes; its three preceding PFFILE tails/696
bytes remain outside this delivery, so #362 stays partial.

The independent #366 standard delivery merged as #897 during integration.
Its26 functions/1,540 bytes/71 targets remain canonical current-main source
and are rechecked as existing coverage, with no repeated closure or source
credit here. This delivery therefore adds19 source units/86 functions/32,656
code bytes. Earlier #882 utility/chdir coverage remains8 functions/484 bytes.
Current-main rebase preservation is checked against explicit immutable base
commits; every outside configuration entry, split, symbol attribute and
ledger row is retained. All actual source inputs, final ELF/map extents,
allocated payloads, alignment and complete frozen DOL pass. Final acceptance
still requires the committed-head latest-main gate and guarded merge.

The complete sector unit subsequently merged independently as #911 during
integration. Its four functions/1,832 bytes remain current-main coverage,
so the final new-source contribution is19 units/86 functions/32,656 bytes.
The complete original #363 inventory still requires this delivery's path
and volume rows; the sector merge alone does not establish issue completion.
