# Sprint a06 findings

## Traps

- A load through a `const` member function is not value-numbered against a later non-const read-modify-write of the same field. `CPiiPersonalData::IsSpecHidden() const` followed by `SetNewlyDisclosed(true)` reloaded the flags word; a non-const `IsSpecHidden()` overload made `CDlgPiiListImpl::OnSelectionChanged` 100%. Keep the const overload for const callers and cast to const where a matched unit needs the const load (`ScriptPiiProperties.cpp`).
- `#pragma inline_depth`, `inline_max_size`, `inline_max_total_size` and `inline_bottom_up` have no effect with `-ipa file`; `auto_inline off` and `dont_inline` do. Inlining into `CDlgPiiListImpl::Show` and `Run` is budgeted per caller, so adding code to a caller can silently stop inlining of an earlier call site.
- Defining `CDlgPiiListImpl::AppendItems` (called twice from `CDlgPiiList::Show`) dropped every other inline in Show (Show fell from 69% to 27%, ctor and small accessors all became calls). `#pragma auto_inline off` around the AppendItems definition restores Show and keeps AppendItems at 99.78%. Moving the Impl constructor in-class, or adding more statements to Show, flips the same all-or-nothing switch; the budget is per caller, not per pragma.
