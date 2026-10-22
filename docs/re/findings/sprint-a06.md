# Sprint a06 findings

## Traps

- A load through a `const` member function is not value-numbered against a later non-const read-modify-write of the same field. `CPiiPersonalData::IsSpecHidden() const` followed by `SetNewlyDisclosed(true)` reloaded the flags word; a non-const `IsSpecHidden()` overload made `CDlgPiiListImpl::OnSelectionChanged` 100%. Keep the const overload for const callers and cast to const where a matched unit needs the const load (`ScriptPiiProperties.cpp`).
- `#pragma inline_depth`, `inline_max_size`, `inline_max_total_size` and `inline_bottom_up` have no effect with `-ipa file`; `auto_inline off` and `dont_inline` do. Inlining into `CDlgPiiListImpl::Show` and `Run` is budgeted per caller, so adding code to a caller can silently stop inlining of an earlier call site.
