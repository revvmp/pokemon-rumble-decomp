
## CNoticeDisplay (issues 160-166)

### Traps

- Placeholder `fn_`/`dtor_` template instantiations are not renamed by name_callees.py when they are reached through vtables. Map them by size, instruction shape and the vtable contents in `auto_07_80399380_data.s`; the data symbols name the owning class order. In this unit function_imp order is CLose, CFight, CSimpleText, CYouWin; shared_ptr_deleter order is CSimpleText, CFight, CLose, CYouWin.
- A file-scope `const char* const` array lands in `.rodata`, but the original `gAreaPaneNames` is in `.data`; declare it without the second `const`.
- The unit still cannot be flipped to Matching: the order of weak instantiations (shared_ptr ct/as/rf and vtables) differs from the original. The original emits `__rf__` of each notice class after that class's `Run`, and the deleter vtable for CNoticeDisplay first in `.data`; this compile emits them in a batch after `Show`. Residual non-100% functions: FormatAreaName (register swap), CFight/CYouWin/CSimpleText Init (`li` per argument vs register copy, plus `.data` offsets that follow the ordering problem).
