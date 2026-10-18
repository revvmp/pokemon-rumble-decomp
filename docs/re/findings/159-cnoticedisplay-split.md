# Issue 159: CMultiRanking end and the CNoticeDisplay.cpp split

Target `wpse-v1`, frozen manifest `config/targets/wpse-v1.json`, analysed in the dtk 1.8.4 split assembly under `build/wpse-v1/asm/`. No decompiler output was used.

## F-159-1: file boundaries in `0x800CD800`–`0x800D00B0`

### `CMultiRanking.cpp` starts at `0x800CD730`, inside #158

The issue gave `0x800CD800` as the start of `CMultiRanking`. The file starts one function earlier, at `0x800CD730` (0xC8 bytes), which is in #158's block (`0x800CD180`–`0x800CD7F8`).

- `0x800CD730` reads the `.sdata2` float `0.0f` at `0x804B9F98`, which `CMultiRanking`'s `0x800CD800`, `0x800CEB20` and `0x800CF1D0` also read. `CMsgBalloon` code uses its own `0.0f` at `0x804B9F7C`. Float literals are local to an object, so `0x800CD730` is in `CMultiRanking.cpp`.
- Its only caller is `0x800CF070`, slot 2 of the `CMultiRanking` vtable `0x803A563C`.
- Every function from `0x800CD180` to `0x800CD720` is called only from `CMsgBalloon` code (`0x800CBE30`, `0x800CBF10`, `0x800CC2C0`, `0x800CC550`) or from vtables built next to it, so those belong to `CMsgBalloon.cpp`.
- The first `CMultiRanking` `.data` object is the string `ranking_p1` at `0x803A5550`; `below_site` and `above_site` at `0x803A5520`–`0x803A554F` are read by `CMsgBalloon`'s `0x800C7410`.

Under the file ownership rule, #158 owns the `CMultiRanking.cpp` split, object line and header. #158 was unassigned when this work was done, so this contribution does not split `CMultiRanking.cpp`. Its 22 functions in #159 (`0x800CD800`–`0x800CF860`) remain unreconstructed. Confidence high.

### `CNoticeDisplay.cpp`

F-160-1 places the start at `0x800CF8A0`. The end is the start of `CNullObj.cpp`:

| Section | Range | End evidence |
| --- | --- | --- |
| `extab` | `0x8001F948`–`0x80020CC0` | Record of `0x800D4B50` at `0x80020CC0` |
| `extabindex` | `0x800497E8`–`0x80049B60` | Entry of `0x800D4B50` at `0x80049B60` |
| `.text` | `0x800CF8A0`–`0x800D4B50` | `0x800D4B50` stores the `CNullObj` vtable `0x803A6C68` after calling the `BWorldObj` constructor; `ScriptBinding_NewNullObj` calls it. The last `CNoticeDisplay` function `0x800D4B10` (referenced from `CNoticeDisplay.cpp` `.data`) ends at `0x800D4B50` without padding |
| `.data` | `0x803A5840`–`0x803A6BD0` | `0x803A6B58` (0x78 bytes) is the last `default_delete<@unnamed@CNoticeDisplay_cpp@::...>` name; `0x803A6BD0` is the `CNullObj::CMotionProgram` deleter vtable |
| `.sdata` | `0x804B6CD8`–`0x804B6E80` | `0x804B6E80` and `0x804B6E88` are `CNullObj` type info, referenced only from `CNullObj` data |
| `.sdata2` | `0x804B9FB0`–`0x804B9FC0` | Functions up to `0x800D4B50` read `0x804B9FB0`–`0x804B9FBC`; `CNullObj` code reads `0x804B9FC0` onward |

The file has no `.bss`, `.sbss` or `.ctors` entry. `0x803BE468`, `0x8035B9D0`, `0x8035B9F8`, `0x804B5C48` and `0x804B9098` are shared objects of other files. The split, the `NonMatching` object line and `include/game/CNoticeDisplay.h` are committed; `configure.py`, `ninja` and `tools/verify/verify.py` exit 0.

## F-159-2: the CNoticeDisplay functions of #159

| Address | Function | Status |
| --- | --- | --- |
| `0x800CF8A0` | Area-name formatter used by three notice `Init` functions | Not reconstructed: inlines `std::wstring` and `NIntl::basic_formatter` (#327) |
| `0x800CFF50` | `WaitMotion(shared_ptr<CLayout::CMotionProgram>, bool waitFinish)` | 100% |
| `0x800D0030` | `WaitMotionStart(shared_ptr<CLayout::CMotionProgram>)` | 100% |

`WaitMotion` yields the fiber (`YieldFiber`, `0x8019E770`) until the input from `GetUIInput()` (`0x80124A40`) is connected and has trigger bit `0x20`, or the motion is set and, depending on the flag, finished (state 2 or 3) or started (state not 0). `WaitMotionStart` passes a copy and `false`. Its only caller, `0x800D2BE0` (#162), passes the motion that `0x801D12F0` creates for the `end` animation of the `TEXT` pane. The motion type is `CLayout::CMotionProgram`, from the deleter vtable `0x803C074C` stored by `0x801D1FE0`.

The motion test is one condition, `pMotion && ((waitFinish && IsFinished()) || (!waitFinish && IsStarted()))`. Two separate `if` statements give the same size but put the second `return` on the fall-through path (97.5%).

Both functions are written with external linkage. If they were in an unnamed namespace they would not be emitted until #162's caller exists (F-93 trap), so the linkage is unproven.

## Traps

- **A survey class start can be one function late.** The issue put `CMultiRanking` at `0x800CD800`, but a helper at `0x800CD730` in the previous block shares its float literals. Compare `.sdata2` constants, which are per object, across a block boundary before taking a split.
- **Separate `if` statements are not a combined condition.** In `WaitMotion`, `if (a && b) return; if (c && d) return;` and `if ((a && b) || (c && d)) return;` compile to the same size, but only the combined form branches to the epilogue after the final test.
- **Git Bash `python -` heredocs can hang.** In this environment, Git Bash commands that piped a heredoc into `python -` never returned. Write the script to a scratch file and run it from PowerShell.
