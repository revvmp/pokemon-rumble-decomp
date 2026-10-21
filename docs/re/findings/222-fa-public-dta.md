# F-222-4: Distinct public FA directory-search record

This bounded prerequisite corrects the public FADta storage contract required by the actual three-function DCIM producer in issue #222. It does not reconstruct another source unit or change the original issue completion count. The accepted five-function Font.cpp unit is recorded in F-222-2.

## Independent native consumer

Native FindNextDcfIndex at 80125F20 allocates directory records at stack offsets 0x47C and 0x38, exactly 0x444 bytes apart. Both are passed to the existing WaitFaFsfirst/WaitFaFsnext actual async wrappers. Attribute accesses are at object offset 0x22C; short-name accesses are at 0x22D. For the first record these are frame offsets 0x6A8 and 0x6A9, and for the second they are 0x264 and 0x265. The complete native frame is 0x8E0 bytes. The natural reconstructed caller with two complete public records reproduces that frame extent and both placements.

## Public and private contracts

The earlier public FADta forward alias to PF_DTA was an opaque pointer-only approximation. It does not establish an allocation size. The complete private PF_DTA legitimately has Unicode arrays beginning at offset 0x444, and its size is 0x870. That private type, its fields and all its consumers are preserved. The public record ends at that same 0x444 boundary and is a distinct type.

The original public FADta field names, array bounds and scalar types are corroborated by [FA types](https://github.com/galaxymaster2007/THPConv/blob/e7f2a17eeff43bc6b822a01af66ce670822ab77b/include/revolution/fa/fa_types.h), pinned to e7f2a17eeff43bc6b822a01af66ce670822ab77b. The matching synchronous declarations are corroborated by [FA API](https://github.com/galaxymaster2007/THPConv/blob/e7f2a17eeff43bc6b822a01af66ce670822ab77b/include/revolution/fa/fa_api.h). The restrictive Nintendo notices and absence of a permissive root license were inspected. No restricted header text or implementation is imported. Only factual public names, types, bounds and ABI observations inform the independent declaration, checked against target accesses and actual consumers. The older CC0 IPL public FADta also ends at 0x444; that reconstructed reference independently corroborates extent rather than proving original member spelling. Existing public opaque file handles remain compatible aliases; new directory/volume handles likewise forward to the existing private tags without storage duplication.

| Offset | Public member | Proven extent/type |
| --- | --- | --- |
| 0 | pFile | Existing opaque FAFile pointer |
| 4 | pDir | Opaque directory pointer |
| 8 | pVol | Opaque volume pointer |
| 0xC | parentStartCluster | u32 |
| 0x10 | parentPos | u32 |
| 0x14 | status | u32 |
| 0x18 | numEntryLFNs | u8 |
| 0x19 | ordinal | u8 |
| 0x1A | checkSum | u8 |
| 0x1B | attr | u8 |
| 0x1C | regExp | 520 signed bytes |
| 0x224 | Time | u16 |
| 0x226 | Date | u16 |
| 0x228 | FileSize | u32 |
| 0x22C | Attribute | u8 |
| 0x22D | FileName | 13 signed bytes |
| 0x23A | LongName | 522 signed bytes |

Natural alignment gives exactly 0x444 bytes without explicit padding fields. FAChdir(const char*) at 8021D850 forwards to the native pfstub_chdir and maps a nonzero status to -1. FAErrnum() at 8021D980 queries pfstub_errnum. Both return signed 32-bit values, as the pinned interface and native branches establish. Their symbols are named without changing their 44- and 64-byte extents.

## Existing consumers

The existing source-selected game/FaAsync.cpp consumes only FADta pointers. Its two descriptive C++ symbols change from an inferred private PF_DTA tag to the independently recovered public FADta tag; these are binding corrections, not changed caller ABI. Every function byte and relocation remains identical after accounting for those two intended names. No async work-area, semaphore, callback or result-field contract changes.

All 757 existing source objects were captured before this header edit. The current comparison checks every function plus every allocated section and relocation, not just fuzzy scores. It normalizes only the two explicit waiter binding corrections and compiler-generated numeric symbol spellings by exact section/value/size. Both comparisons report zero changed objects. The full configure, ninja, original SHA-1 link check and frozen SHA-256 verifier pass. The private FA directory implementation and source-selected async waiter therefore preserve their accepted native artifacts.

Outside 222-reference/fa-public-control contains the complete public/private layout producer, successful compilation command, object, twenty exact size/offset measurements, six actual native stack/field instructions, consumer baseline objects and reusable audits. The independent DCIM producer remains outside in 222-reference/dcim: two parsers reproduce 1,020 instruction bytes with every relocation, while the final index picker retains a four-byte pointer-hoisting difference and is not claimed as source accepted.

## Traps

An opaque alias can safely describe pointer-only calls yet be wrong for allocating the complete public object. Preserve both independently evidenced public and private record contracts when a native caller proves different extents; do not remove real Unicode storage from the library type to fit a game stack.

The pinned public header is encoded in CP932 rather than UTF-8. It is retained outside as original bytes for factual inspection, with its SHA-256 recorded; no encoding-normalized restricted header is committed.
