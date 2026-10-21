# F-59-1: Complete CDisplayPhaseCtrl source

Issue #59 originally contains sixteen functions and 2,028 instruction bytes. Its first seven functions, `0x800709E0` through `0x80070BE0`, total 536 bytes and belong to the preceding CDisconMsg translation unit owned by #58. This change reconstructs the complete following CDisplayPhaseCtrl unit: nine functions and 1,492 instruction bytes. It does not claim completion of the seven preceding helpers.

RTTI establishes the original global class spelling CDisplayPhaseCtrl and its ICalc base. The complete class is 28 bytes: inherited virtual pointer at zero, phase at four, requested and forced visibility flags at eight/nine, layout pointer at twelve, pane pointer at sixteen, and shared motion owner at twenty/twenty-four. Member and nonvirtual method spellings are descriptive inferences. The three virtual slots are destructor, Calc and inherited CalcMtx. Native constructor stores, all member accesses and the native vtable establish the complete layout.

The nine entries are constructor `0x80070C20`/48, destructor `0x80070C50`/140, pane-target setter `0x80070CE0`/12, name-target setter `0x80070CF0`/64, force setter `0x80070D30`/8, Show `0x80070D40`/12, Hide `0x80070D50`/12, IsVisible `0x80070D60`/20, and Calc `0x80070D80`/1,176. The text split ends at the last function end `0x80071218`; the following alignment belongs to CDivided.

Calc uses the canonical GetCurrentGameMap/IsPausing interface proved by #56 and canonical CLayout motion and pane interfaces. The six-state controller starts a begin motion and queues loop, or starts end. A motion finishing while a reverse request is pending immediately starts the opposite transition. Private Begin and End operations are ordinary member definitions reused by request and completion dispatch. Unchanged file IPA inlines these operations, retaining native state switches even after known phase stores.

## Native acceptance

The canonical-object audit proves all 1,492 instruction bytes and 55 actual relocation targets against the frozen DOL. It compares branch destinations, register/opcode bits and semantic local literal contents. Three native exception records total 504 bytes; every byte, indexed function extent and destructor target matches. Eight functions score literal objdiff 100%; Calc's six local string-symbol spelling differences produce 99.89796%, while independent target and byte comparison is exact.

The source-selected Matching build passes configure, Ninja and frozen-manifest verification. Every allocated linked ELF section, including addresses, lengths, alignment and bytes, equals the original-selected baseline. The complete retained unit comprises text 1,528 bytes, extab 504, extabindex 36, data 56, sdata 32 and sdata2 eight. Ordinary linker removal discards unreferenced private helper bodies and duplicate earlier ICalc/shared-pointer definitions. No forced flags, section directives or explicit padding are used. The actual Ninja graph selects `src/game/CDisplayPhaseCtrl.o` and excludes `obj/game/CDisplayPhaseCtrl.o`. Linked method addresses and sizes are exact. All 719 declared source objects remain byte-identical across baseline and source-selected builds. Receipts and actual build logs are under `C:/MassStorage/GitHub/59-reference/`: `native-text-paired.json`, `native-eh-paired.json`, `linked-native-proof.json`, `all-source-proof.json` and `controls.log`.

## Traps

Directly duplicating and simplifying transition cases loses the original inlining context and produces a shorter Calc. Ordinary reusable private methods restore the native switches; grouped switch dispatch on the saved phase restores the final four bytes. Explicitly marking helpers inline preserves instructions but changes retained literal emission order. Ordinary member definitions reproduce begin/loop/end before RTTI. This is evidence for this unit, not a general source-order rule.

The issue's first seven helpers remain #58's source authority. Its independent complete-code proof at `C:/MassStorage/GitHub/58-reference/58-run-context/58-complete-code-proof.json` establishes the following inventory, but whole-unit source selection and retained layout remain pending:

| Address | Bytes | Native identity |
| --- | --- | --- |
| `0x800709E0` | 76 | CFiber shared deleter get_deleter |
| `0x80070A30` | 36 | CFiber shared deleter dispose |
| `0x80070A60` | 116 | move_ptr<CLayout> destructor |
| `0x80070AE0` | 116 | move_ptr<CFiber> destructor |
| `0x80070B60` | 64 | CFiber deleter destructor |
| `0x80070BA0` | 64 | CLayout deleter destructor |
| `0x80070BE0` | 64 | bound-function function_imp destructor |

These seven functions total 536 bytes; together with this unit the original denominator is sixteen functions and 2,028 bytes. CDisplayPhaseCtrl alone supplies nine of sixteen original functions and must not close #59 before the preceding 536 bytes also pass source acceptance. Prerequisite [F-59-2](59-icalc-inheritance.md) reconciles the inherited ICalc matrix-slot identity against all current consumers before this unit's final acceptance.
