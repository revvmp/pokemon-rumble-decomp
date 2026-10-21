# CComboDisplay.cpp and shared layout contracts (#56)

## Scope and current status

The original issue contains eighteen functions, 2,108 instruction bytes, at `0x8006DDE0` through `0x8006E6A0`. RTTI identifies the private implementation as `@unnamed@CComboDisplay_cpp@::CComboDisplay` and the public interface as `IComboDisplay`. The source remains outside the repository pending complete interface, data, source-link, and frozen-DOL acceptance. This prerequisite changes shared declarations only and earns no reconstructed-source or completed-issue credit.

An independent scratch producer using the actual Ninja game producer's Wii/1.1 options reproduces all eighteen bodies, every resolved instruction relocation target, and all eleven owned exception records, 616 bytes including indexed code lengths. These are preflight results, not whole-unit acceptance. Outside evidence is `56-reference/56-preflight.json` and `56-reference/56-eh-proof.json`.

## Shared motion and text contracts

| Address | Declaration or behavior | Native evidence |
| --- | --- | --- |
| `0x801D0890` | `shared_ptr<CLayout::CMotionProgram> CLayout::CreateMotion(const char*, Pane*, f32, f32)` | Hidden result, layout, animation name and pane use `r3` through `r6`; frame and rate use `f1` and `f2`. It calls `0x801D1FE0`, then `0x801D0970` when an animation exists. |
| `0x801D0D00` | Named-pane `CreateMotion` overload | #58 independently traces pane-name lookup followed by the raw-pane overload. |
| `0x801D0DB0` | `QueueMotion` with the same argument types | #58 independently traces construction through `0x801D1FE0`, insertion after the first program, removal of subsequent queued programs, and immediate start when the list is empty. |
| `0x801D1040` | Named-pane `QueueMotion` overload | #115 independently traces pane-name lookup followed by `0x801D0DB0`. |
| `0x801CB140` | `void CMotionProgram::Wait()` | #58 independently traces the raw `this` receiver, repeated state-3 test and `YieldFiber`; the existing `IsFinished` predicate also accepts state 2 and remains unchanged. |
| `0x801D1730` | `u16 CLayout::GetMotionLength(const char*)` | Animation lookup followed by SDK `AnimTransform::GetFrameSize()`, whose canonical return is `u16`; the combo factory zero-extends sixteen bits. |
| `0x801D60B0` | `TextBox* CLayout::SetText(const char*, const wchar_t*)` | Calls `strlen` for the pane name, `wcslen` for the text, checks TextBox RTTI, updates text buffers, and returns the last affected text box. |
| `0x8006BD60` | Wide `FormatString` overload returning `std::wstring` | Initializes the twelve-byte wide string, prepares variadic arguments and calls `0x801F5BB0`; the combo caller passes UTF-16 `L"%d"` and a count. |

The motion constructor at `0x801D1FE0` stores the return of `0x801D2180` at program offset `0x0C`. That routine looks up the `map<Pane*, shared_ptr<PaneMP>>`, defaults a null input to the SDK root pane, and returns the mapped `PaneMP*`. The previous `Pane*` member interpretation was incorrect. Changing the member type to `PaneMP*` preserves the 24-byte program layout. Its state at `0x14` is exactly 3 in the combo transition predicate, represented by the additive `IsEnded()` method.

Method spellings for these previously unnamed routines are descriptive inferences. Native argument, result, storage, and ownership evidence determines their contracts.

## Shared layout resources

`GetDefaultFontMap()` at `0x80126280` locks the weak FontMap owner at `0x804B9090`, returning `shared_ptr<CLayout::FontMap>`. The lock may return empty; it is not an unconditional empty-font factory.

`gSharedLayoutArchives` at `0x804B9098` is a pointer to a null-terminated array of constant archive-name pointers. `0x801CABD0` traverses it, and `0x80127CF0` assigns the first mapped value's archive array at offset 20. Only the first word of the existing eight-byte placeholder is proven to be this pointer. This prerequisite neither renames nor resizes that outside symbol; its second word remains unproven. The resource function, variable and header names are descriptive inferences coordinated with #55 and #58.

## Traps

- A motion's input pane and stored target have different types. `0x801D2180` accepts `Pane*` but returns the mapped `PaneMP*`; the unchanged pointer width conceals an incorrect declaration from layout checks.
- A matching function body does not establish a complete unit. The combo scratch has exact code and exception records while its full IGameMap interface and default-pane storage boundary still require independent acceptance.
- An eight-byte small-BSS placeholder can include alignment. A raw static pane pointer emits four bytes at eight-byte section alignment and preserves the native load without a constructor; a shared owner instead introduces initialization and destructor-registration code absent from this unit. The raw-pointer hypothesis still needs the complete source-link boundary check before adoption.
