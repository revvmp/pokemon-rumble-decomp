# F-280-1: photo unit feasibility checkpoint

The original issue contains six functions and 12,504 code bytes at `8016CB20` through `8016FC08`. A fresh helper claim and isolated worktree start at main `5a5b2211`. No source selector, split, production source, shared header or symbol changes are accepted.

The first three functions reproduce all 108 instruction bytes under canonical Wii/1.1 game flags. The insertion callback sets the card-present and card-changed bytes at `804B90D8` and `804B90D9`; removal clears the former. The thread entry receives a 24-byte record containing a signed result at zero, the existing complete `PF_DEV_INF` at four and a signed drive byte at twenty. It stores the result of `FASetupfsi(drive, 6)`, then overwrites it with `FAGetdev(drive, &info)` and returns that result through the thread entry return register. Both direct native call targets and all three SDA references are resolved against the frozen DOL. These descriptive callback and record names are inferred.

The two synchronous API names and scalar arguments are factual reading evidence from the pinned THPConv FA public interface at commit `e7f2a17eeff43bc6b822a01af66ce670822ab77b`. Its restrictive source is not imported. The native wrappers at `8021DF00` and `8021DB00` call command producers at `8021D3A0` and `8021D1A0`; the latter writes the output pointer into command offset `1C`. The existing `PF_DEV_INF` record matches the actual photo caller's four output words.

The fourth function, `8016DF60`, is a two-argument wide formatter. Ordinary source `(NIntl::basic_formatter<wchar_t>(format) % directory % filename).str()` compiles to 1,204 bytes against the native 1,212. Its native constructor caches the first string's address in `r23` at `8016DF9C`, before `wcslen` at `8016DFA4`; the current canonical string constructor recomputes the address after the length call. This independently repeats the unresolved contract documented for the stage loaders and formatters. No allocator, register, flag or constructor permutation is attempted again here.

Raw controls are preserved outside the repository in `C:/MassStorage/GitHub/280-reference`: `280-photo-callbacks.cpp`, `280-photo-callbacks.o`, `280-audit-callbacks.py`, `280-callback-proof.json`, and `280-photo-formatter.cpp/.o`. The callback audit applies actual branch and SDA relocations before comparison. Its result is three exact functions, not source-selected acceptance. Both large save/dialog routines remain unimplemented, and the issue is incomplete.

## F-280-2: Actual save routine and FA contracts

A renewed helper claim uses the current public `FADta` definition from PR #1185 and preserves the earlier feasibility checkpoint. The outside `280-photo-save.cpp` now reconstructs the screenshot-save routine as ordinary C++ with the real dialog, layout, session, fiber, FA and reset-guard interfaces. It includes card attachment, mounting, device-query threading, free-space checks, DCF directory selection, creation and writing, and the success/error dialogs. The second large deletion-dialog routine remains to be written. No production source, split or matching selector is changed.

The draft compiles with canonical game flags and emits 5,228 bytes against the native save routine's 5,064. It is not accepted. Its three actual callback definitions still pass the strict relocated comparison for all 108 bytes; `280-audit-save-callbacks.py` also checks the frozen executable SHA-256 first. `280-compile-save.py` records the exact arguments, source/object hashes and exit status. The current containing producer includes the actual two-argument formatter after the save routine, retaining the original strong-function order.

Both native large routines build the same twelve-byte FA driver-table layout and twenty-byte cache settings. The public pinned FA declarations and the existing `PF_DRV_TBL` and `PF_CACHE_SETTING` layouts agree here, unlike the previously corrected directory-search record. Save uses 256 FAT pages, four data pages, 64-sector FAT buffers and one-sector data buffers; its allocated cache buffer is 133,120 bytes. The cleanup control-block vtable at `0x803A42AC` independently names `shared_ptr_deleter<unsigned char, void (*)(void*)>`, agreeing with the draft's byte owner and `GameMemory::Free` deleter. The drive-letter validation tests the locale classification mask `0x200`, which is `isupper`, rather than the generic alphabetic classification.

The native save and deletion-dialog stack frames dynamically align to 32 bytes. Their cache-page arrays begin at aligned stack offsets `0xA60` and `0x640`, respectively. The draft gives the save cache-page array explicit alignment as a source hypothesis; this reproduces the alignment requirement but is not final evidence of the original declaration. All native wide success-message literals are decoded directly, including the Japanese yen-sign path separators versus the other languages' backslashes. Their final literal placement remains unaccepted.

Outside reading aids for both native routines are saved as `280-save-analysis.txt` and `280-dialog-analysis.txt`. They are not reconstructed source. The next work is the complete deletion-dialog body, then strict instruction, target, data, EH and source-selected verification of the actual containing unit.

## F-280-3: Complete producer and shared-owner swap

The outside producer now contains all six original bodies. Save emits 5,056 bytes against 5,064, the formatter 1,204 against 1,212, and the deletion dialog 6,120 against 6,120. These supersede the earlier incomplete checkpoints above. No photo source unit is selected as Matching. Direct caller-scope completion statements, the actual `IGameSession::GetInstance()` call, signed counters and the observed cache-page alignment recover the native deletion-dialog instruction stream without artificial lifetimes or register shims.

The remaining sixteen dialog instruction differences came from two inlined shared-owner resets. The pinned reading reference `KooShnoo/pokemon-rumble` commit `11c112f7548b26ab2cecfcf52e201b8d3b76120f`, blob `e57966d2bb1331d2b2b21137c9bc389e90c6c573`, retains the Freescale February 2006 smart-pointer implementation, with adaptations elsewhere in that header. Its `shared_ptr::swap` reuses one erased pointer temporary for both pointer exchanges. The previous reconstruction used two separately typed temporaries. Applying the evidenced single-temporary contract reproduces every deletion-dialog non-relocation instruction and all 222 direct call targets. The const-preserving pointer erasure also compiles for `shared_ptr<const int>`.

The initial same-base control at main `5133f82a` built all 759 actual Ninja source objects and preserved all 710 Matching objects. Of those source objects, 721 were raw identical, 36 differed only in compiler-generated numeric names, and two already NonMatching objects changed three functions. `InitSound` newly scores 100%; no formerly 100% function loses that score. All allocated linked ELF sections remain identical and the freshly linked DOL retains frozen SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. These controls must be repeated against the current merge base after rebasing.

Outside evidence is in `280-reference/primary-swap/`, including `consumer-proof.json`, `link-proof.json`, `whole-code-proof.json`, the original reference header and the const-owner compile consumer. The photo producer is still incomplete acceptance evidence: deletion-dialog exception data emits 2,032 bytes against 2,004 with unresolved references, and owned data placement remains unaccepted. Exact instruction bytes alone do not establish a reconstructed source unit or close this issue.

## F-280-4: Original C declarations resolve dialog exception ranges

The unadapted September 2006 CodeWarrior distribution provides `MSL_C/MSL_Common/Include/wstring.h`. Its declarations of `wcslen` and `wcsncpy` carry `_MSL_CANT_THROW`; `ansi_parms.h` defines that macro as `__attribute__((nothrow))` when throw specifications are enabled. Reading reference: [original archive](https://archive.org/download/cw_consoles/Wii/CW_GCN-3_0_Alpha5-2-060920.zip), archive SHA-256 `6bac434a2ff28f75c44fdd9606b3d58c3ff302b081a8717e137417af1c98a515`. This is distinct from the adapted GitHub header used for the earlier swap control.

The reconstructed C declarations lacked those attributes. The actual deletion-dialog producer consequently added two exception ranges around the wide copy and length calls while its formatted message was alive. The native table covers the containing range without that cleanup. Restoring precisely the two original declarations changes the outside dialog exception table from 2,032 to the native 2,004 bytes, with every non-relocation byte and every cleanup target exact. The 6,120 instruction bytes and all 222 direct call targets remain exact.

Two previously unnamed cleanup bindings are independently proved from the same actual producer: `shared_ptr<IDisconMsg::CDisable>` at `8008F210`, 132 bytes and three relocated call targets, and `Metrowerks::move_ptr<IDisconMsg::CDisable>` at `80090D20`, 136 bytes and two relocated call targets. Their complete instruction streams match before those identities are used to verify the dialog's six cleanup references. This is not a guessed binding based only on a convenient address.

At main `ad33af87`, the real all-source build compares 759 objects: 757 remain raw identical, including all 710 Matching objects. Only the exception sections of the existing NonMatching `CGameEnding` and `CNoticeDisplay` objects change; their function bodies and relocations remain identical. The full build exits zero, all linked allocated ELF sections and the DOL remain identical, and frozen verification passes. No formerly 100% function loses that score. Per-rebase controls must repeat these checks before landing.

Outside receipts are `280-reference/primary-wstring/{compile.json,whole-code-proof.json,eh-proof.json,discon-cleanup-proof.json,consumer-proof.json,link-proof.json}`. The Save and formatter bodies and the photo unit's complete owned data placement remain unaccepted, so this prerequisite does not close #280. A separate control using the complete original C++ headers currently stops at conflicting reconstructed exception declarations; no result is asserted for that mixed-header compilation.

## Traps

- An early typed formatter producer can expose the existing string lifetime/code-generation blocker before reconstructing two large filesystem routines. Preserve the positive callbacks and exact native retained-address site; do not repeat the prior allocator and constructor sweeps without new primary evidence.
- Exact function instructions can coexist with incorrect exception ranges when a reconstructed C declaration omits an original nothrow attribute. Check the primary declaration before changing game object lifetimes; the photo-dialog copy/length range demonstrates this distinction.

## F-280-5: Final screenshot function instructions

The current source reproduces all six target functions, including all 5,064 bytes of SaveScreenshotToRemoteMemory. Direct compilation and objdiff results are saved under ignored tools/local/session/280-ppu/cardflag2.json. Full source-link acceptance remains pending.

The missing exception-specification handler is immediately after the 4,096-byte stack allocation. An inline AllocatePhotoStack(size, alignment) throw() forwarder around GameMemory::Allocate reproduces the handler and the missing 24-byte frame record. This is an intentional D-0016 source-shaping helper, not proof of an original helper name. It supersedes the earlier hypothesis in sprint-t1 that an additional shared_ptr temporary must be responsible. Default construction plus reset and explicit copy-initialization probes did not reproduce the target.

The three free-space message blocks belong directly in the screenshot function. Expanding the prior GetPhotoSpaceMessage helper restores the order of all four-byte block-count locals and the three string temporaries relative to the final success-message string. It also removes twelve extra move-instruction bytes. Grouping bps*spc before multiplying the required clusters reproduces the target arithmetic instruction order.

Two prior source behaviors were incorrect: write-protection query failure and a protected card must proceed through the unmount/reset cleanup, rather than jump directly to detach; and the cleanup calls FAUnmount(drive, 0), not the undeclared-in-target FASync. The target call is the existing FAUnmount symbol at 0x8021DF40 with the public two-argument signature. Setting sCardPresent before clearing sCardChanged reproduces the final four register choices without a register shim.

## Additional traps

- A missing unexpected handler should be located relative to its protected call before adding an object lifetime. Here the target handler belongs immediately after allocation, and a throw() forwarding function reproduces it without an extra shared_ptr.
- Extracting repeated message blocks into an inline helper changes caller stack-object allocation order and can introduce pointer-result moves. Exact direct caller blocks were necessary for this function.

## F-280-6: Last source-link exception differences

The first complete native PhotoSave trial linked successfully and differed from the frozen executable in only four bytes. All instructions, function addresses, data, small data and section sizes were exact. The differing words were two screenshot exception-range descriptors: source ranges began at offsets 0xB88 and 0xD6C with length 14 instructions, while the target begins at 0xB90 and 0xD74 with length 12. The ranges end at identical locations.

The runtime searches these ranges using return addresses, not call addresses. The differing starts therefore identify the preceding snprintf calls, not the subsequent WaitFaMkdir and FAChdir calls. An initial investigation misread that boundary; its ineffective FA redeclarations were removed. A source-local first declaration of snprintf with __attribute__((nothrow)) changes exactly those four descriptor bytes, preserving all six function instructions. The declaration stays local until broader evidence supports changing the shared stdio interface. The direct comparison is tools/local/session/280-ppu/snprintfnothrow.json; final native verification is still required.


The first declaration matters to this compiler: adding a nothrow attribute after an existing declaration did not update the call contract. Also, exception ranges must be read using the return-PC lookup in Runtime/Gecko_ExceptionPPC.cpp; mapping a range start directly to a call instruction misidentifies its preceding protected call.

## Final acceptance

The coordinator retained PhotoSave as Matching after a fresh native ninja build exited 0 and frozen main.dol verification passed. All six original functions, exception records and owned data now link from C++ into the byte-identical executable. The local snprintf contract is the accepted exception fix; the rejected FA redeclarations are absent. Original hashes are unchanged.

The AllocatePhotoStack throw() forwarder remains an intentional D-0016 source-shaping helper. The direct caller message blocks, corrected unmount call and cleanup paths are supported by the target instructions.
