# F-510-1: Bluetooth BTE transport and logging units

## Evidence and scope

Issue #510 owns 0x80313A60-0x80313E34. Disassembly of the verified `build/wpse-v1/main.elf` with binutils 2.42-2, together with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, identifies three complete units and the start of a fourth. Petari is a naming/reference aid; the target instruction comparison is the evidence for correspondence.

| Unit | Target text range, exclusive end | Functions | Implementation state |
| --- | --- | --- | --- |
| bte_hcisu.c | 0x80313A60-0x80313B30 | bte_hcisu_send, bte_hcisu_task, bte_hcisu_close, bta_ci_hci_msg_handler | Reconstructed; NonMatching |
| bte_init.c | 0x80313B30-0x80313B60 | BTE_InitStack | Reconstructed; NonMatching |
| bte_logmsg.c | 0x80313B60-0x80313D00 | LogMsg, LogMsg_0 through LogMsg_6 | Reconstructed; NonMatching |
| bte_main.c | Begins at 0x80313D00, continues beyond this issue | BTUInterruptHandler, BTA_Init, BTA_CleanUp in this issue | Three #510 functions reconstructed; #511 callback and data remain fallback |

The function order and bodies agree with the reference. `BTUInterruptHandler` switches to a 0x1000-byte fiber stack, `BTA_Init` performs Bluetooth stack initialization and periodic alarm registration, and `BTA_CleanUp` cancels that alarm and shuts down transport/GKI. The next function at 0x80313E40 is the reference's `bta_usb_close_evt`; it belongs to #511 and is not renamed here. The complete unit ends at 0x80313E70. Its split and the #511 handoff are recorded below.

## Data and ABI

`p_hcisu_if` is at 0x804B9B78, `p_hcisu_cfg` at 0x804B9B7C. The transport callback prefix has init/open/close/send pointers at offsets 0/4/8/12. The reference supplies a fifth event-handler pointer at 16; that callback's return type is not exercised by these four functions. The interface layout is reference-derived beyond offset 12, and is not offered as a finalized shared SDK ABI. `tHCI_CFG` remains opaque rather than inventing field names.

`HC_BT_HDR` is the reference's eight-byte header: event, len, offset and layer_specific are unsigned 16-bit fields at offsets 0, 2, 4 and 6. Only event is accessed by this unit. The task parameter is unused, so its source type cannot be distinguished by this code alone.

The logger uses a static 2000-byte buffer, with an original allocation/padding span 0x804A9040-0x804A9820. Its newline format occupies 0x804B8EE0-0x804B8EE8 including padding. An explicit 32-byte alignment now makes the compiler emit the 2000-byte buffer into .bss rather than common storage while preserving every logger instruction. The original split includes the following 16-byte alignment tail; final source-link acceptance remains pending.

The logger's 12-byte PowerPC variadic state contains byte-sized GPR/FPR indices, two reserved bytes, and two pointers. The compiler intrinsic produces the same register-save sequence and stack layout as the target. Logging wrappers test the byte at 0x804B9B80, the reference's `bte_target_mode`, before tail-calling LogMsg.

## Verification and limits

Wii/1.0 with the existing base flags produces 100% objdiff code scores for all 13 reconstructed functions: 184 bytes in bte_hcisu, 44 in bte_init and 348 in bte_logmsg, excluding padding. This is 576 reconstructed code bytes; none is counted as source-linked progress. The logging BSS does not yet match the source storage representation.

`python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` all exit 0. Verification uses the original objects for these three NonMatching units. It establishes that the new splits and names preserve the full DOL, not acceptance of their source replacements.

External symbol identities to coordinate with their owning issues before source linking:

| Address | Recovered external name |
| --- | --- |
| 0x80311160 | GKI_freebuf |
| 0x80311320 | GKI_send_msg |
| 0x803326AC | RFCOMM_Init |
| 0x80325984 | GAP_Init |
| 0x80329258 | HID_DevInit |
| 0x80329CDC | HID_HostInit |
| 0x801EB7B0 | vsprintf |
| 0x804B9B80 | bte_target_mode, owned by bte_main.c |

These out-of-region function symbols are not renamed in this change. The three reconstructed units intentionally remain NonMatching. Completion still requires external-name coordination, #511 callback/data integration, and full verification with accepted sources linked. All sixteen #510 functions have now been reconstructed, as detailed below.

## Source provenance and reusable lesson

Petari's root CC0 license does not override file-level notices. The BTE sources and hci.h include Broadcom Apache-2.0 notices and changes by muff1n1634. The notices, modifications and complete license are retained in [BTE provenance](../licenses/510-bte-NOTICE.md). Inspect file-level notices before reusing other BTE files; do not classify the entire library as CC0 based on the repository root.

## F-510-2: Complete main-unit split and remaining three functions

The pinned reference has four emitted functions in bte_main.c. The target matches their order and signatures: BTUInterruptHandler at 0x80313D00 (24 bytes), BTA_Init at 0x80313D20 (220), BTA_CleanUp at 0x80313E00 (52), and the #511-owned bta_usb_close_evt at 0x80313E40 (48). The next unit begins at 0x80313E70. The split owns .text 0x80313D00?0x80313E70, .bss 0x804A9820?0x804AA850, .sdata 0x804B8EE8?0x804B8EF0, and .sbss 0x804B9B80?0x804B9B88. The neighboring callback remains original fallback, without a changed name or implementation. Coordination: [#511 comment](https://github.com/revvmp/pokemon-rumble-decomp/issues/511#issuecomment-6232093902).

The new source implements only the three #510-owned functions. All three score 100% in objdiff with Wii/1.0 and base flags. Across the four BTE units, all sixteen #510 functions now score 100%, totaling 872 instruction bytes. These are reconstructed and locally compared, with zero newly source-linked bytes: all four units remain NonMatching. The main-unit report has four functions and 344 instruction bytes, with three matching functions and 296 matching bytes; its remaining 48 bytes belong to #511.

The original main data comprises a 0x1000-byte fiber stack at 0x804A9820, a 0x30-byte alarm allocation at 0x804AA820, the transport configuration at 0x804B8EE8, the target-mode byte at 0x804B9B80, and application callback pointer at 0x804B9B84. Only those owned unit data symbols were renamed. Source declarations leave storage and OSAlarm/OSContext opaque until integration with the neighboring callback and SDK headers. The reference establishes a four-byte transport configuration containing two unsigned 16-bit values. A two-element unsigned-short storage view preserves this known extent without inventing names for its unproven fields.

An incomplete external tHCI_CFG declaration prevented the compiler from using small-data addressing for its address, adding an instruction and moving later instructions. Replacing only its external storage view with the proven four-byte array raised BTA_Init from 95.90909% to 100%. The alarm interval is twice the integer quotient of the bus clock divided by four and then 1000; the generated multiplication and shift match the target.

BTE main external identities, in addition to F-510-1: OSSwitchFiber 0x802B4B20, OSSetPeriodicAlarm 0x802B3280, OSCancelAlarm 0x802B3310, GKI_init 0x80312410, GKI_enable 0x80312650, GKI_run 0x80312550, GKI_delay 0x80312580, GKI_shutdown 0x803124A0, BTE_Init 0x80325858, BTA_DmIsDeviceUp 0x80316E40, btu_task_init 0x80313E70, btu_task_msg_handler 0x80313EB0, hcisu_h2 0x80397E30, and wait4hci 0x804B8EA0. These out-of-region symbols are untouched by this issue; OSSetPeriodicAlarm and OSCancelAlarm are now named by the merged #464 work.

Validation uses the real configure/ninja/report/verifier workflow. The clean-build command and exact result are recorded in the pull request. No frozen manifests or expected hashes changed. Full verification proves the split preserves main.dol using original fallback objects; it does not accept these sources as linked replacements.
