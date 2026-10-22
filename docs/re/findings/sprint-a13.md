# Sprint a13 findings

## Traps

- **Class-by-value parameters are passed as pointers to caller-made copies.** `CPostEffect(bool, shared_ptr<CFileData>)` and `BindCaptureTexture(shared_ptr<CModel>)` take their shared_ptr by value. A `const shared_ptr<T>&` parameter produced a different stack frame and temp lifetime (Create 78% to 100%, Show frame 0x20 too small). When a caller shows an extra shared_ptr copy in a temp slot and the callee never releases its argument, the parameter is by value.
- **Weak template bodies bloat the split object's `.data`/`.sdata`.** Making CSaveManager `Matching` shifted the DOL header (byte 0x37) because the object carries weak `std::__cdeque_buf<void*>` bodies, their strings and RTTI that the original object referenced as externals. Name every shared RTTI/vtable (`__RTTI__Q23std11logic_error`, `__vt__Q23std9exception`, `__vt__5ICalc`, ...) in `symbols.txt` first; sizes then converge. A unit with only a fully-owned template set (CPostEffect) linked to identical section sizes with every function not yet exact, so the bloat is specific to deque users.
- **Auto-naming template placeholders by size.** `tools` scratch script idea: for each `fn_`/`dtor_` target symbol, list source-object symbols of the same size (`powerpc-eabi-nm -S`), try each rename and keep the one that reaches 100% in unitdiff. This named 20 CPpuEngine template functions in one pass.
- **`neg/or/srwi` after a call means `bool(x)` of an int.** `result.ToInt() != 0` returned as bool materialises this; use it to tell bool-returning helpers apart.
- **`LoadLocalizedFile`'s third parameter is `u32`, not `bool`.** The 8-byte inline `LoadResourceFile(path, flags, mode, region)` forwards `mode` unchanged; naming the callee `...UlbPCc` made callers insert a bool conversion.
- **CW converts `x / 2.0f` to a different instruction order than `x * 0.5f`.** `halfRange = (far - near) / 2.0f` matched CShadowCamera; `* 0.5f` and `0.5f *` both swapped operands of `fmuls`.
- **`top *= nearClip` vs `top = tan * nearClip`** differ in operand order of `fmuls`; the compound assignment keeps the left operand first.
