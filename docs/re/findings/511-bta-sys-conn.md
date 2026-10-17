# F-511-1: BTA connection notification unit

## Scope and ownership

Target `wpse-v1`, immutable `orig/wpse-v1/main.dol`, SHA-256 `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. Issue #511 owns the eight retained functions of `bta_sys_conn.c`, `.text` `0x80314388–0x803145DC` (exclusive end), 596 bytes. DOL file offsets subtract `0x8004FD40` from these addresses. Function order, callback accesses and status constants agree with the Petari BTE reference at commit `e0ab141914921726a595573dc48d3f13d0922efe`.

The preceding code at `0x803142A0` is not the start of this unit: it calls an earlier timer-list routine; the next two functions copy and compare Bluetooth addresses. The three callback setters at `0x80314388`, `0x80314398`, `0x803143A8` establish the actual connection-unit start. The reference's application open/close and SCO-open helpers are absent from the linked target; no speculative source is added for them.

`bte_main.c` starts at `0x80313D00`, owned by assigned #510; its tail remains untouched. `bta_sys_main.c` starts at `0x803145DC`, owned by #511, and ends at `0x80314820`; the following `ptim.c` starts at `0x80314820` and continues into #512. Only the complete connection unit is split here. The remaining 17 issue functions stay in original fallbacks.

## Functions

| Address | Size | Recovered original name |
| --- | --- | --- |
| `0x80314388` | `0x10` | `bta_sys_rm_register` |
| `0x80314398` | `0x10` | `bta_sys_compress_register` |
| `0x803143A8` | `0x10` | `bta_sys_pm_register` |
| `0x803143B8` | `0xC4` | `bta_sys_conn_open` |
| `0x8031447C` | `0xC4` | `bta_sys_conn_close` |
| `0x80314540` | `0x34` | `bta_sys_sco_close` |
| `0x80314574` | `0x34` | `bta_sys_idle` |
| `0x803145A8` | `0x34` | `bta_sys_busy` |

Names are high confidence from reference function order and instruction correspondence. The last three use status values 5, 6 and 7, respectively. Open/close call resource-management, power-management and compression callbacks in that order when present.

## Data and interface

The only external relocation target is `bta_sys_cb`, `.bss` `0x804AA8D8`, structure size `0x8C` in an existing `0x90`-byte allocation including four trailing padding bytes, formerly `lbl_804AA8D8`. Its owning unit starts at `0x803145DC` within this claim. The initialization function clears exactly `0x8C` bytes and accesses the timer control block at `0x68`, task identifier at `0x7D`; connection functions access callback pointers at `0x80`, `0x84`, `0x88`. This rename leaves allocation and fallback data unchanged.

`include/RVL_SDK/bte/sys.h` describes the reference's complete control block: 26 registration pointers at `0x00`, timer state at `0x68` (queue first/last pointers and signed last-tick count, signed period, byte timer identifier), byte timer/task/event state at `0x7C/0x7D/0x7E`, and the three callbacks. Target evidence confirms the exercised offsets; unexercised registration and timer member meanings are reference-derived. `TIMER_LIST_ENT` remains opaque. Callback arguments are byte status, subsystem identifier and application identifier, followed by a Bluetooth address pointer.

## Compiler and validation

Wii/1.0 with `cflags_base` plus `-func_align 4` produces all eight functions at 100% objdiff, with the exact 596-byte text extent. This is a unit-specific BTE alignment result, not a replacement for the generic SDK default-alignment finding. The target function at `0x80314398` and every following packed start demonstrate why default 16-byte alignment cannot represent this unit. Objdiff alone does not establish linker identity; full linked-artifact verification is required.

The split-only `python configure.py`, `.venv/Scripts/ninja` and frozen-manifest verifier all exited 0 before source registration. Final source-linked and clean-build evidence is reported in the pull request and ledger.

## Provenance

Modified reference source and declarations derive from Petari's `bta_sys_conn.c`, `bta_sys_int.h`, `bta_sys.h` and `ptim.h`, with original dependency includes simplified, comments removed and only linked functions retained. These files carry Copyright (C) 2003-2012 Broadcom Corporation and changes by muff1n1634 under Apache-2.0. Applicable notices and full license are preserved by [issue 510's BTE notice](../licenses/510-bte-NOTICE.md) and [Apache-2.0 text](../licenses/510-apache-2.0.txt).

## Neighbor coordination

Issue #514 needs `bdcpy` (`0x803142B4`, size `0x34`) and `bta_sys_sendmsg` (`0x80314760`, size `0x1C`). Both lie in #511. The former copies six address bytes directly in reference order; the latter passes task byte at control-block offset `0x7D` and mailbox byte at configuration offset `2` to the GKI message sender. Their instruction correspondence to the same Petari reference supports these high-confidence names. They are renamed for the neighboring source API, but remain original fallback implementations.

## Traps

The generic RVL_SDK default alignment cannot reproduce the packed starts of `bta_sys_conn.c`. Wii/1.0 with `-func_align 4` produces all eight retained functions and passes the full artifact comparison. This is a unit-specific result; do not infer one BTE-wide compiler or alignment from it.
