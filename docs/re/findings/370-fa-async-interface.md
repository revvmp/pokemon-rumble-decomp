# F-370-1: Native FA asynchronous public interface

Issue #370 was open/unassigned and claimed through the helper into an isolated worktree from actual main `813a7c745d081a82bf86e3ef76d7fbd54fc02e68`. Its complete original inventory is 28 functions/2,916 retained instruction bytes. This bounded prerequisite publishes only the independently authored public ABI and ten APIs consumed by #314. It introduces no source body, object registration, split, symbol extent change, production storage, or code-byte credit.

## Public command work area

The public `FACommandBlock` is an opaque SDK work-area contract: a user pointer, 70 unsigned32-bit work words, and a NAND information record. The latter has 78 path bytes and one32-bit size field. Natural alignment places async work at 4, NAND info at `0x11C`, its size at `0x16C`, and the complete command at `0x170` bytes with alignment 4. Internal operation fields are not exposed, and no volatile qualifier is inferred from this public array.

This resolves the earlier unknown middle range without manufacturing semantic fields or padding. Actual `FAAttachAsync` at `0x8021D6C0` computes its NAND path address with immediate`0x11C` at `0x8021D788`, copies exactly78 bytes, and stores the NAND size at `0x8021D794` using displacement`0x16C`. Five independent native async stubs advance the public pointer by4 before interpreting private command storage. The callback receives the signed result and the original public command pointer. Public file, directory-search and driver-table handles use forward aliases to the existing canonical PF tags; they create no duplicate storage layouts.

The original work-area/type/API names and scalar array extent are corroborated by the public [FA type interface](https://github.com/galaxymaster2007/THPConv/blob/e7f2a17eeff43bc6b822a01af66ce670822ab77b/include/revolution/fa/fa_types.h) and [asynchronous API declarations](https://github.com/galaxymaster2007/THPConv/blob/e7f2a17eeff43bc6b822a01af66ce670822ab77b/include/revolution/fa/fa_api_async.h), pinned to `e7f2a17eeff43bc6b822a01af66ce670822ab77b`. That repository has no permissive root license, and those headers carry a restrictive Nintendo notice. No restricted header or implementation is imported, copied into this repository or used as a source implementation. Only factual interface names, types and work-area extent inform this original bounded declaration, independently checked against actual native accesses and consumers. Related CC0 IPL headers predate this async interface and cannot establish its layout or Async function identities.

## Native callers and consumer proof

| Native API address | Public declaration | Caller contract |
| --- | --- | --- |
| `8021D6C0` | `FAAttachAsync` | Four logical arguments, first u8, then callback and public block |
| `8021DC90` | `FAMountAsync` | s8 drive, callback, block |
| `8021DF90` | `FAUnmountAsync` | s8 drive/u32 mode, callback, block; #371 callee declaration only |
| `8021D880` | `FACreateAsync` | path/mode/file-output, callback, block |
| `8021DAA0` | `FAFwriteAsync` | buffer/size/count/file/count-output, callback, block |
| `8021D9F0` | `FAFsfirstAsync` | path/u8 attributes/search-output, callback, block |
| `8021DA50` | `FAFsnextAsync` | search record, callback, block |
| `8021DE60` | `FARemoveAsync` | path, callback, block; #371 declaration only |
| `8021DC40` | `FAMkdirAsync` | path, callback, block |
| `8021DEB0` | `FARmdirAsync` | path, callback, block; #371 declaration only |

The #314 owner independently compiled its actual 11 C++ functions against the exact proposed header, normalized SHA-256 `56880ea4bf1b63f56b9a93d625b2d664cc02d0ad20145da4fd3fb1536bf76e86`. All 2,308 native instruction bytes, 115 text relocation targets,760 exception-record bytes, 120 exception-index bytes and 40 exception/index targets match. Those functions include the callback plus the ten real call sites, with semaphore at outer `0x170` and result at `0x17C`. The actual proof is `314-reference/fa-wait.cpp`, its real compile-command JSON and strict`314-audit-fa.py`; no dummy forwarding consumer substitutes for this proof. Header ownership and its bounded reuse were coordinated before publication.

`370-public-layout-probe.c` includes the actual canonical proposal and existing SDK semaphore type. `370-audit-public-layout.py` checks 12 emitted size/offset measurements: command 170, NAND record54, user0, work4, NAND11C, path0 within NAND, size50 within NAND, complete waiter180, semaphore170, result17C, enclosing alignment record174 and member offset 4. It additionally checks the actual immutable-DOL instructions for the NAND tail and five async-prefix accesses. The global diagnostic BSS has exactly170 bytes. These are outside-repository diagnostics, not production data or source credit.

## Original issue inventory and ownership

| Original address | Original extent | Current evidence |
| --- | --- | --- |
| `0x8021D200` | `0x4C` | External exact native probe only |
| `0x8021D250` | `0x3C` | External exact native probe only |
| `0x8021D290` | `0x3C` | External exact native probe only |
| `0x8021D2D0` | `0x4C` | External exact native probe only |
| `0x8021D320` | `0x3C` | External exact native probe only |
| `0x8021D360` | `0x3C` | External exact native probe only |
| `0x8021D3A0` | `0x50` | External exact native probe only |
| `0x8021D3F0` | `0x50` | External exact native probe only |
| `0x8021D440` | `0x3C` | External exact native probe only |
| `0x8021D480` | `0x240` | External exact native probe only |
| `0x8021D6C0` | `0x160` | Native API reconstruction pending |
| `0x8021D820` | `0x30` | Native API reconstruction pending |
| `0x8021D850` | `0x2C` | Native API reconstruction pending |
| `0x8021D880` | `0x4C` | Native API reconstruction pending |
| `0x8021D8D0` | `0xA8` | Native API reconstruction pending |
| `0x8021D980` | `0x40` | Native API reconstruction pending |
| `0x8021D9C0` | `0x2C` | Native API reconstruction pending |
| `0x8021D9F0` | `0x54` | Native API reconstruction pending |
| `0x8021DA50` | `0x4C` | Native API reconstruction pending |
| `0x8021DAA0` | `0x54` | Native API reconstruction pending |
| `0x8021DB00` | `0x30` | Native API reconstruction pending |
| `0x8021DB30` | `0xD0` | Native API reconstruction pending |
| `0x8021DC00` | `0x38` | Native API reconstruction pending |
| `0x8021DC40` | `0x4C` | Native API reconstruction pending |
| `0x8021DC90` | `0x4C` | Native API reconstruction pending |
| `0x8021DCE0` | `0x9C` | Native API reconstruction pending |
| `0x8021DD80` | `0x20` | Native API reconstruction pending |
| `0x8021DDA0` | `0x24` | Native API reconstruction pending |

The first nine independent public stubs reproduce 612 native bytes and all nine independently resolved calls under the established Wii/1.0 SDK configuration with an ordinary private operation field. The Unicode dispatcher reproduces all 576 native bytes and its 96-byte jump table with every actual target independently resolved to the owned wide APIs. These are external diagnostics only; no source is selected by this header prerequisite. Several newly asynchronous stub names are descriptive, not inferred from the obsolete IPL sequence. Actual command IDs and lower dispatch targets establish their identities.

The first-owned FA API source begins at `0x8021D6C0` and may extend through the eight preceding #371 functions before the kernel adapter. #371 belongs to an external assigned contributor. Its bodies, splits and code credit are preserved; any eventual whole-unit source acceptance requires coordination through the PR. Only bounded outside callee declarations needed by actual clients appear here. This issue remains incomplete after the header merges.

## Validation state

The unchanged-main full baseline completed on clean `813a7c745d081a82bf86e3ef76d7fbd54fc02e68` with the new header physically absent. Configure/map, plain Ninja and explicit frozen verification each exited 0. `370-fingerprint-baseline.py` captured SHA-256 fingerprints of all 535 actual source objects on that clean head. The bounded header/docs are now ready for their candidate gate: repeat configure/plain Ninja/explicit verification, compare every whole source-object fingerprint and repeat the actual layout/native-consumer proof. The final exact latest-main gate remains pending; historical and external probes alone do not authorize a merge.

Candidate committed head `2a50b805` passed configure/map, plain Ninja and explicit frozen verification, each exiting 0. All 535 actual whole source-object SHA-256 fingerprints equal the clean unchanged-main baseline. The actual canonical header was placed first in the real #314 consumer's include search, with a separate output owned by this investigation: `370-repeat-actual-game-consumer.py` again passes all 11 functions/2,308 native bytes, 115 independently paired text targets, 760 exception bytes, 120 index bytes and 40 paired exception/index targets. This uses the tracked canonical header directly, rather than the earlier approved copy. Recompiled actual-header layout/native-access diagnostics also exit 0 through `370-audit-public-layout.py`. No production source bytes or runtime storage are added. Final exact latest-main integration and guarded merge remain pending.

## Traps

- A related synchronous SDK map can assign the right public spelling to the wrong asynchronous ABI. Check actual callback/work-pointer argument positions and private command dispatch before selecting a name or signature.
- The unused middle of a public SDK work-area record is not evidence for invented semantic members. An independently documented opaque array contract plus native tail offsets establishes storage without exposing private fields or implying an internal volatile qualifier.
- A publicly readable SDK header can still carry a restrictive file-level notice. Inspect licensing before reuse; preserve only factual ABI evidence and independently authored declarations here, with no restricted implementation import.
- A diagnostic object's section alignment does not establish record alignment. The compiler gives this standalone BSS section alignment 8; embedding the command after one byte places it at 4, proving the actual public record alignment 4 independently.
