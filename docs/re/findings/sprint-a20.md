# Sprint a20: MSL container headers

## Landed changes

- **vector trivially copyable fast paths (+6 matched functions).** `include/vector` dispatches `push_back`, `push_back_realloc` and `__vec_split_buffer::construct_at_end(n, x)` on `__vec_trivial_copy<T>`: `is_POD<T>` or `(__builtin_trivial_members(T) & 7) == 7` (trivial destructor, assignment and copy constructor). `reallocate` and `move_construct_to_end` still use `is_POD<T>`, which is why the original has `++size; back() = x` in `push_back` and in the new-buffer fill, but a null-checked placement construct in `reserve`. The four `vector<SD_*::Data>::push_back` functions, `CBattleRoyalBox::ExportBattleRoyalData` and `CPii::ExportPersonalData` reach 100%.

## Traps

- `__builtin_trivial_members(T)` bits (Wii/1.1): 1 trivial destructor, 2 trivial assignment, 4 trivial copy constructor, 8 trivial default constructor. A class with only virtual functions reports 1.
- Keep the symbol name of `__vec_split_buffer::construct_at_end(size_type, const T&)`: dispatching with a new tag argument at the call site renames the function and regresses every unit that already matched it.
