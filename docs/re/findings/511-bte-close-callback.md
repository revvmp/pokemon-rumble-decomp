# F-511-2: BTE transport close callback

Issue #511 owns `bta_usb_close_evt` at `0x80313E40`, size `0x30`, the final retained function in `bte_main.c`. Issue #510 owns that unit's first function, split, storage, and remaining three functions. This supporting change provides the missing callback so #510 can finish the shared source unit; it does not complete the other #511 units.

The original `wpse-v1/main.dol` has frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Its callback bytes begin at file offset `0x2C4100`. Binutils 2.42-2 disassembly confirms a twelve-instruction function: load `_bte_app_info` from SDA, return when null, test the signed input byte, then tail-call the application callback with zero for nonnegative transport status or one for negative status. The terminal return is retained after both tail-call paths.

The original name and signed-byte parameter agree with Petari commit `e0ab141914921726a595573dc48d3f13d0922efe`, `src/RVL_SDK/bte/bte_main.c`. The modified implementation follows that reference with a descriptive parameter name and the existing callback typedef. File-specific Broadcom Apache-2.0 notices and reference provenance remain in [the BTE notice](../licenses/510-bte-NOTICE.md).

Wii/1.0 with the existing base options reproduces all twelve instructions, with the expected SDA21 relocation to `_bte_app_info` at `0x804B9B84`. The function's `symbols.txt` entry is renamed without changing its size, type, or split. The complete unit remains `NonMatching` in this prerequisite; #510 supplies and verifies the missing storage and accepted full-unit source link separately. Full fallback-build and verifier results are recorded in the pull request.

## Traps

No new reusable trap emerged. A matching callback object does not by itself prove acceptance of the shared unit; the successful complete source link and frozen-artifact verification remain required.
