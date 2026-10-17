# F-760-2: Complete HBMBase extension methods and source link

Issue #760 owns the final three functions of the existing `RVL_SDK/hbm/HBMBase.cpp` split established by F-756-1. The source now contains all 37 functions after #759 supplied the intervening methods. No split boundaries changed.

| Address | Size | Method | Comparison |
| --- | --- | --- | --- |
| `0x80185E00` | `0x390` | `HomeButton::createInfoEx(const HBMDataInfoEx*)` | 100% |
| `0x80186190` | `0x120` | `HomeButton::destroyInfoEx()` | 100% |
| `0x801862B0` | `0x84` | `HomeButton::set_text_ex()` | 100% |

The three methods add 1,332 bytes of reconstructed C++. The complete unit has 40,428 function bytes; previously reconstructed functions retain their existing credit. The entire `.text` section, including padding and weak-function handling, and `.rodata`, `.data`, and `.bss` all compare at 100% in objdiff. The complete unit is registered `Matching` and linked from source.

Reference: Kirby's Epic Yarn (`Swiftshine/key` commit `7f2a9ab9`, CC0), NAND `src/hbm/homebutton/HBMBase.cpp`. Function names, resource construction, seven extension animation controllers, and teardown agree with the target. This target omits the reference's final `bar_line_00` scaling in `createInfoEx`; only `scBtnWareName[0]` is translated to `(-1.33f, -35.0f, 0.0f)` before parsing text. `destroyInfoEx` intentionally leaves the freed font pointer uncleared, as the native function does.

The shared message table declaration was corrected separately in prerequisite PR #896; see F-760-1 in `760-hbm-text-table-header.md`. Its consumers from #759 explicitly select language slot zero. Source uses the existing real classes, allocation APIs, destructors, and list iterators. No native instruction arrays or original-byte fallback implement these methods.

The creation method first scored 99.97369% while #759's intervening bodies were absent, because their missing constants and strings changed the referenced pools. Once #759 merged, all three methods and every pool matched at 100% without changing the creation method's instruction structure.

Validation: `python configure.py`, `.venv/Scripts/ninja`, and `python tools/verify/verify.py verify --manifest config/targets/wpse-v1.json --reference-dir orig/wpse-v1 --build-dir build/wpse-v1` exit 0 on the final committed branch. Verification compares the rebuilt `main.dol` directly with the frozen original and checks its declared hash. Objdiff reports 37/37 functions and all four sections at 100%.

## Traps

When one source unit spans several issues, missing neighboring bodies can leave string and floating-point pools incomplete even when an added method's non-relocation instructions are identical. Re-evaluate the complete unit after those bodies merge; do not add padding constants to compensate. Evidence: F-760-2.
