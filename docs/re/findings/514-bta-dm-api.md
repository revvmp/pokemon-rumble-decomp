# F-514-1: Bluetooth device manager public API

## Boundaries and recovered names

Target `wpse-v1`, immutable `main.dol` SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. GNU binutils 2.42-2 disassembly of the verified ELF and Petari commit `e0ab141914921726a595573dc48d3f13d0922efe` establish `bta_dm_api.c` at `.text` `0x80316D80â€“0x803171EC`, with registration records at `.sdata2` `0x804BB600â€“0x804BB610`. Ends are exclusive. Subtract `0x8004FD40` from text addresses for DOL file offsets.

The source unit starts inside issue #514. The preceding functions are the end of `bta_dm_act.c`, whose first function belongs to assigned issue #512; they remain untouched. The following two dispatchers are `bta_dm_main.c` and remain fallback. This avoids claiming a false unit boundary at an issue boundary.

| Address | Size | Original dtk label | Recovered name | Source code score |
| --- | --- | --- | --- | --- |
| 0x80316D80 | 0x88 | fn_80316D80 | BTA_EnableBluetooth | Not implemented |
| 0x80316E08 | 0x38 | fn_80316E08 | BTA_DisableBluetooth | 100% |
| 0x80316E40 | 0x38 | fn_80316E40 | BTA_DmIsDeviceUp | 100% |
| 0x80316E78 | 0x64 | fn_80316E78 | BTA_DmSetDeviceName | 92% |
| 0x80316EDC | 0x58 | fn_80316EDC | BTA_DmSetVisibility | 100% |
| 0x80316F34 | 0x84 | fn_80316F34 | BTA_DmSearch | 93.93939% |
| 0x80316FB8 | 0x38 | fn_80316FB8 | BTA_DmSearchCancel | 100% |
| 0x80316FF0 | 0x8C | fn_80316FF0 | BTA_DmPinReply | 94.28571% |
| 0x8031707C | 0xF0 | fn_8031707C | BTA_DmAddDevice | 100% |
| 0x8031716C | 0x48 | fn_8031716C | BTA_DmRemoveDevice | 100% |
| 0x803171B4 | 0x38 | fn_803171B4 | BTA_DmSendHciReset | 100% |

Names have high confidence from target behavior, event numbers, allocation sizes, call structure and reference sequence. The target omits the reference's discover, bond, authorization, signal-strength and keep-ACL wrappers. Do not restore those APIs merely to follow a newer reference.

## Interfaces and data

The existing eight-byte `HC_BT_HDR` is reused. The name message is 40 bytes, with 32 name bytes at offset 8. Visibility stores byte-sized modes at offsets 8 and 9 but allocates 272 bytes, the target's complete message-union size. Search allocates 28 bytes, copies ten inquiry bytes at offset 8, and stores services and callback at offsets 20 and 24. PIN reply allocates 32 bytes, stores the six-byte address at 8, accept at 14, length at 15 and sixteen PIN bytes at 16. Header types and field ordering follow the reference; inquiry internals and callback payload types are reference-derived, since the target wrapper only copies their storage. This is a private API header, not a finalized shared BTE ABI.

The two registration records each contain a callback and a null disable pointer. Their callback targets are `bta_dm_sm_execute` at 0x803171EC and `bta_dm_search_sm_execute` at 0x80317284. No source registration data is emitted yet. `BTA_EnableBluetooth` clears 260 bytes of `bta_dm_cb` at 0x804AAA14; its complete layout belongs with the device-manager implementation and is intentionally not replaced by a byte-array placeholder.

External identities needed before linking this source, without out-of-region renames:

| Address | Recovered name |
| --- | --- |
| 0x80310ED0 | GKI_getbuf |
| 0x80312560 | GKI_sched_lock |
| 0x80312570 | GKI_sched_unlock |
| 0x803142B4 | bdcpy |
| 0x80314760 | bta_sys_sendmsg |
| 0x8031C824 | BTM_IsDeviceUp |
| 0x8031BEC4 | BTM_SecAddDevice |
| 0x8031C04C | BTM_SecDeleteDevice |
| 0x80345D20 | strncpy |
| 0x80397F18 | bta_service_id_to_btm_srv_id_lkup_tbl |

## Build evidence and remaining work

Wii/1.0 with base flags and `-func_align 4` reproduces the packed function extents and seven function bodies (624 code bytes) at 100% objdiff score. Ten functions, totaling 996 bytes, are implemented. Three contain a scheduling difference: the target moves an argument into r4 before storing the event halfword, while this source schedules that move after the store. Direct Wii/1.1 and Wii/1.0 `-ipa file` probes did not resolve this. These probes do not establish a unique original compiler. Wii/1.0 remains the documented SDK candidate; alignment is a unit-specific observed requirement.

The complete 1,132-byte unit remains `NonMatching`. `python configure.py`, `.venv/Scripts/ninja` and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit 0. A separate split-only build also passes verification. These comparisons prove preservation of the whole DOL while the original fallback remains linked, not source-link acceptance or issue completion.

Next: coordinate the complete control-block layout with #512, implement the initializer and registration records, resolve the three scheduler differences, coordinate external symbol names with their owners, then attempt source linkage and full verification. Other functions owned by #514 remain unreconstructed. No new reverse engineering tooling trap emerged; the known issue-boundary versus source-boundary distinction was confirmed. Provenance and Apache-2.0 notices are preserved in [the BTE notice](../licenses/514-bte-NOTICE.md).

## F-514-2: Partial-work issue closure keywords

GitHub recognized a closing-keyword phrase in the description of [pull request 558](https://github.com/revvmp/pokemon-rumble-decomp/pull/558), despite the sentence negating the action and the description beginning with a partial-work reference. When that pull request merged on 2026-10-17 at 09:40:26 UTC, it automatically closed [issue 494](https://github.com/revvmp/pokemon-rumble-decomp/issues/494). The issue's ClosedEvent identified pull request 558 as the closer. Independent issue-events API inspection confirmed the closure timestamp and reopening at 09:41:08 UTC. The agent removed the phrase and reopened the unfinished issue.

This contribution also leaves issue 514 incomplete. Use a simple `Part of` reference and describe remaining work without placing a closing keyword immediately before an issue reference anywhere in a partial-work pull request description. Negating the sentence does not disable GitHub's keyword matching. Check issue state after merging partial work. This is a confirmed integration-workflow observation, not a reverse engineering discovery; it changes no binary matching requirements.
