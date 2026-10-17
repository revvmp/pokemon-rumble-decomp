# Issue 470: OS state, records, networking, UTF, and play time

## F-470-3: Complete issue coverage and source acceptance

Target `wpse-v1`, original `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. All 39 issue functions, totaling 8,252 function bytes, are source-linked: the existing OSIpc unit supplies three functions, the OSTime unit accepted by #469 supplies four, and this contribution supplies the remaining 32. Completing OSPlayTime also reconstructs seven neighboring #471 functions. This contribution adds 39 functions and 8,484 function bytes in seven units; it does not close #471.

| Unit | Functions | Function bytes | Sections |
| --- | --- | --- | --- |
| OSUtf.c | 4 | 576 | `.text` `802BD730–802BD980`; `.data` `803C8D70–803D4BB0` |
| OSStateTM.c | 11 | 1,724 | `.text` `802BD9C0–802BE0C4`; `.data` `803D4BB0–803D4C78`; `.bss` `80453340–80453400`; `.sbss` `804B9518–804B9538` |
| OSPlayRecord.c | 4 | 1,792 | `.text` `802BE0D0–802BE7EC`; `.data` `803D4C78–803D4CC0`; `.bss` `80453400–804535F8`; `.sdata` `804B8AA8–804B8AAC`; `.sbss` `804B9540–804B9560` |
| OSStateFlags.c | 2 | 524 | `.text` `802BE7F0–802BEA08`; `.data` `803D4CC0–803D4CE8`; `.bss` `80453600–80453620` |
| OSNet.c | 8 | 1,380 | `.text` `802BEA10–802BEFAC`; `.data` `803D4CE8–803D4E50`; `.bss` `80453620–804536E0`; `.sdata` `804B8AB0–804B8AB4`; `.sbss` `804B9560–804B9570` |
| OSNandbootInfo.c | 2 | 520 | `.text` `802BEFB0–802BF1B8`; `.data` `803D4E50–803D4E70` |
| OSPlayTime.c | 8 | 1,968 | `.text` `802BF1C0–802BF998`; `.data` `803D4E70–803D4EB0`; `.bss` `804536E0–80453710`; `.sdata` `804B8AB8–804B8AC0`; `.sbss` `804B9570–804B9588`; `.sdata2` `804BB278–804BB288` |

Source provenance: [Petari OS sources](https://github.com/SMGCommunity/Petari/tree/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/os) at `e0ab141914921726a595573dc48d3f13d0922efe`, CC0-1.0, without file-level license exceptions. The network resource-manager helpers, RTC synchronization, and shutdown state machine are adapted from that revision's [NWC24Ipc.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/nwc24/NWC24Ipc.c), [NWC24Time.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/nwc24/NWC24Time.c), and [NWC24System.c](https://github.com/SMGCommunity/Petari/blob/e0ab141914921726a595573dc48d3f13d0922efe/src/RVL_SDK/nwc24/NWC24System.c), corroborated and simplified against this target's complete instruction and data streams. This is the smaller networking implementation linked with OSNet, not the full NWC24 library. [ogws OSNet.c](https://github.com/doldecomp/ogws/blob/f5a1bb5ed7692a9e09e737bc1236f592274dcf73/src/revolution/OS/OSNet.c), CC0-1.0, independently corroborates the same retained diagnostic-name strings; no code from it is copied into this contribution.

### Target-specific behavior

Wii/1.0 with existing cflags_sdk reproduces every unit. State-manager callback setters use the reference's null/default callback behavior: passing NULL selects the empty SDK callback, and a previous default callback is returned to callers as NULL. OSSetResetCallback precedes OSSetPowerCallback. Their pointer accesses, the initialization stores, and default callback addresses resolve the otherwise identical masked instruction patterns. Diagnostic lines are 348 for shutdown, 412 for hot reset, and 820 for the state-event handler. These follow the reference's alternate callback-handling branch with later line values; they do not identify this game's region or revision as that reference release.

OSNet contains the smaller resource-manager implementation. It registers shutdown priority 110, opens /dev/net/kd/request, starts with five shutdown retries, and returns positive one when preparation succeeds. The initial OS network routine consequently suspends the scheduler on that positive result, as observed. Scheduler and RTC requests reject a missing current thread; IOS open -6 maps to -29, other open/IOCTL/close failures to -42. Shutdown is asynchronous, retries failed requests, and reports its give-up string. RTC synchronization subtracts SC counter bias from elapsed time-base seconds. The scheduler and RTC close-result precedence is preserved exactly rather than normalized to a newer implementation.

Play-time expiration uses the existing __OSExpireCallback function pointer, not the reference's alternate expire-thread mechanism. A present callback is invoked; otherwise every active thread is suspended and a reboot thread is created. The timer is armed at limit plus twenty seconds, versus one second in the reference. Its panic line is 0x2E1. The public __OSWriteExpiredFlagIfSet entry remains emitted while its call is inlined in the reboot thread. UTF tables and conversion behavior are unchanged from the reference and match all 48,704 data bytes, including the real page-pointer table.

### Data and validation

Unreferenced networking diagnostic tags are real retained target data. They are represented as force-active local character arrays alongside their corresponding routines; the unused resume tag is retained with the scheduler tags. Keeping them explicit preserves the original literal pool without adding executable payloads. Resource-manager buffers and zero-initialized state have explicit static storage in target order. Narrow string and scalar boundaries are annotated, and UTF halfword maps are marked noreloc so pointer-like pairs remain character data; UcsSjisTable retains actual pointer relocations.

All seven new units are Matching and linked from source. Focused objdiff-cli 3.8.2 comparisons show every code/data section at 100%, including the existing OSTime and OSIpc units. The split-only and source-linked DOL builds pass the configured SHA-1 check; the source-linked DOL passes the frozen SHA-256 manifest and direct byte comparison. Counts exclude dtk gap symbols. No original bytes or expected hashes are changed.

Commands: python configure.py --no-progress; .venv/Scripts/ninja all_source; .venv/Scripts/ninja; objdiff-cli diff for the nine relevant units; python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1. All exit zero. Final integration repeats these on the exact commit merged after rebasing onto latest main, using a fresh ignored build directory after shared configuration conflicts.

## Traps

- Optimized-away resource-manager user arguments also discard their diagnostic literals. Force-active local tag arrays preserve the observed strings and their placement without dummy instructions or incomplete unused functions. Replacing all literals with individually named global arrays changed address-load scheduling in preparation; local tags retain both code and data matches.
- Zero-initialized resource-manager buffers and state must retain target order. Uninitialized function-local objects were allocated in a different order; explicit static zero-initialized storage reproduces all section bases and accesses. This is a confirmed storage-layout requirement for this unit, not a project-wide ordering rule.
- Cutting a data split halfway through dtk's trailing gap made objdiff reject a gap symbol extending beyond its section. OSNet data ends at 803D4E50 and OSNandbootInfo data at 803D4E70, retaining their complete alignment gaps. The split-only DOL then passes.
