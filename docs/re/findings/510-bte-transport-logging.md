# F-510-1: Bluetooth BTE transport and logging units

## Evidence and scope

Issue #510 owns 0x80313A60-0x80313E34. Disassembly of the verified `build/wpse-v1/main.elf` with binutils 2.42-2, together with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, identifies three complete units and the start of a fourth. Petari is a naming/reference aid; the target instruction comparison is the evidence for correspondence.

| Unit | Target text range, exclusive end | Functions | Implementation state |
| --- | --- | --- | --- |
| bte_hcisu.c | 0x80313A60-0x80313B30 | bte_hcisu_send, bte_hcisu_task, bte_hcisu_close, bta_ci_hci_msg_handler | Reconstructed; NonMatching |
| bte_init.c | 0x80313B30-0x80313B60 | BTE_InitStack | Reconstructed; NonMatching |
| bte_logmsg.c | 0x80313B60-0x80313D00 | LogMsg, LogMsg_0 through LogMsg_6 | Reconstructed; NonMatching |
| bte_main.c | Begins at 0x80313D00, continues beyond this issue | BTUInterruptHandler, BTA_Init, BTA_CleanUp in this issue | Names recovered; original fallback |

The function order and bodies agree with the reference. `BTUInterruptHandler` switches to a 0x1000-byte fiber stack, `BTA_Init` performs Bluetooth stack initialization and periodic alarm registration, and `BTA_CleanUp` cancels that alarm and shuts down transport/GKI. The next function at 0x80313E40 is the reference's `bta_usb_close_evt`; it belongs to #511 and is not renamed here. No partial split pretending to be the complete bte_main.c is introduced.

## Data and ABI

`p_hcisu_if` is at 0x804B9B78, `p_hcisu_cfg` at 0x804B9B7C. The transport callback prefix has init/open/close/send pointers at offsets 0/4/8/12. The reference supplies a fifth event-handler pointer at 16; that callback's return type is not exercised by these four functions. The interface layout is reference-derived beyond offset 12, and is not offered as a finalized shared SDK ABI. `tHCI_CFG` remains opaque rather than inventing field names.

`HC_BT_HDR` is the reference's eight-byte header: event, len, offset and layer_specific are unsigned 16-bit fields at offsets 0, 2, 4 and 6. Only event is accessed by this unit. The task parameter is unused, so its source type cannot be distinguished by this code alone.

The logger uses a static 2000-byte buffer, with an original allocation/padding span 0x804A9040-0x804A9820. Its newline format occupies 0x804B8EE0-0x804B8EE8 including padding. The source's local static currently emits as common storage; the target split labels the entire 2016-byte span as BSS. Resolving the 16-byte alignment tail and common-storage placement remains necessary before linking this object from source.

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

These out-of-region function symbols are not renamed in this change. The three reconstructed units intentionally remain NonMatching. Completion also requires coordinating bte_main.c with #511, reconstructing the three remaining functions owned by #510, proving the complete unit boundaries, and full verification with accepted sources linked.

## Source provenance and reusable lesson

Petari's root CC0 license does not override file-level notices. The BTE sources and hci.h include Broadcom Apache-2.0 notices and changes by muff1n1634. The notices, modifications and complete license are retained in [BTE provenance](../licenses/510-bte-NOTICE.md). Inspect file-level notices before reusing other BTE files; do not classify the entire library as CC0 based on the repository root.
