# F-369-3: Complete stub probe and qualification confidence

This continues [F-369-1 and F-369-2](369-fa-filelock-stubs.md) for the original 22-function issue inventory. The complete four-function `pf_stub.c` probe reproduces all 956 retained native code bytes, every independently resolved relocation, and the complete `0x3D4` text section including 24 internal alignment bytes. Its range is `0x8021C4A0–0x8021C874`; the following 12 alignment bytes before `PFSYS_initializeSYS` belong outside the split. There is no allocated data in this unit. Its referenced globals `gOpenDisk` and `gOpenPartition` are independently named in the pinned original FA map, at target `0x80421510` and `0x80421578`; they remain external storage belonging to the later FA layer.

The typed initializer uses the F-368-2 work area and ordinary task/mailbox configuration records established by actual kernel wrappers. Setting task priority before stack size preserves the observed assignment order. Command submission uses an actual canonical `OSSemaphore` local plus a three-field configuration record; declaration order determines which equal-sized local receives the first stack slot. The result routine uses signed scalar request counts, demonstrated by native `cmpw` instructions; unsigned fields change two words. The provisional helper spelling `pfk_send_mailbox` was rejected by the strict original-name guard before publication: the original name is `pfk_send_message`.

## Physical command ownership and fields

The queue stores a pointer to a caller-supplied request record, rather than copying the record. Native `FAAttach` at `0x8021D6C0` receives the outer record in `r8`, initializes it, and passes it through the attach stub and command-submission routine to `pfk_send_message`. The worker later consumes that same memory and invokes its callback. The physical common command begins four bytes into the outer record.

The apparent unknown word at common offset `0x20` is the terminating element of `PF_DRV_TBL* driver_table[2]`, beginning at `0x1C`. The outer initializer zeroes that element at `0x8021D7B4`; the attach stub passes the array address at `0x8021CED4–0x8021CEDC`. The independently reconstructed `pf2_attach` walks that null-terminated driver pointer array. The worker consumes element zero's drive/status fields. Common offsets `0x2C` and `0x30` carry disk and partition pointers, set by the outer initializer at `0x8021D7D8–0x8021D7E0` and copied by the worker into the two external arrays. This eliminates an invented reserved/padding member from the temporary command record.

## Compatible compiler setting

Default SDK `-ipa file` and automatic inlining expand the result helper into the worker when all four functions compile together; the native worker instead calls that helper. Either `-ipa off` or `-inline noauto` produces the same exact complete four-function text and relocation targets with the currently matching probe. The latter is the narrower compatible setting. These experiments establish compatible configurations, not the original spelling or mechanism of the inlining policy. No repository compiler flag has changed. A bounded final unit override still requires actual canonical-header consumers, source objects, section/data/relocation checks and full frozen DOL verification.

Wii 1.0a, 1.1 and 1.3 do not resolve the ordinary-field worker range-check discrepancy. Three GC 3.0 compilers, optimization levels O2/O3/O4,s, reversed equivalent range expressions and ordinary local boundary variables also did not provide an exact worker. The source snapshots and measured objects remain outside the repository; these bounded failures do not establish impossibility.

## Qualification remains a source-fidelity decision

The current exact probe uses a volatile operation field and takes one ordinary local snapshot for each standard/Unicode-family range test. A direct ordinary-field expression emits a 340-byte worker instead of the native 356; ordinary-field local snapshots emit 352 bytes under default, noauto and ipaoff. The last missing instruction is the native reload before the Unicode-family test. The volatile/snapshot form reproduces all native instructions. Full packet layout and complete-unit results are retained by `369-audit-stub.py` against both compatible outputs.

That byte evidence does not prove an original volatile declaration. The actual caller-owned asynchronous pointer handoff is established, but no concrete post-enqueue write to the operation field during the same in-flight request has yet been identified, and the pinned IPL public header predates this asynchronous interface. The qualification's original confidence remains unresolved. Before publication, complete the public packet contract and reuse/polling investigation, record the chosen qualification and confidence, and review its behavioral basis rather than accepting it solely to force an extra load. The temporary record and complete stub source remain outside the repository.

## Traps

- Isolated function probes can hide automatic inlining across a complete source unit. Compile all retained functions together before accepting the unit or selecting its compiler flags.
- An extra native reload does not independently establish volatile qualification. Preserve alternative ordinary-field results and investigate the actual shared packet contract before publishing a qualifier.

Together with the two file-lock functions and three system functions, the complete stub result covers nine original issue functions and 1,440 of 3,624 native bytes externally. None is linked source credit. The standard dispatcher and twelve public stub entries still require reconstruction, and all actual Ninja/canonical integration/full-DOL gates remain pending.
