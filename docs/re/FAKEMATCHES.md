# Fake-match ledger

Functions matched with source that is probably not the original, under [D-0016](../DECISIONS.md#d-0016-documented-fake-matches-for-the-remaining-long-tail). Replace an entry with plausible source when one is found, then remove its row.

| Unit | Function | Fake construct | Honest attempts |
| --- | --- | --- | --- |
| CDlgPiiList | `.data` order of the `shared_ptr_deleter<CUIManager>` vtable | `typedef char UIManagerPtrStorage[sizeof(std::tr1::shared_ptr<CUIManager>)];` before `PiiItemVector`, which completes `shared_ptr<CUIManager>` before the `mItems` vector | [sprint-ida](findings/sprint-ida.md#fixes-from-the-follow-up-session), [sprint-a17](findings/sprint-a17.md#cdlgpiilist-flip-attempt) |
| CDlgPiiList | `CDlgPiiListImpl::OnAccept` (inlined `CountRemoteUnmarked`) | dead `if (false)` block in `CountRemoteUnmarked` that dereferences and increments a null `const_iterator*`, so `*` and `++` are referenced before `begin` | [sprint-ida](findings/sprint-ida.md#fixes-from-the-follow-up-session), [sprint-a17](findings/sprint-a17.md#cdlgpiilist-flip-attempt) |
| ScriptGame | `MakeMapID` | `#pragma opt_dead_assignments off` around the function | [sprint-a13](findings/sprint-a13.md), [sprint-ra](findings/sprint-ra.md#diagnosed-not-fixed) |
| ScriptGame | `IncMapClearedNum` | `#pragma opt_common_subs off` around the function | [sprint-a03](findings/sprint-a03.md), [sprint-a05](findings/sprint-a05.md) |
| main | `HandleFileError` (inlined `UsesAlternateErrorMessage`) | `int number = drive;` copy so `drive < 2 \|\| number >= 512` is not folded into one range compare | [sprint-a12](findings/sprint-a12.md#unsolved) |
| main | `main` (call to `ResourceFileConfig::SetErrorHandler`) | `__declspec(noinline)` on `SetErrorHandler` | [sprint-a12](findings/sprint-a12.md) |
