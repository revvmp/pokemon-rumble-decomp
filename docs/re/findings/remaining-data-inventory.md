# Remaining data inventory

The accepted 2026-11-04 two-function checkpoint has 1,778,445 of 1,788,360 source-linked data bytes, or 99.44558 percent. Exactly two original fallback units remain in the normal final link: PiiPropData with its two unfinished CPU functions and associated data, and the DSP instruction image. There are no remaining standalone alignment objects or unexplained anonymous storage ranges.

| Remaining category | Bytes | Meaning |
| --- | ---: | --- |
| PiiPropData data | 1723 | Metadata, literals and tables attached to the unit whose constructor and Reset remain unfinished |
| DSP instruction image | 8192 | axDspSlave, executable DSP code; deliberately excluded from raw-byte source copying |
| Total | 9915 | Explicit original-data dependencies |

CDlgPiiSelector, CPlayerPii, NPpuVM and ef_animcurve now link their complete data from source, following the earlier CLayout, CGameResult and CPiiListFilterSort acceptances. All 568 animation table/literal bytes and every retained function pass the full native source-link comparison. See [animation acceptance](animcurve-20261104-continuation.md).

## Reconstructed system-message archive

Read-only inspection identifies gSystemMessageArchive at .rodata:0x80363A80 as a U8 archive, with magic 55 AA 38 2D, root offset 32, header size 271 and payload offset 320. Its 11,488 bytes have SHA-256 e27d380bbad3aff2ef2d41cfdc5b77590390a68937fef6edc87502f2d1813161. The eleven nodes include six files:

| Path under arc | Offset | Bytes |
| --- | --- | ---: |
| anim/fatal_errmsg_begin.brlan | 0x140 | 368 |
| anim/fatal_errmsg_button_a.brlan | 0x2C0 | 384 |
| anim/fatal_errmsg_end.brlan | 0x440 | 364 |
| blyt/fatal_errmsg.brlyt | 0x5C0 | 7688 |
| timg/button_a.tpl | 0x23E0 | 2112 |
| timg/window.tpl | 0x2C20 | 192 |

The main initialization path passes this archive, fatal_errmsg and fot_wanpaku_b.brfnt to InitializeSystemMessages. The archive now rebuilds from nine semantic JSON/PNG inputs and links from the ordinary compiler output with frozen verification passing. The encoder reads no DOL or original member binary. See [asset recovery and acceptance](system-message-archive-20261104.md). The separate 8,192-byte axDspSlave block is DSP executable code, not an image or layout asset, and cannot be reclassified as ordinary source data. Its identity and diagnostic tooling are recorded in [DSP findings](ax-dsp-image-20261104.md).

## Resolved storage and alignment

The former 808-byte anonymous BSS region after IdleContext contains the unused SDK IdleThread, a complete 792-byte OSThread, followed by sixteen alignment bytes. A pinned independent SDK source declares and retains that exact typed object. The accepted compiler output places it at 0x80453018 and ends at 0x80453330; the following OSStateTM buffer supplies its own native alignment. The normal complete DOL verifies. See [declaration, provenance and positive/negative controls](anonymous-data-20261104.md).

Nineteen further automatic gaps total 352 bytes. Fourteen already follow naturally from accepted source alignment; five require restoring evidenced alignment attributes or an unused reference-source overload whose aligned work buffer is itself discarded. The last seven-byte ControllerTableAlignmentPadding unit is also supplied naturally by StageInfo's eight-byte section alignment. All accepted omission trials and the normal pipeline reproduce the unchanged frozen executable. These 375 bytes, including IdleThread's following gap, are excluded from the data denominator without adding source credit. Detailed ranges and source declarations are in [the anonymous-data audit](anonymous-data-20261104.md).

The filter placeholder is separately resolved as the one-byte std::tr1::placeholders::_1 object at 0x804B9028, with its surrounding eleven alignment bytes excluded at the earlier six-function checkpoint. See [placeholder acceptance](cpiilistfiltersort-source-link.md#placeholder-ownership-and-denominator-correction). The code-alignment ledger independently accounts for 3,792 code-padding bytes and makes no claim about asset or DSP reconstruction.
