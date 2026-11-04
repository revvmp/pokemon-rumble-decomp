# F-53-2: CBattleRoyalDisplay delayed formatting emission

2026-11-04, target `wpse-v1`, Wii/1.1 with configured game flags. This continues the emission-order investigation in [sprint-e1](sprint-e1.md) and [sprint-a12](sprint-a12.md).

## Evidence and rejected approaches

The target places wide `FormatString` after the first group of template destructors and before the implicit display destructor. A normal strong definition emits too early; a simple inline definition follows `Calc`. Previous class-template friend experiments completed the class either too early or never requested the friend after its definition became available.

A delayed void template wrapper combined with an unreachable `wstring` object restores the exact function order, but passing pane names hoists address computations into an extra register. Boolean template pane selection avoids that instruction difference but emits format and pane strings too late, including duplicate format literals. Explicit destructor instantiation emits the destructor before the virtual method group. These candidates were not retained.

## Documented fake

The retained source keeps the original `Calc` formatting expressions. A new inline template, `PrepareBattleNumberFormat`, completes `BattleNumberFormatDefinition<T>` and returns the address of the existing wide `FormatString` overload. Its caller discards the result. Completing the class makes the unchanged formatting body available as an inline friend during template generation; returning its function pointer then requests that body for the next generation pass.

The helper and empty object have no runtime effect and optimize away. This is D-0016 source shaping to control CodeWarrior emission, not a claim about the original source design. No executable bytes or assembly are inserted.

## Validation

Scratch candidate `tools/local/session/battle-order/14.cpp` reproduces the target order of all 20 compared functions. Nineteen score 100%; `get_deleter` retains the baseline 99.73684% due only to its RTTI relocation name. The `.sdata` content through the target extent agrees before relocation, with additional weak ICalc RTTI expected to be discarded. The integrating agent subsequently completed the full source link (`ninja` exit 0), and the frozen verification against `config/targets/wpse-v1.json` passed. `main.dol` SHA-256 is `ad1a59896d3ade6b2ac35c24faa578fb3cdf8c2e2d3a9040ba8784cd033053da`. The weak helper body and extra RTTI are discarded as required. The unit is retained as `Matching`, with its code and data linked from source.

## Traps

Defining a friend through delayed class-template completion is insufficient by itself: the function must also be requested after completion. A discarded function-pointer return triggers that compiler request without adding runtime operations. Direct string-format wrappers can preserve function order while changing pane-address scheduling and literal-pool placement.
